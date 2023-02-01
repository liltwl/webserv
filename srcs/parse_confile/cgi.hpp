#ifndef CGI_HPP
#define CGI_HPP

#include "server.hpp"
#include <iostream>
#include <vector>


class cgi
{
protected:
    std::string           _name;
    std::string                 _cgi_path;
public:
    cgi();
    ~cgi();

    std::string                 get_cgi_path() const;
    std::string                 get_cgi_name() const;
    void                        set_cgi_path(std::string cgi_path);
    void                        set_cgi_name(std::string name);
    bool                        not_predefined(std::string &word) const;
    cgi                         &operator=(cgi const & rhs);
};


#endif