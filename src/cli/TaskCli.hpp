#pragma once

#include "TaskService.hpp"

#include <ostream>
#include <string_view>
#include <vector>

namespace taskmanager
{

int runTaskCli(
    const std::vector<std::string_view>& args,
    TaskService& taskService,
    std::ostream& output);

} // namespace taskmanager
