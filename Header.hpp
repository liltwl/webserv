 #pragma once

#include "request.hpp"
#include "./Response.hpp"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;


std::string get_type(string location, int mode)
{
    map<string,string>ext;
    ext["3g2"] = "video/3gpp2";
  ext["3gp"] = "video/3gpp";
  ext["3gpp"] = "video/3gpp";
  ext["ac"] = "application/pkix-attr-cert";
  ext["adp"] = "audio/adpcm";
  ext["ai"] = "application/postscript";
  ext["apng"] = "image/apng";
  ext["appcache"] = "text/cache-manifest";
  ext["asc"] = "application/pgp-signature";
  ext["atom"] = "application/atom+xml";
  ext["atomcat"] = "application/atomcat+xml";
  ext["atomsvc"] = "application/atomsvc+xml";
  ext["au"] = "audio/basic";
  ext["aw"] = "application/applixware";
  ext["bdoc"] = "application/bdoc";
  ext["bin"] = "application/octet-stream";
  ext["bmp"] = "image/bmp";
  ext["bpk"] = "application/octet-stream";
  ext["buffer"] = "application/octet-stream";
  ext["ccxml"] = "application/ccxml+xml";
  ext["cdmia"] = "application/cdmi-capability";
  ext["cdmic"] = "application/cdmi-container";
  ext["cdmid"] = "application/cdmi-domain";
  ext["cdmio"] = "application/cdmi-object";
  ext["cdmiq"] = "application/cdmi-queue";
  ext["cer"] = "application/pkix-cert";
  ext["cgm"] = "image/cgm";
  ext["class"] = "application/java-vm";
  ext["coffee"] = "text/coffeescript";
  ext["conf"] = "text/plain";
  ext["cpt"] = "application/mac-compactpro";
  ext["crl"] = "application/pkix-crl";
  ext["css"] = "text/css";
  ext["csv"] = "text/csv";
  ext["cu"] = "application/cu-seeme";
  ext["davmount"] = "application/davmount+xml";
  ext["dbk"] = "application/docbook+xml";
  ext["deb"] = "application/octet-stream";
  ext["def"] = "text/plain";
  ext["deploy"] = "application/octet-stream";
  ext["disposition-notification"] = "message/disposition-notification";
  ext["dist"] = "application/octet-stream";
  ext["distz"] = "application/octet-stream";
  ext["dll"] = "application/octet-stream";
  ext["dmg"] = "application/octet-stream";
  ext["dms"] = "application/octet-stream";
  ext["doc"] = "application/msword";
  ext["dot"] = "application/msword";
  ext["drle"] = "image/dicom-rle";
  ext["dssc"] = "application/dssc+der";
  ext["dtd"] = "application/xml-dtd";
  ext["dump"] = "application/octet-stream";
  ext["ear"] = "application/java-archive";
  ext["ecma"] = "application/ecmascript";
  ext["elc"] = "application/octet-stream";
  ext["emf"] = "image/emf";
  ext["eml"] = "message/rfc822";
  ext["emma"] = "application/emma+xml";
  ext["eps"] = "application/postscript";
  ext["epub"] = "application/epub+zip";
  ext["es"] = "application/ecmascript";
  ext["exe"] = "application/octet-stream";
  ext["exi"] = "application/exi";
  ext["exr"] = "image/aces";
  ext["ez"] = "application/andrew-inset";
  ext["fits"] = "image/fits";
  ext["g3"] = "image/g3fax";
  ext["gbr"] = "application/rpki-ghostbusters";
  ext["geojson"] = "application/geo+json";
  ext["gif"] = "image/gif";
  ext["glb"] = "model/gltf-binary";
  ext["gltf"] = "model/gltf+json";
  ext["gml"] = "application/gml+xml";
  ext["gpx"] = "application/gpx+xml";
  ext["gram"] = "application/srgs";
  ext["grxml"] = "application/srgs+xml";
  ext["gxf"] = "application/gxf";
  ext["gz"] = "application/gzip";
  ext["h261"] = "video/h261";
  ext["h263"] = "video/h263";
  ext["h264"] = "video/h264";
  ext["heic"] = "image/heic";
  ext["heics"] = "image/heic-sequence";
  ext["heif"] = "image/heif";
  ext["heifs"] = "image/heif-sequence";
  ext["hjson"] = "application/hjson";
  ext["hlp"] = "application/winhlp";
  ext["hqx"] = "application/mac-binhex40";
  ext["htm"] = "text/html";
  ext["html"] = "text/html";
  ext["ics"] = "text/calendar";
  ext["ief"] = "image/ief";
  ext["ifb"] = "text/calendar";
  ext["iges"] = "model/iges";
  ext["igs"] = "model/iges";
  ext["img"] = "application/octet-stream";
  ext["in"] = "text/plain";
  ext["ini"] = "text/plain";
  ext["ink"] = "application/inkml+xml";
  ext["inkml"] = "application/inkml+xml";
  ext["ipfix"] = "application/ipfix";
  ext["iso"] = "application/octet-stream";
  ext["jade"] = "text/jade";
  ext["jar"] = "application/java-archive";
  ext["jls"] = "image/jls";
  ext["jp2"] = "image/jp2";
  ext["jpe"] = "image/jpeg";
  ext["jpeg"] = "image/jpeg";
  ext["jpf"] = "image/jpx";
  ext["jpg"] = "image/jpeg";
  ext["jpg2"] = "image/jp2";
  ext["jpgm"] = "video/jpm";
  ext["jpgv"] = "video/jpeg";
  ext["jpm"] = "image/jpm";
  ext["jpx"] = "image/jpx";
  ext["js"] = "application/javascript";
  ext["json"] = "application/json";
  ext["json5"] = "application/json5";
  ext["jsonld"] = "application/ld+json";
  ext["jsonml"] = "application/jsonml+json";
  ext["jsx"] = "text/jsx";
  ext["kar"] = "audio/midi";
  ext["ktx"] = "image/ktx";
  ext["less"] = "text/less";
  ext["list"] = "text/plain";
  ext["litcoffee"] = "text/coffeescript";
  ext["log"] = "text/plain";
  ext["lostxml"] = "application/lost+xml";
  ext["lrf"] = "application/octet-stream";
  ext["m1v"] = "video/mpeg";
  ext["m21"] = "application/mp21";
  ext["m2a"] = "audio/mpeg";
  ext["m2v"] = "video/mpeg";
  ext["m3a"] = "audio/mpeg";
  ext["m4a"] = "audio/mp4";
  ext["m4p"] = "application/mp4";
  ext["ma"] = "application/mathematica";
  ext["mads"] = "application/mads+xml";
  ext["man"] = "text/troff";
  ext["manifest"] = "text/cache-manifest";
  ext["map"] = "application/json";
  ext["mar"] = "application/octet-stream";
  ext["markdown"] = "text/markdown";
  ext["mathml"] = "application/mathml+xml";
  ext["mb"] = "application/mathematica";
  ext["mbox"] = "application/mbox";
  ext["md"] = "text/markdown";
  ext["me"] = "text/troff";
  ext["mesh"] = "model/mesh";
  ext["meta4"] = "application/metalink4+xml";
  ext["metalink"] = "application/metalink+xml";
  ext["mets"] = "application/mets+xml";
  ext["mft"] = "application/rpki-manifest";
  ext["mid"] = "audio/midi";
  ext["midi"] = "audio/midi";
  ext["mime"] = "message/rfc822";
  ext["mj2"] = "video/mj2";
  ext["mjp2"] = "video/mj2";
  ext["mjs"] = "application/javascript";
  ext["mml"] = "text/mathml";
  ext["mods"] = "application/mods+xml";
  ext["mov"] = "video/quicktime";
  ext["mp2"] = "audio/mpeg";
  ext["mp21"] = "application/mp21";
  ext["mp2a"] = "audio/mpeg";
  ext["mp3"] = "audio/mpeg";
  ext["mp4"] = "video/mp4";
  ext["mp4a"] = "audio/mp4";
  ext["mp4s"] = "application/mp4";
  ext["mp4v"] = "video/mp4";
  ext["mpd"] = "application/dash+xml";
  ext["mpe"] = "video/mpeg";
  ext["mpeg"] = "video/mpeg";
  ext["mpg"] = "video/mpeg";
  ext["mpg4"] = "video/mp4";
  ext["mpga"] = "audio/mpeg";
  ext["mrc"] = "application/marc";
  ext["mrcx"] = "application/marcxml+xml";
  ext["ms"] = "text/troff";
  ext["mscml"] = "application/mediaservercontrol+xml";
  ext["msh"] = "model/mesh";
  ext["msi"] = "application/octet-stream";
  ext["msm"] = "application/octet-stream";
  ext["msp"] = "application/octet-stream";
  ext["mxf"] = "application/mxf";
  ext["mxml"] = "application/xv+xml";
  ext["n3"] = "text/n3";
  ext["nb"] = "application/mathematica";
  ext["oda"] = "application/oda";
  ext["oga"] = "audio/ogg";
  ext["ogg"] = "audio/ogg";
  ext["ogv"] = "video/ogg";
  ext["ogx"] = "application/ogg";
  ext["omdoc"] = "application/omdoc+xml";
  ext["onepkg"] = "application/onenote";
  ext["onetmp"] = "application/onenote";
  ext["onetoc"] = "application/onenote";
  ext["onetoc2"] = "application/onenote";
  ext["opf"] = "application/oebps-package+xml";
  ext["otf"] = "font/otf";
  ext["owl"] = "application/rdf+xml";
  ext["oxps"] = "application/oxps";
  ext["p10"] = "application/pkcs10";
  ext["p7c"] = "application/pkcs7-mime";
  ext["p7m"] = "application/pkcs7-mime";
  ext["p7s"] = "application/pkcs7-signature";
  ext["p8"] = "application/pkcs8";
  ext["pdf"] = "application/pdf";
  ext["pfr"] = "application/font-tdpfr";
  ext["pgp"] = "application/pgp-encrypted";
  ext["pkg"] = "application/octet-stream";
  ext["pki"] = "application/pkixcmp";
  ext["pkipath"] = "application/pkix-pkipath";
  ext["pls"] = "application/pls+xml";
  ext["png"] = "image/png";
  ext["prf"] = "application/pics-rules";
  ext["ps"] = "application/postscript";
  ext["pskcxml"] = "application/pskc+xml";
  ext["qt"] = "video/quicktime";
  ext["raml"] = "application/raml+yaml";
  ext["rdf"] = "application/rdf+xml";
  ext["rif"] = "application/reginfo+xml";
  ext["rl"] = "application/resource-lists+xml";
  ext["rld"] = "application/resource-lists-diff+xml";
  ext["rmi"] = "audio/midi";
  ext["rnc"] = "application/relax-ng-compact-syntax";
  ext["rng"] = "application/xml";
  ext["roa"] = "application/rpki-roa";
  ext["roff"] = "text/troff";
  ext["rq"] = "application/sparql-query";
  ext["rs"] = "application/rls-services+xml";
  ext["rsd"] = "application/rsd+xml";
  ext["rss"] = "application/rss+xml";
  ext["rtf"] = "application/rtf";
  ext["rtx"] = "text/richtext";
  ext["s3m"] = "audio/s3m";
  ext["sbml"] = "application/sbml+xml";
  ext["scq"] = "application/scvp-cv-request";
  ext["scs"] = "application/scvp-cv-response";
  ext["sdp"] = "application/sdp";
  ext["ser"] = "application/java-serialized-object";
  ext["setpay"] = "application/set-payment-initiation";
  ext["setreg"] = "application/set-registration-initiation";
  ext["sgi"] = "image/sgi";
  ext["sgm"] = "text/sgml";
  ext["sgml"] = "text/sgml";
  ext["shex"] = "text/shex";
  ext["shf"] = "application/shf+xml";
  ext["shtml"] = "text/html";
  ext["sig"] = "application/pgp-signature";
  ext["sil"] = "audio/silk";
  ext["silo"] = "model/mesh";
  ext["slim"] = "text/slim";
  ext["slm"] = "text/slim";
  ext["smi"] = "application/smil+xml";
  ext["smil"] = "application/smil+xml";
  ext["snd"] = "audio/basic";
  ext["so"] = "application/octet-stream";
  ext["spp"] = "application/scvp-vp-response";
  ext["spq"] = "application/scvp-vp-request";
  ext["spx"] = "audio/ogg";
  ext["sru"] = "application/sru+xml";
  ext["srx"] = "application/sparql-results+xml";
  ext["ssdl"] = "application/ssdl+xml";
  ext["ssml"] = "application/ssml+xml";
  ext["stk"] = "application/hyperstudio";
  ext["styl"] = "text/stylus";
  ext["stylus"] = "text/stylus";
  ext["svg"] = "image/svg+xml";
  ext["svgz"] = "image/svg+xml";
  ext["t"] = "text/troff";
  ext["t38"] = "image/t38";
  ext["tei"] = "application/tei+xml";
  ext["teicorpus"] = "application/tei+xml";
  ext["text"] = "text/plain";
  ext["tfi"] = "application/thraud+xml";
  ext["tfx"] = "image/tiff-fx";
  ext["tif"] = "image/tiff";
  ext["tiff"] = "image/tiff";
  ext["tr"] = "text/troff";
  ext["ts"] = "video/mp2t";
  ext["tsd"] = "application/timestamped-data";
  ext["tsv"] = "text/tab-separated-values";
  ext["ttc"] = "font/collection";
  ext["ttf"] = "font/ttf";
  ext["ttl"] = "text/turtle";
  ext["txt"] = "text/plain";
  ext["u8dsn"] = "message/global-delivery-status";
  ext["u8hdr"] = "message/global-headers";
  ext["u8mdn"] = "message/global-disposition-notification";
  ext["u8msg"] = "message/global";
  ext["uri"] = "text/uri-list";
  ext["uris"] = "text/uri-list";
  ext["urls"] = "text/uri-list";
  ext["vcard"] = "text/vcard";
  ext["vrml"] = "model/vrml";
  ext["vtt"] = "text/vtt";
  ext["vxml"] = "application/voicexml+xml";
  ext["war"] = "application/java-archive";
  ext["wasm"] = "application/wasm";
  ext["wav"] = "audio/wav";
  ext["weba"] = "audio/webm";
  ext["webm"] = "video/webm";
  ext["webmanifest"] = "application/manifest+json";
  ext["webp"] = "image/webp";
  ext["wgt"] = "application/widget";
  ext["wmf"] = "image/wmf";
  ext["woff"] = "font/woff";
  ext["woff2"] = "font/woff2";
  ext["wrl"] = "model/vrml";
  ext["wsdl"] = "application/wsdl+xml";
  ext["wspolicy"] = "application/wspolicy+xml";
  ext["x3d"] = "model/x3d+xml";
  ext["x3db"] = "model/x3d+binary";
  ext["x3dbz"] = "model/x3d+binary";
  ext["x3dv"] = "model/x3d+vrml";
  ext["x3dvz"] = "model/x3d+vrml";
  ext["x3dz"] = "model/x3d+xml";
  ext["xaml"] = "application/xaml+xml";
  ext["xdf"] = "application/xcap-diff+xml";
  ext["xdssc"] = "application/dssc+xml";
  ext["xenc"] = "application/xenc+xml";
  ext["xer"] = "application/patch-ops-error+xml";
  ext["xht"] = "application/xhtml+xml";
  ext["xhtml"] = "application/xhtml+xml";
  ext["xhvml"] = "application/xv+xml";
  ext["xm"] = "audio/xm";
  ext["xml"] = "application/xml";
  ext["xop"] = "application/xop+xml";
  ext["xpl"] = "application/xproc+xml";
  ext["xsd"] = "application/xml";
  ext["xsl"] = "application/xml";
  ext["xslt"] = "application/xslt+xml";
  ext["xspf"] = "application/xspf+xml";
  ext["xvm"] = "application/xv+xml";
  ext["xvml"] = "application/xv+xml";
  ext["yaml"] = "text/yaml";
  ext["yang"] = "application/yang";
  ext["yin"] = "application/yin+xml";
  ext["yml"] = "text/yaml";
  ext["zip"] = "application/zip";

    if(mode == 1)
    {
        vector<string> str;
        str = split(location,'.');
        if(ext.count(str.back()) == 0)
            return(string("Text/plain"));
        else
            return(ext[str.back()]);
    }
    else if(mode == 2)
    {
        for (map<string,string>::iterator i = ext.begin(); i != ext.end(); i++)
        {
            if(i->second == location)
            {
                return(i->first);
            }
        }
        return("");
    }
};
class Statuscode{
    private:
        map<string, string> codes;
        string errorpage;
        int error;
    public:
        Statuscode()
        {
            codes["200"] = "OK";
            codes["404"] = "Not Found";
            codes["500"] = "Internal Server Error";//error with cgi
            codes["400"] = "Bad Request";
            codes["413"] = "Request Entity Too Large";
            codes["405"] = "Method Not Allowed";
            codes["204"] = "No Content";
            codes["201"] = "Created";
            codes["403"] = "Forbidden";
            codes["409"] = "Conflit";
            codes["501"] = "Not Implemented;";
            codes["205"] = "Reset Content;";
            this->error = 0;
        }
        string get_code(int code)
        {
            return ("HTTP/1.1 " + to_string(code) + " " + codes[to_string(code)]);
        }
        void set_error(vector<vector<string> > pages, int code, int size)
        {
            int i = 0;
            vector<vector<string> >::iterator ptr;
            for(ptr = pages.begin() ; ptr != pages.end(); ptr++)
            {
                cout << "is in this error : " << ptr->front()<< " "<< endl;
                if(to_string(code) == ptr->back())
                {
                    this->error = 1;
                    this->errorpage = ptr->front();
                }
            }
            //cout << "is in this error : " <<this->errorpage << " "<< endl;
        }
        string renderpage(string filename,int code)
        {
      
        ifstream f(filename); //taking file as inputstream
        string str;
        if(f) {
        ostringstream ss;
        ss << f.rdbuf(); // reading data
        str = ss.str();
        }
        else
            return("<!DOCTYPE html><html><head><title>" + codes[to_string(code)] + "</title></style</head><body><p><b>" + codes[to_string(code)]  + " " + to_string(code) + "</b></p></body></html>");
    return str;
        
};
        string get_errorhtml(int code, string uploadfile)
        {
             string page;
            if(this->error == 0)
            {
                if(code == 201)
                {
                         page = "<!DOCTYPE html><html><head><title>" + codes[to_string(code)] + "</title></style</head><body><p><b>" + codes[to_string(code)]  + " " + to_string(code) +" here : " + uploadfile + "</b></p></body></html>";
                }
                else
                {
                      
                    page = "<!DOCTYPE html><html><head><title>" + codes[to_string(code)] + "</title></style</head><body><p><b>" + codes[to_string(code)]  + " " + to_string(code) + "</b></p></body></html>";
                }
                return page;
            }
            else
                return(this->renderpage(this->errorpage, code));

        }
};
string getDate()
{
    time_t now = time(0);
    string dt = ctime(&now);
    return dt;
}
class Header
{
    private:
        
        string Server;
        string firstline;
        string date;
        map<string,string> headers;
    public:
        
    Header(server &serv)
    {
        this->Server = "tiza";//serv.get_name(0);
        this->date = getDate();
    }
   
    void set_firstline(string firstline)
    {
        this->firstline = firstline;
    }
    string get_Header()
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
    void setHeader(string key, string value)
    {
        this->headers[key] = value;
    }
};

