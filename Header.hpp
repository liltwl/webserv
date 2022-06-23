 #pragma once

#include "request.hpp"

#include "./Response.hpp"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;




class Header
{
    private:
        
        string Server;
        string firstline;
        string date;
        map<string,string> headers;
    public:
        
    Header(server &serv);
    
   
    void set_firstline(string firstline);
    string get_Header();
    void setHeader(string key, string value);

};

