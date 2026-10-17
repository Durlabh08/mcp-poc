#include "FileTaskRepository.hpp"
#include "TaskCli.hpp"
#include "TaskService.hpp"

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <vector>

int main(int argc, char* argv[])
{
    std::filesystem::path databasePath{"tasks.db"};
    int firstCommandArgument = 1;

    if (argc >= 2 && std::string_view{argv[1]} == "--db")
    {
        if (argc < 3)
        {
            std::cout
                << "{\"success\":false,\"error\":\"invalid_arguments\","
                << "\"message\":\"--db requires a path.\"}\n";
            return 2;
        }

        databasePath = argv[2];
        firstCommandArgument = 3;
    }

    std::vector<std::string_view> arguments;
    arguments.reserve(static_cast<std::size_t>(argc - firstCommandArgument));

    for (int index = firstCommandArgument; index < argc; ++index)
    {
        arguments.emplace_back(argv[index]);
    }

    try
    {
        taskmanager::FileTaskRepository repository{databasePath};
        taskmanager::TaskService taskService{repository};

        return taskmanager::runTaskCli(arguments, taskService, std::cout);
    }
    catch (const std::exception& exception)
    {
        std::cerr << "task_cli error: " << exception.what() << '\n';
        std::cout
            << "{\"success\":false,\"error\":\"application_error\","
            << "\"message\":\"Task operation failed.\"}\n";
        return EXIT_FAILURE;
    }
}
