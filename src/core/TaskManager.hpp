#pragma once

#include "Task.hpp"

#include <optional>
#include <string>
#include <vector>

namespace taskmanager
{

class TaskManager
{
public:
    explicit TaskManager(std::vector<Task> tasks = {});

    [[nodiscard]]
    TaskId addTask(std::string description);

    [[nodiscard]]
    std::optional<Task> getTask(TaskId id) const;

    [[nodiscard]]
    const std::vector<Task>& getTasks() const noexcept;

    [[nodiscard]]
    bool completeTask(TaskId id);

    [[nodiscard]]
    bool removeTask(TaskId id);

private:
    using TaskIterator = std::vector<Task>::iterator;
    using ConstTaskIterator = std::vector<Task>::const_iterator;

    [[nodiscard]]
    TaskIterator findTask(TaskId id);

    [[nodiscard]]
    ConstTaskIterator findTask(TaskId id) const;

    TaskId nextId_{1};
    std::vector<Task> tasks_;
};

} // namespace taskmanager
