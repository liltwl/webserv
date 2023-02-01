#include "config_file.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>


size_t parse_config::get_lines_size() const
{
    return (_lines.size());
}

void    parse_config::before_start_parsing(int argc, char **argv)
{
    std::ifstream file;
    struct stat buffer;
    if (argc > 2)
    {
        basic_error("Usage: ./parse_confile <path_to_config_file>", NULL, "");
        exit(1);
    }
    if (argc == 2)
    {
        int i = stat(argv[1], &buffer);
        if (i == -1)
        {
            std::cout << "Error: this config file not found" << std::endl;
            exit (1);
        }
	    file.open(argv[1]);
        if (!file.is_open())
            exit (basic_error("Error: file " , argv[1] , " not found"));
    }
    else
    {
        file.open("./conf.conf");
        if (!file.is_open())
            exit (basic_error("Error: default conf not found", NULL, ""));
    }
	std::string line;
	while (std::getline(file, line))
	{
		std::replace( line.begin(), line.end(), '\t', ' ');
        size_t i = 0;
		while (i < line.size() && line[i] == ' ')
        {
			i++;
        }
		if (line != " " && line != "" && (i != line.size()))
			_lines.push_back(line);	
	}
    file.close();
    start_parsing();
}

void    parse_config::accolade_error()
{
    size_t i = 0;
    int accolade = 0;
    while (i < _words.size())
    {
        if (_words[i] == "{")
            ++accolade;
        else if (_words[i] == "}")
            --accolade;
        ++i;
    }
    if (accolade != 0)
    {
        std::cout <<"Error: File Not Well Formated(accolade error)" << std::endl;
        exit(1); 
    }
}

void       parse_config::split_by_space()
{
    for (std::vector<std::string>::iterator it = _lines.begin(); it != _lines.end(); ++it)
    {
        std::string line = *it;
        std::stringstream ss(line);
        std::string word;
        while (std::getline(ss, word, ' '))
        {
            if (word == "")
                continue;
            _words.push_back(word);
        }
    }
}


unsigned int   parse_config::server_parsing(unsigned int &i)
{
    server s;
    bool location_flag = false;
    bool cgi_flag = false;
    while (1)
    {
        if (i >= _words.size() || (_words[i] == "}" && !location_flag && !cgi_flag))
            break ;
        if (_words[i] == "server_names")
            i = s.fill_name(_words, i);
        else if (_words[i] == "listen")
        i = s.fill_listen(_words, i);
        else if (_words[i] == "root")
            s.set_root(_words[i + 1]);
        else if (_words[i] == "allow_methods")
            i = s.fill_allowed_methods(_words, i);
        else if (_words[i] == "upload_path")
            s.set_upload_path(_words[i + 1]);
        else if (_words[i] == "index")
            i = s.fill_index(_words, i);
        else if (_words[i] == "error_page")
            s.set_error_pages(_words[i + 1], _words[i + 2]);
        else if (_words[i] == "autoindex")
            i = s.fill_autoindex(_words, i);
        else if (_words[i] == "redirection")
            s.set_redirections(_words[i + 1], _words[i + 2]);
        else if (_words[i] == "client_max_body_size")
            s.set_client_max_body_size(_words[i + 1]);
        else if (_words[i] == "location")
            i = s.fill_location(_words, i, location_flag);
        else if (_words[i] == "cgi")
           i = s.fill_cgi(_words, i, cgi_flag);
        i++;
    }
    s.set_to_default();
    this->_servers.push_back(s);
    return i;
}

void    parse_config::specified_words(std::string &tmp)
{
    std::string err;
    err = "Error: ";
    err += tmp;
    err += " is not a valid word";
    if (tmp != "server_names" && tmp != "server" && tmp != "cgi_path" && tmp != "root" &&
        tmp != "allow_methods" && tmp != "upload_path" && tmp != "index"
        && tmp != "error_page" && tmp != "autoindex" && tmp != "redirection"
        && tmp != "client_max_body_size" && tmp != "location" && tmp != "cgi"
        && tmp != "{" && tmp != "}" && tmp != "listen")
        {
            std::cout << err << std::endl;
            exit(1);    
        }
}

void    parse_config::syntax_error()
{
    for (std::vector<std::string>::iterator it = _lines.begin(); it != _lines.end(); ++it)
    {
        std::string line = *it;
        std::stringstream ss(line);
        std::string tmp;
        if (!line.empty())
        {
            while (std::getline(ss, tmp, ' '))
            {
                if (tmp == "")
                    continue;
                break;
            }
            specified_words(tmp);
        }
    }
}

void    parse_config::check_host_server_names_error()
{
    size_t i = 0;
    while (i < _servers.size())
    {
        size_t k = i + 1;
        while (k < _servers.size())
        {
            if ((_servers[i].get_listen_port() == _servers[k].get_listen_port()))
            {
                size_t t = 0;
                while (t < _servers[i].get_name_size())
                {
                    size_t y = 0;
                    while (y < _servers[k].get_name_size())
                    {
                        if (_servers[i].get_name(t).compare(_servers[k].get_name(y)) == 0)
                        {    
                            std::cout <<"Error: there is two server have same server_name and port" << std::endl;
                            exit(1);
                        }
                        y++;
                    }
                    t++;
                }
            }
            k++;
        }
        i++;
    }
}

void    parse_config::start_parsing()
{
    split_by_space();
    if (_words.size() == 0)
    {
        std::cout <<"Error: File Not Well Formated(no word)" << std::endl;
        exit(1);
    }
    accolade_error();
    syntax_error();
    for (unsigned int i = 0; i < _words.size(); i++)
    {
		if (_words[i] == "server" && ((i + 1) < _words.size()))
        {
			i++;
			if (_words[i] == "{")
				i = parse_config::server_parsing(i);
		}
		else
        {
            std::cout <<"Error: every server configuration must be startes by server" << std::endl;
            exit(1);
        }
	}
    check_host_server_names_error();
}

void    parse_config::read_server()
{
    size_t i = 0;

    while (i < _servers.size())
    {
        std::cout << "--------------SERVER" << i << "---------------" <<std::endl;
        unsigned int j = 0;
        std::cout << "server_names: ";
        while (j < this->_servers[i].get_name_size())
        {
            std::cout << this->_servers[i].get_name(j) << " ";
            j++;
        }
        std::cout << std::endl;
        std::cout << "listen_host: " << this->_servers[i].get_listen_host() << std::endl;
        std::cout << "listen_port: " <<this->_servers[i].get_listen_port() << std::endl;
        std::cout << "root: " << this->_servers[i].get_root() << std::endl;
        j = 0;
        std::cout << "allowed_methods: ";
        while (j < this->_servers[i].get_allowed_methods_size())
        {
            std::cout << this->_servers[i].get_allowed_methods(j) << " ";
            j++;
        }
        std::cout << std::endl;
        std::cout << "upload_path: " << this->_servers[i].get_upload_path() << std::endl;
        j = 0;
        std::cout << "index: ";
        while (j < this->_servers[i].get_index_size())
        {
            std::cout << this->_servers[i].get_index(j) << " ";
            j++;
        }
        std::cout << std::endl;
        if (this->_servers[i].get_autoindex())
            std::cout << "auto index: on" << std::endl;
        else
            std::cout << "auto index: off" << std::endl;
        j = 0;
        while (j < _servers[i].get_error_pages_size())
        {
            std::cout << "error_pages: ";
            std::vector<std::string> tmp;
            tmp = _servers[i].get_error_pages(j);
            std::cout << tmp[0];
            std::cout << "   ";
            std::cout << tmp[1] << std::endl;
            j++;
        }
        j = 0;
        while (j < _servers[i].get_redirections_size())
        {
            std::cout << "redirections: ";
            std::vector<std::string> tmp;
            tmp = _servers[i].get_redirections(j);
            std::cout << tmp[0];
            std::cout << "   ";
            std::cout << tmp[1] << std::endl;
            j++;
        }
        std::cout << "client_max_body_size: " << this->_servers[i].get_client_max_body_size() << std::endl;
        j = 0;
        std::cout << "********location*********" << std::endl;
        while (j < _servers[i].get_location_size())
        {
            location t = this->_servers[i].get_location(j);
            std::cout << "location_path: "<< t.get_locations_path() << std::endl;
            std::cout << "upload_path: " << t.get_upload_path() << std::endl;
            std::cout << "max_body_size: "<< t.get_client_max_body_size() << std::endl;
            unsigned int k = 0;
            std::cout << "root: "<< t.get_root() << std::endl;
            if (t.get_autoindex())
                std::cout << "auto index: on" << std::endl;
            else
                std::cout << "auto index: off" << std::endl;
            std::cout << "index: ";
            while (k < t.get_index_size())
            {
                std::cout << t.get_index(k) << " ";
                k++;
            }
            std::cout << std::endl;
            k = 0;
            std::cout << "allowed_methods: ";
            while (k < t.get_methods_size())
            {
                std::cout << t.get_methods(k) << " ";
                k++;
            }
            std::cout << std::endl;
            std::cout << "***********************" << std::endl;
            j++;
        }
        j = 0;
        std::cout << "********CGI*********" << std::endl;
        while (j < _servers[i].get_cgi_size())
        {
            cgi t = this->_servers[i].get_cgi(j);
            std::cout << "cgi name:" << t.get_cgi_name() << std::endl;
            std::cout << "cgi_path: "<< t.get_cgi_path() << std::endl;
            std::cout << "***********************" << std::endl;
            j++;
        }
        i++;
    }
}

int parse_config::basic_error(std::string error_message, char const *av, std::string error_message2)
{
    if (av && error_message2 != "" && error_message != "")
        std::cout << error_message << av << error_message2 << std::endl;
    else if (av && error_message != "")
        std::cout << error_message << av << std::endl;
    else if (error_message != "")
        std::cout << error_message << std::endl;    
    return (0);
}

std::string parse_config::get_lines(int i) const
{
    return (_lines[i]);
}

void   parse_config::set_lines(std::vector<std::string> lines)
{
    _lines = lines;
}