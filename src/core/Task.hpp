#pragma once

#include <cstdint>
#include <string>

namespace taskmanager
{

using TaskId = std::uint64_t;

struct Task
{
    TaskId id{};
    std::string description;
    bool completed{false};
};

} // namespace taskmanager
