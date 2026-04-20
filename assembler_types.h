#ifndef ASSEMBLER_TYPES_H
#define ASSEMBLER_TYPES_H

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

#include <string>

struct SourceLine {
    std::string raw_text;
    std::string label;
    std::string opcode;
    std::string operand;
    std::string comment;
    int line_number;
    int address;
    bool is_comment;
    bool is_blank;
    bool has_error;
    std::string error_message;

    SourceLine()
        : line_number(0),
          address(-1),
          is_comment(false),
          is_blank(false),
          has_error(false) {}
};

struct OpEntry {
    std::string mnemonic;
    int opcode_byte;
    int valid_formats; 
};

struct ListingRecord {
    int line_number;
    std::string loc_str;
    std::string label;
    std::string opcode;
    std::string operand;
    std::string object_code;
};

#endif
