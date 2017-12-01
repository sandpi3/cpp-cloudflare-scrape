#include "cfscraper.hpp"

jsResponse::jsResponse(){

    const std::string ua[3] = {"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_2) AppleWebKit/601.3.9 (KHTML, like Gecko) Version/9.0.2 Safari/601.3.9",
                              "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/47.0.2526.111 Safari/537.36",
                              "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:15.0) Gecko/20100101 Firefox/15.0.1"
                            };
    srand (time(NULL));
    int i = rand() % 3;

    this->userAgent = ua[i];
}

std::string jsResponse::getNewLoc(std::string url){
    std::string domain;

    std::smatch m;
    std::regex_search(url, m, std::regex("^(?:https?:\\/\\/)?(?:[^@\\n]+@)?(?:www\\.)?([^:\\/\\n]+)"));
    domain = m[0];

    this->domainLength = m[1].length();

    return domain + "/cdn-cgi/l/chk_jschl";
}

cpr::Response jsResponse::get(std::string url, cpr::Header originalHeaders){
    originalHeaders["User-Agent"] = this->userAgent;
    auto r = cpr::Get(cpr::Url{url}, originalHeaders);

    if( r.status_code == 503 && r.header["Server"] == "cloudflare-nginx" && r.text.find("jschl_vc") != std::string::npos && r.text.find("jschl_vc") != std::string::npos){
        if(this->setup(r.text)){
            std::string newUrl = this->getNewLoc(url);
            int answer = this->domainLength + this->answer;
            cpr::Parameters pms = cpr::Parameters{
                {"jschl_vc", this->jschl_vc},
                {"pass", this->pass},
                {"jschl_answer", std::to_string(answer)}};
            
            cpr::Response nr = cpr::Get(cpr::Url{newUrl}, r.cookies, cpr::Header{{"Referer", url}, {"User-Agent", this->userAgent}}, pms);

            return cpr::Get(cpr::Url{url}, nr.cookies, originalHeaders);
        }
    }

    return r;  
}

bool jsResponse::setup(std::string html){
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    std::regex r("setTimeout\\(function\\(\\)\\{\\s+(var "
    "s,t,o,p,b,r,e,a,k,i,n,g,f.+?\\r?\\n[\\s\\S]+?a\\.value =.+?)\\r?\\n");
    std::smatch m;
    std::regex_search(html, m, r);
    std::string q = m[1];
    if(q != ""){
        std::regex_search(q, m, std::regex("a\\.value = (parseInt\\(.+?\\)).+"));
        q = std::regex_replace(q, std::regex("a\\.value = (parseInt\\(.+?\\)).+"), "");
        std::string m2 = m[1];
        if(m2 == "")
            return false;
        q += m2;
        q = std::regex_replace(q, std::regex("\\s{3,}[a-z](?: = |\\.).+"), "");

        std::regex_search(html, m, std::regex("name=\"jschl_vc\" value=\"(\\w+)\""));
        if(m[1] != "")
            this->jschl_vc = m[1];
        else return false;
        
        std::regex_search(html, m, std::regex("name=\"pass\" value=\"(.+?)\""));
        if(m[1] != "")
            this->pass = m[1];
        else return false;

        if(q != "")
            this->question = q;
        else
            return false;

        this->getAnswer();
        return true;
    }
    return false;
}

int jsResponse::getAnswer(){

    duk_context *ctx = duk_create_heap_default();
    duk_eval_string(ctx, this->question.c_str());
    const char *charInt = duk_to_string(ctx, -1);
    char *c;

    int r = 0;

    if(charInt)
        r = strtol(charInt, &c, 10);

    duk_pop(ctx);
    duk_destroy_heap(ctx);

    this->answer = r;

    return r;
}

jsResponse::~jsResponse(){
    
}
