#include "webserv.hpp"
#include "Response.hpp"

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

# define delim_size 200

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
    if ((enf += recv(fd, &nl, 2, 0)) > 0 && (nl[0] ==  13 && nl[1] == '\n'))
        return i;
    return (-1);
}

void headerpars(int fd, Request& ss)
{
    string line;
    vector<string> str;
    int i, j;

    for(i = 0;(j = getnextline(fd, line))> 0; i++)
    {
        if(line.size() == 0) return ;
        str = split(line, ' ');
        if (i == 0)
        {
            ss.setrqmethod(str[0]);
            ss.setlocation(str[1]);
            ss.setversion(str[2]);
        }
        else
        {
            str = split(line, ':');
            ss.addheaders(str[0],str[1].substr(1, str[1].size()));
        }
        line.clear();
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
        cout << stmp << endl;

        if (isxdigit(stmp[0])) 
            len = stol(stmp, nullptr, 16);
        stmp.clear();
        if (len > 0)
        {
            body_len += getlenline(fd, stmp, len);
            cout << len  << " :chunk length : "<< body_len << endl;
            ss.addbody(stmp, body_len);
        }
        else if (len == 0)
        {
            cout << len << " :final byte"<< endl;
            if (getlenline(fd, stmp, 1) == 1)
                cout << "content length > " << endl;
            fds.events = POLLOUT;
        }
    }
    else
    {
        cout << "error" << endl;
        fds.events = POLLOUT;
    }
    cout <<ss.get_body_len() <<" == " << req_len << endl;
}

void body_pars(int fd, Request& ss, pollfd &fds)
{
    string line;

    int len = stoi(ss.get_headrs().find("Content-Length")->second);
    getlenline(fd, ss.body, len);
    if (getlenline(fd, line, 1) > 0)
        cout << "Content-Length < content len " << endl;
    fds.events = POLLOUT;
}

void Requeststup(int fd, Request& ss, pollfd &fds)
{
    string line;
    vector<string> str;
    int i, j;

    cout << "header :" << ss.empty_header() << endl;
    if (ss.empty_header())
        headerpars(fd, ss);
    else if(ss.get_headrs().count("Transfer-Encoding"))
        chunked_body_pars(fd, ss, fds);
    else if(ss.get_headrs().count("Content-Length"))
        body_pars(fd, ss, fds);
    if (!ss.get_headrs().count("Content-Length"))
        fds.events = POLLOUT;
    cout << "||||||||||||||||||||||||||||||||||||||||||" << endl;
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





/**********************/




void servers(vector<server> &ss,vector<pollfd> &fds)
{
    sockaddr_in sockaddr;
    int yes = 1;
    pollfd fd;

    for (int i = 0; i < ss.size(); i++)
    {
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_addr.s_addr = inet_addr(ss[i].addr.c_str());
        sockaddr.sin_port = htons(ss[i].port);
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        guard((ss[i].sockfd = sockfd), "Failed to create socket.");
        int flags = guard(fcntl(sockfd, F_GETFL), "could not get flags on TCP listening socket");
        guard(fcntl(sockfd, F_SETFL, O_NONBLOCK | flags), "could not set TCP listening socket to be non-blocking");
        guard((int)bind(ss[i].sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)),"Failed to bind.");
        guard(listen(sockfd, 255), "Failed to listen on socket.");
        /************/
        ss[i].sockaddr = sockaddr;
        /*************/
        fd.fd = ss[i].sockfd;
        fd.events = POLLIN;
        fds.push_back(fd);
    }
}

void addclienttoserver(server &ss,vector<client>& clients,vector<pollfd> &fds)
{
    string str = "client";
    client stmp;
    size_t addrlen = sizeof(ss.sockaddr);
    pollfd fds1;
                
    int connection = accept(ss.sockfd, (struct sockaddr*)&ss.sockaddr, (socklen_t*)&addrlen);
    guard(connection, "Failed to grab connection.");
    fds1.fd = connection;
    fds1.events = POLLIN;
    fds.push_back(fds1);
    stmp.set_fd(connection);
    stmp.set_serv(ss);
    cout << ss.get_name(0) <<"  thse seerver is"<< endl;
    clients.push_back(stmp);
    cout << "client 200 ok" << endl;
}




void delete_client(vector<client>& clients, int i, int d ,vector<pollfd> &fds)
{
    int k =0;

    for (vector<client>::iterator it = clients.begin(); it != clients.end();k++, it++)
        if (k == i)
        {
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





int main(int argc, char **argv)
{
    string line;
    vector<server> ss;
    vector<client> clients;
    Request req;
    int maxfd=0;
    vector<pollfd> fds;

      /********************************/
    serversetup(ss, argv[1]);
      /********************************/
    cout << ss[0].name << " " << ss[0].addr << "::"<<  ss[0].port << ":" << ss.size() << " : "<< ss[0].location.begin()->first  << endl;
    map<int ,string> trr =ss[0].geterrorpages();
    /***********************/
    servers(ss, fds);

    while (1)
    {
        guard(poll(fds.data(), fds.size(), -1), "serv poll() failed");
    
        for (int i = ss.size(), j = 0; i < clients.size() + ss.size(); i++, j++)
        {
            if (fds[i].revents != 0 && fds[i].revents & POLLIN)
            {
                cout << j << ": index :";
                Requeststup(fds[i].fd, clients[j].req, fds[i]);
                if (clients[j].req.empty())
                    continue;
                cout << clients[j].req.rqmethod << " " << clients[j].req.location << " " << clients[j].req.vrs << endl;  //hadi dyal rqst lbghiti tpintehom
                for (map<string, string>::iterator it = clients[j].req.headers.begin(); it != clients[j].req.headers.end(); it++)
                {
                    cout << it->first << ": " << it->second <<endl;
                }
                // cout << "body :" << clients[j].req.body << endl;
                cout << "*" << clients[j].req.headers["Connection"] << "*----" << endl;
            }
            else if (fds[i].revents != 0 && fds[i].revents & POLLOUT)
            {
  
                                /*********Response*********/

                clients[j].respond(fds[i]);
                                /******************/

                if (fds[i].events == POLLIN)
                {
                    delete(clients[j].res);
                    clients[j].res = NULL;
                    clients[j].req.clear();
                }
                if (!(clients[j].req.headers.count("Connection") && clients[j].req.headers.at("Connection") == "keep-alive"))
                    delete_client(clients, j--, i, fds);
            }
            else if (fds[i].revents != 0 && fds[i].revents & POLLHUP)
            {
                delete_client(clients, j--, i, fds);
                continue;
            }
            if (fds[i].events == POLLIN) 
                fds[i].events = POLLHUP;
            else if (fds[i].events == POLLHUP)
                fds[i].events = POLLIN;
        }
        for (int i = 0; i < ss.size(); i++)
            if (fds[i].revents != 0 && (fds[i].revents & POLLIN))
                addclienttoserver(ss[i], clients, fds);
        cout<< "__________the_end____________" << endl;
    }
    //}

}
//}