#include "process.h"
#include <iostream>


void process(const HTTPRequest &req, HTTPResponse &res){
    res.ver = "HTTP/1.1";
    res.sta = 200;
    res.des = "OK";
    res.feas["Content-Type"] = "text/html;charset=utf-8";
    // res.feas["Connection"] = "keep";
    // res.feas["Cache-Control"] = "private";
    // res.feas["Bdpagetype"] = "1";
    // res.feas["Bdqid"] = "0xf3c9743300024ee4";
    // res.feas["Content-Encoding"] = "gzip";
    // res.feas["Date"] = "Fri, 26 Feb 2021 08:44:35 GMT";
    // res.feas["Expires"] = "Fri, 26 Feb 2021 08:44:35 GMT";
    // res.feas["Date"] = "Fri, 26 Feb 2021 08:44:35 GMT";
    // res.feas["Server"] = "BWS/1.1";
    // res.feas["Transfer-Encoding"] = "chunked";
    
    // Set-Cookie: BDSVRTM=13; path=/
    // Set-Cookie: BD_HOME=1; path=/
    // Set-Cookie: H_PS_PSSID=33514_33257_33273_31660_33570_26350; path=/; domain=.baidu.com
    // Strict-Transport-Security: max-age=172800
    // Traceid: 1614329075128412289017566699583927635684
    // X-Ua-Compatible: IE=Edge,chrome=1
    std::FILE *f = fopen("res/index.html", "r");
    char buff[100];
    std::size_t len;
    do{
        len = fread((char*)buff, 1, 100, f);
        res.cont.append(&buff[0], len);
    }while(len == 100);
}