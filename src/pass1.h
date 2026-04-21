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

// We use this struct to store the result of pass 1
struct Pass1Result {
    std::vector<SourceLine> lines; // this vector stores the parsed lines
    SymTab symtab;
    int start_address; // this is the start address of the program
    int program_length; // this is the length of the program
    std::vector<std::string> errors; // this vector stores the errors encountered during pass 1
};

class Pass1 {
   public:
    // this runs pass 1 and returns a Pass1Result struct
    Pass1Result run(const std::vector<SourceLine>& parsed, const OpTab& optab) const;

   private:
    // this gets the byte size of a given operand
    int get_byte_size(const std::string& operand) const;
    // this gets the size of a given literal token
    int get_literal_size(const std::string& literal_token) const;
    // this parses a number and returns the value in decimal or hex if specified
    int parse_number(const std::string& text, bool hex_default) const;
    // this converts a string to uppercase
    std::string upper(const std::string& s) const;
};

#endif
