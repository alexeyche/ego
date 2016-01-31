
#include <ego/base/base.h>
#include <ego/util/log/log.h>
#include <ego/util/string.h>

#include <ctime>

namespace NEgo {

    struct THttpRequest {
        TString Method;
        TString Path;
        TString Version;

        TVector<TPair<TString, TString>> Headers;

        TString Body;
    };

    struct THttpResponse {
        TString Version;
        ui32 Code;
        TString Status;

        TVector<TPair<TString, TString>> Headers;

        TString Body;
    };

    std::ostream& operator<< (std::ostream& stream, const THttpRequest& httpReq) {
        stream << httpReq.Method << " " << httpReq.Path << " " << httpReq.Version << "\r\n";
        for(const auto& h: httpReq.Headers) {
            stream << h.first << ": " << h.second << "\r\n";
        }
        stream << "\r\n";
        stream << httpReq.Body;
        return stream;
    }

    std::ostream& operator<< (std::ostream& stream, const THttpResponse& httpResp) {
        stream << httpResp.Version << " " << httpResp.Code << " " << httpResp.Status << "\r\n";
        for(const auto& h: httpResp.Headers) {
            stream << h.first << ": " << h.second << "\r\n";
        }
        stream << "\r\n";
        stream << httpResp.Body;
        return stream;
    }

    THttpRequest ParseHttpRequest(const TString&& requestString) {
        std::istringstream inpStream(requestString);

        THttpRequest request;

        TString httpSpecString;
        ENSURE(std::getline(inpStream, httpSpecString), "Premature end of http request: " << requestString);
        httpSpecString = NStr::Trim(httpSpecString);
        TVector<TString> httpSpec = NStr::Split(httpSpecString, ' ');

        ENSURE(httpSpec.size() >= 3, "Bad http specifications: " << httpSpecString);
        request.Method = httpSpec[0];
        request.Path = httpSpec[1];
        request.Version = httpSpec[2];

        TVector<TPair<TString, TString>> headers;
        while (true) {
            TString headerString;
            ENSURE(std::getline(inpStream, headerString), "Premature end of http headers: " << requestString);

            if (NStr::Trim(headerString).empty()) {
                break;
            }

            TVector<TString> headersVals = NStr::Split(headerString, ':', 1);
            ENSURE(headersVals.size() == 2, "Failed to parse headers: " << headerString);

            request.Headers.push_back(MakePair(headersVals[0], headersVals[1]));
        }

        TString line;
        while (std::getline(inpStream, line)) {
            request.Body += line;
        }
        return request;
    }

    TString GetDateAndTime() {
        time_t rawtime;
        struct tm * timeinfo;
        char buffer[80];

        time (&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S",timeinfo);
        return TString(buffer);
    }

    class TResponseBuilder {
    public:
        TResponseBuilder(const THttpRequest& req) {
            Response.Version = req.Version;
        }

        TResponseBuilder& StaticFile(TString path) {
            TVector<TString> pathSpl = NStr::Split(path, '?', 1);
            TString file = NStr::LStrip(pathSpl[0], "/");
            
            std::ifstream f(file);
            if(!f) {
                throw TEgoFileNotFound() << "Failed to read file " << file;
            }
            if (NStr::EndsWith(file, ".html")) {
                TString line;
                while (std::getline(f, line)) {
                    std::regex e("(<!--[ ]*#include[ ]+virtual[ ]*=[ ]*\"(.*)\"[ ]*-->)");
                    std::smatch match;
                    if (std::regex_search(line, match, e) && match.size()>1) {
                        TString includeFile = match.str(2);
                        ENSURE(NStr::EndsWith(includeFile, ".html"), "Can't make SSI for non html files");
                        L_DEBUG << "SSI for " << includeFile;
                        StaticFile(includeFile);
                    }
                    Response.Body += line;    
                }
            } else {
                Response.Body += TString((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());    
            }
            
            DeduceMimeTypeFromFile(file);
            return *this;
        }
        
        void DeduceMimeTypeFromFile(TString path) {
            if (NStr::EndsWith(path, ".css")) {
                Response.Headers.push_back(MakePair("Content-Type", "text/css"));    
            } else 
            if (NStr::EndsWith(path, ".html")) {
                Response.Headers.push_back(MakePair("Content-Type", "text/html"));    
            } else 
            if (NStr::EndsWith(path, ".js")) {
                Response.Headers.push_back(MakePair("Content-Type", "application/javascript"));    
            } else 
            if (NStr::EndsWith(path, ".woff")) {
                Response.Headers.push_back(MakePair("Content-Type", "application/x-font-woff"));    
            } else
            if (NStr::EndsWith(path, ".ttf")) {
                Response.Headers.push_back(MakePair("Content-Type", "application/octet-stream"));    
            } else {
                L_DEBUG << "Can't deduce mime type for " << path;
                Response.Headers.push_back(MakePair("Content-Type", "text/html"));    
            }
        }
        
        TResponseBuilder& Good() {
            Response.Code = 200;
            Response.Status = "OK";
            return *this;
        }
        
        TResponseBuilder& NotFound() {
            Response.Code = 404;
            Response.Status = "Not found";
            return *this;
        }

        TResponseBuilder& InternalError() {
            Response.Code = 500;
            Response.Status = "Internal Server Error";
            return *this;
        }

        TString& Body() {
            return Response.Body;
        }

        TResponseBuilder& Body(const TString&& body) {
            Response.Body = body;
            return *this;
        }

        THttpResponse FormResponse() {
            Response.Headers.push_back(MakePair("Content-Length", TString(NStr::TStringBuilder() << Response.Body.size())));
            Response.Headers.push_back(MakePair("Connection", "Closed"));
            Response.Headers.push_back(MakePair("Date", GetDateAndTime()));
            Response.Headers.push_back(MakePair("Server", "EgoServer"));
            Response.Headers.push_back(MakePair("LastModified", GetDateAndTime()));
            return Response;
        }

    private:
        THttpResponse Response;
    };

} // namespace NEgo
