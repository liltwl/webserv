#include "Client.hpp"

client::client():fd(0) , ss(NULL), res(NULL){}

void client::set_fd(int _fd)
{
fd = _fd;
}

void client::set_serv(server& _ss)
{
    ss = &_ss;
}
void client::set_servers(server _serv)
{
    serv.push_back(_serv);
}
    int client::get_fd()
{
    return fd;
}
server* client::get_serv()
{
    return ss;
}
void client::respond(pollfd &fds)
{
    if(this->res == NULL)
    {
        res = new Response(req,*ss);
        string a = res->responde();
        send(fd, a.c_str(),a.size(),0);
        fds.events = POLLIN;
        cout << a.size() << "  size"<< endl;
       delete(res);
    }
};