#pragma once


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
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>


using namespace std;

vector<string> split (const string &s, char delim);


typedef struct  location{
    string root;
    vector<string> methods;
}              loc;

class server
{
    public :
        sockaddr_in sockaddr;
        int sockfd;

        string name;
        int port;
        string addr;
        string root;
        int autoindex;
        int body_limit;
        vector<string> methods;
        map<string,loc> location;

        vector<string> others;


    public :
        server()
        {

        }
        server(server const& _server) : name(_server.name), port(_server.port), addr(_server.addr), root(_server.root) {
            *this = _server;
        }

        server& operator=(server const& _server)
        {
            if (this != &_server)
            {
                name = _server.name;
                port = _server.port;
                addr = _server.addr;
                root = _server.root;
                autoindex = _server.autoindex;
                body_limit = _server.body_limit;
                others = _server.others;
                methods = _server.methods;
                location = _server.location;
            }
            return *this;
        }
        ~server()
        {
            //close (sockfd);
        }
        void setadd(string add, int _port)
        {
            addr = add;
            port = _port;
        }
        void setroot(string _root)
        {
            root = _root;
        }
        void setname(string m){
            name = m;
        }
        void setlocation(map<string,loc> _l)
        {
            location = _l;
        }
        void setmethods(vector<string> m)
        {
            methods = m;
        }
        void setautoindex(int val)
        {
            autoindex = val;
        }
        void setbody_limit(int val)
        {
            body_limit = val;
        }

        void setothers(string &val)
        {
            //others[key] = val;
            others.push_back(val);
        }

        vector<string>& getmethods()
        {
            return methods;
        }

        
        void set_fd(int _fd)
        {
            sockfd = _fd;
        }

        int get_auind()
        {
            return autoindex;
        }

        string get_name()
        {
            return name;
        }

        map<int, string> geterrorpages()
        {
            map<int, string> tmp;
        
            for (int i = 0; i < others.size();i++)
            {
                vector<string> str;
                string line, val;
                int key;
                line = others[i];
                str = split(line, ' ');
                if (str[0].compare("error_page") == 0)
                {
                    val = str[str.size() - 1].substr(0, str[str.size() - 1].size() - 2);
                    for (int j = 1; j < str.size() - 1; j++)
                    {
                        cout << str[j] << endl;
                        key = stoi(str[j]);
                        tmp[key] = val;
                    }
                }
            }
            return tmp;
        }
};




class Request
{
    public :
        string rqmethod;
        string location;
        string vrs;
        map<string, string> headers;
        string body;
        size_t body_len;


        Request(): body_len(0) {}
        Request& operator=(Request const& other)
        {
            if (this != &other)
            {
                rqmethod = other.rqmethod;
                location = other.location;
                vrs = other.vrs;
                headers = other.headers;
                body = other.body;
                body_len = other.body_len;
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
        void addheaders(string key, string value)
        {
            headers[key] = value;
        }
        void addbody(string line, size_t len)
        {
            body += line;
            body_len += len;
        }

        void clear()
        {
            rqmethod.clear();
            location.clear();
            vrs.clear();
            headers.clear();
            body.clear();
            body_len = 0;
        }

        int empty_header()
        {
            if (rqmethod.empty() || location.empty() || vrs.empty() || headers.empty())
                return 1;
            return 0;
        }

        map<string, string> get_headrs()
        {
            return headers;
        }

        size_t get_body_len()
        {
            return body_len;
        }

        size_t empty()
        {
            cout << rqmethod.empty()<< " empty" << endl;
            return (rqmethod.empty());
        }
};


class client
{
    public :
        int fd;
        server  *ss;
        Request req;
    
    client():fd(0) , ss(NULL){}
    ~client(){
        // if (fd > 0)
        // close (fd);
    }
    void set_fd(int _fd)
    {
        fd = _fd;
    }

    void set_serv(server& _ss)
    {
        ss = &_ss;
    }

    int get_fd()
    {
        return fd;
    }

    server* get_serv()
    {
        return ss;
    }
};





vector<string> allow_methodstup(vector<string> str);
string location_pars(server *ss, vector<server> &tmp, std::ifstream &file, vector<string> str);
int server_pars(vector<server> &tmp,std::ifstream &file, server *ss);
void serversetup(vector<server> &tmp,std::ifstream &file);
int guard(int n, string err);
void ft_spaceskip(string &line);


