#include "shared_state.hpp"
#include "websocket_session.hpp"

shared_state::shared_state(std::string doc_root) : doc_root_{doc_root} {}

void shared_state::join(websocket_session *session) {
    std::lock_guard<std::mutex> lock{mutex_};

    sessions_.insert(session);
}

void shared_state::leave(websocket_session *session) {
    std::lock_guard<std::mutex> lock{mutex_};

    sessions_.erase(session);
}

void shared_state::send(std::string message) {
    // put the message inside a shared pointer.
    const auto ss = boost::make_shared<const std::string>(std::move(message));

    // make a local list of all the weak pointers
    // representing the sessions, so we can do the actual
    // sending without holding the mutex:

    std::vector<boost::weak_ptr<websocket_session>> v;

    {
        std::lock_guard<std::mutex> lock{mutex_};
        v.reserve(sessions_.size());

        for (auto p : sessions_)
            v.emplace_back(p->weak_from_this());
    }

    for (auto const &wp : v)
        if (auto sp = wp.lock())
            sp->send(ss);
}