#include "shared_state.hpp"
#include "websocket_session.hpp"

shared_state::shared_state(std::string doc_root, net::io_context &ioc)
    : doc_root_{doc_root}, tasks_queue_{}, timer_{ioc} {}

void shared_state::join(websocket_session *session) {
    std::lock_guard<std::mutex> lock{mutex_};

    sessions_.emplace(session);
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

void shared_state::send(std::string id, std::string message) {
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

list<task> shared_state::tasks() noexcept { return tasks_queue_; }

void shared_state::push_task(task task) {
    std::unique_lock<std::mutex> lock{mutex_};

    tasks_queue_.push_back(task);

    lock.unlock();
    cv_.notify_one();
}

void shared_state::pop_task() {
    std::lock_guard<std::mutex> lock{mutex_};

    tasks_queue_.pop_front();
}

task &shared_state::peek_task() {
    std::lock_guard<std::mutex> lock{mutex_};

    return tasks_queue_.front();
}

// <-[T1 T2 T3 T4 T]<-
// process(T0)
void shared_state::process_tasks() {
    std::unique_lock<std::mutex> lock{mutex_};

    while (tasks_queue_.empty()) {
        cv_.wait(lock);
    }

    spdlog::info("SS::process_tasks waking because of a new task");

    // process task.
    task t = std::move(tasks_queue_.front());

    curr_ = boost::make_shared<task>(t);

    tasks_queue_.pop_front();

    t.print();
    spdlog::info("Processing task of {} sec. duration.",
                 t.duration().seconds());

    timer_.expires_from_now(t.duration());

    auto self = shared_from_this();

    timer_.async_wait([self, t](const boost::system::error_code &ec) {
        spdlog::info("Timer timeout.");

        auto update_msg = t.serialize();
        update_msg["type"] = "update";

        self->send(std::string(t.assignee()), update_msg.dump());

        self->process_tasks();
    });
}

boost::shared_ptr<task> shared_state::current() { return curr_; }
