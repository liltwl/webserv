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



typedef struct  location{
    string root;
    vector<string> methods;
}              loc;

class server
{
    public:
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


    public:
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
                others = _server.others;
                methods = _server.methods;
                location = _server.location;
            }
            return *this;
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
};


class Request
{
    public :
        string rqmethod;
        string location;
        string vrs;
        map<string, string> headers;
        string body;


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
            body= line;
        }

        void clear()
        {
            rqmethod.clear();
            location.clear();
            vrs.clear();
            headers.clear();
            body.clear();
        }
};

