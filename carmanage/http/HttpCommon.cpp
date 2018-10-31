#include <HttpCommon.h>

const std::string fas::http::Method::GET = "GET";
const std::string fas::http::Method::PUT = "PUT";
const std::string fas::http::Method::HEAD = "HEAD";
const std::string fas::http::Method::TRACE = "TRACE";
const std::string fas::http::Method::POST = "POST";
const std::string fas::http::Method::DELETE = "DELETE";
const std::string fas::http::Method::OPTIONS = "OPTIONS";

fas::http::ServerOptions::ServerOptions() :
    serverPath_("/home/carfile/"),
    flagPath_("/home/carfile/flg/"),
    dataPath_("/home/carfile/data/") {
}

std::string fas::http::ServerOptions::getServerPath() const {
    return this->serverPath_;
}

void fas::http::ServerOptions::setServerPath(const std::string& path) {
    // FIXME : this method is not safe.
    this->serverPath_ = path;
}

std::string fas::http::ServerOptions::getFlagPath() const {
    return this->flagPath_;
}

void fas::http::ServerOptions::setFlagPath(const std::string& path) {
    this->flagPath_ = path;
}

std::string fas::http::ServerOptions::getDataPath() const{
    return this->dataPath_;
}

void fas::http::ServerOptions::setDataPath(const std::string& path){
    this->dataPath_ = path;
}

fas::http::ServerOptions::~ServerOptions() {

}

fas::http::ContentTypes::ContentTypes() {
    suffixTypes_.insert({
        {".doc", "application/msword"},
        {".gif", "image/gif"},
        {".java", "java/*"},
        {".jpe", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".jpg", "application/x-jpg"},
        {".jsp", "text/html"},
        {".mp4", "video/mpeg4"},
        {".mpeg", "video/mpg"},
        {".mpv", "video/mpg"},
        {".ppt", "application/x-ppt"},
        {".wmv", "video/x-ms-wmv"},
        {".xhtml", "text/html"},
        {".xls", "application/x-xls"},
        {".xml", "text/xml"},
        {".apk", "application/vnd.android.package-archive"},
        {".png", "image/png"},
        {".mp3", "audio/mp3"},
        {".midi", "audio/mid"},
        {".jpg", "image/jpeg"},
        {".ico", "application/x-ico"},
        {".htm", "text/html"},
        {".html", "text/html"},
        {".exe", "pplication/x-msdownload"},
        {".dot", "application/msword"},
        {".css", "text/css"},
        {".class", "java/*"},
        {".avi", "video/avi"},
    });
}

std::string fas::http::ContentTypes::getType(const std::string& suffix) {
    auto iter = suffixTypes_.find(suffix);
    if (iter == suffixTypes_.end()) {
        return "text/html; charset=UTF-8";
    }
    return iter->second;
}
