 #pragma once

//#include "request.hpp"
#include "./Response.hpp"


using namespace std;



class Statucode
{
    private:
        map<string, string> codes;
        string errorpage;
        int error;
    public:
        Statucode();
        ~Statucode(){}
        string get_code(int code);
        void set_error(vector<vector<string> > pages, int code, int size);
        string renderpage(string filename,int code);
        string get_errorhtml(int code, string uploadfile);

};

std::string get_type(string location, int mode);