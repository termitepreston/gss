#pragma once

#include <chrono>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

#include "net.hpp"

using json = nlohmann::json;

class task {
    std::string assignee_;
    std::string desc_;
    boost::posix_time::time_duration duration_;

  public:
    task(std::string assignee, std::string desc, int duration);
    void print();
    json serialize() const noexcept;

    std::string_view assignee() const noexcept;
    std::string_view desc() const noexcept;
    boost::posix_time::time_duration duration() const noexcept;
};