#pragma once

#include "beast.hpp"
#include "net.hpp"
#include "shared_state.hpp"
#include <boost/optional.hpp>
#include <boost/smart_ptr.hpp>
#include <cstdlib>
#include <memory>
#include <spdlog/spdlog.h>

class shared_state;

class Ticker : public boost::enable_shared_from_this<Ticker> {
    net::io_context &ioc_;
    boost::posix_time::seconds interval_;
    net::deadline_timer timer_;
    boost::shared_ptr<shared_state> state_;

    void tick(const boost::system::error_code &ec);

  public:
    Ticker(net::io_context &ioc, boost::posix_time::seconds interval,
           const boost::shared_ptr<shared_state> &state);
    void run();
};