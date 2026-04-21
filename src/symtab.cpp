#include "symtab.h"

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

// this inserts a new symbol into the symbol table
bool SymTab::insert(const std::string& symbol, int address, std::string& error_out) {
    if (symbol.empty()) return true; // check if not empty
    if (symbols_.find(symbol) != symbols_.end()) { // check if the symbol is already in the map
        error_out = "Duplicate symbol: " + symbol;
        return false;
    }
    symbols_[symbol] = address; // add the symbol and address to the map
    return true;
}

// this looks up a symbol in the symbol table and sets the address by reference and returns true if found, false otherwise
bool SymTab::lookup(const std::string& symbol, int& address_out) const {
    // looks for the given symbol param in the symbols map
    std::map<std::string, int>::const_iterator it = symbols_.find(symbol);
    if (it == symbols_.end()) return false;
    address_out = it->second;
    return true;
}

// this returns the symbols map
const std::map<std::string, int>& SymTab::entries() const { return symbols_; } 
