#pragma once

#include <string>
#include <format>
#include <iostream>

void log(const std::string& msg)
{
    std::cout << std::format("{}\n", msg);
}