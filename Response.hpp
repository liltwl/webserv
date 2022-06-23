 #pragma once

#include "request.hpp"
#include "Header.hpp"
#include "Client.hpp"
#include "Statucode.hpp"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

class Header;
class Statucode;
class Response
{
    private:
        string methode;
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

        Statucode *codI;

    public:
            int handleGet();
        int handleDelete();
            int handlePost();
        void findlocation();
    Response( Request &_req, server &_serv);
    long get_size();
    int handleredirection();
    void renderindex(string path);
    string responde();
    int findindexfile(string paths);

int handlerequest(Request &req, server &serv);
};















