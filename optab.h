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
    OpTab();

    bool has(const std::string& mnemonic) const;
    OpEntry get(const std::string& mnemonic) const;

   private:
    std::unordered_map<std::string, OpEntry> table_;
    void add(const std::string& mnem, int opcode, int mask);
};

#endif
