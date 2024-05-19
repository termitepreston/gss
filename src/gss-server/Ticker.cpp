#include "Ticker.hpp"

Ticker::Ticker(net::io_context &ioc, boost::posix_time::seconds interval,
               const boost::shared_ptr<SharedState> &state)
    : ioc_{ioc}, interval_{interval}, timer_{ioc, interval}, state_{state} {}

void Ticker::tick(const boost::system::error_code &ec) {
    spdlog::info("ticking...");

    auto now = boost::posix_time::second_clock::local_time();

    state_->send("ticking.");

    timer_.expires_at(timer_.expires_at() + interval_);

    auto self = shared_from_this();

    timer_.async_wait(
        [self](const boost::system::error_code &ec) { self->tick(ec); });
}

void Ticker::run() {
    auto self = shared_from_this();

    timer_.async_wait([self](const boost::system::error_code &ec) {
        spdlog::info("Initial tick.");
        self->tick(ec);
    });
}