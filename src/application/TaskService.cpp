#include "TaskService.hpp"

#include <utility>

namespace taskmanager
{

TaskService::TaskService(ITaskRepository& repository)
    : repository_(repository),
      taskManager_(repository.load())
{
}

TaskId TaskService::addTask(std::string description)
{
    auto candidate = taskManager_;
    const auto id = candidate.addTask(std::move(description));

    repository_.save(candidate.getTasks());
    taskManager_ = std::move(candidate);

    return id;
}

std::optional<Task> TaskService::getTask(const TaskId id) const
{
    return taskManager_.getTask(id);
}

const std::vector<Task>& TaskService::getTasks() const noexcept
{
    return taskManager_.getTasks();
}

bool TaskService::completeTask(const TaskId id)
{
    auto candidate = taskManager_;

    if (!candidate.completeTask(id))
    {
        return false;
    }

    repository_.save(candidate.getTasks());
    taskManager_ = std::move(candidate);

    return true;
}

bool TaskService::removeTask(const TaskId id)
{
    auto candidate = taskManager_;

    if (!candidate.removeTask(id))
    {
        return false;
    }

    repository_.save(candidate.getTasks());
    taskManager_ = std::move(candidate);

    return true;
}

} // namespace taskmanager
