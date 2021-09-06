#ifndef __M_HTTP_H
#define __M_HTTP_H
#include <string>
#include <map>



enum HTTPState{
    LINE_ACT,
    LINE_URL,
    LINE_VER,
    LINE_FIN,
    HEAD_KEY,
    HEAD_VAL,
    KEY_FIN,
    HEAD_FIN,
    CONT_FIN,
};

struct HTTPRequest{
    std::string act;
    std::string url;
    std::string ver;
    std::map<std::string, std::string> feas;
    std::string cont;
    HTTPState state=LINE_ACT;
};

struct HTTPResponse{
    std::string ver;
    int sta;
    std::string des;
    std::map<std::string, std::string> feas;
    std::string cont;
};

bool http_request_sparse(const std::string &buff, HTTPRequest &req);

bool http_response_build(const HTTPResponse &resp, std::string &buff);

#endif