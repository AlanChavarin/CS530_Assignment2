#ifndef PASS1_H
#define PASS1_H

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

#include <string>
#include <vector>

#include "assembler_types.h"
#include "optab.h"
#include "symtab.h"

struct Pass1Result {
    std::vector<SourceLine> lines;
    SymTab symtab;
    int start_address;
    int program_length;
    std::vector<std::string> errors;
};

class Pass1 {
   public:
    Pass1Result run(const std::vector<SourceLine>& parsed, const OpTab& optab) const;

   private:
    int get_byte_size(const std::string& operand) const;
    int get_literal_size(const std::string& literal_token) const;
    int parse_number(const std::string& text, bool hex_default) const;
    std::string upper(const std::string& s) const;
};

#endif
