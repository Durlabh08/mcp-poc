#pragma once

#include "ITaskRepository.hpp"
#include "TaskManager.hpp"

#include <optional>
#include <string>
#include <vector>

namespace taskmanager
{

class TaskService
{
public:
    explicit TaskService(ITaskRepository& repository);

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
    ITaskRepository& repository_;
    TaskManager taskManager_;
};

} // namespace taskmanager
