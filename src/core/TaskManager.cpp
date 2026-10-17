#include "TaskManager.hpp"

#include <algorithm>
#include <utility>

namespace taskmanager
{

TaskManager::TaskManager(std::vector<Task> tasks)
    : tasks_(std::move(tasks))
{
    for (const auto& task : tasks_)
    {
        if (task.id >= nextId_)
        {
            nextId_ = task.id + 1;
        }
    }
}

TaskId TaskManager::addTask(std::string description)
{
    const TaskId id = nextId_++;

    tasks_.push_back(Task{
        .id = id,
        .description = std::move(description),
        .completed = false
    });

    return id;
}

std::optional<Task> TaskManager::getTask(const TaskId id) const
{
    const auto it = findTask(id);

    if (it == tasks_.end())
    {
        return std::nullopt;
    }

    return *it;
}

const std::vector<Task>& TaskManager::getTasks() const noexcept
{
    return tasks_;
}

bool TaskManager::completeTask(const TaskId id)
{
    const auto it = findTask(id);

    if (it == tasks_.end())
    {
        return false;
    }

    it->completed = true;
    return true;
}

bool TaskManager::removeTask(const TaskId id)
{
    const auto it = findTask(id);

    if (it == tasks_.end())
    {
        return false;
    }

    tasks_.erase(it);
    return true;
}

TaskManager::TaskIterator TaskManager::findTask(const TaskId id)
{
    return std::ranges::find(tasks_, id, &Task::id);
}

TaskManager::ConstTaskIterator TaskManager::findTask(const TaskId id) const
{
    return std::ranges::find(tasks_, id, &Task::id);
}

} // namespace taskmanager
