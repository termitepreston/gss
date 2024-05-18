#pragma once

#include "beast.hpp"
#include "net.hpp"
#include <boost/smart_ptr.hpp>
#include <string>

class SharedState;

class listener : public boost::enable_shared_from_this<listener> {
    net::io_context &ioc_;
    tcp::acceptor acceptor_;
    boost::shared_ptr<SharedState> state_;

    void fail(beast::error_code ec, const char *what);
    void on_accept(beast::error_code ec, tcp::socket socket);

  public:
    listener(net::io_context &ioc, tcp::endpoint endpoint,
             const boost::shared_ptr<SharedState> &state_);

    void run();
};