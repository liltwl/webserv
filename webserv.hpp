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

        void clear()
        {
            rqmethod.clear();
            location.clear();
            vrs.clear();
            headers.clear();
            body.clear();
        }
};

