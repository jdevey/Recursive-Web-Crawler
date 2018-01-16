//Recursive web crawler...

#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include <vector>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

using namespace std;

std::map<std::string, std::string> modifyMap(std::string k, std::string v, std::map<std::string, std::string> b) {
    b[k] = v;
    return b;
}

bool isHTTP(string s) {
    if (s.size() < 8) return false;
    string compare = "http://";

    for (int i = 0; i < 7; ++i) {
        if (s[i] != compare[i]) return false;
    }
    return true;
}

bool isHTTPS(string s) {
    if (s.size() < 9) return false;
    string compare = "https://";

    for (int i = 0; i < 8; ++i) {
        if (s[i] != compare[i]) return false;
    }
    return true;
}

std::string extractDomain(std::string url) {
    std::regex re("^http.?://[^/]+");
    std::smatch m;

    std::string ret("");

    if (std::regex_search(url, m, re)) {
        ret = m[0].str();
    }
    
    return ret;
}

std::string grabString(std::string originalURL) {
    try {
        curlpp::Cleanup cleaner;
        curlpp::Easy request;
        curlpp::Options::Url curlURL(originalURL);
        request.setOpt(curlURL);
        
        curlpp::Options::FollowLocation follow(true);
        request.setOpt(follow);
        
        std::ostringstream os;
        os << request << std::endl;
        
        std::string sourceCode(os.str());
        return sourceCode;
    }
    catch (curlpp::LogicError& error) {
        return error.what();
        //std::cout << error.what() << std::endl;
    }
    catch (curlpp::LogicError& error) {
        return error.what();
        //std::cout << error.what() << std::endl;
    }
}


void recurseCrawl(std::map <std::string, std::string>* urlList, std::string currentUrl, int currentDepth, int depth) {
    
    //std::string prefix = extractDomain(currentUrl);
    
    std::string sourceCode = grabString(currentUrl);
    
    //std::cout << std::endl << std::endl << sourceCode << std::endl << std::endl << std::endl << std::endl;
    
    //CREDIT TO HUNTER HENRICHSEN FOR THIS REGULAR EXPRESSION
    std::string hrefRegexSource("<a .*?href=\"((?:(https?:\\/\\/)([^\\/\"#]+)|\\/)([^\"]*))\".*?>");
    //std::string hrefRegexSource("<a .*?href=\"((?:(https?:\/\/)([^\/\"#]+)|\/)([^\"]*))\".*?>\"");
    
    std::regex rg(hrefRegexSource);
    std::smatch match;
    std::vector <std::string> list;
    
    while (std::regex_search(sourceCode, match, rg)) {
        for (auto submatch : match) {
            if ((isHTTP(submatch) || isHTTPS(submatch)) && submatch.length() < 60 && urlList->count(submatch) == 0) {
                (*urlList)[submatch] = submatch;
                for (int i = 0; i < currentDepth - 1; ++i) cout << "\t";
                cout << submatch << endl;
                if (currentDepth < depth) recurseCrawl(urlList, submatch, currentDepth + 1, depth);
            }
        }
        //foutz << endl;
        sourceCode = match.suffix().str();
    }
    
}

int main(int argc, char** argv) {
    //std::string originalURL = "http://www-cs-faculty.stanford.edu/~knuth/faq.html";
    std::string originalURL = "http://cs.usu.edu"; //Recommended starting point for testing
    //Default value
    int maxDepth = 3;

    if (argc > 1) originalURL = argv[1];

    if (argc > 2) maxDepth = atoi(argv[2]);

    std::cout << "Recursively crawling the web to a depth of " << maxDepth << " links beginning from " << originalURL <<  "..." << endl << endl;
    
    std::map <std::string, std::string> urlList;
    
    recurseCrawl(&urlList, originalURL, 1, maxDepth);

    return 0;
}
