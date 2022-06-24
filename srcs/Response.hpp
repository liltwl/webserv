 #pragma once

#include "request.hpp"
#include "Header.hpp"
#include "Client.hpp"
#include "Statucode.hpp"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "Cgi.hpp"
using namespace std;

class Header;
class Statucode;
class Response
{
    private:
        string methode;
        string full_cgi_res;
        string cgi_body;
        bool    indexstats;
        string  index_path;
        long size;
        string rlocation;
        string body;
        string path;
        Request&req;
        location reqloc;
        server &serv;
        string uploadfile;
        //bool header_sent;
        Header *header;
        int status;
        //long long wrotebytes;
        long long filesize;
        bool php_cgi;
        string cgipath;
        int   is_chunked;
        int    fd_file;
        Statucode *codI;

    public:

    int handleGet();
    int handleDelete();
    int handlePost();
    void findlocation();
    Response( Request &_req, server &_serv);
    long get_size();
    int get_is_chunked()
    {
        return is_chunked;
    }
    int handleredirection();
    void renderindex(string path);
    string responde();
    int findindexfile(string paths);
    int handle_cgi();
    int iscgi(Request &req, server &serv);
    int handlerequest(Request &req, server &serv);

};















