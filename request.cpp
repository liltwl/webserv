#include "request.hpp"


Request::Request(): body_len(0) 
{
}

Request::~Request()
{
}

Request::Request(Request const& other)
{
    *this = other;
}

Request& Request::operator=(Request const& other)
{
    if (this != &other)
    {
        rqmethod = other.rqmethod;
        location = other.location;
        vrs = other.vrs;
        headers = other.headers;
        body = other.body;
        body_len = other.body_len;
    }
    return *this;
}

void Request::setrqmethod(std::string val)
{
    rqmethod = val;
}

void Request::setlocation(std::string val)
{
    location = val;
}

void Request::setversion(std::string val)
{
    vrs = val;
}
void Request::setbody_limit(int val)
{
    body_len = val;
}

void Request::addheaders(std::string key, std::string value)
{
    headers[key] = value;
}

void Request::addbody(std::string line, size_t len)
{
    body += line;
    body_len += len;
}

std::string Request::get_method()
{
    return rqmethod;
}

std::string  Request::get_location()
{
    return location;
}

std::string Request::get_version()
{
    return vrs;
}

std::map<std::string, std::string> Request::get_headrs()
{
    return headers;
}

std::string& Request::get_body()
{
    return body;
}

int Request::get_body_len()
{
    return body_len;
}

void Request::clear()
{
    rqmethod.clear();
    location.clear();
    vrs.clear();
    headers.clear();
    body.clear();
    body_len = 0;
}

int Request::empty_header()
{
    if (rqmethod.empty() || location.empty() || vrs.empty() || headers.empty())
        return 1;
    return 0;
}

size_t Request::empty()
{
    return (rqmethod.empty() || location.empty() || vrs.empty());
}
