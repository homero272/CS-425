//////////////////////////////////////////////////////////////////////////////
//
//  HTTPResponse.h
//
//  A class that generates HTTP responses for various requests and error
//    situations

#ifndef __HTTPRESPONSE_H__
#define __HTTPRESPONSE_H__

#include <chrono>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "Connection.h"
#include "HTTPRequest.h"

struct HTTPResponse {

    HTTPResponse(const HTTPRequest& request, std::string root = ".") {

        using clock = std::chrono::system_clock;
        auto time = clock::to_time_t(clock::now());
        _date += std::ctime(&time);
        
        switch (request.type) {
            case HTTPRequest::GET: {
                std::string path = root + request.filename;
                if (request.filename == "/") { path += "index.html"; }

                std::ifstream infile{path};

                if (!infile) { 
                    error(); 
                    break;
                }

                infile.seekg(0, std::ios_base::end);
                size_t length = infile.tellg();
                _data.resize(length);
                infile.seekg(0, std::ios_base::beg);
                infile.read(&_data[0], _data.size());
                
                std::stringstream ss;

                ss  << "HTTP/1.1 200 OK\n"
                    << _date
                    << "Content-Length: " << _data.size() << "\n"
                    << "Content-Type: " << request.contentType() << "\n"
                    << "\n";

                _header = ss.str();
            } break;

            default:
                error();
                break;
        }
    }

    void error()
        { _header = "HTTP/1.1 404 Not Found\n" + _date + "\n"; }

    const std::string& header() const 
        { return _header; }

    //------------------------------------------------------------------------
    //
    //  Operators
    //

    friend Session& operator << (Session& s, const HTTPResponse& r)
        { return s << r._header << r._data; }

    friend std::ostream& operator << (std::ostream& os, const HTTPResponse& r)
        { return os << r._header; }

  private:
    std::string       _header;
    std::string       _date{ "Date: " };
    std::vector<char> _data;
};

#endif // __HTTPRESPONSE_H__