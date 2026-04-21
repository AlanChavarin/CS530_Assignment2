#ifndef SYMTAB_H
#define SYMTAB_H

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

#include <map>
#include <string>

class SymTab {
   public:
    // this inserts a new symbol into the symbol table
    bool insert(const std::string& symbol, int address, std::string& error_out);

    // this looks up a symbol in the symbol table and sets the address by reference and returns true if found, false otherwise
    bool lookup(const std::string& symbol, int& address_out) const;

    // this returns the symbols map
    const std::map<std::string, int>& entries() const;

   private:
    std::map<std::string, int> symbols_; // this is used to store the symbols as keys and addresses as values
};

#endif
