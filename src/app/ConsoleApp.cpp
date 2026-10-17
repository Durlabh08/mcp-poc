#include "ConsoleApp.hpp"

#include <iostream>
#include <limits>
#include <string>
#include <utility>

namespace taskmanager
{

ConsoleApp::ConsoleApp(TaskService& taskService)
    : taskService_(taskService)
{
}

void ConsoleApp::run()
{
    bool running = true;

    while (running)
    {
        displayMenu();

        int choice{};
        std::cin >> choice;

        if (!std::cin)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input.\n";
            continue;
        }

        switch (choice)
        {
        case 1:
            addTask();
            break;
        case 2:
            listTasks();
            break;
        case 3:
            showTask();
            break;
        case 4:
            completeTask();
            break;
        case 5:
            removeTask();
            break;
        case 0:
            running = false;
            break;
        default:
            std::cout << "Unknown option.\n";
            break;
        }
    }
}

void ConsoleApp::displayMenu() const
{
    std::cout
        << "\nTask Manager\n"
        << "------------\n"
        << "1. Add task\n"
        << "2. List tasks\n"
        << "3. Get task\n"
        << "4. Complete task\n"
        << "5. Remove task\n"
        << "0. Exit\n"
        << "\nSelect: ";
}

void ConsoleApp::addTask()
{
    std::cout << "Description: ";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string description;
    std::getline(std::cin, description);

    if (description.empty())
    {
        std::cout << "Description cannot be empty.\n";
        return;
    }

    const auto id = taskService_.addTask(std::move(description));
    std::cout << "Created task " << id << ".\n";
}

void ConsoleApp::listTasks() const
{
    const auto& tasks = taskService_.getTasks();

    if (tasks.empty())
    {
        std::cout << "No tasks.\n";
        return;
    }

    for (const auto& task : tasks)
    {
        std::cout
            << task.id
            << " [" << (task.completed ? 'X' : ' ') << "] "
            << task.description
            << '\n';
    }
}

void ConsoleApp::showTask() const
{
    std::cout << "Task id: ";

    TaskId id{};
    std::cin >> id;

    const auto task = taskService_.getTask(id);

    if (!task)
    {
        std::cout << "Task not found.\n";
        return;
    }

    std::cout
        << task->id
        << " [" << (task->completed ? 'X' : ' ') << "] "
        << task->description
        << '\n';
}

void ConsoleApp::completeTask()
{
    std::cout << "Task id: ";

    TaskId id{};
    std::cin >> id;

    if (!taskService_.completeTask(id))
    {
        std::cout << "Task not found.\n";
        return;
    }

    std::cout << "Task completed.\n";
}

void ConsoleApp::removeTask()
{
    std::cout << "Task id: ";

    TaskId id{};
    std::cin >> id;

    if (!taskService_.removeTask(id))
    {
        std::cout << "Task not found.\n";
        return;
    }

    std::cout << "Task removed.\n";
}

} // namespace taskmanager
