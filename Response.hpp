#include "webserv.hpp"
#include <dirent.h>
using namespace std;
string getDate()
{
    time_t now = time(0);
    string dt = ctime(&now);
    return dt;
}
int handlerequest(Request &req, server &serv)
{
    //cout << endl << "request location :" << req.location;
    if(req.body.size() != atoi(req.headers["Content-Length"].c_str()))
    {
        return 413;
    }
    if(serv.location.count(req.location) == 0 && req.location != "/")
    {
        return 404;
    }
    if(
        (serv.location.find(req.location) != serv.location.end() && find(serv.location[req.location].methods.begin(),serv.location[req.location].methods.end(),req.rqmethod) == serv.location[req.location].methods.end())
        || (req.location  == "/" && find(serv.methods.begin(),serv.methods.end(),req.rqmethod) == serv.methods.end())
    )
    {
 
        return 405;
    }
    return(200);
}

class Header
{
    private:
        
        string Server;
        int ContentLength;
        string ContentType;  
    public:
    string date;
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
            codes["500"] = "Internal Server Error";//error with cgi
            codes["400"] = "Bad Request";
            codes["413"] = "Request Entity Too Large";
            codes["405"] = "Method Not Allowed";
            codes["204"] = "No Content";
            codes["201"] = "Created";
        }
        string get_code(int code)
        {
            return ("HTTP/1.1 " + to_string(code) + " " + codes[to_string(code)]);
        }

};
string renderpage(string filename)
{

    ifstream f(filename); //taking file as inputstream
   string str;
   if(f) {
      ostringstream ss;
      ss << f.rdbuf(); // reading data
      str = ss.str();
   }
  
    return str;
    
}

string renderror(int error)
{
    if (error == 404)
    {
        
        return(renderpage("./www/html/error/404.html"));
    }
    if (error == 405)
    {
        return(renderpage("./www/html/error/405.html"));
    }
    if (error == 413)
    {
        return(renderpage("./www/html/error/413.html"));
    }
    else
        return("undefined status");

}
string handleGet(Request &req, server &serv, int &status)
{
    if(strcmp(req.location.c_str(), "/") == 0)
    {
        DIR *dir;
        if(opendir(req.location.c_str()) == NULL || status == 404)
        {
            status = 404;
            return(renderror(404));
        }
    }
    return(renderpage("./www/html/test.html"));
}
string generatebody(Request &req, server &serv, int *status)
{
    string body;
    if(req.rqmethod == "GET" )
    {
        return(handleGet(req, serv, *status));
    }
    else 
        return("methode not yet implemented");
}
class Response
{
    private:
        Statuscode code;
        string firstline;
        string methode;
        Header header;
        string 
        body;
        int    status;
        string path;
    public:
    Request &req;
    server &serv;
    Response();
    Response( Request &_req, server &_serv) : req(_req) , serv(_serv) , header(Header("text/html", 0))
    {
        this->req = _req;
        this->status = handlerequest(req, serv);
        /*if(status !=  200)
        {
            this->body = renderror(this->status);
        }
        else*/
        this->body =renderror(this->status);
        //this->body = generatebody(req, serv, &status);
    
        this->firstline = code.get_code(status);
        this->methode = _req.rqmethod;
        
        this->path = _req.location;
        //this->header = Header("Text", 0);
        //this->body = ""; //this->path ;
    };
    std::string respond()
    {
        string response = firstline + "\r\n";
        response += "Date:  " + header.date + " \r\n";
        response += "Server: webserv\r\n";
        
    
        response += "Content-Length: " + to_string(body.size()) + "\r\n";
        response += "Content-Type: " + this->header.get_ContemnentType() + "\r\n";
        response += "\r\n";
        response += this->body;
        //cout << endl << endl << response << endl;
        return response;
    };
    int get_response_size()
    {
        return (this->respond().size());
    }
    void clear()
    {
        
    }


};
