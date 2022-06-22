 #pragma once

#include "request.hpp"
#include "Header.hpp"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

class Response
{
    private:
        string methode;
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
        int handleGet();
        int handleDelete();
        Statuscode code;
        int handlePost();
        void findlocation();
    public:
    
    Response( Request &_req, server &_serv) : req(_req) , serv(_serv)
    {
        this->filesize = 0;
        //this->header_sent = false;
        
        this->header = new Header(serv);
      
        this->status = this->handlerequest(req,serv);
        
        
        this->methode = req.get_method();
        this->path = "";

    }
    long get_size(){return this->size;};
    void renderindex(string path);
    string responde();

int handlerequest(Request &req, server &serv);
};

void Response::findlocation()
{
    
    string path;
    int matchsize = 0;
    cout << "ttttt" << endl;
    //cout <<  this->serv.get_location(i).get_name() << endl;
    cout <<  serv.get_name(0) << endl;
    for(int i = 0; i < this->serv.get_location_size();i++)
    {
        cout <<  serv.get_name(0) << endl;
        if(strncmp(this->req.get_location().c_str(),this->serv.get_location(i).get_name().c_str(),this->serv.get_location(i).get_name().size()) == 0 && matchsize < this->serv.get_location(i).get_name().size()) // chack if the request location exist in the config file locations and icrement i[0]
        {
            path = this->serv.get_location(i).get_name() ;
           // cout <<
            //cout <<  this->serv.get_location(i).get_name() << endl;
            matchsize = this->serv.get_location(i).get_name().size();
            this->reqloc = this->serv.get_location(i);
        }
    }
    cout << "aaaaa" << endl;
    if(matchsize > 0)
    {
        this->rlocation = path;
    }
    else
        this->rlocation = this->serv.get_root();
   

}
bool checkmethod(server &serv, string location , string Method)
{
    std::vector<std::string> methods = serv.get_allowed_methods();
    if(location == serv.get_root())
    {
        if(find(methods.begin(),methods.end(),Method) != methods.end())
            return true;
        else
            return false;
    }
    else
    {
        for(int i = 0 ; i <= serv.get_location_size(); i++)
        {
            if(serv.get_location(i).get_name() == location)
            {
                methods = serv.get_location(i).get_methods();
                if(find(methods.begin(),methods.end(),Method) != methods.end())
                    return true;
            }
        }
        return(false);
    }
};
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
int Response::handlerequest(Request &req, server &serv)
{
    cout << "respond dsdsd"<< serv.get_name(0) << endl;
    this->findlocation();
    cout << "respond ZZZZZ" << endl;
    if(checkmethod(serv,this->rlocation,req.get_method()) == false)
    {
        return 405;
    }
    if(req.get_method() == "GET")
    {
        return this->handleGet();
    }
    else if(req.get_method() == "POST")
    {

        return(this->handlePost());
    }
    else if(req.get_method() == "DELETE")
    {
        return(this->handleDelete());
        //cout << "the request is delete" << endl << endl<<endl;
    }
    else
    {
        return 501;
    }
    return(200);
};
long long file_size(string path)
{
    /*ifstream in_file(path.c_str(), ifstream::binary);
    in_file.seekg(0, ios::end);
    long file_size = in_file.tellg();
    return file_size;*/
    
    struct stat stat_buf;
    int rc = stat(path.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
    /*
    FILE *fp = fopen(path.c_str(),"r");
    fseek(fp,0L,SEEK_END);
    long long size = ftell(fp);
    return(size);*/
}
void Response::renderindex(string paths)
{
    string str;
    DIR *dr;
    struct dirent *e;
    dr = opendir(path.c_str());
    this->header->setHeader("Content-Type","Text/html");
    this->body = "<html>\n<head>\n<body>\n<table>\n";
    if(dr)
    {
        
        while((e = readdir(dr)) != false)
        {
            if(e->d_name != string(".") && e->d_name != string(".."))
        {
            this->body += string("<br>") ;
            if(this->req.get_location().back() == '/')
            {
                this->body += string("<a href=\"") + this->req.get_location() + e->d_name + string("\">")+ e->d_name + string("</a>"); 
            }
            else
            {
                    this->body += string("<a href=\"") + this->req.get_location() + string("/") + e->d_name + string("\">")+ e->d_name + string("</a>"); 

            }
            
        }
        }   
    }
    this->body += "</table>\n</body>\n</head>\n</html>\n";
    closedir(dr);
    //return(this->body);
}

int Response::handleGet()
{
    string paths;
    if(this->rlocation != this->serv.get_root())
    {
        paths = this->reqloc.get_root() + this->req.get_location().substr(this->rlocation.size());
    }
    else
    {
        paths = this->serv.get_root() +  this->req.get_location();
    }
    if(finddir(paths) == 0)
    {
        return(404);
    }
    if(finddir(paths) == 1)
    {
        this->header->setHeader("Content-Type",get_type(this->req.get_location(), 1));
        this->filesize = file_size(path);
        this->header->setHeader("Content-Length",to_string(this->filesize));
        this->path = paths;
        return(200);
    }
    if(finddir(paths) == 2)
    {
        if(this->serv.get_autoindex() == true)
        {
            if(this->req.get_location().back() != '/')
            {
                this->header->setHeader("Location",this->req.get_location() + "/");
                return(301);
            }
            this->renderindex(paths);
            return(200);
        }
        else
            return(403);
    }
    return (205);
    
};
int Response::handlePost()
{
    string file_name;
    if(this->reqloc.get_upload_path() != "")
    {
       file_name = this->reqloc.get_upload_path() +  to_string(rand()) + "." + get_type(this->req.get_headrs()["Content-Type"], 2);
    }
    else
    {
         file_name = this->serv.get_upload_path() + to_string(rand()) + "." + get_type(this->req.get_headrs()["Content-Type"], 2);
    }
    ofstream f(file_name.c_str());
    if(f)
    {
        f << this->req.get_body();

    }
    this->uploadfile = file_name;
    return(201);
};
int Response::handleDelete()
{
    string paths;
    if(this->rlocation != this->serv.get_root())
    {
        paths = this->reqloc.get_root() + this->req.get_location().substr(this->rlocation.size());
    }
    else
    {
        paths = this->serv.get_root() +  this->req.get_location();
    }
    if(finddir(paths) == 0)
    {
        return(404);
    }
    if(finddir(path) == 1)
    {
        int status;
        status = remove(path.c_str());
        if(status == 0)
            return 204;
        else
            if(access(path.c_str(),W_OK) == 0)
                return 500;
            else
                return (403);     
    }
    if(finddir(paths) == 2)
    {
        if(this->req.get_location().back() != '/')
            return 409;
        if(access(path.c_str(),W_OK) == 0)
        {
            if(deletedir(path) == 0)
            {
                return 204;
            } 
            else
                return 500;
        }
        else
            return(403);
    }
};

class client
{
    public :
        int fd;
        server  *ss;
        Request req;
        Response *res;
      
        string chunk;
    
    client():fd(0) , ss(NULL), res(NULL){}
    ~client(){
        // if (fd > 0)
        // close (fd);
    }
    void set_fd(int _fd)
    {
        fd = _fd;
    }

    void set_serv(server& _ss)
    {
        ss = &_ss;
    }

    int get_fd()
    {
        return fd;
    }

    server* get_serv()
    {
        return ss;
    }
    void respond(pollfd &fds)
    {
        if(this->res == NULL && !this->req.empty_header())
        {
           cout << "respond forfdfdd" << endl;
            cout << "hannibal " << ss->get_name(0) << endl;
            
            res = new Response(req,*ss);
           cout << "respond forgesd" << endl;
            string a = res->responde();
            send(fd, a.c_str(),res->get_size(),0);
            cout << "wssl lhna" <<endl;
            cout << a <<endl;
           // exit(10);
           fds.events = POLLIN;
        }
    };
        //else
        //{
            //if(res->header_sent == false )
            //{
            //    res->header->set_firstline(res->code.get_code(res->status));
            //    send(fd, res->header->get_Header().c_str(),res->header->get_Header().size(),0);
           //     res->header_sent = true;
           //     cout << res->header->get_Header() << endl;
           // }
           // while(res->is_chunked == true)
           // {
             //   chunk = res->get_chunk();
             //   send(fd, chunk.c_str(),chunk.size(),0);   
            //    cout << "CHUNK :"  <<  chunk << endl;
            //    if (chunk == "0\r\n")
            //        fds.events = POLLIN;
                //i++;
          //  }
            //if(res->is_chunked == false)
            //{
              //  send(fd, string("0\r\n\r\n").c_str(),string("0\r\n\r\n").size(),0);
                //fds.events = POLLIN;
            //}
           //chunk = string("0\r\n\r\n");
        
    //   }
        // if(res->is_chunked == false)
        // {
        //     delete res;
        // }
   // }
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

string Response::responde()
{
    string content;
    //cout << "responde"<< endl;

    //: yummy_cookie=choco
    if(this->status == 200)
    {
        if(this->path != "")
        {
            content += renderpage(this->path) ;
            this->filesize = content.size();
        }
        else
        {
            content += this->body;
            this->filesize = content.size();
        }
    }
    else if (this->status != 200)
    {   
        if(this->status == 201)
        {  
           content += this->code.get_errorhtml(this->status,this->uploadfile);
           this->filesize = content.size();
        }
        else
        {
            content += this->code.get_errorhtml(this->status,"");
            this->filesize = content.size();
        }
    }
    header->set_firstline(code.get_code(status));
    if(this->filesize > 0 )
            header->setHeader("Content-Length" , to_string(this->filesize));
    else
            header->setHeader("Content-Length" , "0");
    content = this->header->get_Header() + content;
    header->setHeader("Set-Cookie" , "yuupi=choco");
    //cout << content << endl;
    this->size = content.size();
    return(content);
};