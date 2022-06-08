#include "webserv.hpp"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;
string getDate()
{
    time_t now = time(0);
    string dt = ctime(&now);
    return dt;
}
int finddir(string path) // this function ruturns 0 i it's not a file nor a directory 1 uif it's a file and 2 if it's dir
{
    struct stat path_stat;
    stat(path.c_str(), &path_stat);
    if(S_ISREG(path_stat.st_mode))
    {
        return(1);
    }
    else if(S_ISDIR(path_stat.st_mode))
    {
        return(2);
    }
    else
        return 0;
}

string findlocation(Request &req, server &serv)
{
    string path;
    int matchsize = 0;
    for(map<string,loc>::iterator it = serv.location.begin(); it !=  serv.location.end(); it++)
    {
        if(strncmp(req.location.c_str(),it->first.c_str(), it->first.size()) == 0 && matchsize < it->first.size()) // chack if the request location exist in the config file locations and icrement i[0]
        {
            path = it->first ;
            matchsize = it->first.size();

//            if(find(it->second.methods.begin(), it->second.methods.end(),req.rqmethod) !=  it->second.methods.end() )// if he found the location it chacke if the method is allowed in the specified location and acrement i[1]
  //              i[1]++;
            //return(it->first);
        }
    }
    if(matchsize > 0)
        return(path);

    return (serv.root);


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
            codes["403"] = "Forbidden";
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
            status = 498;
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
        string rlocation;
        Header header;
        string body;
        int    status;
        string path;
    public:
    Request &req;
    server &serv;
    Response();
    Response( Request &_req, server &_serv) : req(_req) , serv(_serv) , header(Header("text/html", 0))
    {
        this->req = _req;
        this->status = this->handlerequest(req, serv);
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
    int handlerequest(Request &req, server &serv);

    int handleGet();
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



};
bool checkmethod(server &serv,string location ,string Method)
{
    if(location == serv.root)
    {
        if(find(serv.methods.begin(), serv.methods.end(),Method) !=  serv.methods.end()) 
        {
                return true;
        }
        else 
                return false;  
    }// if he found the location it chacke if the method is allowed in the specified location and acrement i[1]
    else if(find(serv.location[location].methods.begin(),serv.location[location].methods.end(),Method) != serv.location[location].methods.end())
        return true;
    else
        return false;
}

int Response::handlerequest(Request &req, server &serv)
{
    //string reallocation
    /*if(req.body.size() != atoi(req.headers["Content-Length"].c_str()))
    {
        return 413;
    }*/
    //int i[2] = {0,0};

    
    //std::vector<string>::iterator l;
    /*for(map<string,loc>::iterator it = serv.location.begin(); it !=  serv.location.end(); it++)
    {
        if(strncmp(req.location.c_str(),it->first.c_str(), it->first.size()) == 0) // chack if the request location exist in the config file locations and icrement i[0]
        {
            if(find(it->second.methods.begin(), it->second.methods.end(),req.rqmethod) !=  it->second.methods.end() )// if he found the location it chacke if the method is allowed in the specified location and acrement i[1]
                i[1]++;
            i[0]++;
        }
    }
    if(i[0] == 0 && req.location != "/")
    {
        cout << "l9lwiiiiiiiiiiiiuiiuiuiuiuiuiuiuiuiuiupfg[iuoptiytjyeiostyjesioyjeiojyes" << endl;
        return 404;
    }
    if(req.location == "/")
    {
        if(find(serv.methods.begin(), serv.methods.end(),req.rqmethod) !=  serv.methods.end())
            i[1]++;
    }
    if(i[1] == 0)
    {
        //cout << "l9lwiiiiiiiiiiiiuiiuiuiuiuiuiuiuiuiuiupfg[iuoptiytjyeiostyjesioyjeiojyes" << endl;
        return 405;
    }*/
    this->rlocation = findlocation(req,serv);
    if(checkmethod(serv,this->rlocation,req.rqmethod) == false)
    {
        return 405;
    }

    cout << "this>rlocation ::::::::::::::::" << this->rlocation<< endl;
    if(req.rqmethod == "GET")
    {
        return handleGet();
    }
    return(200);
};
int Response::handleGet()
{
    //string path = this->rlocation + this->req.location; 
    if(finddir(req.location) == 0)
    {
        return(404);
    }
    if(finddir(path) == 2)
    {
        if(serv.autoindex == 0)
        {
            return 403;
        }
    }

    //cout << "ur mthode is GET" << endl << endl << endl;
    //cout << this->req.location  << "<< location is"<< endl;
    return(205);
}
