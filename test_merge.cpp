#include "request.hpp"
#include "Response.hpp"

using namespace std;
# define delim_size 2000

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
            break;
        i++;
    }
    line.erase(0,i);
}

int getnextline(int fd, string &line)
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

int getlenline(int fd, string &line, int len)
{
    char delim[delim_size + 1];
    char nl[2];
    int enf = 0, k =0;
    int i = 0;

    while (i < len && (enf = recv(fd, &delim, i +delim_size< len? delim_size : (len - i), 0))> 0)
    {
        i += enf;
        line.append(delim, enf);
    }
    if ((enf = recv(fd, &nl, 2, 0)) > 0 && (nl[0] ==  13 && nl[1] == '\n'))
        return i;
    cout << enf << " == " << len << endl;
    return (enf == -1?i:-1);
}

void headerpars(int fd, Request& ss, client& clients)
{
    string line;
    vector<string> str;
    int i, j;

    for(i = 0;(j = getnextline(fd, line))> 0; i++)
    {
        if(line.size() == 0) return ;
            str = split(line, ' ');
        if (i == 0 && str.size() > 2)
        {
            ss.setrqmethod(str[0]);
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
        string sr_name = ss.get_headrs().find("Host")->second;
        vector<server>& serv = clients.serv;
        for (int i = 0; i < serv.size(); i++)
            for (int j = 0; j < serv[i].get_name_size(); j++)
                if (sr_name == serv[i].get_name(j))
                {
                    clients.set_serv(serv[i]);
                    break ;
                }
    }
}

void chunked_body_pars(int fd, Request& ss, pollfd &fds)
{
    string line;
    vector<string> str;
    int i, j;
    int req_len = ss.get_headrs().count("Content-Length")? stoi( ss.get_headrs().find("Content-Length")->second) : 0; // update
    int body_len = 0;
    int k =1;
    int len = 0;

    if (ss.get_body_len() <= req_len)
    {
        string stmp ;
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
    cout <<ss.get_body_len() <<" == " << req_len << endl;
}

void body_pars(int fd, Request& ss, pollfd &fds)
{
    string line;

    int len = stoi(ss.get_headrs().find("Content-Length")->second);
    ss.setbody_limit(getlenline(fd, ss.get_body(), len));

    cout << ss.get_body_len() << endl;
    fds.events = POLLOUT;
}

void Requeststup(int fd, client& clients, pollfd &fds)
{
    string line;
    vector<string> str;
    Request& ss = clients.req;
    int i, j;
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

int guard(int n, string err)
{
    if (n < 0)
    {
        cout << (err) << endl; 
        exit(1); 
    }
    return n;
}





// /**********************/


int is_binded_server(vector<server> ss, int i)
{
    server serv = ss[i];

    for (int j = 0; j < i; j++)
        if (serv.get_listen_port()== ss[j].get_listen_port() && serv.get_listen_host()== ss[j].get_listen_host())
            return 0;
    return 1;
}

void servers(vector<server> ss,vector<pollfd> &fds)
{
    pollfd fd;

    for (int i = 0; i < ss.size(); i++)
    {
        if (is_binded_server(ss, i))
        {
            sockaddr_in sockaddr = ss[i].get_sock_ader();
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            guard(sockfd, "Failed to create socket.");
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

void addclienttoserver(server &ss,vector<client>& clients,vector<pollfd> &fds, int fd,std::vector<server> &serv)
{
    string str = "client";
    client stmp;
    size_t addrlen = sizeof(ss.get_sock_ader());
    pollfd fds1;
    sockaddr_in sockaddr = ss.get_sock_ader();
                
    int connection = accept(fd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
    guard(connection, "Failed to grab connection.");
    fds1.fd = connection;
    fds1.events = POLLIN;
    fds.push_back(fds1);
    stmp.set_fd(connection);
    stmp.set_serv(ss);
    for (int i = 0; i < serv.size(); i++)
    {
        if (serv[i].get_listen_port()== ss.get_listen_port() && serv[i].get_listen_host()== ss.get_listen_host())
            stmp.set_servers(serv[i]);
    }
    clients.push_back(stmp);
    cout << "client 200 ok" << endl;
}




void delete_client(vector<client>& clients, int i, int d ,vector<pollfd> &fds)
{
    int k =0;

    for (vector<client>::iterator it = clients.begin(); it != clients.end();k++, it++)
        if (k == i)
        {
            cout << "delete client num :"<< i <<endl;
            clients.erase(it);
            break ;
        }
    k = 0;
    for (vector<pollfd>::iterator it = fds.begin(); it != fds.end(); it++ , k++)
        if (k == d)
        {
            close ((*it).fd);
            fds.erase(it);
            break ;
        }
}

void serv(vector<pollfd>& fds, vector<client>& clients, vector<server>& ss)
{
    while (1)
    {
        guard(poll(fds.data(), fds.size(), -1), "serv poll() failed");
        int j = 0;
        for (int i = 0; i < ss.size(); i++)
        {
            if (is_binded_server(ss, i) && (fds[j].revents & POLLIN))
                addclienttoserver(ss[i], clients, fds, fds[j++].fd, ss);
            else if (is_binded_server(ss, i))
                j++;
        }
        for (int i = j, j = 0; j < clients.size(); i++, j++)
        {
            if (fds[i].revents != 0 && fds[i].revents & POLLIN)
                Requeststup(fds[i].fd, clients[j], fds[i]);
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
                fds[i].events = POLLHUP;
            else if (fds[i].events & POLLHUP)
                fds[i].events = POLLIN;
        }
        cout << "__________the_end____________" << endl;
    }
}

int main(int argc, char **argv)
{
    vector<client> clients;
    vector<pollfd> fds;
    parse_config root;

    root.before_start_parsing(argc, argv);
    vector<server> ss(root.get_server_vect());
    servers(ss, fds);
    serv(fds, clients, ss);
    return (0);
}
