#include "pass1.h"

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

#include <cctype>
#include <cstdlib>
#include <sstream>

Pass1Result Pass1::run(const std::vector<SourceLine>& parsed, const OpTab& optab) const {
    Pass1Result result;
    result.lines = parsed;
    result.start_address = 0;
    result.program_length = 0;

    int locctr = 0;
    bool start_seen = false;

    for (std::size_t i = 0; i < result.lines.size(); ++i) {
        SourceLine& line = result.lines[i];
        if (line.is_comment || line.is_blank) continue;

        std::string op = upper(line.opcode);
        bool is_extended = (!op.empty() && op[0] == '+');
        std::string base_op = is_extended ? op.substr(1) : op;

        if (op == "START") {
            start_seen = true;
            if (!line.operand.empty()) {
                locctr = parse_number(line.operand, true);
                result.start_address = locctr;
            }
            line.address = locctr;
            continue;
        }

        line.address = locctr;

        // literal pool emitted lines in sample format use "*" as a synthetic label
        bool is_literal_pool_line =
            (line.label == "*" && !line.opcode.empty() && line.opcode[0] == '=');

        std::string err;
        if (!line.label.empty() && line.label != "*" && !result.symtab.insert(line.label, locctr, err)) {
            std::ostringstream os;
            os << "Line " << line.line_number << ": " << err;
            result.errors.push_back(os.str());
        }

        if (is_literal_pool_line) {
            int lsz = get_literal_size(line.opcode);
            if (lsz < 0) {
                std::ostringstream os;
                os << "Line " << line.line_number << ": Invalid literal '" << line.opcode << "'";
                result.errors.push_back(os.str());
            } else {
                locctr += lsz;
            }
            continue;
        }

        if (base_op == "END" || base_op == "BASE" || base_op == "NOBASE") {
            continue;
        } else if (base_op == "WORD") {
            locctr += 3;
        } else if (base_op == "RESW") {
            locctr += 3 * parse_number(line.operand, false);
        } else if (base_op == "RESB") {
            locctr += parse_number(line.operand, false);
        } else if (base_op == "BYTE") {
            int sz = get_byte_size(line.operand);
            if (sz < 0) {
                std::ostringstream os;
                os << "Line " << line.line_number << ": Invalid BYTE operand '" << line.operand << "'";
                result.errors.push_back(os.str());
            } else {
                locctr += sz;
            }
        } else if (base_op == "EQU" || base_op == "ORG" || base_op == "LTORG" ||
                   base_op == "EXTDEF" || base_op == "EXTREF" || base_op == "CSECT") {
            std::ostringstream os;
            os << "Line " << line.line_number << ": Unsupported directive '" << base_op << "'";
            result.errors.push_back(os.str());
        } else if (optab.has(base_op)) {
            OpEntry e = optab.get(base_op);
            if (is_extended) {
                if (!(e.valid_formats & (1 << 4))) {
                    std::ostringstream os;
                    os << "Line " << line.line_number << ": " << base_op << " does not support format 4";
                    result.errors.push_back(os.str());
                }
                locctr += 4;
            } else if (e.valid_formats & (1 << 3)) {
                locctr += 3;
            } else if (e.valid_formats & (1 << 2)) {
                locctr += 2;
            } else {
                locctr += 1;
            }
        } else {
            std::ostringstream os;
            os << "Line " << line.line_number << ": Unknown opcode/directive '" << line.opcode << "'";
            result.errors.push_back(os.str());
        }
    }

    if (!start_seen) {
        result.start_address = 0;
    }
    result.program_length = locctr - result.start_address;
    return result;
}

int Pass1::get_byte_size(const std::string& operand) const {
    if (operand.size() < 3) return -1;
    char kind = operand[0];
    if (operand[1] != '\'' || operand[operand.size() - 1] != '\'') return -1;
    std::string body = operand.substr(2, operand.size() - 3);
    if (kind == 'C' || kind == 'c') {
        return static_cast<int>(body.size());
    }
    if (kind == 'X' || kind == 'x') {
        if (body.size() % 2 != 0) return -1;
        for (std::size_t i = 0; i < body.size(); ++i) {
            if (!std::isxdigit(static_cast<unsigned char>(body[i]))) return -1;
        }
        return static_cast<int>(body.size() / 2);
    }
    return -1;
}

int Pass1::get_literal_size(const std::string& literal_token) const {
    if (literal_token.size() < 2 || literal_token[0] != '=') return -1;
    return get_byte_size(literal_token.substr(1));
}

int Pass1::parse_number(const std::string& text, bool hex_default) const {
    if (text.empty()) return 0;
    char* end = NULL;
    long value = std::strtol(text.c_str(), &end, hex_default ? 16 : 10);
    if (end != NULL && *end == '\0') return static_cast<int>(value);
    value = std::strtol(text.c_str(), &end, 10);
    return static_cast<int>(value);
}

std::string Pass1::upper(const std::string& s) const {
    std::string out = s;
    for (std::size_t i = 0; i < out.size(); ++i) {
        out[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(out[i])));
    }
    return out;
}
