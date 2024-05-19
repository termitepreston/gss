#include "Task.hpp"
#include <iostream>

Task::Task(std::string assignee, std::string desc, int duration)
    : assignee_{assignee}, desc_{desc}, duration_{duration} {}

std::string_view Task::assignee() const noexcept { return assignee_; }
std::string_view Task::desc() const noexcept { return desc_; }
boost::posix_time::minutes Task::duration() const noexcept { return duration_; }

void Task::print() {
    std::cout << "Task: " << assignee_ << " " << desc_ << std::endl;
}