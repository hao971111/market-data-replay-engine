#pragma once
#include <vector>
#include <string>
#include "tick.hpp"
#include "symbol_table.hpp"
std::vector<Tick> LoadTicksCsv(const std::string& file_name, SymbolTable& symbol_table);
