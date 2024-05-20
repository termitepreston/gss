#pragma once

#include "beast.hpp"
#include "net.hpp"
#include "shared_state.hpp"

#include <cstdlib>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

class shared_state;

class websocket_session
    : public boost::enable_shared_from_this<websocket_session> {
    beast::flat_buffer buffer_;
    websocket::stream<beast::tcp_stream> ws_;
    boost::shared_ptr<shared_state> state_;
    std::vector<boost::shared_ptr<const std::string>> queue_;
    std::string uid_;

    void fail(beast::error_code ec, const char *what);
    void on_accept(beast::error_code ec);
    void on_read(beast::error_code ec, size_t bytes_transferred);
    void on_write(beast::error_code ec, size_t bytes_transferred);

  public:
    websocket_session(tcp::socket &&socket,
                      const boost::shared_ptr<shared_state> &state);

    ~websocket_session();

    template <class Body, class Allocator>
    void run(http::request<Body, http::basic_fields<Allocator>> req);

    void send(boost::shared_ptr<std::string const> const &ss);
    std::string uid() const noexcept;

  private:
    void on_send(boost::shared_ptr<std::string const> const &ss);
};

#include <iostream>

using namespace std;

template <class Body, class Allocator>
void websocket_session::run(
    http::request<Body, http::basic_fields<Allocator>> req) {

    ws_.set_option(
        websocket::stream_base::timeout::suggested(beast::role_type::server));

    ws_.set_option(
        websocket::stream_base::decorator([](websocket::response_type &res) {
            res.set(http::field::server, "Generic Service Scheduler");
        }));

    ws_.async_accept(req,
                     beast::bind_front_handler(&websocket_session::on_accept,
                                               shared_from_this()));
}