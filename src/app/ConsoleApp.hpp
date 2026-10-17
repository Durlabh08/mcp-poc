#pragma once

#include "TaskService.hpp"

namespace taskmanager
{

class ConsoleApp
{
public:
    explicit ConsoleApp(TaskService& taskService);

    void run();

private:
    void displayMenu() const;
    void addTask();
    void listTasks() const;
    void showTask() const;
    void completeTask();
    void removeTask();

    TaskService& taskService_;
};

} // namespace taskmanager
