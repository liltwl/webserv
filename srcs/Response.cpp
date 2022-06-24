#include "Response.hpp"


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
int Response::iscgi(Request &req, server &serv)
{
    vector<string> str;
    str =  split(req.get_location(),'.');
    //string st;
   
    int i = 0;
       
    while(i < int(serv.get_cgi_size()))
    {
        if(serv.get_cgi(i).get_cgi_name() == string(".") +str.back())
        {
            if(str.back() == "php")
            {   
                this->cgipath = serv.get_cgi(i).get_cgi_path();

                return(true);
            }
        }
        i++;
    }

    return(false);
}
Response::Response(Request &_req, server &_serv) : req(_req) , serv(_serv), is_chunked(0)
{
        this->filesize = 0;
        this->codI = new Statucode; 
        this->php_cgi = iscgi(this->req,this->serv);

        this->header = new Header(serv);     
        this->status = this->handlerequest(req,serv);          
        this->methode = req.get_method();
        this->path = "";
        this->indexstats = false;
        this->cgipath = "";
        
}
long Response::get_size(){return this->size;};

void Response::findlocation()
{
    string path;
    int matchsize = 0;
    for(int i = 0; i < int(this->serv.get_location_size());i++)
    {
        if(strncmp(this->req.get_location().c_str(),this->serv.get_location(i).get_locations_path().c_str(),this->serv.get_location(i).get_locations_path().size()) == 0 && matchsize < int(this->serv.get_location(i).get_locations_path().size())) // chack if the request location exist in the config file locations and icrement i[0]
        {
            path = this->serv.get_location(i).get_locations_path() ;

            matchsize = this->serv.get_location(i).get_locations_path().size();
            this->reqloc = this->serv.get_location(i);
        }
    }  
    if(matchsize > 0)
        this->rlocation = path;
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
        for(int i = 0 ; i < int(serv.get_location_size()); i++)
        {
            if(serv.get_location(i).get_locations_path() == location)
            {
                methods = serv.get_location(i).get_methods();
                if(find(methods.begin(),methods.end(),Method) != methods.end())
                    return true;
            }
        }
        return(false);
    }
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

int Response::handleredirection()
{
    int i = 0;

    while(i < int(this->serv.get_redirections_size()))
    {
        if(this->req.get_location() == this->serv.get_redirections(i).front())
        {
            this->header->setHeader("Location",this->serv.get_redirections(i).back());
            return(1);
        }
        i++;
    }
 return 0;   
}
int Response::handle_cgi()
{
    Cgi *cj = new Cgi();
    string script;
    string cgpath;
    string outpath[2];
    vector<string>spli = split(this->req.get_location(), '/');
    string input = "";
    if(this->rlocation != this->serv.get_root())
        script = this->reqloc.get_root() +  this->req.get_location().substr(this->rlocation.size());
    else
        script = this->serv.get_root() + this->req.get_location();

    cgpath = this->cgipath;
 
if(this->req.get_method() == "POST")
{
    string fname;
    fname = "/tmp/" + string("ccp") + ".data";
    ofstream f(fname.c_str());
    if(f)
    {
        f << this->req.get_body();
    }
    input = fname;
}
    if(cj->execute(input, script,cgpath,this->req) != 1)
    {
        delete cj;
        this->php_cgi = false;
        return(500);
    }
    this->full_cgi_res = cj->get_resp_path();
    this->cgi_body = cj->get_body_path();

    
    this->header->setHeaders(cj->get_headers());
    delete cj;
    return(200);
}
int Response::handlerequest(Request &req, server &serv)
{
    if(req.empty_header()||this->req.get_body_len() == -1 || this->req.get_body_len() < (req.get_headrs().count("Content-Length") ?stoi(this->req.get_headrs().find("Content-Length")->second):0))
        return(400);
    if(this->handleredirection() != 0)
        return 301;
    this->findlocation();
    if(checkmethod(serv,this->rlocation,req.get_method()) == false)
        return 405;
    
    if(this->php_cgi == true)
    { 
        return(this->handle_cgi());
    }

    if(req.get_method() == "GET")
        return this->handleGet();
    else if(req.get_method() == "POST")
        return(this->handlePost());
    else if(req.get_method() == "DELETE")
        return(this->handleDelete());
    else
        return 501;
    return(200);
}
long long file_size(string path)
{
    struct stat stat_buf;
    int rc = stat(path.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

void Response::renderindex(string paths)
{
    string str;
    DIR *dr;
    struct dirent *e;
    dr = opendir(paths.c_str());
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
                    this->body += string("<a href=\"") + this->req.get_location() + e->d_name + string("\">")+ e->d_name + string("</a>"); 
                else
                    this->body += string("<a href=\"") + this->req.get_location() + string("/") + e->d_name + string("\">")+ e->d_name + string("</a>");    
            }
        }   
    }
    this->body += "</table>\n</body>\n</head>\n</html>\n";
   closedir(dr);
}
int Response::findindexfile(string paths)
{
    int i = 0;

    while(i < int(this->reqloc.get_index_size()))
    {
      if (finddir( paths + this->reqloc.get_index(i) ) == 1)
      {
        this->index_path = paths + this->reqloc.get_index(i);
        return(0);
      }
        i++;
    }
    return(1);
}

int Response::handleGet()
{
    string paths;

    if(this->rlocation != this->serv.get_root())
    {
        paths = this->reqloc.get_root();
        paths = paths + this->req.get_location().substr(this->rlocation.size());
    }
    else
    {
        paths = this->serv.get_root() ;
        paths = paths  + this->req.get_location();
    }
    if(finddir(paths) == 0)
    {
        return(404);
    }
    if(finddir(paths) == 1)
    {
        this->header->setHeader("Content-Type",get_type(this->req.get_location(), 1));
        this->filesize = file_size(paths);
        this->header->setHeader("Content-Length",to_string(this->filesize));
        this->body = renderpage(paths);
        this->path = paths;
        return(200);
    }
    if(finddir(paths) == 2)
    {
        if(this->req.get_location().back() != '/')
        {
            this->header->setHeader("Location",this->req.get_location() + "/");
            return(301);
        }
        if(this->req.get_location() == this->reqloc.get_locations_path() && this->reqloc.get_index_size() != 0)
        {
            if(this->findindexfile(paths) == 0)
            {
                this->header->setHeader("Content-Type",get_type(this->index_path, 1));
                 this->filesize = file_size(this->index_path);
                this->header->setHeader("Content-Length",to_string(this->filesize));
                this->body = renderpage(paths) = renderpage(this->index_path);
                return(200);
            }
        }
        if (this->rlocation == this->serv.get_root())
        {
                if(this->serv.get_autoindex() == true)
                {

                this->renderindex(paths);

                return(200);
                }
                else 
                 return(403);

        }
        else if(this->rlocation != this->serv.get_root())
        {
                if(this->reqloc.get_autoindex() == true)
                {
                    this->renderindex(paths);
                    return (200);
                }
                else 
                return(403);
        }     
    }
        return (205);
}

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
    ofstream f(string("."+file_name).c_str());
    if(f)
    {
        f << this->req.get_body();

    }
    this->uploadfile = file_name;
    return(201);
}

int deletedir(string path)
{
    struct dirent *entry = NULL;
    DIR *dir = NULL;
    dir = opendir(path.c_str());
     
    while((entry = readdir(dir)))
    {   
            
        DIR *sub_dir = NULL;
        FILE *file = NULL;
        string abs_path;
        if(*(entry->d_name) != '.')
        {   
                abs_path =  path + entry->d_name;
                if((finddir(abs_path) == 2))
                {   
                    sub_dir = opendir(abs_path.c_str());
                    
                        closedir(sub_dir);
                        deletedir(abs_path.c_str());
                }   
                else 
                {   
                        if(finddir(abs_path) == 1)
                        { 
                                fclose(file);
                                if(remove(abs_path.c_str()) != 0)
                                    return(1);
                        }
                        else
                            return(1);  
                }   
        }   
    }   
    if(remove(path.c_str())!= 0)
        return(2);
    return(0);
}
int Response::handleDelete()
{
    string paths;
    if(this->rlocation != this->serv.get_root())
    {
        paths = this->reqloc.get_root() ;
        paths.pop_back();
        paths = paths + this->req.get_location().substr(this->rlocation.size());
    }
    else
    {  
        paths = this->serv.get_root() ;
        paths.pop_back();
        paths = paths  + this->req.get_location();
    }
    if(finddir(paths) == 0)
        return(404);
    if(finddir(paths) == 1)
    {
        int status;
        status = remove(paths.c_str());
        
        if(status == 0)
            return 204;
        else
        {
            if(access(paths.c_str(),W_OK) == 0)
                return 500;
            else
                return (403);     
        }
    }
    if(finddir(paths) == 2)
    {
        if(this->req.get_location().back() != '/')
            return 409;
        if(access(paths.c_str(),W_OK) == 0)
        {
            if(deletedir(paths) == 0)
                return 204;
            else
                return 500;
        }
        else
            return(403);
    }
    return 1;
}


string Response::responde()
{
    string content;
    if(this->php_cgi == true)
    {
        header->set_firstline(codI->get_code(200));
        this->header->setHeader("Content-Length",to_string(file_size(this->cgi_body)));
        content = this->header->get_Header() + renderpage(this->cgi_body);
        cout << "the request" << endl << content << endl;
        return(content);
    }
    if (is_chunked == 1)
    {
        string str;
        char buff[5002];
        int i = read(fd_file, buff,5000);
        if (i < 5000)
            is_chunked = 0;

        filesize -= i;
        str.append(buff, i);
        return str;
    }
    if(this->status == 200)
    {
        filesize = file_size(this->path);

        if (file_size(this->path) > 2000)
        {
            is_chunked = 1;
            fd_file = open(path.c_str(), O_RDONLY);
        }
        else
        {
            content += this->body;
            this->filesize = content.size();
        }
    }
    else if (this->status != 200)
    {  
        this->codI->set_error(this->serv.get_error_pages(), this->status) ;
        if(this->status == 201)
        {  
           content += this->codI->get_errorhtml(this->status,this->uploadfile);
           this->filesize = content.size();
        }
        else
        {
            content += this->codI->get_errorhtml(this->status,"");
            this->filesize = content.size();
        }
    }
    header->set_firstline(codI->get_code(status));
    if(this->filesize > 0 )
            header->setHeader("Content-Length" , to_string(this->filesize));
    else
            header->setHeader("Content-Length" , "0");
    //  header->setHeader("Set-Cookie" , "yuupi=zaba");
    content = this->header->get_Header() + content;
    this->size = content.size();

    return(content);
}