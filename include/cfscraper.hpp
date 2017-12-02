#include "duktape.h"
#include <ctype.h>
#include <stdlib.h>
#include <string>
#include <regex>
#include <chrono>
#include <thread>
#include "cpr/cpr.h"

class jsResponse{
public:
    jsResponse();
    ~jsResponse();
    cpr::Response get(std::string url, cpr::Header originalHeaders = cpr::Header());
    
private:
    bool setup(std::string html);    
    std::string getNewLoc(std::string url);  
    int answer;
    int domainLength;
    cpr::Cookies cookies;
    int getAnswer();    
    std::string userAgent;
    std::string question;
    std::string jschl_vc;
    std::string pass;

};
