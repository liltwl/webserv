#include <vector>
#include <iostream>
#include <iterator>
#include <utility>
#include <ctime>
#include <iomanip>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <map>
#include <random>
#include <fstream>
#include <sstream>
#include <sys/socket.h> // For socket functions
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/poll.h>



using namespace std;


class server
{
    public:
        sockaddr_in sockaddr;
        int sockfd;

        string name;
        int port;
        string addr;
        string root;

    public:
        server(string _name, int _port, string _addr,string _root) : name(_name), port(_port), addr(_addr), root(_root) {}
        server(){}
        server(server const& _server) : name(_server.name), port(_server.port), addr(_server.addr), root(_server.root) {}

        server& operator=(server const& _server)
        {
            if (this != &_server)
            {
                name = _server.name;
                port = _server.port;
                addr = _server.addr;
                root = _server.root;
            }
            return *this;
        }
};


class Request
{
    public :
        string rqmethod;
        string location;
        string vrs;
        map<string, string> headers;
        vector<string> body;


        Request(){}
        Request& operator=(Request const& other)
        {
            if (this != &other)
            {
                rqmethod = other.rqmethod;
                location = other.location;
                vrs = other.vrs;
                headers = other.headers;
                body = other.body;
            }
            return *this;
        }
        void setrqmethod(string val)
        {
            rqmethod = val;
        }

        void setlocation(string val)
        {
            location = val;
        }
        void setversion(string val)
        {
            vrs = val;
        }
        void addheaders(string name, string value)
        {
            headers[name] = value;
        }
        void addbody(string line)
        {
            body.push_back(line);
        }
};


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

    while (is_valid_fd(fd) && (enf = read(fd, &delim, 1))> 0 && delim != '\n')
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

void servers(vector<server> &ss,pollfd *fds)
{
    sockaddr_in sockaddr;

    for (int i = 0; i < ss.size(); i++)
    {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if ((ss[i].sockfd = sockfd) == -1) 
        {
            std::cout << "Failed to create socket. errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_addr.s_addr = inet_addr(ss[i].addr.c_str());
        sockaddr.sin_port = htons(ss[i].port);
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
    }
}

int main(int argc, char **argv)
{
    std::ifstream file;
    string line  = argv[1];
    vector<server> ss;
    Request req;
    pollfd fds[10];

    file.open(line);
    ss = serversetup(file);
    cout << ss[0].name << " " << ss[0].addr<< "::"<< ss[0].port << endl;
    servers(ss, fds);

    


    
    while (1)
    {
        int rc = poll(fds, 1, (3 * 60 * 1000));
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
                cout << "efewrfewf" << endl ;

        for (int i = 0; i < ss.size(); i++)
        {
            size_t addrlen = sizeof(ss[i].sockaddr);

            char buffer[1000];
            
            if(fds[i].events == 0)
                continue;
            if (fds[i].events != POLLIN)
            {
                printf("  Error! revents = %d\n", fds[i].revents);
                break;
            }
            if (fds[i].fd == ss[i].sockfd)
            {
                int connection = accept(ss[i].sockfd, (struct sockaddr*)&ss[i].sockaddr, (socklen_t*)&addrlen);
                if (connection < 0) {
                    std::cout << "Failed to grab connection. errno: " << errno << std::endl;
                    exit(EXIT_FAILURE);
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