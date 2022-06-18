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
#include "parse_confile/config_file.hpp"

using namespace std;



class Request
{
    public :
        string rqmethod;
        string location;
        string vrs;
        map<string, string> headers;
        string body;
        size_t body_len;

    public :

        Request(): body_len(0) {}
        Request(Request const& other)
        {
            *this = other;
        }
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

        string get_method()
        {
            return rqmethod;
        }

        string  get_location()
        {
            return location;
        }


        string get_version()
        {
            return vrs;
        }

        map<string, string> get_headrs()
        {
            return headers;
        }

        string get_body()
        {
            return body;
        }

        size_t get_body_len()
        {
            return body_len;
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

        size_t empty()
        {
            return (rqmethod.empty() || location.empty() || vrs.empty());
        }
};

class client
{
    public :
        int fd;
        server  *ss;
        Request req;
        //Response *res;
      
        string chunk;
    
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
    // void respond(pollfd &fds)
    // {
    //     if(!this->res)
    //     {
    //         res = new Response(req,*ss);
    //     }
    //     if(res->is_chunked == false)
    //     {
    //         send(fd, res->respond().c_str(),res->get_response_size(),0);
    //         fds.events = POLLIN;
    //     }
    //     else
    //     {
    //         if(res->header_sent == false )
    //         {
    //             res->header->set_firstline(res->code.get_code(res->status));
    //             send(fd, res->header->get_Header().c_str(),res->header->get_Header().size(),0);
    //             res->header_sent = true;
    //             cout << res->header->get_Header() << endl;
    //         }
    //         while(res->is_chunked == true)
    //         {
    //             chunk = res->get_chunk();
    //             send(fd, chunk.c_str(),chunk.size(),0);   
    //             cout << "CHUNK :"  <<  chunk << endl;
    //             if (chunk == "0\r\n")
    //                 fds.events = POLLIN;
    //             //i++;
    //         }
    //         //if(res->is_chunked == false)
    //         //{
    //           //  send(fd, string("0\r\n\r\n").c_str(),string("0\r\n\r\n").size(),0);
    //             //fds.events = POLLIN;
    //         //}
    //        //chunk = string("0\r\n\r\n");
        
    //     }
    //     // if(res->is_chunked == false)
    //     // {
    //     //     delete res;
    //     // }
    // }
};




// vector<string> allow_methodstup(vector<string> str);
// string location_pars(server *ss, vector<server> &tmp, std::ifstream &file, vector<string> str);
// int server_pars(vector<server> &tmp,std::ifstream &file, server *ss);
// void serversetup(vector<server> &tmp,string filename);
int guard(int n, string err);
// void ft_spaceskip(string &line);


