#pragma once
#include <vector>
#include <string>
#include "tick.hpp"

std::vector<Tick> LoadTicksCsv(const std::string& file_name);
