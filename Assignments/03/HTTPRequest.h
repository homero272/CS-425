//////////////////////////////////////////////////////////////////////////////
//
//  HTTPRequest.h
//
//  A class representing an parsed HTTP request

#ifndef __HTTPREQUEST_H__
#define __HTTPREQUEST_H__

#include <map>
#include <string>
#include <sstream>

struct HTTPRequest {

    enum RequestType { GET, POST, ERROR };
    enum ContentType { HTML, JPEG, PNG };

    using Options = std::map<std::string, std::string>;

    std::string  request;
    RequestType  type;
    std::string  filename;
    std::string  protocol;

    HTTPRequest(const std::string& request) : request{request} {
        std::istringstream req{ request };

        req >> _typeStr;
        if (_typeStr == "GET") {
            type = GET;

            // We know the format of a GET command is
            //   GET /<filename> <protocol>
            req >> filename;

            {
                // Determine the requested filetype based on its
                //   filename extensions
                auto pos = filename.find('.') + 1;
                std::string ext{&filename[pos]};

                using ContentMap = std::map<std::string, ContentType>;
                ContentMap contentTypes = {
                    { "html", HTML },
                    { "jpg", JPEG },
                    { "jpeg", JPEG },
                    { "png", PNG }
                };

                _contentType = contentTypes[ext];
            }

            req >> protocol;

            while (req) {
                std::string  tmp;
                std::getline(req, tmp);

                if (tmp.empty()) { break; }

                // Skip Windows stupidity
                if (tmp == "\r") { continue; }

                auto pos = tmp.find(':');
                std::string key{tmp, 0, pos};
                
                pos += 2;  // Skip ": "
                
                std::string value{&tmp[pos]};
                if (value.ends_with('\r')) { value.pop_back(); }
                
                _options.insert({key, value});
            }
        }
    }

    std::string contentType() const {
        using ContentMap = std::map<ContentType, std::string>;
        ContentMap contentTypes{
            { HTML, "text/html" },
            { JPEG, "image/jpeg" },
            { PNG, "image/png" }
        };

        return contentTypes[_contentType];
    }

    std::string options() const {
        std::stringstream ss;

        for (auto& [key, value]: _options) {
            ss << "    " << key << ": " << value << "\n";
        }

        return std::move(ss.str());
    }

    friend std::ostream& operator << (std::ostream& os, const HTTPRequest& r)
        { return os << r._typeStr << " " << r.filename << " " << r.protocol; }

  private:
    std::string  _typeStr;
    ContentType  _contentType;
    Options      _options;

};

#endif // __HTTPREQUEST_H__