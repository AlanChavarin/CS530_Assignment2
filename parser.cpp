#include "parser.h"

// CS530 Assignment 2 - LXE Assembler
// Team: Alan Chavarin (cssc2513, RedID 827690364)
//       Amir Ali (cssc2503, RedID 132395455)

#include <fstream>
#include <sstream>

namespace {

std::string trim(const std::string& s) {
    std::size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    std::size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

}  // namespace

std::vector<SourceLine> Parser::parse_file(const std::string& path, std::string& error_out) const {
    std::vector<SourceLine> lines;
    std::ifstream in(path.c_str());
    if (!in) {
        error_out = "Unable to open input file: " + path;
        return lines;
    }

    std::string raw;
    int line_number = 1;
    while (std::getline(in, raw)) {
        lines.push_back(parse_line(raw, line_number));
        ++line_number;
    }
    return lines;
}

SourceLine Parser::parse_line(const std::string& line, int line_number) const {
    SourceLine out;
    out.raw_text = line;
    out.line_number = line_number;
    out.address = -1;

    std::string cleaned = line;
    if (!cleaned.empty() && cleaned[0] == '.') {
        out.is_comment = true;
        out.comment = cleaned;
        return out;
    }

    if (trim(cleaned).empty()) {
        out.is_blank = true;
        return out;
    }

    bool has_label = !cleaned.empty() && cleaned[0] != ' ' && cleaned[0] != '\t';
    std::istringstream iss(cleaned);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) {
        out.is_blank = true;
        return out;
    }

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
    } else {
        out.opcode = tokens[0];
        if (tokens.size() >= 2) out.operand = tokens[1];
        if (tokens.size() >= 3) {
            out.comment = tokens[2];
            for (std::size_t i = 3; i < tokens.size(); ++i) {
                out.comment += " " + tokens[i];
            }
        }
    }

    return out;
}
