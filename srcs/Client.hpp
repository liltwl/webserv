 #pragma once
#include "Response.hpp"
#include <sys/poll.h>

using namespace std;
class Response;
class client
{
    public :
        int fd;
        server  *ss;
        std::vector<server> serv;
        Request req;
        Response *res;
        short  event; 
      
        string chunk;
    
    client();
    ~client(){};
    void set_fd(int _fd);
    void set_event(short  val);
    short  get_event();
    void set_serv(server& _ss);
    void set_servers(server _serv);
    int get_fd();
    server* get_serv();
    void respond(pollfd &fds);
};