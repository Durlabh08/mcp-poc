#pragma once

#include "Task.hpp"

#include <vector>

namespace taskmanager
{

class ITaskRepository
{
public:
    virtual ~ITaskRepository() = default;

    [[nodiscard]]
    virtual std::vector<Task> load() = 0;

    virtual void save(const std::vector<Task>& tasks) = 0;
};

} // namespace taskmanager
