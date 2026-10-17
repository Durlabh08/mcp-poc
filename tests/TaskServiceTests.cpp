#include "TaskService.hpp"

#include <cstdlib>
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

class MemoryTaskRepository final : public taskmanager::ITaskRepository
{
public:
    std::vector<taskmanager::Task> tasks;
    int saveCount{};
    bool failSave{};

    std::vector<taskmanager::Task> load() override
    {
        return tasks;
    }

    void save(const std::vector<taskmanager::Task>& newTasks) override
    {
        if (failSave)
        {
            throw std::runtime_error{"simulated persistence failure"};
        }

        tasks = newTasks;
        ++saveCount;
    }
};

void testLoadsExistingTasks()
{
    MemoryTaskRepository repository;
    repository.tasks.push_back({.id = 7, .description = "Existing", .completed = false});

    taskmanager::TaskService service{repository};

    require(service.getTasks().size() == 1, "existing tasks should load");
    require(service.addTask("Next") == 8, "ID sequence should continue after loaded tasks");
}

void testAddPersists()
{
    MemoryTaskRepository repository;
    taskmanager::TaskService service{repository};

    service.addTask("Learn MCP");

    require(repository.saveCount == 1, "adding a task should persist");
    require(repository.tasks.size() == 1, "repository should contain added task");
}

void testCompletePersists()
{
    MemoryTaskRepository repository;
    taskmanager::TaskService service{repository};
    const auto id = service.addTask("Complete me");
    const auto initialSaveCount = repository.saveCount;

    require(service.completeTask(id), "complete should succeed");
    require(repository.saveCount == initialSaveCount + 1, "completion should persist");
    require(repository.tasks.front().completed, "persisted task should be completed");
}

void testRemovePersists()
{
    MemoryTaskRepository repository;
    taskmanager::TaskService service{repository};
    const auto id = service.addTask("Remove me");

    require(service.removeTask(id), "remove should succeed");
    require(repository.tasks.empty(), "removed task should disappear from repository");
}

void testFailedSaveDoesNotCommitInMemoryMutation()
{
    MemoryTaskRepository repository;
    taskmanager::TaskService service{repository};
    repository.failSave = true;

    bool threw = false;
    try
    {
        service.addTask("Should fail");
    }
    catch (const std::runtime_error&)
    {
        threw = true;
    }

    require(threw, "persistence failure should propagate");
    require(service.getTasks().empty(), "failed persistence must not mutate live state");
}

} // namespace

int main()
{
    testLoadsExistingTasks();
    testAddPersists();
    testCompletePersists();
    testRemovePersists();
    testFailedSaveDoesNotCommitInMemoryMutation();

    std::cout << "All TaskService tests passed.\n";
    return EXIT_SUCCESS;
}
