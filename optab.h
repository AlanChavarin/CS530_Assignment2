#ifndef OPTAB_H
#define OPTAB_H

// CS530 Assignment 2 - LXE Assembler
// Team: Alan Chavarin (cssc2513, RedID 827690364)
//       Amir Ali (cssc2503, RedID 132395455)

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
