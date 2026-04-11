#ifndef LISTING_H
#define LISTING_H

// CS530 Assignment 2 - LXE Assembler
// Team: Alan Chavarin (cssc2513, RedID 827690364)
//       Amir Ali (cssc2503, RedID 132395455)

#include <string>
#include <vector>

#include "assembler_types.h"
#include "symtab.h"

class ListingWriter {
   public:
    bool write_listing(const std::string& path, const std::vector<ListingRecord>& records,
                       std::string& error_out) const;
    bool write_symtab(const std::string& path, const SymTab& symtab, std::string& error_out) const;
};

#endif
