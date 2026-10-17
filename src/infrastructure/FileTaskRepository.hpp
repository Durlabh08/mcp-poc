#pragma once

#include "ITaskRepository.hpp"

#include <filesystem>

namespace taskmanager
{

class FileTaskRepository final : public ITaskRepository
{
public:
    explicit FileTaskRepository(std::filesystem::path path);

    [[nodiscard]]
    std::vector<Task> load() override;

    void save(const std::vector<Task>& tasks) override;

private:
    std::filesystem::path path_;
};

} // namespace taskmanager
