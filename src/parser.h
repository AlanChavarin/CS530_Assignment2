#ifndef PARSER_H
#define PARSER_H

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

#include <string>
#include <vector>

#include "assembler_types.h"

class Parser {
   public:
    // this parses the file and returns a vector of SourceLine structs
    std::vector<SourceLine> parse_file(const std::string& path, std::string& error_out) const;

   private:
    // this parses a single line and returns a SourceLine struct
    SourceLine parse_line(const std::string& line, int line_number) const;
};

#endif
