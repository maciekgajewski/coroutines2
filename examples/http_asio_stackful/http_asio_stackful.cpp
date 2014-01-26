#include "socket_streambuf.hpp"

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/NetException.h>

#include <iostream>
#include <memory>
#include <ctime>

// Goal:
// HTTP server responding with 14-byte response "hello, world!\n"
// using asio's stackful coroutines

// case-insensitive comparison
template<typename StringA, typename StringB>
bool ci_equal(const StringA& a, const StringB& b)
{
    return std::equal(
        std::begin(a), std::end(a), std::begin(b),
        [](char ac, char bc) { return (ac & 0x1f) == (bc & 0x1f); }
        );
}


// this could be a pure function, but I can't move parameters into it, hence the class
class client_connection
{
private:
    ba::ip::tcp::socket socket_;

public:
    client_connection(ba::ip::tcp::socket&& s) : socket_(std::move(s)) { }

    ~client_connection() { std::cout << "connection destroyed" << std::endl; }

    void run(ba::yield_context yield)
    {
        socket_istreambuf input_buffer(socket_, yield);
        socket_ostreambuf output_buffer(socket_, yield);

        std::istream input_stream(&input_buffer);
        std::ostream output_stream(&output_buffer);

        // HTTP request processign loop
        while(true)
        {
            Poco::Net::HTTPRequest request;

            // read request
            try
            {
                request.read(input_stream);
            }
            catch(const Poco::Net::NoMessageException&)
            {
                std::cout << "contection ended" << std::endl;
                break;
            }

            // honour HTTP 1.0 vs 1.1 and Connection: close
            bool keep_alive = false;
            if (request.getVersion() == Poco::Net::HTTPMessage::HTTP_1_1 && ci_equal(request.get("Connection", ""), "close"))
            {
                keep_alive = true;
            }
            else if (ci_equal(request.get("Connection", ""), "keep-alive"))
            {
                keep_alive = true;
            }

            // start building response
            Poco::Net::HTTPResponse response;

            // set Date. This is ugly, this should be one-lines with std::put_time
            std::time_t now = std::time(nullptr);
            char date_buffer[64];
            std::strftime(date_buffer, 64, "%a, %d %b %Y %T GMT", std::gmtime(&now));
            response.add("Date", date_buffer);

            // set keep-alive
            if (keep_alive)
            {
                response.add("Connection", "Keep-Alive");
            }

            // handling is separated and potentially customized
            handle_request(request, input_stream, response, output_stream);

            if (!keep_alive)
                break;
        }
    }

    void handle_request(const Poco::Net::HTTPRequest& request, std::istream& input_stream, Poco::Net::HTTPResponse& response, std::ostream& output_stream)
    {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        response.add("Content-Length", "14");
        response.add("Content-Type", "text/plain");

        response.write(output_stream);
        output_stream << "hello, world!\n";
        output_stream.flush();
    }
};

int main(int, char**)
{
    ba::io_service io_service;



    ba::spawn(io_service, [&io_service](ba::yield_context yield)
    {
        ba::ip::tcp::acceptor acceptor(io_service);
        ba::ip::tcp::endpoint endpoint(ba::ip::address(), 8080);

        acceptor.open(endpoint.protocol());
        acceptor.set_option(ba::ip::tcp::acceptor::reuse_address(true));
        acceptor.bind(endpoint);
        acceptor.listen();

        std::cout << "Listening for incoming connections" << std::endl;

        while(true)
        {
            ba::ip::tcp::endpoint peer_endpoint;
            ba::ip::tcp::socket peer_socket(io_service);
            acceptor.async_accept(peer_socket, peer_endpoint, yield);

            std::cout << "Connection received from " << peer_endpoint << std::endl;

            ba::strand strand(io_service);
            std::shared_ptr<client_connection> cc = std::make_shared<client_connection>(std::move(peer_socket));

            ba::spawn(strand,  [cc](ba::yield_context yield) { cc->run(yield); });
        }
    });

    //ba::io_service::work work(io_service);

    std::cout << "Entering main loop" << std::endl;
    io_service.run();

}

