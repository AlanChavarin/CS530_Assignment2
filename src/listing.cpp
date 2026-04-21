#include "listing.h"

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

#include <fstream>
#include <iomanip>
#include <sstream>

namespace {

// converts int value to hex string
std::string hex_addr(int value, int width) {
    std::ostringstream os;
    os << std::uppercase << std::hex << std::setw(width) << std::setfill('0') << (value & 0xFFFFFF); // formats the hex val
    return os.str();
}

}

// this properly formats the listing file and writes it to the path parameter
bool ListingWriter::write_listing(const std::string& path, const std::vector<ListingRecord>& records,
                                  std::string& error_out) const {
    std::ofstream out(path.c_str());
    // if the file cant be created, return an err
    if (!out) {
        error_out = "unable to create listing file: " + path;
        return false;
    }

    // loop through the records and write them to the file
    for (std::size_t i = 0; i < records.size(); ++i) {
        const ListingRecord& r = records[i];
        if (r.line_number >= 0) {
            out << std::setw(5) << r.line_number << " ";
        } else {
            out << "      ";
        }
        out << std::setw(6) << r.loc_str << " "
            << std::left << std::setw(10) << r.label
            << std::setw(8) << r.opcode
            << std::setw(12) << r.operand
            << r.object_code << std::right << "\n";
    }
    return true;
}

// this writes the symbol table nicely formated to the path parameter file
bool ListingWriter::write_symtab(const std::string& path, const SymTab& symtab, std::string& error_out) const {
    std::ofstream out(path.c_str());
    // if the file cant be created, return an error
    if (!out) {
        error_out = "unable to create symbol table file: " + path;
        return false;
    }
    // we loop through the symbol table and write the entries to the file
    const std::map<std::string, int>& m = symtab.entries();
    for (std::map<std::string, int>::const_iterator it = m.begin(); it != m.end(); ++it) {
        out << std::left << std::setw(12) << it->first << " " << hex_addr(it->second, 4) << "\n";
    }
    return true;
}
