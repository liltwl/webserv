#include "cgi.hpp"

cgi::cgi() : _name(""), _cgi_path("")
{
}

cgi::~cgi()
{
}


void cgi::set_cgi_path(std::string cgi_path)
{
    if (not_predefined(cgi_path))
       this->_cgi_path = cgi_path;
    else
    {
        std::cout << "Error: cgi path not well defined" << std::endl;
        exit(0);
    } 
    
}

void    cgi::set_cgi_name(std::string name)
{
    if (not_predefined(name))
       this->_name = name;
    else
    {
        std::cout << "Error: cgi name not well defined" << std::endl;
        exit(0);
    } 
    
}

std::string cgi::get_cgi_name() const
{
    return this->_name;
}

std::string cgi::get_cgi_path() const
{
    return this->_cgi_path;
}

/*
    operator
*/

cgi    &cgi::operator=(cgi const &rhs)
{
    this->_name = rhs._name;
    this->_cgi_path = rhs._cgi_path;
    return *this;
}

bool cgi::not_predefined(std::string &word) const
{
    if (word != "}" && word != "server" && word != "{" && word != "listen" 
            && word != "root" && word != "allow_methods" && word != "server_names"
            && word != "upload_path" && word != "index"
            && word != "error_page" && word != "autoindex"
            && word != "redirection" && word != "client_max_body_size"
            && word != "location" && word != "cgi" && word != "cgi_path")
            return (1);
    return (0);
}