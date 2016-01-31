
#include <ego/base/base.h>
#include <ego/util/log/log.h>
#include <ego/util/string.h>


namespace NEgo {

    struct THttpRequest {
        TString Method;
        TString Path;
        TString Version;

        TVector<TPair<TString, TString>> Headers;

        TString Body;
    };

    std::ostream& operator<< (std::ostream& stream, const THttpRequest& httpReq) {
        stream << httpReq.Method << " " << httpReq.Path << " " << httpReq.Version << "\n";
        for(const auto& h: httpReq.Headers) {
            stream << h.first << ": " << h.second << "\n";
        }
        stream << "\n";
        stream << httpReq.Body;
        return stream;
    }

    THttpRequest ParseHttpRequest(const TString&& requestString) {
        std::istringstream inpStream(requestString);

        THttpRequest request;

        TString httpSpecString;
        ENSURE(std::getline(inpStream, httpSpecString), "Premature end of http request: " << httpSpecString);
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



} // namespace NEgo
