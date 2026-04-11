#ifndef PARSER_H
#define PARSER_H

// CS530 Assignment 2 - LXE Assembler
// Team: Alan Chavarin (cssc2513, RedID 827690364)
//       Amir Ali (cssc2503, RedID 132395455)

#include <string>
#include <vector>

#include "assembler_types.h"

class Parser {
   public:
    std::vector<SourceLine> parse_file(const std::string& path, std::string& error_out) const;

   private:
    SourceLine parse_line(const std::string& line, int line_number) const;
};

#endif
