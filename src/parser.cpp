#include "parser.h"

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

#include <fstream>
#include <sstream>

namespace {

// this trims the string of whitespace (helper funct)
std::string trim(const std::string& s) {
    std::size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    std::size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

}

// this parses the file and returns a vector of SourceLine structs
std::vector<SourceLine> Parser::parse_file(const std::string& path, std::string& error_out) const {
    std::vector<SourceLine> lines; // this is the vector of SourceLine structs that will be returned
    std::ifstream in(path.c_str()); 
    if (!in) { // if the file cant be opened, return an error
        error_out = "unable to open input file: " + path;
        return lines;
    }

    std::string raw; // raw of source code
    int line_number = 1;
    // we loop through the file, get each line, parse it, and add it to the vector
    while (std::getline(in, raw)) {
        lines.push_back(parse_line(raw, line_number));
        ++line_number;
    }
    return lines;
}

// this takes a line and parses it into a SourceLine struct
SourceLine Parser::parse_line(const std::string& line, int line_number) const {
    SourceLine out;
    out.raw_text = line; // raw of source code
    out.line_number = line_number; // line number of source code
    out.address = -1;

    std::string cleaned = line;
    // in SIC/XE source, comment lines commonly start with '.' and many
    // textbook samples also use '*' banner/comment lines.
    if (!cleaned.empty() && (cleaned[0] == '.' || cleaned[0] == '*')) {
        out.is_comment = true;
        out.comment = cleaned;
        return out;
    }

    // if the line is empty, set the is_blank to true
    if (trim(cleaned).empty()) {
        out.is_blank = true;
        return out;
    }

    bool has_label = !cleaned.empty() && cleaned[0] != ' ' && cleaned[0] != '\t'; // checks if the line has a label
    std::istringstream iss(cleaned); 
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) { // we loop through the tokens and add them to the vector
        tokens.push_back(token);
    }

    // if the line is empty, set is_blank to true
    if (tokens.empty()) {
        out.is_blank = true;
        return out;
    }

    // if the line has a label, set the label, opcode, and operand
    if (has_label) {
        out.label = tokens[0];
        if (tokens.size() >= 2) out.opcode = tokens[1];
        if (tokens.size() >= 3) out.operand = tokens[2];
        if (tokens.size() >= 4) {
            out.comment = tokens[3];
            for (std::size_t i = 4; i < tokens.size(); ++i) {
                out.comment += " " + tokens[i];
            }
        }
    } else { // if the line does not have a label, set the opcode and operand
        out.opcode = tokens[0];
        if (tokens.size() >= 2) out.operand = tokens[1];
        if (tokens.size() >= 3) {
            out.comment = tokens[2];
            for (std::size_t i = 3; i < tokens.size(); ++i) {
                out.comment += " " + tokens[i];
            }
        }
    }

    return out; // return the SourceLine struct
}
