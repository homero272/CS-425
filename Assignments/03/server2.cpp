
#include <iostream>
#include <semaphore>
#include <thread>
#include <chrono>

// These are custom classes that encode the web transactions.  They're
//   actually quite simple (mostly because we're solving a very limited)
//   problem.
#include "Connection.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"

// This is the unique networking "port" that your web server is communicating
//   with the web browser with.  Make sure to use you're unique port, otherwise
//   you'll stomp on other people trying to do the same thing.
//
// Common ports, particularly for this web and related stuff are:
//   - 22 : ssh port
//   - 25 : email port
//   - 80 : http port (unencrypted web connection)
//   - 8080 : https port (encrypted web connection)
//
//  (Don't use any of them.  Generally, above 9000 is usually pretty clear)
//
const uint16_t DefaultPort = 8108; // Update this variable with your assigned port value


template <typename Data, ptrdiff_t MaxDataSlots = 8>
class RingBuffer {

    std::vector<Data> buffer;

    using size_type = std::vector<Data>::size_type;

    struct Index : std::atomic<size_type> {
        Index(auto i) {
            this->store(i);
        }

        void operator ++ () {
            auto v = this->load();
            ++v %= MaxDataSlots;
            this->store(v);
        }
    };

    Index nextStorable = 0;
    Index nextReadable = 0;

    std::counting_semaphore<MaxDataSlots> storable{MaxDataSlots};
    std::counting_semaphore<MaxDataSlots> readable{0};

    std::mutex storedMutex;
    std::mutex readMutex;

public:
     RingBuffer(size_t count = MaxDataSlots) : buffer(count) { 

     }


    void store(const Data& t) {
        storable.acquire();
        {
            std::lock_guard{storedMutex};
            //std::cout << "storing " << t << " at " << nextStorable << std::endl;
            this->buffer.at(nextStorable) = t;
            ++nextStorable;
        }
        readable.release();
    }

    Data read() {
        Data t;
        readable.acquire();
        {
            std::lock_guard{readMutex};
            t = this->buffer.at(nextReadable);
            //std::cout << "reading " << t << " at " << nextReadable << std::endl;
            ++nextReadable;
        }
        storable.release();
        return t;
    }
};


int main(int argc, char* argv[]) {
    uint16_t port = argc > 1 ? std::stol(argv[1]) : DefaultPort;
    bool quittingTime = false;

    RingBuffer<int, 8> buffer;

    // Producer: Accept connections and store them in the buffer
    std::jthread producer{[&]() {
        Connection connection(port);

        while (connection) {
            int client = connection.accept();
            buffer.store(client);
        }

        //std::cout << "Producer: I'm done\n";
    }};

    // Consumer: Process sessions from the buffer
    std::jthread consumer{[&]() {
        

        while (true) {
            int client = buffer.read();
            if(client < 0){break;}
            // Create a session for the client
            Session session(client);
            const char* root = "/home/faculty/shreiner/public_html/03";

            // Handle the session
            std::string msg;
            session >> msg;

            HTTPRequest request(msg);
            HTTPResponse response(request, root);

            session << response;
        }

        //std::cout << "Consumer: I'm done\n";
    }};

    // Sleep for some time (you can replace this with your termination logic)
    std::jthread boss{[&]() {
        std::chrono::milliseconds workingHours(3);
        std::this_thread::sleep_for(workingHours);
        quittingTime = true;
    }};
    

    // Join the threads to ensure they finish
    producer.join();
    consumer.join();
    boss.join();
}

