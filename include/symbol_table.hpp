
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

class SymbolTable {
    std::unordered_map<std::string, uint32_t> symbol_to_id;
    std::vector<std::string> id_to_symbol;
    uint32_t next_id = 0;
public:
    uint32_t get_or_create_id(const std::string& symbol) {
        if(symbol_to_id.find(symbol) == symbol_to_id.end()) {
            symbol_to_id[symbol] = next_id++;
            id_to_symbol.emplace_back(symbol);
        }
        return symbol_to_id[symbol];
    }

    const std::string& get_symbol(uint32_t symbol_id) const {
        return id_to_symbol.at(symbol_id);
    }
};