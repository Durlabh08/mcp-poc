#include "ConsoleApp.hpp"
#include "FileTaskRepository.hpp"
#include "TaskService.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main()
{
    try
    {
        taskmanager::FileTaskRepository repository{"tasks.db"};
        taskmanager::TaskService taskService{repository};
        taskmanager::ConsoleApp app{taskService};

        app.run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Application error: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
