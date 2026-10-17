#include "TaskCli.hpp"

#include <charconv>
#include <iomanip>
#include <sstream>
#include <string>

namespace taskmanager
{
namespace
{

std::string escapeJson(const std::string_view value)
{
    std::ostringstream escaped;

    for (const unsigned char ch : value)
    {
        switch (ch)
        {
        case '"':
            escaped << "\\\"";
            break;
        case '\\':
            escaped << "\\\\";
            break;
        case '\b':
            escaped << "\\b";
            break;
        case '\f':
            escaped << "\\f";
            break;
        case '\n':
            escaped << "\\n";
            break;
        case '\r':
            escaped << "\\r";
            break;
        case '\t':
            escaped << "\\t";
            break;
        default:
            if (ch < 0x20)
            {
                escaped
                    << "\\u"
                    << std::hex
                    << std::setw(4)
                    << std::setfill('0')
                    << static_cast<int>(ch)
                    << std::dec;
            }
            else
            {
                escaped << static_cast<char>(ch);
            }
            break;
        }
    }

    return escaped.str();
}

void writeTask(std::ostream& output, const Task& task)
{
    output
        << "{\"id\":" << task.id
        << ",\"description\":\"" << escapeJson(task.description) << '"'
        << ",\"completed\":" << (task.completed ? "true" : "false")
        << '}';
}

void writeError(
    std::ostream& output,
    const std::string_view error,
    const std::string_view message)
{
    output
        << "{\"success\":false,\"error\":\""
        << escapeJson(error)
        << "\",\"message\":\""
        << escapeJson(message)
        << "\"}\n";
}

bool parseTaskId(const std::string_view text, TaskId& id)
{
    const auto* const begin = text.data();
    const auto* const end = begin + text.size();

    const auto result = std::from_chars(begin, end, id);

    return result.ec == std::errc{} && result.ptr == end;
}

int runAdd(
    const std::vector<std::string_view>& args,
    TaskService& taskService,
    std::ostream& output)
{
    if (args.size() != 2 || args[1].empty())
    {
        writeError(output, "invalid_arguments", "Usage: task_cli add <description>");
        return 2;
    }

    const auto id = taskService.addTask(std::string{args[1]});
    const auto task = taskService.getTask(id);

    output << "{\"success\":true,\"task\":";
    writeTask(output, *task);
    output << "}\n";

    return 0;
}

int runGet(
    const std::vector<std::string_view>& args,
    TaskService& taskService,
    std::ostream& output)
{
    if (args.size() != 2)
    {
        writeError(output, "invalid_arguments", "Usage: task_cli get <id>");
        return 2;
    }

    TaskId id{};

    if (!parseTaskId(args[1], id))
    {
        writeError(output, "invalid_task_id", "Task ID must be an unsigned integer.");
        return 2;
    }

    const auto task = taskService.getTask(id);

    if (!task)
    {
        writeError(output, "task_not_found", "Task does not exist.");
        return 3;
    }

    output << "{\"success\":true,\"task\":";
    writeTask(output, *task);
    output << "}\n";

    return 0;
}

int runList(
    const std::vector<std::string_view>& args,
    TaskService& taskService,
    std::ostream& output)
{
    if (args.size() != 1)
    {
        writeError(output, "invalid_arguments", "Usage: task_cli list");
        return 2;
    }

    output << "{\"success\":true,\"tasks\":[";

    bool first = true;

    for (const auto& task : taskService.getTasks())
    {
        if (!first)
        {
            output << ',';
        }

        writeTask(output, task);
        first = false;
    }

    output << "]}\n";

    return 0;
}

int runComplete(
    const std::vector<std::string_view>& args,
    TaskService& taskService,
    std::ostream& output)
{
    if (args.size() != 2)
    {
        writeError(output, "invalid_arguments", "Usage: task_cli complete <id>");
        return 2;
    }

    TaskId id{};

    if (!parseTaskId(args[1], id))
    {
        writeError(output, "invalid_task_id", "Task ID must be an unsigned integer.");
        return 2;
    }

    if (!taskService.completeTask(id))
    {
        writeError(output, "task_not_found", "Task does not exist.");
        return 3;
    }

    const auto task = taskService.getTask(id);

    output << "{\"success\":true,\"task\":";
    writeTask(output, *task);
    output << "}\n";

    return 0;
}

int runRemove(
    const std::vector<std::string_view>& args,
    TaskService& taskService,
    std::ostream& output)
{
    if (args.size() != 2)
    {
        writeError(output, "invalid_arguments", "Usage: task_cli remove <id>");
        return 2;
    }

    TaskId id{};

    if (!parseTaskId(args[1], id))
    {
        writeError(output, "invalid_task_id", "Task ID must be an unsigned integer.");
        return 2;
    }

    if (!taskService.removeTask(id))
    {
        writeError(output, "task_not_found", "Task does not exist.");
        return 3;
    }

    output
        << "{\"success\":true,\"removedId\":"
        << id
        << "}\n";

    return 0;
}

} // namespace

int runTaskCli(
    const std::vector<std::string_view>& args,
    TaskService& taskService,
    std::ostream& output)
{
    if (args.empty())
    {
        writeError(
            output,
            "invalid_arguments",
            "Usage: task_cli [--db <path>] <add|get|list|complete|remove> [arguments]");
        return 2;
    }

    if (args[0] == "add")
    {
        return runAdd(args, taskService, output);
    }

    if (args[0] == "get")
    {
        return runGet(args, taskService, output);
    }

    if (args[0] == "list")
    {
        return runList(args, taskService, output);
    }

    if (args[0] == "complete")
    {
        return runComplete(args, taskService, output);
    }

    if (args[0] == "remove")
    {
        return runRemove(args, taskService, output);
    }

    writeError(output, "unknown_command", "Unknown task command.");
    return 2;
}

} // namespace taskmanager
