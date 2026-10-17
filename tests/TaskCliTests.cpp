#include "TaskCli.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

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

class MemoryTaskRepository final : public taskmanager::ITaskRepository
{
public:
    std::vector<taskmanager::Task> tasks;

    std::vector<taskmanager::Task> load() override
    {
        return tasks;
    }

    void save(const std::vector<taskmanager::Task>& newTasks) override
    {
        tasks = newTasks;
    }
};

int run(
    taskmanager::TaskService& service,
    std::ostringstream& output,
    std::initializer_list<std::string_view> arguments)
{
    output.str({});
    output.clear();

    return taskmanager::runTaskCli(
        std::vector<std::string_view>{arguments},
        service,
        output);
}

void testCrudCommands()
{
    MemoryTaskRepository repository;
    taskmanager::TaskService service{repository};
    std::ostringstream output;

    require(
        run(service, output, {"add", "Learn MCP"}) == 0,
        "add command should succeed");

    require(
        output.str() ==
            "{\"success\":true,\"task\":{\"id\":1,\"description\":\"Learn MCP\",\"completed\":false}}\n",
        "add command should return created task as JSON");

    require(
        run(service, output, {"get", "1"}) == 0,
        "get command should succeed");

    require(
        output.str().find("\"description\":\"Learn MCP\"") != std::string::npos,
        "get command should return the task");

    require(
        run(service, output, {"list"}) == 0,
        "list command should succeed");

    require(
        output.str() ==
            "{\"success\":true,\"tasks\":[{\"id\":1,\"description\":\"Learn MCP\",\"completed\":false}]}\n",
        "list command should return all tasks");

    require(
        run(service, output, {"complete", "1"}) == 0,
        "complete command should succeed");

    require(
        output.str().find("\"completed\":true") != std::string::npos,
        "complete command should return completed task");

    require(
        run(service, output, {"remove", "1"}) == 0,
        "remove command should succeed");

    require(
        output.str() == "{\"success\":true,\"removedId\":1}\n",
        "remove command should return removed ID");

    require(
        repository.tasks.empty(),
        "remove command should persist the removal");
}

void testInvalidArguments()
{
    MemoryTaskRepository repository;
    taskmanager::TaskService service{repository};
    std::ostringstream output;

    require(
        run(service, output, {"get", "abc"}) == 2,
        "non-numeric task ID should be rejected");

    require(
        output.str().find("\"error\":\"invalid_task_id\"") != std::string::npos,
        "invalid task ID should return a structured error");

    require(
        run(service, output, {"unknown"}) == 2,
        "unknown command should be rejected");

    require(
        output.str().find("\"error\":\"unknown_command\"") != std::string::npos,
        "unknown command should return a structured error");
}

void testMissingTask()
{
    MemoryTaskRepository repository;
    taskmanager::TaskService service{repository};
    std::ostringstream output;

    require(
        run(service, output, {"complete", "999"}) == 3,
        "missing task should return not-found exit code");

    require(
        output.str().find("\"error\":\"task_not_found\"") != std::string::npos,
        "missing task should return a structured error");
}

void testJsonEscaping()
{
    MemoryTaskRepository repository;
    taskmanager::TaskService service{repository};
    std::ostringstream output;

    require(
        run(service, output, {"add", "Quote \" and slash \\"}) == 0,
        "add should accept JSON-sensitive characters");

    require(
        output.str().find("Quote \\\" and slash \\\\") != std::string::npos,
        "JSON-sensitive characters should be escaped");
}

} // namespace

int main()
{
    testCrudCommands();
    testInvalidArguments();
    testMissingTask();
    testJsonEscaping();

    std::cout << "All TaskCli tests passed.\n";

    return EXIT_SUCCESS;
}
