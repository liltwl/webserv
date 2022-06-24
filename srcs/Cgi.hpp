#pragma once
#include "Response.hpp"

using namespace std;

class Cgi
{
    private:
        std::string resp;
        std::string respbody;
        std::map<std::string, std::string>	headers;
    public:
        Cgi();
        ~Cgi();
        void                    set_resp_path(std::string full_path);
        void                    set_body_path(std::string body_path);
        string                  get_resp_path();
        string                  get_body_path();
        map<string, string>     get_headers();
        void                    set_headers(std::string &line);



        string sethttpheader(const char *key);

        void    set_env(Request &req,string script);
        
        int     execute(string input, string script, string cgipath,Request &req);
        
};
