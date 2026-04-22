#ifndef PASS2_H
#define PASS2_H

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

#include <string>
#include <map>
#include <utility>
#include <vector>

#include "assembler_types.h"
#include "optab.h"
#include "pass1.h"

// this is used to store the result of pass 2
struct Pass2Result {
    std::vector<ListingRecord> listing_records; // this vector stores the listing records that will be written to the listing file
    std::vector<std::string> errors; // vector that stores the lsit of errors
    // literal token (e.g. =C'EOF') -> address, in first-seen source order (for symtab .st)
    std::vector<std::pair<std::string, int> > literal_table;
};

class Pass2 {
   public:
    Pass2Result run(const Pass1Result& pass1, const OpTab& optab) const; // this runs the main pass 2 function

   private:
    std::string encode_line(const SourceLine& line, const SourceLine* next, const SymTab& symtab,
                            const std::map<std::string, int>& literal_addresses,
                            const OpTab& optab, bool& base_active, int& base_address,
                            std::vector<std::string>& errors) const; // this encodes the line and returns the object code

    int parse_constant(const std::string& text, bool& ok) const; // this parses the constant and returns the value
    int parse_byte_value(const std::string& operand, bool& ok, int& width_bits) const; // this parses the byte value and returns the value
    std::string encode_literal_token(const std::string& literal_token, bool& ok) const; // this encodes the literal token and returns the value in hex
    int get_register_code(const std::string& name) const; // this gets the register code for a given register name
    std::string upper(const std::string& s) const; // this converts the string to uppercase
    std::string hex(int value, int width) const; // this converts the value to hex
};

#endif
