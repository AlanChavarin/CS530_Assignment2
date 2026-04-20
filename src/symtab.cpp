#include "symtab.h"

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

bool SymTab::insert(const std::string& symbol, int address, std::string& error_out) {
    if (symbol.empty()) return true;
    if (symbols_.find(symbol) != symbols_.end()) {
        error_out = "Duplicate symbol: " + symbol;
        return false;
    }
    symbols_[symbol] = address;
    return true;
}

bool SymTab::lookup(const std::string& symbol, int& address_out) const {
    std::map<std::string, int>::const_iterator it = symbols_.find(symbol);
    if (it == symbols_.end()) return false;
    address_out = it->second;
    return true;
}

const std::map<std::string, int>& SymTab::entries() const { return symbols_; }
