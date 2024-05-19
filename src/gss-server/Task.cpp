#include "Task.hpp"
#include <iostream>

task::task(std::string assignee, std::string desc, int duration)
    : assignee_{assignee}, desc_{desc},
      duration_{boost::posix_time::minutes{duration}} {}

std::string_view task::assignee() const noexcept { return assignee_; }
std::string_view task::desc() const noexcept { return desc_; }
boost::posix_time::time_duration task::duration() const noexcept {
    return duration_;
}

void task::print() {
    std::cout << "Task: ('" << assignee_ << "', '" << desc_ << "', "
              << duration_.minutes() << ")" << std::endl;
}

json task::serialize() const noexcept {
    return {{"name", assignee_},
            {"description", desc_},
            {"duration", duration_.minutes()}};
}