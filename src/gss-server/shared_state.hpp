#pragma once

#include <boost/smart_ptr.hpp>

#include "Task.hpp"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_set>
#include <vector>

class websocket_session;

class SharedState : public boost::enable_shared_from_this<SharedState> {
    const std::string doc_root_;

    std::mutex mutex_;
    std::condition_variable cv_;

    std::unordered_set<websocket_session *> sessions_;
    std::queue<Task> tasks_queue_;
    boost::asio::deadline_timer timer_;

  public:
    explicit SharedState(std::string doc_root, net::io_context &ioc);

    std::string const &doc_root() const noexcept { return doc_root_; }

    void process_tasks();

    void join(websocket_session *session);
    void leave(websocket_session *session);
    void send(std::string message);
    void send(std::string id, std::string message);

    void push_task(Task task);
    void pop_task();
    Task &peek_task();
};