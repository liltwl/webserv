 #pragma once

#include "request.hpp"
#include "./Response.hpp"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;



class Statuscode{
    private:
        map<string, string> codes;
        string errorpage;
        int error;
    public:
        Statuscode();
        string get_code(int code);
        void set_error(vector<vector<string> > pages, int code, int size);
        string renderpage(string filename,int code);
        string get_errorhtml(int code, string uploadfile);

};
