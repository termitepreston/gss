#pragma once

#include "beast.hpp"
#include "net.hpp"
#include <boost/smart_ptr.hpp>
#include <router.hpp>
#include <string>

class shared_state;
class http_session;

class listener : public boost::enable_shared_from_this<listener> {
    net::io_context &ioc_;
    tcp::acceptor acceptor_;
    boost::shared_ptr<shared_state> state_;
    boost::shared_ptr<boost::urls::router<
        std::function<void(http_session &, boost::urls::matches)>>>
        router_;

    void fail(beast::error_code ec, const char *what);
    void on_accept(beast::error_code ec, tcp::socket socket);

  public:
    listener(net::io_context &ioc, tcp::endpoint endpoint,
             const boost::shared_ptr<shared_state> &state,
             boost::shared_ptr<boost::urls::router<
                 std::function<void(http_session &, boost::urls::matches)>>>
                 router);

    void run();
};