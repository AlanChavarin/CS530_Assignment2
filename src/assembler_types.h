#ifndef ASSEMBLER_TYPES_H
#define ASSEMBLER_TYPES_H

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

#include <string>

// we use this to store the source line and the parsed information from that line
struct SourceLine {
    std::string raw_text; // raw source line
    std::string label; // label of the source line
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

// this is used to store the mnemonic, opcode, and valid formats
struct OpEntry {
    std::string mnemonic;
    int opcode_byte; 
    int valid_formats; // this is a mask for the valid formats
};

// this is used to store the listing record, which is later used to write the listing file
struct ListingRecord {
    int line_number;
    std::string loc_str; // this stores the address in hex
    std::string label;
    std::string opcode;
    std::string operand;
    std::string object_code; // object code for that instruction
};

#endif
