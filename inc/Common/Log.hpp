#pragma once

#include <string>
#include <format>
#include <iostream>

static void log(const std::string& msg)
{
    std::cout << std::format("{}\n", msg);
}