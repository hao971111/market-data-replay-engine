#pragma once
#include <vector>
#include <string>
#include "tick.hpp"
#include "symbol_table.hpp"
std::vector<Tick> LoadTicksCsv(const std::string& file_name, SymbolTable& symbol_table);
std::vector<Tick> LoadTicksBin(const std::string& file_name, SymbolTable& symbol_table);
bool SaveTicksBin(const std::string& file_name, const std::vector<Tick> &ticks);