 #pragma once
#include "Response.hpp"
using namespace std;
class client
{
    public :
        int fd;
        server  *ss;
        std::vector<server> serv;
        Request req;
        Response *res;
      
        string chunk;
    
    client();
    ~client();
    void set_fd(int _fd);
    void set_serv(server& _ss);
    void set_servers(server _serv);
    int get_fd();
    server* get_serv();
    void respond(pollfd &fds);
};