#include "location.hpp"

location::location() : _locations_path(""),
                        _allow_methods(std::vector<std::string>()),
                        _root(""),
                        _client_max_body_size(-1),  
                        _index(std::vector<std::string>()),
                        _autoindex(false),
                        _upload_path("")
                        {}

location::~location() {}

location::location(const location &obj)
{
    if (this != &obj)
    {
        *this = obj;
    }
}

std::string                 location::get_locations_path() const{ return this->_locations_path; }
std::vector<std::string>    location::get_methods() const { return this->_allow_methods; }
std::vector<std::string>    location::get_index() const { return this->_index; }
std::string                 location::get_root() const { return this->_root; }
bool                        location::get_autoindex() const { return this->_autoindex; }
long long int               location::get_client_max_body_size() const { return this->_client_max_body_size; }
std::string                 location::get_upload_path() const{return _upload_path; }

void location::set_locations_path(std::string locations_path) { this->_locations_path = locations_path; }
void location::set_methods(std::string methods)
{
    if (methods == "POST" || methods == "GET" || methods == "DELETE")
        _allow_methods.push_back(methods);
    else
    {
        std::cout << "Error: allowed methods in location not well defined" << std::endl;
        exit(1);
    }
}
void location::set_root(std::string root) {
    if (not_predefined(root))
        this->_root = root;
    else
    {
        std::cout << "Error: root in location not well defined" << std::endl;
        exit(1);
    }
}
void location::set_autoindex(bool autoindex) { this->_autoindex = autoindex; }
void location::set_index(std::string index) { this->_index.push_back(index); }
bool location::is_number(const std::string& str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		char current = str[i];
		if (current >= '0' && current <= '9')
			continue;
		return false;
	}
	return true;
}

void    location::set_client_max_body_size(std::string client_max_body_size)
{
    if (not_predefined(client_max_body_size) && is_number(client_max_body_size))
       this->_client_max_body_size =std::stoi(client_max_body_size);
    else
    {
        std::cout << "Error: client max body size should be number" << std::endl;
        exit(1);
    }
}

void    location::set_upload_path(std::string upload_path)
{
    if (not_predefined(upload_path))
        _upload_path = upload_path;
    else
    {
        std::cout << "Error: upload_path is empty" << std::endl; 
        exit(1);
    }
}

unsigned int location::fill_allowed_methods(std::vector<std::string> words, unsigned int i)
{
    i++;
    while (i < words.size() && not_predefined(words[i]))
    {
        set_methods(words[i]);
        i++;
    }
    if (get_methods_size() == 0)
    {
        std::cout << "Error: location allow_method is empty" << std::endl;
        exit(1);
    }
    i--;
    return i;
}

unsigned int location::fill_index(std::vector<std::string> words, unsigned int i)
{
    i++;
    while (i < words.size() && words[i] != "}" && words[i] != "server" && words[i] != "{" && words[i] != "listen" 
            && words[i] != "root" && words[i] != "allow_methods"
            && words[i] != "upload_path" && words[i] != "index"
            && words[i] != "error_page" && words[i] != "autoindex"
            && words[i] != "redirection" && words[i] != "client_max_body_size"
            && words[i] != "location" && words[i] != "cgi")
    {
        set_index(words[i]);
        i++;
    }
    if (get_index_size() == 0)
    {
        std::cout << "Error: location index is empty" << std::endl; 
        exit(1);
    }
    i--;
    return i;
}

unsigned int location::fill_autoindex(std::vector<std::string> words, unsigned int i)
{
    if (words[i + 1] == "on")
        set_autoindex(true);
    else
        set_autoindex(false);
    return i;
}

unsigned int location::get_index_size() const
{
    return (_index.size());
}


std::string                 location::get_index(unsigned int i) const
{
    return  (this->_index[i]);
}

unsigned int location::get_methods_size() const
{
    return (_allow_methods.size());
}


std::string                 location::get_methods(unsigned int i) const
{
    return  (this->_allow_methods[i]);
}

void    location::set_client_max_body_size(long long int client_max_body_size)
{
    _client_max_body_size = client_max_body_size;
}

location    &location::operator=(location const &rhs)
{
    this->_locations_path = rhs._locations_path;
    this->_allow_methods = rhs._allow_methods;
    this->_root = rhs._root;
    this->_autoindex = rhs._autoindex;
    this->_client_max_body_size = rhs._client_max_body_size;
    this->_index = rhs._index;
    this->_upload_path = rhs._upload_path;
    return *this;
}

bool location::not_predefined(std::string &word) const
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
