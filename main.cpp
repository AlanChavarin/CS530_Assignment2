// CS530 Assignment 2 - LXE Assembler
// Team: Alan Chavarin (cssc2513, RedID 827690364)
//       Amir Ali (cssc2503, RedID 132395455)

#include <iostream>
#include <string>
#include <vector>

#include "listing.h"
#include "optab.h"
#include "parser.h"
#include "pass1.h"
#include "pass2.h"

namespace {

std::string replace_ext(const std::string& path, const std::string& ext) {
    std::size_t dot = path.find_last_of('.');
    if (dot == std::string::npos) return path + ext;
    return path.substr(0, dot) + ext;
}

void print_errors(const std::vector<std::string>& errors) {
    for (std::size_t i = 0; i < errors.size(); ++i) {
        std::cerr << "  - " << errors[i] << "\n";
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "No input files provided.\n";
        std::cerr << "Usage: ./lxe file1.sic [file2.sic ...]\n";
        return 1;
    }

    Parser parser;
    OpTab optab;
    Pass1 pass1;
    Pass2 pass2;
    ListingWriter writer;

    int overall_status = 0;
    for (int i = 1; i < argc; ++i) {
        std::string input = argv[i];
        std::cout << "Processing: " << input << "\n";

        std::string parse_error;
        std::vector<SourceLine> lines = parser.parse_file(input, parse_error);
        if (!parse_error.empty()) {
            std::cerr << parse_error << "\n";
            overall_status = 1;
            continue;
        }

        Pass1Result p1 = pass1.run(lines, optab);
        if (!p1.errors.empty()) {
            std::cerr << "Pass 1 errors for " << input << ":\n";
            print_errors(p1.errors);
            overall_status = 1;
            continue;
        }

        Pass2Result p2 = pass2.run(p1, optab);
        if (!p2.errors.empty()) {
            std::cerr << "Pass 2 errors for " << input << ":\n";
            print_errors(p2.errors);
            overall_status = 1;
            continue;
        }

        std::string list_file = replace_ext(input, ".l");
        std::string sym_file = replace_ext(input, ".st");

        std::string err;
        if (!writer.write_listing(list_file, p2.listing_records, err)) {
            std::cerr << err << "\n";
            overall_status = 1;
            continue;
        }
        if (!writer.write_symtab(sym_file, p1.symtab, err)) {
            std::cerr << err << "\n";
            overall_status = 1;
            continue;
        }
        std::cout << "  wrote " << list_file << " and " << sym_file << "\n";
    }

    return overall_status;
}
