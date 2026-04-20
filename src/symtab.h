#ifndef SYMTAB_H
#define SYMTAB_H

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

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
