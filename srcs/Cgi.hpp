#pragma once
#include "Response.hpp"

using namespace std;

class Cgi
{
    private:
        std::string resp;
        std::string respbody;
        std::map<std::string, std::string>	headers;
    public:
        Cgi():
    resp(""),
    respbody(""),
    headers(){};
        ~Cgi(){};
        void    set_resp_path(std::string full_path) { resp = full_path;};
        void    set_body_path(std::string body_path) { respbody = body_path;};
        void    set_headers(std::string &line)
        {
        	size_t found;
            //cout << <<line << endl;
        	line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        	std::stringstream ss(line);
        	std::string lne;
        	std::string key;
        	std::string value;
        	while (getline(ss, lne, '\n'))
        	{
        		found = lne.find(":");
        		if (found != std::string::npos)
        		{
        			key = lne.substr(0, found);
        			value = lne.substr(found + 2);
        			key.erase(remove(key.begin(), key.end(), ' '), key.end());
        			value.erase(remove(value.begin(), value.end(), ' '), value.end());
        			headers[key] = value;
        		}
        	}
        };



        string sethttpheader(const char *key)
        {
            int i = 0; 
            string kkey = "HTTP_";
            while(key[i])
            {
            kkey += toupper(key[i]);
            i++;
            }
            return(kkey);
        };
        void    set_env(Request &req,string script ,string input)
        {
            setenv( "GATEWAY_INTERFACE", "CGI/1.1",1);
            setenv( "SERVER_PROTOCOL", "HTTP/1.1",1);
            setenv( "SERVER_PORT","4243",1);
            setenv( "REMOTE_ADDR","0.0.0.0" ,1);
            setenv( "PATH_INFO", req.get_location().c_str(),1);
            setenv( "SCRIPT_FILENAME" , script.c_str(),1); //need full path
            setenv( "QUERY_STRING" ,"" ,1);
            setenv( "REQUEST_METHOD" , req.get_method().c_str(),1);
            setenv( "REDIRECT_STATUS" , "true",1);
            setenv( "FCGI_ROLE" , "RESPONDER",1);
            setenv( "REQUEST_SCHEME" , "http",1);
            setenv( "REMOTE_PORT" ,"0",1);
            
            if(req.get_headrs().count("Content-Length")  == 0)
            {
                setenv( "CONTENT_TYPE" , req.get_headrs()["Content-Type"].c_str(),1);
                setenv( "CONTENT_LENGTH" , to_string(req.get_body().size()).c_str(),1);
            }
            else
            {
                    setenv( "CONTENT_TYPE","text/html; charset=UTF-8" ,1);
                    setenv( "CONTENT_LENGTH", "0" ,1);
            }
             cout << "zab" << endl;
            map<string,string> tnp = req.get_headrs();
            for(map<string,string>::iterator it = tnp.begin(); it != tnp.end(); it++)
            {
                    setenv(this->sethttpheader(it->first.c_str()).c_str(),it->second.c_str(),1);
            }
        };
        int     execute(string input, string script, string cgipath,Request &req )
        {

            pid_t pid = -1;
            int fd[2] = {-1};
            int last_fd = -1;
            if (input != "")
                fd[0] = open(input.c_str(), O_RDONLY, 0666);
            if(this->resp == "")
            {
                this->resp = "/tmp/" + string("outiz") + ".html";
                this->respbody = "/tmp/" + string("intiz") + ".html";
            }

            cout << "****** script " << script<< endl;
            cout << "****** cgipath" << cgipath << endl;
            cout << "****** " << this->resp<< endl;
            cout << "****** " << this->respbody<< endl;
            fd[1] = open(this->resp.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
            last_fd = open(this->respbody.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
            if ((fd[0] == -1 && input != "") || fd[1] == -1 || last_fd == -1)
                return 501;
            
            pid = fork();
            if(pid == -1)
            {
                close(fd[1]);
                if (this->resp != "")
                    remove(this->resp.c_str());
                if (fd[0] > 0)
                    close(fd[0]);
                return 502;
            }
            if (pid == 0)
            {
                
                set_env(req, script, input);
               
                if (fd[0] > 0)
                {
                    dup2(fd[0], 0);
                }
                dup2(fd[1], 1);
                close(fd[1]);
                char *tmp[3];
                tmp[0] = (char *)cgipath.c_str();
                tmp[1] = (char *)script.c_str();
                tmp[2] = NULL;
                int ret = execvp(tmp[0], tmp);
                std::cout << "execve failed: " << strerror(errno) << std::endl;
                remove(this->resp.c_str());
                exit(ret);// IHAVE TO RETURN BAD GATEWAY
                //dup to 1 && (dup to 0 if body is here)
            }
            else
            {
            time_t t = time(NULL);
            close(fd[1]);
            if (fd[0] > 0)
                close(fd[0]); 
            int status = 0, stats = 0, timeout = 0;
            stats = stats + 1 - 1;
            timeout = 0;
            while (difftime(time(NULL),  t) <= 60)
            {
                status = waitpid(pid ,&status, 0);
                if (status == 0)
                    timeout = 1;
                else
                {
                    timeout = 0;
                    break ;
                }
            }
            if (timeout)
            {
                kill(pid, SIGKILL);
                return 408;
            }
            else
            {
                fd[1] = open(resp.c_str(), O_RDWR, 0644);
                if (fd[1] == -1)
                {
                    close(fd[1]);
                    return 503;
                }
                char buffer[2000] = {0};
                std::string output;
                std::string headers;
		        int ret = 1;
                lseek(fd[1], 0, SEEK_SET);
                bool headers_done = false;
                while (ret > 0)
                {
                    memset(buffer, 0, 2000);
                    ret = read(fd[1], buffer, 2000 - 1);
                    if (ret == -1)
                    {
                        close(fd[1]);
                        return 504;
                    }
                    output += buffer;
                    if (output.find("\r\n\r\n") != std::string::npos && !headers_done)
                    {
                        headers = output.substr(0, output.find("\r\n\r\n"));
                        output = output.erase(0, output.find("\r\n\r\n") + 4);
                        headers_done = true;
                        set_headers(headers);
                    }
                    if (headers_done)
                    {
                        int t = write(last_fd, output.c_str(), output.size());
                        output.clear();
                        if (t == -1)
                        {

                            close(fd[1]);
                            close(last_fd);
                            return 505;
                        }
                        else if (t == 0)
                            continue;
                    }
                }
                close(fd[1]);
                close(last_fd);

            }
            cout << "lkilab"<< endl;
            return(1);
        }
    };
};
