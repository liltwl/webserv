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
    if ( delim != 0)
        enf = recv(fd, &delim, 1, 0);   //hadi yarabak khasak mazal t9adha 3la hsab error status
    return (delim == 0 || line.size() == 0 ? 0 : enf);
}

int getlenline(int fd, string &line, int len)
{
    char delim;
    int enf = 0;
    int i = 0;

    while ((enf += recv(fd, &delim, 1, 0))> 0 && i < len)
    {
        i++;
        line.push_back(delim); 
    }
    if (delim != 0)
        recv(fd, &delim, 1, 0);
    return ( delim == 0 && enf == 1? 0 : i);
}

void headerpars(int fd, Request& ss)
{
    string line;
    vector<string> str;
    int i, j;

    for(i = 0;(j = getnextline(fd, line))> 0; i++)
    {
        if(line.size() == 1) return;
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
}

void Requeststup(int fd, Request& ss, pollfd &fds)
{
    string line;
    vector<string> str;
    int i, j;

    cout << "header :" << ss.empty_header() << endl;
    if (ss.empty_header())
    {
        headerpars(fd, ss);
        return ;
    }
    /*if (j == 0 && i == 1)
        return;*/
    if(ss.headers.count("Content-Length") && ss.headers.count("Transfer-Encoding"))
    {
        int req_len = stoi(ss.headers.find("Content-Length")->second);
        int body_len = 0;
        int k =1; 
        if (ss.get_body_len() < req_len)
        {
            string stmp ;
            getnextline(fd, stmp);
            int len = stol(stmp, nullptr, 16);
            stmp.clear();
            if (len != 0)
            {
                cout << len << endl;
                body_len += getlenline(fd, stmp, len);
                ss.addbody(stmp, body_len);
            }
            else
                cout << "content length > body len " << endl;
        }
        if (ss.get_body_len() == req_len)
            fds.events = POLLOUT;
        cout << body_len <<" == " << req_len << endl;
    }
    else if(ss.headers.count("Content-Length"))
    {
        int len = stoi(ss.headers.find("Content-Length")->second);
        getlenline(fd, ss.body, len);
        if (getlenline(fd, line, 1))
        {
            cout << "Content-Length < content len " << endl;
        }
        fds.events = POLLOUT;
    }
    else
        fds.events = POLLOUT;
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
                delete ss;
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

void addclienttoserver(server &ss,vector<client>& clients, int k)
{
    string str = "client";
    client stmp;

    stmp.set_fd(k);
    stmp.set_serv(ss);
    clients.push_back(stmp);
}




void delete_client(vector<client>& clients, int i)
{
    cout << "Deleting client " << i << endl;
    vector<client>::iterator it = clients.begin();
    for (int j = 0; j < i && it != clients.end();j++, it++);
    if (it != clients.end())
        clients.erase(it);
}





int main(int argc, char **argv)
{
    std::ifstream file;
    string line  = argv[1];
    vector<server> ss;
    vector<client> clients;
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
        for (int i = 0; i < ss.size() ; i++)
        {
            fds[i].events = POLLIN;
        }
        int rc = poll(fds, ss.size()+clients.size(), -1);
        if (rc < 0)
        {
            perror("serv poll() failed");
            break;
        }

        if (rc == 0)
        {
            printf("serv poll() timed out.  End program.\n");
            break;
        }
        for (int i = ss.size(), j = 0; i < clients.size() + ss.size(); i++, j++)
        {
            if (fds[i].revents != 0 && fds[i].revents & POLLIN)
            {
                cout << j << ": index :";
                // if (!req.empty())
                //     req.clear();
                Requeststup(fds[i].fd, clients[j].req, fds[i]);
                if (clients[j].req.empty())
                    continue;
                cout << clients[j].req.rqmethod << " " << clients[j].req.location << " " << clients[j].req.vrs << endl;  //hadi dyal rqst lbghiti tpintehom
                for (map<string, string>::iterator it = clients[j].req.headers.begin(); it != clients[j].req.headers.end(); it++)
                {
                    cout << it->first << ": " << it->second <<endl;
                }
                co÷ut << "body :" << clients[j].req.body << endl;
                cout << "*" << clients[j].req.headers["Connection"] << "*----" << endl;
            }
            else if (fds[i].revents != 0 && fds[i].revents & POLLOUT)
            {

                std::ofstream newfile;
                newfile.open("wwwww.png", std::ios::trunc);
                newfile << clients[j].req.body;

                                /*********Response*********/

                Response response(clients[j].req, *(clients[j].get_serv()));
                cout << response.respond().size() << " ====== " << response.get_response_size()<< endl;
                send(fds[i].fd, response.respond().c_str(), response.get_response_size(), 0);
                
                
                                /******************/


                fds[i].events = POLLIN;
                if (!(clients[j].req.headers.count("Connection") && clients[j].req.headers.at("Connection") == "keep-alive"))
                {
                    close (fds[i].fd);
                    fds[i].fd = 0;
                    delete_client(clients, j);//ss.e
                }
                clients[j].req.clear();
            }
            //response.clear();
        }
        for (int i = 0; i < ss.size(); i++)
        {
            size_t addrlen = sizeof(ss[i].sockaddr);

            // if (fds[i].events != POLLIN)
            // {
            //     printf("Error! revents = %d\n", fds[i].revents);
            //     break;
            // }
            if (fds[i].revents != 0 && fds[i].revents & POLLIN)
            {
                int connection = accept(ss[i].sockfd, (struct sockaddr*)&ss[i].sockaddr, (socklen_t*)&addrlen);
                
                if (connection < 0) {
                    std::cout << "Failed to grab connection= "<< i <<". errno: " << errno << std::endl;
                    continue;
                }
                
                pollfd fds1;
                fds[ss.size() + clients.size()].fd = connection;
                fds[ss.size() + clients.size()].events = POLLIN;
                addclienttoserver(ss[i], clients, connection);
                cout << "client 200 ok" << endl;
            }
        }
        cout<< "__________the_end____________" << endl;
    }
    //}

}
//}