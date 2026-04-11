#ifndef SYMTAB_H
#define SYMTAB_H

// CS530 Assignment 2 - LXE Assembler
// Team: Alan Chavarin (cssc2513, RedID 827690364)
//       Amir Ali (cssc2503, RedID 132395455)

#include <map>
#include <string>

class SymTab {
   public:
    bool insert(const std::string& symbol, int address, std::string& error_out);
    bool lookup(const std::string& symbol, int& address_out) const;
    const std::map<std::string, int>& entries() const;

   private:
    std::map<std::string, int> symbols_;
};

#endif
