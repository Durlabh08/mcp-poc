#include "FileTaskRepository.hpp"

#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <utility>

namespace taskmanager
{

FileTaskRepository::FileTaskRepository(std::filesystem::path path)
    : path_(std::move(path))
{
}

std::vector<Task> FileTaskRepository::load()
{
    if (!std::filesystem::exists(path_))
    {
        return {};
    }

    std::ifstream input{path_};

    if (!input)
    {
        throw std::runtime_error{"Unable to open task database for reading."};
    }

    std::vector<Task> tasks;
    TaskId id{};
    int completed{};
    std::string description;

    while (input >> id >> completed >> std::quoted(description))
    {
        tasks.push_back(Task{
            .id = id,
            .description = std::move(description),
            .completed = completed != 0
        });
    }

    if (!input.eof())
    {
        throw std::runtime_error{"Task database contains invalid data."};
    }

    return tasks;
}

void FileTaskRepository::save(const std::vector<Task>& tasks)
{
    std::ofstream output{path_, std::ios::trunc};

    if (!output)
    {
        throw std::runtime_error{"Unable to open task database for writing."};
    }

    for (const auto& task : tasks)
    {
        output
            << task.id << ' '
            << (task.completed ? 1 : 0) << ' '
            << std::quoted(task.description)
            << '\n';
    }

    if (!output)
    {
        throw std::runtime_error{"Failed while writing task database."};
    }
}

} // namespace taskmanager
