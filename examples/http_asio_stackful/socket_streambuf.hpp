#pragma once

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

#include <streambuf>

namespace ba = boost::asio;

class socket_istreambuf : public std::streambuf
{
public:

    using std::streambuf::int_type;
    using std::streambuf::traits_type;

    explicit socket_istreambuf(ba::ip::tcp::socket& sock, ba::yield_context yield)
    : socket_(sock), yield_(yield)
    {
        setg(end(), end(), end());
    }

    socket_istreambuf(const socket_istreambuf&) = delete;

protected:

    virtual int_type underflow() override
    {
        if (gptr() < egptr())
        {
//            std::cout << "STREAM: buffer not exhausetd, why are you bothering me?" << std::endl;
            return traits_type::to_int_type(*gptr());
        }
        else
        {
            //std::cout << "STREAM: need to refill..." << std::endl;
            // need to load some data
            std::size_t n = socket_.async_read_some(ba::buffer(buffer_, BUFFER_SIZE), yield_);
            if (n == 0)
            {
                return traits_type::eof();
            }
            else
            {
                //std::cout << "STREAM: " << n << " bytes added to buffer" << std::endl;
                setg(begin(), begin(), begin() + n);
                return traits_type::to_int_type(*gptr());
            }
        }
    }

private:

    static constexpr unsigned BUFFER_SIZE = 4096;

    typedef std::array<char, BUFFER_SIZE> buffer_type;

    const char* begin() const { return buffer_.data(); }
    const char* end() const { return buffer_.data() + BUFFER_SIZE; }

    char* begin() { return buffer_.data(); }
    char* end() { return buffer_.data() + BUFFER_SIZE; }

    buffer_type buffer_;
    ba::ip::tcp::socket& socket_;
    ba::yield_context yield_;
};


class socket_ostreambuf: public std::streambuf
{
public:

    using std::streambuf::int_type;
    using std::streambuf::traits_type;

    explicit socket_ostreambuf(ba::ip::tcp::socket& sock, ba::yield_context yield)
    : socket_(sock), yield_(yield)
    {
        setp(begin(), end());
    }

    socket_ostreambuf(const socket_istreambuf&) = delete;

protected:

    virtual int_type overflow(int_type ch) override
    {
        ba::async_write(socket_, ba::buffer(begin(), pptr() - begin()), yield_);
        if (ch != traits_type::eof())
        {
            buffer_[0] = traits_type::to_char_type(ch);
            setp(begin()+1, end());
        }
        else
        {
            setp(begin(), end());
        }
        return traits_type::to_int_type('a');
    }

    virtual int sync()
    {
        overflow(traits_type::eof());
        return 0;
    }

private:

    static constexpr unsigned BUFFER_SIZE = 4096;

    typedef std::array<char, BUFFER_SIZE> buffer_type;

    const char* begin() const { return buffer_.data(); }
    const char* end() const { return buffer_.data() + BUFFER_SIZE; }

    char* begin() { return buffer_.data(); }
    char* end() { return buffer_.data() + BUFFER_SIZE; }

    buffer_type buffer_;
    ba::ip::tcp::socket& socket_;
    ba::yield_context yield_;
};

