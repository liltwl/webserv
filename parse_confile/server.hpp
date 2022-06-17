#ifndef SERVER_HPP
#define SERVER_HPP
#include "config_file.hpp"
#include "location.hpp"
#include "cgi.hpp"
#include <string>
#include <iostream>
class location;
class cgi;
class server
{
    
protected:
    std::vector<std::string>                _name;
    int                                     _listen_port;
    std::string                             _listen_host;
    std::vector<std::string>                _allowed_methods;
    std::vector<std::string>                _index;
    std::string                             _upload_path;
    std::vector<std::vector<std::string> >  _error_pages;
    std::vector<std::vector<std::string> >  _redirections;
    std::string                             _root;
    std::vector<location>                   _location;
    std::vector<cgi>                        _cgi;
    long long int                           _client_max_body_size;
    bool                                    _autoindex;

public:
    server();
    ~server();
    server(const server &src);
    void                         set_to_default();
    void                         set_name(std::string name);
    void                         set_listen(std::string listen);
    void                         set_allowed_methods(std::string allowed_methods);
    void                         set_index(std::string index);
    void                         set_error_pages(std::string error_pages, std::string number_error);
    void                         set_redirections(std::string redirection_from, std::string redirection_to);
    void                         set_root(std::string root);
    void                         set_upload_path(std::string upload_path);
    void                         set_location(location location);
    void                         set_cgi(cgi cgi);
    void                         set_client_max_body_size(std::string client_max_body_size);
    void                         set_client_max_body_size(long long int client_max_body_size);
    void                         set_autoindex(bool autoindex);
    std::string                  get_name(int) const;
    std::string                  get_listen_host() const;
    unsigned int                 fill_listen(std::vector<std::string> words, unsigned int i);
    int                          get_listen_port() const;
    std::string                  get_allowed_methods(int i) const;
    std::vector<std::string>     get_allowed_methods() const;
    std::string                  get_index(int i) const;
    std::vector<std::string>     get_index() const;
    std::vector<std::string>     get_error_pages(int i) const;
    std::vector<std::vector<std::string> >     get_error_pages() const{return _error_pages;};
    unsigned int                 get_error_pages_size() const;
    unsigned int                 get_redirections_size() const;
    std::vector<std::string>     get_redirections(int i) const;
    std::vector<std::vector<std::string> > get_redirections() const;
    std::string                  get_root() const;
    std::string                  get_upload_path() const;
    location                     get_location(int i) const;
    unsigned int                 get_location_size() const;
    unsigned int                 get_cgi_size() const;
    cgi                          get_cgi(int i) const;
    long long int                get_client_max_body_size() const;
    unsigned int                 get_allowed_methods_size() const;
    bool                         get_autoindex() const;
    unsigned int                 get_index_size() const;
    unsigned int                 get_name_size() const;
    unsigned int                 fill_allowed_methods(std::vector<std::string>, unsigned int);
    unsigned int                 fill_name(std::vector<std::string>, unsigned int);
    unsigned int                 fill_index(std::vector<std::string>, unsigned int);
    unsigned int                 fill_autoindex(std::vector<std::string>, unsigned int);
    unsigned int                 fill_location(std::vector<std::string>, unsigned int, bool&);
    unsigned int                 fill_cgi(std::vector<std::string>, unsigned int, bool&);
    bool                         not_predefined(std::string &) const;
    bool                         is_number(const std::string& str);
    void                         check_host(std::string);
    std::vector<location>        get_location() const;
    std::vector<cgi>             get_cgi() const;
    server                       &operator=(server const & rhs);
};
#endif