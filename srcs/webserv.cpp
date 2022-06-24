#include "request.hpp"
#include "Response.hpp"

# define delim_size 2000

std::vector<std::string> split (const std::string &s, char delim)
{
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim))
    {
        result.push_back(item);
    }
    return result;
}


void ft_spaceskip(std::string &line)
{
    int i = 0;

    while (i < int(line.size())) {
        if (line[i] != '\t' && line[i] != ' ') 
            break;
        i++;
    }
    line.erase(0,i);
}

int getnextline(int fd, std::string &line)
{
    char delim;
    char nl = 0;
    int enf = 0;

    while ((enf += recv(fd, &delim, 1, 0))> 0 && delim != 0)
    {
        if (delim == 13)
        {
            if ((enf += recv(fd, &nl, 1, 0)) > 0 && (nl == '\n' || nl == 0))
                break;
            line.push_back(delim); 
            line.push_back(nl); 
        }
        else
            line.push_back(delim); 
    }
    return (nl == 0||delim == 0 || line.size() == 0 ? 0 : enf);
}

int getlenline(int fd, std::string &line, int len)
{
    char delim[delim_size + 1];
    char nl[2];
    int enf = 0;
    int i = 0;

    while (i < len && (enf = recv(fd, &delim, i +delim_size< len? delim_size : (len - i), 0))> 0)
    {
        i += enf;
        line.append(delim, enf);
    }
    if ((enf = recv(fd, &nl, 2, 0)) > 0 && (nl[0] ==  13 && nl[1] == '\n'))
        return i;
    return (enf == -1?i:-1);
}

void headerpars(int fd, Request& ss, client& clients)
{
    std::string line;
    std::vector<std::string> str;
    int i, j;

    for(i = 0;(j = getnextline(fd, line))> 0; i++)
    {
        if(line.size() == 0) return ;
            str = split(line, ' ');
        if (i == 0 && str.size() > 2)
        {
            ss.setrqmethod(str[0]);
            int j = str[1].find('?');
            if (j > 0)
            {
                ss.setlocation(str[1].substr(0, j));
                ss.setquery(str[1].substr(j, str[1].size()));
            }
            else
                ss.setlocation(str[1]);
            ss.setversion(str[2]);
        }
        else if (i == 0 && !(str.size() > 2))
        {
            ss.setbody_limit(-1);
            return ;
        }
        else
        {
            str = split(line, ':');
            ss.addheaders(str[0],str[1].substr(1, str[1].size()));
        }
        line.clear();
    }
    if (!ss.empty_header())
    {
        std::string sr_name = ss.get_headrs().find("Host")->second;
        std::vector<server>& serv = clients.serv;
        for (int i = 0; i < int(serv.size()); i++)
            for (int j = 0; j < int(serv[i].get_name_size()); j++)
                if (sr_name == serv[i].get_name(j))
                {
                    clients.set_serv(serv[i]);
                    break ;
                }
    }
}

void chunked_body_pars(int fd, Request& ss, pollfd &fds)
{
    std::string line;
    std::vector<std::string> str;
    int req_len = ss.get_headrs().count("Content-Length")? stoi( ss.get_headrs().find("Content-Length")->second) : 0; // update
    int body_len = 0;
    int len = 0;

    if (ss.get_body_len() <= req_len)
    {
        std::string stmp ;
        getnextline(fd, stmp);

        if (isxdigit(stmp[0])) 
            len = stol(stmp, nullptr, 16);
        stmp.clear();
        if (len > 0)
        {
            body_len += getlenline(fd, stmp, len);
            ss.addbody(stmp, body_len);
        }
        else if (len == 0)
        {
            if (getlenline(fd, stmp, 1) == 1)
                ss.setbody_limit(-1);
            fds.events = POLLOUT;
        }
    }
    else
    {
        ss.setbody_limit(-1);
        fds.events = POLLOUT;
    }

}

void body_pars(int fd, Request& ss, pollfd &fds)
{
    std::string line;

    int len = stoi(ss.get_headrs().find("Content-Length")->second);
    ss.setbody_limit(getlenline(fd, ss.get_body(), len));


    fds.events = POLLOUT;
}

void Requeststup(int fd, client& clients, pollfd &fds)
{
    std::string line;
    std::vector<std::string> str;
    Request& ss = clients.req;
    int max_body_size = clients.ss->get_client_max_body_size();

    if (ss.empty_header())
        headerpars(fd, ss, clients);
    else if(ss.get_headrs().count("Transfer-Encoding"))
        chunked_body_pars(fd, ss, fds);
    else if(ss.get_headrs().count("Content-Length") && stoi(ss.get_headrs().at("Content-Length")) <= max_body_size)
        body_pars(fd, ss, fds);
    else if (ss.get_headrs().count("Content-Length") && stoi(ss.get_headrs().at("Content-Length")) > max_body_size)
        fds.events = POLLOUT;
    if (!ss.get_headrs().count("Transfer-Encoding") && (!ss.get_headrs().count("Content-Length") || ss.get_headrs().at("Content-Length") == "0"))
        fds.events = POLLOUT;
}

int guard(int n, std::string err)
{
    if (n < 0)
    {
        std::cout << err << std::endl;
        exit(1); 
    }
    return n;
}





// /**********************/


int is_binded_server(std::vector<server> ss, int i)
{
    server serv = ss[i];

    for (int j = 0; j < i; j++)
        if (serv.get_listen_port()== ss[j].get_listen_port() && serv.get_listen_host()== ss[j].get_listen_host())
            return 0;
    return 1;
}

void servers(std::vector<server> ss,std::vector<pollfd> &fds)
{
    pollfd fd;
    int set = 1;
    for (int i = 0; i < int(ss.size()); i++)
    {
        if (is_binded_server(ss, i))
        {
            cout << ss[i].get_listen_host() << ": " << ss[i].get_listen_port() << endl;
            sockaddr_in sockaddr = ss[i].get_sock_ader();
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            guard(sockfd, "Failed to create socket.");
            setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
            int flags = guard(fcntl(sockfd, F_GETFL), "could not get flags on TCP listening socket");
            guard(fcntl(sockfd, F_SETFL, O_NONBLOCK | flags), "could not set TCP listening socket to be non-blocking");
            guard((int)bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)),"Failed to bind.");
            guard(listen(sockfd, 255), "Failed to listen on socket.");
            fd.fd = sockfd;
            fd.events = POLLIN;
            fds.push_back(fd);
        }
    }
}

void addclienttoserver(server &ss,std::vector<client>& clients,std::vector<pollfd> &fds, int fd, std::vector<server> &serv)
{
    int    set = 1;
    std::string str = "client";
    client stmp;
    size_t addrlen = sizeof(ss.get_sock_ader());
    pollfd fds1;
    sockaddr_in sockaddr = ss.get_sock_ader();
                
    int connection = accept(fd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
    guard(connection, "Failed to grab connection.");
    setsockopt(connection, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
    fds1.fd = connection;
    fds1.events = POLLIN;
    fds.push_back(fds1);
    stmp.set_fd(connection);
    stmp.set_serv(ss);
    for (int i = 0; i < int(serv.size()); i++)
    {
        if (serv[i].get_listen_port()== ss.get_listen_port() && serv[i].get_listen_host()== ss.get_listen_host())
            stmp.set_servers(serv[i]);
    }
    clients.push_back(stmp);
}




void delete_client(std::vector<client>& clients, int i, int d ,std::vector<pollfd> &fds)
{
    int k =0;

    for (std::vector<client>::iterator it = clients.begin(); it != clients.end();k++, it++)
        if (k == i)
        {
            clients.erase(it);
            break ;
        }
    k = 0;
    for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); it++ , k++)
        if (k == d)
        {
            close ((*it).fd);
            fds.erase(it);
            break ;
        }
}

void serv(std::vector<pollfd>& fds, std::vector<client>& clients, std::vector<server>& ss)
{
    while (1)
    {
        guard(poll(fds.data(), fds.size(), -1), "serv poll() failed");
        int j = 0;
        for (int i = 0; i < int(ss.size()); i++)
        {
            if (is_binded_server(ss, i))
                j++;
        }
        for (int i = j, j = 0; j < int(clients.size()); i++, j++)
        {
            if (fds[i].revents != 0 && fds[i].revents & POLLIN)
            {

                Requeststup(fds[i].fd, clients[j], fds[i]);
            }
            else if (fds[i].revents != 0 && fds[i].revents & POLLOUT)
            {

                clients[j].respond(fds[i]);
                if (fds[i].events == POLLIN)
                {
                    delete(clients[j].res);
                    clients[j].res = NULL;
                    if (!(clients[j].req.get_headrs().find("Connection")->second == "keep-alive"))
                    {
                        delete_client(clients, j--, i--, fds);
                        continue ;
                    }
                    else
                        clients[j].req.clear();
                }
            }
            else if (fds[i].revents != 0 && fds[i].revents & POLLHUP)
            {
                delete_client(clients, j--, i--, fds);
                continue;
            }
            if (fds[i].events & POLLIN)
            {
                clients[j].set_event(fds[i].events);
                fds[i].events = POLLHUP;
            }
            else if (fds[i].events & POLLHUP)
                fds[i].events = clients[j].get_event();
        }
        j = 0;
        for (int i = 0; i < int(ss.size()); i++)
        {
            if (is_binded_server(ss, i) && fds[j].revents != 0 && (fds[j].revents & POLLIN))
                addclienttoserver(ss[i], clients, fds, fds[j++].fd, ss);
            else if (is_binded_server(ss, i))
                j++;
        }

    }
}

int main(int argc, char **argv)
{
    std::vector<client> clients;
    std::vector<pollfd> fds;
    parse_config root;

    root.before_start_parsing(argc, argv);
    std::vector<server> ss(root.get_server_vect());
    servers(ss, fds);

    serv(fds, clients, ss);
    return (0);
}
