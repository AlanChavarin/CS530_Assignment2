#ifndef OPTAB_H
#define OPTAB_H

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

#include <string>
#include <unordered_map>

#include "assembler_types.h"

class OpTab {
   public:
    OpTab(); // default constructor

    bool has(const std::string& mnemonic) const; // checks if the mnemonic is in the optab
    OpEntry get(const std::string& mnemonic) const; // getter function for the optab using the mnemonic

   private:
    std::unordered_map<std::string, OpEntry> table_; // this stores the mnemonics as keys and OpEntry structs as values
    void add(const std::string& mnem, int opcode, int mask); // this adds a new entry to the optab
};

#endif
