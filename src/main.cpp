//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

#include <iostream>
#include <string>
#include <vector>

#include "listing.h"
#include "optab.h"
#include "parser.h"
#include "pass1.h"
#include "pass2.h"

namespace {

// this replaces the extension for the path of a file with ext
std::string replace_ext(const std::string& path, const std::string& ext) {
    std::size_t dot = path.find_last_of('.');
    if (dot == std::string::npos) return path + ext;
    return path.substr(0, dot) + ext; // returns the path with the new extension
}

// this prints the vector of errors to the output
void print_errors(const std::vector<std::string>& errors) {
    for (std::size_t i = 0; i < errors.size(); ++i) {
        std::cerr << "  - " << errors[i] << "\n";
    }
}

} 

int main(int argc, char* argv[]) {
    // first arg is prgram name, so if it's less than 2, no input file
    if (argc < 2) {
        std::cerr << "no input files provided\n";
        std::cerr << "usage is ./lxe file1.sic [file2.sic ...]\n";
        return 1;
    }

    // we initialize the parser and passes.
    Parser parser;
    OpTab optab;
    Pass1 pass1;
    Pass2 pass2;
    ListingWriter writer; // this is used to write the listing and symbol table files.

    // then we parse the command line args
    int overall_status = 0;
    for (int i = 1; i < argc; ++i) {
        std::string input = argv[i];
        std::cout << "processing: " << input << "\n";

        std::string parse_error; // this is for storign the parse like if it has unsupported instruction EQU/ORG.
        std::vector<SourceLine> lines = parser.parse_file(input, parse_error);
        // if there were errors, we print them to the output.
        if (!parse_error.empty()) {
            std::cerr << parse_error << "\n";
            overall_status = 1;
            continue;
        }

        Pass1Result p1 = pass1.run(lines, optab); // does pass 1
        // print the errors if there are any
        if (!p1.errors.empty()) {
            std::cerr << "pass 1 errors for " << input << ":\n";
            print_errors(p1.errors);
            overall_status = 1;
            continue;
        }

        Pass2Result p2 = pass2.run(p1, optab); // does pass 2
        // print the errors if there are any
        if (!p2.errors.empty()) {
            std::cerr << "pass 2 errors for " << input << ":\n";
            print_errors(p2.errors);
            overall_status = 1;
            continue;
        }

        std::string list_file = replace_ext(input, ".l"); // we rename the file extension to .l for the listing file
        std::string sym_file = replace_ext(input, ".st"); // do the same for the symbol table file

        
        std::string err;
        // write the listing file and check for errors
        if (!writer.write_listing(list_file, p2.listing_records, err)) {
            std::cerr << err << "\n";
            overall_status = 1;
            continue;
        }
        // write the symbol table file and check for errors
        if (!writer.write_symtab(sym_file, p1.symtab, err)) {
            std::cerr << err << "\n";
            overall_status = 1;
            continue;
        }
        std::cout << "  wrote " << list_file << " and " << sym_file << "\n";
    }

    return overall_status;
}
