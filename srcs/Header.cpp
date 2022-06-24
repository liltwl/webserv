#include "./Header.hpp"

string getDate()
{
    time_t now = time(0);
    string dt = ctime(&now);
    return dt;
}

Header::Header(server &serv)
{
        this->Server = serv.get_name(0);
        this->date = getDate();
}
void Header::setHeaders(map<string,string> hdrs)
{
    this->headers.insert(hdrs.begin(),hdrs.end());
}
void Header::set_firstline(string firstline)
{
    this->firstline = firstline;
}
string Header::get_Header()
{
    string header;
    header = this->firstline + "\r\n";
    header += "Date: " + this->date + " \r\n";
    header += "Server: " + this->Server + "\r\n";
    for(map<string,string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
    {
        header += it->first + ": " + it->second + "\r\n";
    }
    header += "\r\n";
    return(header);
}
void Header::setHeader(string key, string value)
{
    this->headers[key] = value;
}