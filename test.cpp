#include "webserv.hpp"

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
    int enf;

    while ((enf = recv(fd, &delim, 1, 0))> 0 && delim != '\n')
    {
        line.push_back(delim); 
    }
    return (is_valid_fd(fd) == 0 || line.size() == 1? 0 : enf);
}

void Requeststup(int fd, Request& ss)
{
    string line;
    vector<string> str;

    for(int i = 0;(getnextline(fd, line))> 0; i++)
    {
        //cout << line << endl;
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
        //cout << line << endl;
        line.clear();
    }
    cout << "||||||||||||||||||||||||||||||||||||||||||" << endl;
}

vector<server> serversetup(std::ifstream &file)
{
    vector<server> tmp;
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
                tmp.push_back(*ss);
                delete ss;
                ss = NULL;
            }
            cout << line << endl;
            ss = new server();
        }
        if (!str[0].compare("server_name"))
        {
            cout << line << endl;
            ss->name = split(str[1], ';')[0];
        }
        else if (!str[0].compare("listen"))
        {
            cout << line << endl;
            ss->addr = split(str[1], ':')[0];
            ss->port = stoi(split(str[1], ':')[1]);
        }
    }
    if (ss)
    {
        tmp.push_back(*ss);
        delete ss;
    }
    return tmp;
}
int guard(int n, string err) { if (n == -1) { cout << (err) << endl; exit(1); } return n; }

void servers(vector<server> &ss,pollfd *fds, fd_set *read_fds,fd_set *write_fds,int &maxfd)
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
        if(maxfd < sockfd)
            maxfd = sockfd;


        // int flags = guard(fcntl(sockfd, F_GETFL), "could not get flags on TCP listening socket");
        // guard(fcntl(sockfd, F_SETFL, O_NONBLOCK | flags), "could not set TCP listening socket to be non-blocking");
        // setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        // ioctl(sockfd, FIONBIO, (char *)&yes);



        if (bind(ss[i].sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) 
        {
            std::cout << "Failed to bind to port "<< ss[0].port <<". errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }
        if (listen(sockfd, 10) < 0) 
        {
            std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }
        ss[i].sockaddr = sockaddr;
        fds[i].fd = ss[i].sockfd;
        fds[i].events = POLLIN;
        FD_SET(sockfd, &read_fds[i]);
    }
}

int main(int argc, char **argv)
{
    std::ifstream file;
    string line  = argv[1];
    vector<server> ss;
    Request req;
    int maxfd=0;
    pollfd fds[10];

    fd_set read_fds[10];
	fd_set write_fds[10];


    FD_ZERO(read_fds);
	FD_ZERO(write_fds);
    file.open(line);
    ss = serversetup(file);
    cout << ss[0].name << " " << ss[0].addr<< "::"<< ss[0].port << endl;
    servers(ss, fds, read_fds, write_fds, maxfd);

    
    
    while (1)
    {
        // for (int i = 0; i < ss.size(); i++)
        // {
        //     fds[i].fd = ss[i].sockfd;
        //     fds[i].events = POLLIN;
        // }
        int rc = poll(fds, ss.size() + 1,-1);
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
        cout << "poll return: " << rc << std::endl;
        for (int i = 0; i < ss.size(); i++)
        {
            size_t addrlen = sizeof(ss[i].sockaddr);

            // if(fds[i].events == 0)
            // if(FD_ISSET(i, &read_fds[i]))
            //     continue;
            cout << "efewrfewf" << endl ;
            // if(!FD_ISSET(ss[i].sockfd, &read_fds[i]))
            if (fds[i].events != POLLIN)
            {
                printf("Error! revents = %d\n", fds[i].revents);
                break;
            }
            // if(FD_ISSET(ss[i].sockfd, &read_fds[i]))
            if (fds[i].revents != 0 && fds[i].events & POLLIN)
            {
                int connection = accept(ss[i].sockfd, (struct sockaddr*)&ss[i].sockaddr, (socklen_t*)&addrlen);
                if (connection < 0) {
                    std::cout << "Failed to grab connection= "<< i <<". errno: " << errno << std::endl;
                    continue;
                }
                Requeststup(connection, req);


                //cout <<req.rqmethod << " " << req.location << " " << req.headers.begin()->first <<endl;
                // size_t bytesRead = read(connection, buffer, 1000);
                // std::cout << "The message was: " << buffer<< endl;
                cout << req.rqmethod << " " << req.location << " " << req.vrs << endl;  //hadi dyal rqst lbghiti tpintehom
                for (map<string, string>::iterator it = req.headers.begin(); it != req.headers.end(); it++)
                {
                    cout << it->first << ": " << it->second <<endl;
                }
                req.clear();

                std::string response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
                send(connection, response.c_str(), response.size(), 0);
                cout <<endl << "______________________" << endl;
                close(connection);
            }
            else{
                ;
            }
        }

    }
}