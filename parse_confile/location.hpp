#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "server.hpp"
#include <iostream>
#include <vector>

class cgi;
class location
{
    
protected:
    std::string                 _name;
    std::string                 _locations_path;
    std::vector<std::string>    _allow_methods;
    std::string                 _root;
    long long int               _client_max_body_size;
    std::vector<std::string>    _index;
    bool                        _autoindex;
    std::string                 _upload_path;
public:
    location();
    ~location();
    location (const location &obj);
    void                        set_upload_path(std::string upload_path);
    std::string                 get_upload_path() const;
    std::string                 get_locations_path() const;
    std::vector<std::string>    get_methods() const;
    std::string                 get_root() const;
    bool                        get_autoindex() const;
    long long int               get_client_max_body_size() const;
    std::string                 get_index(unsigned int i) const;
    unsigned int                get_index_size() const;
    std::string                 get_methods(unsigned int i) const;
    std::vector<std::string>    get_index() const;
    unsigned int                get_methods_size() const;
    void                        set_locations_path(std::string locations_path);
    void                        set_client_max_body_size(long long int client_max_body_size);
    void                        set_methods(std::string methods);
    void                        set_root(std::string root);
    void                        set_autoindex(bool autoindex);
    void                        set_client_max_body_size(std::string client_max_body_size);
    void                        set_index(std::string index);
    unsigned int                fill_allowed_methods(std::vector<std::string>, unsigned int);
    void                        fill_allowed_methodes(std::vector<std::string> methods)
    {
        this->_allow_methods = methods;
    }
    unsigned int                 fill_index(std::vector<std::string>, unsigned int);
    void                         fill_indexs(std::vector<std::string> index)
    {
        _index = index;
    }
    unsigned int                 fill_autoindex(std::vector<std::string>, unsigned int);
    bool                         is_number(const std::string &str);
    bool                         not_predefined(std::string &) const;
    location                    &operator=(location const & rhs);
};

#endif
