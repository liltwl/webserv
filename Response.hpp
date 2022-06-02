#include "webserv.hpp"
using namespace std;
int getDate()
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
    //const Request &req;
    Response(const Request &_req, string &contentype , int contentlength)// : req(_req)
    {
        this->header = Header(contentype, contentlength);
        this->path = _req.location;
        this->methode = _req.rqmethod;
        this->status = 
        body = "";
    };


};