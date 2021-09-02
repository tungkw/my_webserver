#include "http.h"


/*
[' ', '\n', '\r', ':', others]

LINE_ACT + ' '      -> LINE_URL
LINE_ACT + *    -> LINE_ACT

LINE_URL + ' '      -> LINE_VER
LINE_URL + *        -> LINE_URL

LINE_VER + '\r'     -> LINE_FIN
LINE_VER + *        -> LINE_VER

LINE_FIN + '\n'     -> HEAD_KEY
LINE_FIN + '\r'     -> LINE_FIN
LINE_FIN + *        -> LINE_VER

HEAD_KEY + ':'      -> HEAD_VAL
HEAD_KEY + '\r'     -> HEAD_FIN
HEAD_KEY + *        -> HEAD_KEY

HEAD_VAL + '\r'     -> KEY_FIN
HEAD_VAL + *        -> HEAD_VAL

KEY_FIN + '\n'      -> HEAD_KEY
KEY_FIN + '\r'      -> KEY_FIN
KEY_FIN + *         -> HEAD_VAL

HEAD_FIN + '\n'     -> CONT_FIN
*/
bool http_request_sparse(const std::string &buff, HTTPRequest &req){
    std::string key, val;
    
    for(int i = 0 ; i < buff.length(); i++){
        char t = buff[i];
        switch (req.state)
        {
        case LINE_ACT:
            if(t == ' ') req.state = LINE_URL;
            else req.act += t;
            break;
        case LINE_URL:
            if(t == ' ') req.state = LINE_VER;
            else req.url += t;
            break;
        case LINE_VER:
            if(t == '\r') req.state = LINE_FIN;
            else req.ver += t;
            break;
        case LINE_FIN:
            if(t == '\n') req.state = HEAD_KEY;
            else if(t == '\r'){
                req.ver += t;
                req.state = HEAD_KEY;
            }
            else{
                req.ver.push_back('\r');
                req.ver += t;
                req.state = LINE_VER;
            }
            break;
        case HEAD_KEY:
            if(t == ':') req.state = HEAD_VAL;
            else if(t == '\r') req.state = HEAD_FIN;
            else key += t;
            break;
        case HEAD_VAL:
            if(t == '\r') req.state = KEY_FIN;
            else{
                if(t == ' ' && val.size()==0) break;
                else val += t;
            }
            break;
        case KEY_FIN:
            if(t == '\n'){
                req.feas.insert(std::make_pair(key, val));
                key.clear();
                val.clear();
                req.state = HEAD_KEY;
            }
            else if(t == '\r') val += t;
            else{
                val.push_back('\r');
                val += t;
                req.state = HEAD_VAL;
            }
            /* code */
            break;
        case HEAD_FIN:
            req.state = CONT_FIN;
            break;
        case CONT_FIN:
            req.cont += t;
            break;
        default:
            break;
        }
    }
    if(req.state == CONT_FIN){
        auto cont_len = req.feas.find("content-length");
        if(cont_len != req.feas.end() && req.cont.size() < std::stoi(cont_len->second)) return false;
        else{
            req.state = LINE_ACT;
            return true;
        }
    }
    else return false;
}


bool http_response_build(const HTTPResponse &resp, std::string &buff){
    buff += resp.ver + " " + std::to_string(resp.sta) + " " + resp.des + "\r\n";
    for(auto iter = resp.feas.begin(); iter!=resp.feas.end(); iter++){
        buff += iter->first + ": " + iter->second + "\r\n";
    }
    buff += "\r\n" + resp.cont;
    return true;
}

