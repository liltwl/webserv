#include "webserv.hpp"
using namespace std;
char *getDate()
{
    time_t now = time(0);
    char* dt = ctime(&now);
    return dt;
}
class Header
{
    private:
        string date;
        string Server;
        int ContentLength;
        string ContentType;  
    public:
    Header(string _ContentType, int ContentLength)
    {
        this->ContentType = _ContentType;
        this->ContentLength = ContentLength;
        this->date = getDate();
        this->Server = "Server: webserv";
    }
    string get_ContemnentType()
    {
        return this->ContentType;
    }
    int get_contentLength()
    {
        return this->ContentLength;
    }   
};
class Statuscode{
    private:
        map<string, string> codes;
    public:
        Statuscode()
        {
            codes["200"] = "OK";
            codes["404"] = "Not Found";
            codes["500"] = "Internal Server Error";
            codes["400"] = "Bad Request";
            codes["413"] = "Request Entity Too Large";
            codes["405"] = "Method Not Allowed";
            codes["403"] = "Forbidden";
            codes["204"] = "No Content";
            codes["201"] = "Created";
        }
        string get_code(int code)
        {
            return ("HTTP/1.1 " + to_string(code) + " " + codes[to_string(code)]);
        }
};

class Response
{
    private:
        string firstline;
        string methode;
        Header &header;
        string body;
        int    status;
        string path;
    public:
    Request &req;
    Response();
/*    Response( Request &_req, Header &_header) : req(_req) , header(_header)
    {
        this->req = _req;
        this->firstline = "HTTP/1.1 200 OK\r\n";
        this->methode = _req.rqmethod;
        this->path = _req.location;
        this->status = 200;
        this->body = "";
    };*/


};