//////////////////////////////////////////////////////////////////////////////
//
//  Connection.h
//
//  An implementation for managing communications between a client and
//    a server through a networking socket connection.
//

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>

#include <atomic>
#include <iostream>
#include <string>
#include <vector>

#include "Check.h"

//////////////////////////////////////////////////////////////////////////////
//
//  --- Session ---
//
//  An encapsulation of the communications between a client and server
//    across a socket.  The Connection class (below) handles the socket
//    and its lifetime.  Once the appropriate handshaking between the
//    client and server is established, the Connection will return a
//    Session to handle the actual receipt and transmission of data between
//    the parties.
//

struct Session {

    //------------------------------------------------------------------------
    //
    //  Constructors and destructors
    //

    Session(int client) : _client(client)
        { /* Empty */ }

    ~Session() {
        if (_client > 0) { CHECK(::close(_client)); }
    }

    //------------------------------------------------------------------------
    //
    //  Methods
    //
    
    int client() const { return _client; }

    void receive(std::string& data, size_t defaultSize = 1024) {
        if (data.empty()) { data.resize(defaultSize); }
        ssize_t bytesRead = 0;
        size_t  bufferSize =  defaultSize;

        while (true) {
            CHECK(bytesRead = ::recv(_client, &data[bytesRead], bufferSize, 0));
            if (bytesRead < bufferSize) { break; }

            data.resize(bytesRead + bufferSize);
        }

        if (data.size() > bytesRead) { data.resize(bytesRead); }
    }

    template <typename T>
    void send(const T& data) {
        size_t bytesSent;
        CHECK(bytesSent = ::send(_client, &data[0], data.size(), 0));
    }

    //------------------------------------------------------------------------
    //
    //  Operators
    //
    
    friend Session& operator >> (Session& session, std::string& s) {
        session.receive(s);
        return session; 
    }

    template <typename T>
    friend Session& operator << (Session& session, const T& v) {
        session.send(v);
        return session;
    }

  private:
    int  _client;
};


//////////////////////////////////////////////////////////////////////////////
//
//  --- Connection ---
//
//  An encapsulation of the communications between a client and server
//    across a socket.  The Connection class (below) handles the socket
//    and its lifetime.  Once the appropriate handshaking between the
//    client and server is established, the Connection will return a
//    Session to handle the actual receipt and transmission of data between
//    the parties.
//

struct Connection {

    //------------------------------------------------------------------------
    //
    //  Constructors and destructors
    //

    Connection(int port = 8000, int maxConnections = 10) {
        CHECK(_socket = ::socket(AF_INET, SOCK_STREAM, 0));

        int option = 1;
        CHECK(::setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, 
            &option, sizeof(int)));

        struct sockaddr_in address{ AF_INET, htons(port), INADDR_ANY };
        
        CHECK(::bind(_socket, (sockaddr*) &address, sizeof(address)));
        CHECK(::listen(_socket, maxConnections));

        struct pollfd pfd{ _socket, POLLIN|POLLERR, 0 };
        CHECK(::poll(&pfd, 1, -1));
    }

    ~Connection() {
        close();
    }

    //------------------------------------------------------------------------
    //
    //  Methods
    //

    operator bool() { return true; }

    int accept() {
        int client;
        CHECK(client = ::accept(_socket, nullptr, nullptr));
        return client;
    }

    void close() {
        CHECK(::shutdown(_socket, SHUT_RD));
        CHECK(::close(_socket));
    }

  private:
    int _socket;
};

#endif // __CONNECTION_H__