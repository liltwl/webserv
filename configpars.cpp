

#include "webserv.hpp"



vector<string> allow_methodstup(vector<string> str)
{
    vector<string> val;
    for (int i = 1; i < str.size(); i++)
    {
        val.push_back(split(str[i], ';')[0]);
    }

    return val;
}


string location_pars(server *ss, vector<server> &tmp, std::ifstream &file, vector<string> str)
{
    loc tmp1;
    string stmp = str[1];
    string line;
    int k = 0;
    cout << "++++++++++++++loca++++++++++++++" << endl;
    for (int j = 0 ; (k = line.find("}")) == -1 && !file.eof(); j++)
    {
        getline(file, line, '\n');
        ft_spaceskip(line);
        str = split(line, ' ');
        if (!str[0].compare("root")&& str.size())
            tmp1.root = split(str[1], ';')[0];
        else if (!str[0].compare("allow_methods")&& str.size())
            tmp1.methods = allow_methodstup(str);
        cout << line << ": location :" << endl;
    }
    ss->location[stmp] = tmp1;
    cout << line.substr(k + 1, line.size()) << ": dlm" << endl;
    return (line.substr(k + 1, line.size()));
}

int server_pars(vector<server> &tmp,std::ifstream &file, server *ss)
{
    vector<string> str;
    string line;
    int k = 0;

    cout << "-----------serv-----------" << endl;
    for (int i = 0; (k = line.find("}")) == -1 && !file.eof(); i++)
    {
        line.clear();
        str.clear();
        getline(file, line, '\n');
        ft_spaceskip(line);
        str = split(line, ' ');
        if (!str[0].compare("server_name")&& str.size())
            ss->setname(split(str[1], ';')[0]);
        else if (!str[0].compare("listen")&& str.size())
            ss->setadd(split(str[1], ':')[0],stoi(split(str[1], ':')[1]));
        else if (!str[0].compare("root")&& str.size())
            ss->setroot(split(str[1], ';')[0]);
        else if (!str[0].compare("allow_methods") && str.size())
            ss->setmethods(allow_methodstup(str));
        else if (!str[0].compare("autoindex")&& str.size())
            ss->setautoindex(split(str[1], ';')[0] == "on" ? 1 : 0);
        else if (!str[0].compare("client_body_limit")&& str.size())
            ss->setbody_limit(stoi(split(str[1], ';')[0]));
        else if (!str[0].compare("location")&& str.size())
            line = location_pars(ss, tmp, file, str);
        else if (!str.empty())
            ss->setothers(line);
    }
    return k;
}


void serversetup(vector<server> &tmp,std::ifstream &file)
{
    vector<string> str;
    server *ss = NULL;
    string line;

    for (int i = 0; !file.eof(); i++)
    {
        line.clear();
        str.clear();
        getline(file, line, '\n');
        ft_spaceskip(line);
        str = split(line, ' ');
        ss = new server();
        cout << str[0] << ":ddd" <<endl;
        if (!str[0].compare("server"))
            guard(server_pars(tmp, file, ss), "error: bad config file.");
        if (ss->get_name().size())
            tmp.insert(tmp.begin(), *ss);
        delete ss;
    }
    cout << "|||||||||||||||||||||||*********||||||||||||||||||||||||||||" << endl;
}

