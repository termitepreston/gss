#include "listener.hpp"
#include "http_session.hpp"
#include <iostream>

listener::listener(
    net::io_context &ioc, tcp::endpoint endpoint,
    boost::shared_ptr<shared_state> const &state,
    boost::shared_ptr<boost::urls::router<
        std::function<void(http_session &, boost::urls::matches)>>>
        router)
    : ioc_(ioc), acceptor_(ioc), state_(state), router_{router} {
    beast::error_code ec;

    // Open the acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
        fail(ec, "open");
        return;
    }

    // Allow address reuse
    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    if (ec) {
        fail(ec, "set_option");
        return;
    }

    // Bind to the server address
    acceptor_.bind(endpoint, ec);
    if (ec) {
        fail(ec, "bind");
        return;
    }

    // Start listening for connections
    acceptor_.listen(net::socket_base::max_listen_connections, ec);
    if (ec) {
        fail(ec, "listen");
        return;
    }
}

void listener::run() {
    // The new connection gets its own strand
    acceptor_.async_accept(
        net::make_strand(ioc_),
        beast::bind_front_handler(&listener::on_accept, shared_from_this()));
}

// Report a failure
void listener::fail(beast::error_code ec, char const *what) {
    // Don't report on canceled operations
    if (ec == net::error::operation_aborted)
        return;
    std::cerr << what << ": " << ec.message() << "\n";
}

// Handle a connection
void listener::on_accept(beast::error_code ec, tcp::socket socket) {
    if (ec)
        return fail(ec, "accept");
    else
        // Launch a new session for this connection
        boost::make_shared<http_session>(std::move(socket), state_, router_)
            ->run();

    // The new connection gets its own strand
    acceptor_.async_accept(
        net::make_strand(ioc_),
        beast::bind_front_handler(&listener::on_accept, shared_from_this()));
}