#include "FileTaskRepository.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
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

std::filesystem::path testPath(const std::string& suffix)
{
    return std::filesystem::temp_directory_path() / ("taskmanager_repository_" + suffix + ".db");
}

void testMissingFileLoadsEmptyCollection()
{
    const auto path = testPath("missing");
    std::filesystem::remove(path);

    taskmanager::FileTaskRepository repository{path};
    require(repository.load().empty(), "missing database should load as empty collection");
}

void testSaveAndLoad()
{
    const auto path = testPath("roundtrip");
    std::filesystem::remove(path);

    taskmanager::FileTaskRepository repository{path};
    const std::vector<taskmanager::Task> expected{
        {.id = 1, .description = "Learn MCP", .completed = false},
        {.id = 2, .description = "Task with spaces", .completed = true}
    };

    repository.save(expected);
    const auto loaded = repository.load();

    require(loaded.size() == 2, "two tasks should be loaded");
    require(loaded[0].id == 1, "first task ID should match");
    require(loaded[0].description == "Learn MCP", "first description should match");
    require(!loaded[0].completed, "first task should be incomplete");
    require(loaded[1].description == "Task with spaces", "quoted description should round-trip");
    require(loaded[1].completed, "second task should be completed");

    std::filesystem::remove(path);
}

void testInvalidDataFails()
{
    const auto path = testPath("invalid");
    {
        std::ofstream output{path};
        output << "invalid data\n";
    }

    taskmanager::FileTaskRepository repository{path};
    bool threw = false;

    try
    {
        static_cast<void>(repository.load());
    }
    catch (const std::runtime_error&)
    {
        threw = true;
    }

    std::filesystem::remove(path);
    require(threw, "invalid database contents should throw");
}

} // namespace

int main()
{
    testMissingFileLoadsEmptyCollection();
    testSaveAndLoad();
    testInvalidDataFails();

    std::cout << "All FileTaskRepository tests passed.\n";
    return EXIT_SUCCESS;
}
