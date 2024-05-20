#pragma once

#include <boost/smart_ptr.hpp>
#include <memory>

#include "Task.hpp"
#include <condition_variable>
#include <list.hpp>
#include <memory>
#include <mutex>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_set>
#include <vector>

class websocket_session;

class shared_state : public boost::enable_shared_from_this<shared_state> {
    const std::string doc_root_;

    std::mutex mutex_;
    std::condition_variable cv_;

    std::unordered_set<websocket_session *> sessions_;
    list<task> tasks_queue_;
    boost::shared_ptr<task> curr_;
    boost::asio::deadline_timer timer_;

  public:
    explicit shared_state(std::string doc_root, net::io_context &ioc);

    std::string const &doc_root() const noexcept { return doc_root_; }

    void process_tasks();
    list<task> tasks() noexcept;

    void join(websocket_session *session);
    void leave(websocket_session *session);
    void send(std::string message);
    void send(std::string id, std::string message);

    void push_task(task task);
    void pop_task();
    task &peek_task();

    boost::shared_ptr<task> current();
};