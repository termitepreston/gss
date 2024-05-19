#pragma once

#include <chrono>
#include <string>
#include <string_view>

#include "net.hpp"

class Task {
    std::string assignee_;
    std::string desc_;
    boost::posix_time::minutes duration_;

  public:
    Task(std::string assignee, std::string desc, int duration);
    void print();

    std::string_view assignee() const noexcept;
    std::string_view desc() const noexcept;
    boost::posix_time::minutes duration() const noexcept;
};