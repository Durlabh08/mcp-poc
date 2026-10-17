#include "TaskManager.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

namespace
{

void require(const bool condition, const std::string& message)
{
    if (!condition)
    {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void testAddTask()
{
    taskmanager::TaskManager manager;
    const auto id = manager.addTask("Learn MCP");
    const auto task = manager.getTask(id);

    require(id == 1, "first task ID should be 1");
    require(task.has_value(), "added task should exist");
    require(task->description == "Learn MCP", "description should match");
    require(!task->completed, "new task should not be completed");
}

void testMultipleTasks()
{
    taskmanager::TaskManager manager;
    const auto firstId = manager.addTask("Task 1");
    const auto secondId = manager.addTask("Task 2");

    require(firstId == 1, "first task ID should be 1");
    require(secondId == 2, "second task ID should be 2");
    require(manager.getTasks().size() == 2, "manager should contain two tasks");
}

void testGetMissingTask()
{
    taskmanager::TaskManager manager;
    require(!manager.getTask(999).has_value(), "missing task should return nullopt");
}

void testCompleteTask()
{
    taskmanager::TaskManager manager;
    const auto id = manager.addTask("Complete me");

    require(manager.completeTask(id), "completeTask should succeed");
    require(manager.getTask(id)->completed, "task should be marked completed");
}

void testCompleteMissingTask()
{
    taskmanager::TaskManager manager;
    require(!manager.completeTask(999), "completing missing task should fail");
}

void testRemoveTask()
{
    taskmanager::TaskManager manager;
    const auto id = manager.addTask("Remove me");

    require(manager.removeTask(id), "removeTask should succeed");
    require(!manager.getTask(id).has_value(), "removed task should not exist");
    require(manager.getTasks().empty(), "task collection should be empty");
}

void testRemoveMissingTask()
{
    taskmanager::TaskManager manager;
    require(!manager.removeTask(999), "removing missing task should fail");
}

void testIdSequenceAfterRemoval()
{
    taskmanager::TaskManager manager;
    const auto firstId = manager.addTask("Task 1");
    require(manager.removeTask(firstId), "first task removal should succeed");

    const auto secondId = manager.addTask("Task 2");
    require(secondId == 2, "removed task IDs should not be reused");
}

void testHydratedIdSequence()
{
    taskmanager::TaskManager manager{{
        {.id = 3, .description = "Existing", .completed = false},
        {.id = 9, .description = "Existing 2", .completed = true}
    }};

    require(manager.addTask("Next") == 10, "hydrated manager should continue after maximum ID");
}

} // namespace

int main()
{
    testAddTask();
    testMultipleTasks();
    testGetMissingTask();
    testCompleteTask();
    testCompleteMissingTask();
    testRemoveTask();
    testRemoveMissingTask();
    testIdSequenceAfterRemoval();
    testHydratedIdSequence();

    std::cout << "All TaskManager tests passed.\n";
    return EXIT_SUCCESS;
}
