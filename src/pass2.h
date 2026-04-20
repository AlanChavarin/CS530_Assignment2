#ifndef PASS2_H
#define PASS2_H

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

#include <string>
#include <map>
#include <vector>

#include "assembler_types.h"
#include "optab.h"
#include "pass1.h"

struct Pass2Result {
    std::vector<ListingRecord> listing_records;
    std::vector<std::string> errors;
};

class Pass2 {
   public:
    Pass2Result run(const Pass1Result& pass1, const OpTab& optab) const;

   private:
    std::string encode_line(const SourceLine& line, const SourceLine* next, const SymTab& symtab,
                            const std::map<std::string, int>& literal_addresses,
                            const OpTab& optab, bool& base_active, int& base_address,
                            std::vector<std::string>& errors) const;

    int parse_constant(const std::string& text, bool& ok) const;
    int parse_byte_value(const std::string& operand, bool& ok, int& width_bits) const;
    std::string encode_literal_token(const std::string& literal_token, bool& ok) const;
    int get_register_code(const std::string& name) const;
    std::string upper(const std::string& s) const;
    std::string hex(int value, int width) const;
};

#endif
