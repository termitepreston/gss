#include "shared_state.hpp"
#include "websocket_session.hpp"

SharedState::SharedState(std::string doc_root, net::io_context &ioc)
    : doc_root_{doc_root}, tasks_queue_{}, timer_{ioc} {}

void SharedState::join(websocket_session *session) {
    std::lock_guard<std::mutex> lock{mutex_};

    sessions_.emplace(session);
}

void SharedState::leave(websocket_session *session) {
    std::lock_guard<std::mutex> lock{mutex_};

    sessions_.erase(session);
}

void SharedState::send(std::string message) {
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

void SharedState::send(std::string id, std::string message) {
    const auto ss = boost::make_shared<const std::string>(std::move(message));

    boost::weak_ptr<websocket_session> session;

    {
        std::lock_guard<std::mutex> lock{mutex_};

        for (auto p : sessions_)
            if (p->uid() == id) {
                spdlog::info("Found websocket session with id = {}", p->uid());
                session = p->weak_from_this();
            }
    }

    if (auto sp = session.lock())
        sp->send(ss);
}

void SharedState::push_task(Task task) {
    std::unique_lock<std::mutex> lock{mutex_};

    tasks_queue_.push(task);

    lock.unlock();
    cv_.notify_one();
}

void SharedState::pop_task() {
    std::lock_guard<std::mutex> lock{mutex_};

    tasks_queue_.pop();
}

Task &SharedState::peek_task() {
    std::lock_guard<std::mutex> lock{mutex_};

    return tasks_queue_.front();
}

void SharedState::process_tasks() {
    std::unique_lock<std::mutex> lock{mutex_};

    while (tasks_queue_.empty()) {
        cv_.wait(lock);
    }

    spdlog::info("SS::process_tasks waking because of a new task");

    // process task.
    Task t = std::move(tasks_queue_.front());
    tasks_queue_.pop();

    t.print();
    spdlog::info("Processing task for {} seconds.", t.duration().seconds());

    timer_.expires_from_now(t.duration());

    auto self = shared_from_this();

    timer_.async_wait([self, t](const boost::system::error_code &ec) {
        spdlog::info("Timer timeout.");
        self->send(std::string(t.assignee()),
                   std::string("Finished processing"));

        self->process_tasks();
    });
}
