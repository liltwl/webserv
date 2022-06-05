#include "webserv.hpp"
#include "Response.hpp"

vector<string> split (const string &s, char delim)
{
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}


void ft_spaceskip(string &line)
{
    int i = 0;

    while (i < line.size()) {
        if (line[i] != '\t' && line[i] != ' ') 
        {
            break;
        }
        i++;
    }
    line.erase(0,i);
}


int is_valid_fd(int fd)
{
    return fcntl(fd, F_GETFL) != -1 || errno != EBADF;
}

int getnextline(int fd, string &line)
{
    char delim;
    int enf = 0;

    while ((enf += recv(fd, &delim, 1, 0))> 0 && delim != 13 && delim != 0)
    {
        //if (delim != 13)
        line.push_back(delim); 
    }
    enf = recv(fd, &delim, 1, 0);
    return (delim == 0 || line.size() == 0 ? 0 : enf);
}

int getlenline(int fd, string &line, int len)
{
    char delim;
    int enf = 0;
    int i = 0;

    // enf = recv(fd, delim, len, 0);
    // delim[enf] = 0;

    while ((enf += recv(fd, &delim, 1, 0))> 0 && i < len)
    {
        i++;
        if (delim != '\r')
            line.push_back(delim); 
    }
    //line = delim;
    return ( delim == 0 && enf == 1? 0 : i);
}

void Requeststup(int fd, Request& ss)
{
    string line;
    vector<string> str;

    for(int i = 0;(getnextline(fd, line))> 0; i++)
    {
        str = split(line, ' ');
        if (i == 0)
        {
            ss.rqmethod = str[0];
            ss.location = str[1];
            ss.vrs = split(str[2], '/')[1];
        }
        else
        {
            str = split(line, ':');
            ss.headers[str[0]] = str[1].substr(1, str[1].size());
        }
        line.clear();
    }

    if(ss.headers.count("Transfer-Encoding"))
    {
        int req_len = stoi(ss.headers.find("Content-Length")->second);
        int body_len = 0;
        int k =1; 
        while (body_len < req_len-1)
        {
            cout << "FErferf" << endl;
            string stmp ;
            getnextline(fd, stmp);
            int len = stol(stmp, nullptr, 16);
            if (len == 0)
            {
                break;
            }
            cout << len << endl;
            body_len += getlenline(fd, ss.body, len);
        }
        cout << body_len <<" == " << req_len << endl;
    }
    else if(ss.headers.count("Content-Length"))
    {
        int len = stoi(ss.headers.find("Content-Length")->second);
        getlenline(fd, ss.body, len);
        if (getlenline(fd, line, 1))
        {
            cout << "Content-Length too small " << endl;
        }
    }
    cout << "||||||||||||||||||||||||||||||||||||||||||" << endl;
}






vector<string> allow_methodstup(vector<string> str)
{
    vector<string> val;
    for (int i = 1; i < str.size(); i++)
    {
        val.push_back(split(str[i], ';')[0]);
        cout << split(str[i], ';')[0] << " :hada ra ghir method" << endl;
    }

    return val;
}


void serversetup(vector<server> &tmp,std::ifstream &file)
{
    vector<string> str;
    server *ss = NULL;
    string line;

    for (int i = 0; !file.eof(); i++)
    {
        getline(file, line, '\n');
        ft_spaceskip(line);
        str = split(line, ' ');
        if (!str[0].compare("server"))
        {

            if (ss)
            {
                tmp.insert(tmp.begin(), *ss);
                //delete ss;
                ss = NULL;
            }
            cout << line << endl;
            ss = new server();
        }
        else if (!str[0].compare("server_name")&& str.size())
        {
            cout << line << endl;
            ss->setname(split(str[1], ';')[0]);
        }
        else if (!str[0].compare("listen")&& str.size())
        {
            cout << line << endl;
            ss->setadd(split(str[1], ':')[0],stoi(split(str[1], ':')[1]));
        }
        else if (!str[0].compare("root")&& str.size())
        {
            ss->setroot(split(str[1], ';')[0]);
        }
        else if (!str[0].compare("allow_methods") && str.size())
        {
            cout << line  << " || " << str[1]  << endl;
            ss->setmethods(allow_methodstup(str));
        }
        else if (!str[0].compare("autoindex")&& str.size())
        {
            if (split(str[1], ';')[0] == "on")
                ss->setautoindex(1);
            else
                ss->setautoindex(0);
        }
        else if (!str[0].compare("client_body_limit")&& str.size())
        {
            ss->setbody_limit(stoi(split(str[1], ';')[0]));
        }
        else if (!str[0].compare("location")&& str.size())
        {
            loc tmp1;
            string stmp = str[1];
            for (int j = 0 ; str[0].compare("}"); j++)
            {
                getline(file, line, '\n');
                ft_spaceskip(line);
                str = split(line, ' ');
                if (!str[0].compare("root")&& str.size())
                {
                    tmp1.root = split(str[1], ';')[0];
                    cout << tmp1.root << endl;
                }
                else if (!str[0].compare("allow_methods")&& str.size())
                {
                    tmp1.methods = allow_methodstup(str);
                }
            }
            ss->location[stmp] = tmp1;
        }
        else if (!str.empty())
        {
            cout << line<< " :others size = " << ss->others.size() << endl;
            ss->setothers(line);
        }
        line.clear();
        str.clear();
    }
    if (ss)
    {
        tmp.insert(tmp.begin(), *ss);
        delete ss;
    }
    cout << tmp[0].others.size() << " = " << tmp[0].name  << endl;
    cout << "|||||||||||||||||||||||*********||||||||||||||||||||||||||||" << endl;
}
int guard(int n, string err) { if (n == -1) { cout << (err) << endl; exit(1); } return n; }

void servers(vector<server> &ss,pollfd *fds)
{
    sockaddr_in sockaddr;
    int yes = 1;

    for (int i = 0; i < ss.size(); i++)
    {
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_addr.s_addr = inet_addr(ss[i].addr.c_str());
        sockaddr.sin_port = htons(ss[i].port);
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if ((ss[i].sockfd = sockfd) == -1) 
        {
            std::cout << "Failed to create socket. errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }


        int flags = guard(fcntl(sockfd, F_GETFL), "could not get flags on TCP listening socket");
        guard(fcntl(sockfd, F_SETFL, O_NONBLOCK | flags), "could not set TCP listening socket to be non-blocking");
        // setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        // ioctl(sockfd, FIONBIO, (char *)&yes);



        if (bind(ss[i].sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) 
        {
            std::cout << "Failed to bind to port "<< ss[0].port <<". errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }
        if (listen(sockfd, 255) < 0) 
        {
            std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }
        ss[i].sockaddr = sockaddr;
        fds[i].fd = ss[i].sockfd;
        fds[i].events = POLLIN;
    }
}













int main(int argc, char **argv)
{
    std::ifstream file;
    string line  = argv[1];
    vector<server> ss;
    Request req;
    int maxfd=0;
    pollfd fds[100];



    file.open(line);
    serversetup(ss, file);
    cout << ss[0].name << " " << ss[0].addr << "::"<<  ss[0].port << ":" << ss[0].others.size() << " : "<< ss[0].location.begin()->first  << endl;
    map<int ,string> trr =ss[0].geterrorpages();
    
    // for (map<int ,string>::iterator it = trr.begin(); it != trr.end(); it++)
    // {
    //     cout << it->first <<"other: " << it->second <<endl;
    // }
    servers(ss, fds);


    
    
    while (1)
    {
        // for (int i = 0; i < ss.size(); i++)
        // {
        //     fds[i].fd = ss[i].sockfd;
        //     fds[i].events = POLLIN;
        // }
        int rc = poll(fds, ss.size(),-1);
        if (rc < 0)
        {
            perror("  poll() failed");
            break;
        }

        if (rc == 0)
        {
            printf("  poll() timed out.  End program.\n");
            break;
        }
        // int rc = 0;
        // if((rc = select(maxfd + 1, read_fds, write_fds, NULL, NULL)) < 0)
		// 	perror("select");
        // cout << "poll return: " << rc << std::endl;
        for (int i = 0; i < ss.size(); i++)
        {
            size_t addrlen = sizeof(ss[i].sockaddr);

            if (fds[i].events != POLLIN)
            {
                printf("Error! revents = %d\n", fds[i].revents);
                break;
            }
            if (fds[i].revents != 0 && fds[i].events & POLLIN)
            {
                int connection = accept(ss[i].sockfd, (struct sockaddr*)&ss[i].sockaddr, (socklen_t*)&addrlen);
                
                if (connection < 0) {
                    std::cout << "Failed to grab connection= "<< i <<". errno: " << errno << std::endl;
                    continue;
                }
                
                pollfd fds1;
                fds1.fd = connection;
                for (size_t j = 0; j == 0 ;)\
                {
                    fds1.events = POLLIN;
                    int rcc = poll(&fds1, 1,1000);
                    if (rcc < 0)
                    {
                        perror("  poll() failed");
                        break;
                    }
                    if (rcc == 0)
                    {
                        printf("  poll() timed out.\n");
                        j = 1;
                        continue;
                    }
                    if (fds1.events != 0 && fds1.events & POLLIN)
                        Requeststup(connection, req);
                    if (req.empty())
                        continue;
                    //cout <<req.rqmethod << " " << req.location << " " << req.headers.begin()->first <<endl;
                    // size_t bytesRead = read(connection, buffer, 1000);
                    // std::cout << "The message was: " << buffer<< endl;
                    cout << req.rqmethod << " " << req.location << " " << req.vrs << endl;  //hadi dyal rqst lbghiti tpintehom
                    for (map<string, string>::iterator it = req.headers.begin(); it != req.headers.end(); it++)
                    {
                        cout << it->first << ": " << it->second <<endl;
                    }
                    cout << "body :" << req.body << endl;
                    cout << "*" << req.headers["Connection"] << "*----" << endl;
                    fds1.events = POLLOUT;
                    Response response(req, ss[i]);
                    rcc = poll(&fds1, 1,100);
                    if (rcc == 0)
                    {
                        printf("  poll() timed out.\n");
                        j = 1;
                        continue;
                    }
                    cout << response.respond().size() << " ====== " << response.get_response_size()<< endl;
                    if (fds1.events != 0 && fds1.events & POLLOUT)
                        send(connection, response.respond().c_str(), response.get_response_size(), 0);
                    if (!(req.headers.count("Connection") && req.headers.at("Connection") == "keep-alive"))
                    {
                        j = 1;
                    }
                    req.clear();
                    //response.clear();
                }
                close (connection);
                cout <<endl << "______________________" << endl;
            }
            else{
                ;
            }
        }

    }
}