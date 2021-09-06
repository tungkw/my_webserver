#include "process.h"
#include <iostream>
#include <sys/file.h>




std::map<std::string, std::string> suf_map({
    {".html", "text/html"},
    {".png", "image/png"},
});

std::map<int, std::string> sta_map({
    {200, "OK"},
    {404, "Not Found"},
});


void process(const HTTPRequest &req, HTTPResponse &res){

    res.ver = "HTTP/1.1";
    res.feas.insert(std::make_pair("Connection", "close"));

    std::string file_name = std::string("res") + req.url;
    if(req.url == "/"){
        file_name += std::string("index.html");
    }

    std::FILE *f = fopen(file_name.c_str(), "r");
    if(f == NULL){
        res.sta = 404;
        file_name = "res/404.html";
        f = fopen(file_name.c_str(), "r");
    }
    else{
        res.sta = 200;
    }

    int p = file_name.find_last_of('.');
    char buff[100];
    std::size_t len;
    do{
        len = fread(&buff[0], 1, 100, f);
        res.cont.append(&buff[0], len);
    }while(len==100&&f!=NULL);
    fclose(f);
    res.des = sta_map[res.sta];
    res.feas.insert(std::make_pair("Content-type", suf_map[file_name.substr(p)]));
    res.feas.insert(std::make_pair("Content-length", std::to_string(res.cont.length())));
}