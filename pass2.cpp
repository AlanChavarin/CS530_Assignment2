#include "pass2.h"

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <map>
#include <sstream>

Pass2Result Pass2::run(const Pass1Result& pass1, const OpTab& optab) const {
    Pass2Result out;
    bool base_active = false;
    int base_address = 0;
    std::map<std::string, int> literal_addresses;

    for (std::size_t i = 0; i < pass1.lines.size(); ++i) {
        const SourceLine& line = pass1.lines[i];
        if (line.label == "*" && !line.opcode.empty() && line.opcode[0] == '=' && line.address >= 0) {
            literal_addresses[line.opcode] = line.address;
        }
    }

    for (std::size_t i = 0; i < pass1.lines.size(); ++i) {
        const SourceLine& line = pass1.lines[i];
        ListingRecord r;
        r.line_number = line.line_number;
        r.loc_str = (line.address >= 0) ? hex(line.address, 4) : "";
        r.label = line.label;
        r.opcode = line.opcode;
        r.operand = line.operand;
        r.object_code = "";

        if (line.is_blank || line.is_comment) {
            out.listing_records.push_back(r);
            continue;
        }

        const SourceLine* next = NULL;
        for (std::size_t j = i + 1; j < pass1.lines.size(); ++j) {
            if (!pass1.lines[j].is_blank && !pass1.lines[j].is_comment) {
                next = &pass1.lines[j];
                break;
            }
        }

        r.object_code = encode_line(line, next, pass1.symtab, literal_addresses, optab,
                                    base_active, base_address, out.errors);
        out.listing_records.push_back(r);
    }
    return out;
}

std::string Pass2::encode_line(const SourceLine& line, const SourceLine* next, const SymTab& symtab,
                               const std::map<std::string, int>& literal_addresses, const OpTab& optab,
                               bool& base_active, int& base_address,
                               std::vector<std::string>& errors) const {
    std::string op = upper(line.opcode);
    bool is_extended = (!op.empty() && op[0] == '+');
    std::string base_op = is_extended ? op.substr(1) : op;

    if (base_op == "START" || base_op == "END" || base_op == "RESW" || base_op == "RESB") return "";
    if (line.label == "*" && !line.opcode.empty() && line.opcode[0] == '=') {
        bool ok_lit = false;
        std::string lit_obj = encode_literal_token(line.opcode, ok_lit);
        if (!ok_lit) {
            std::ostringstream os;
            os << "Line " << line.line_number << ": Invalid literal '" << line.opcode << "'";
            errors.push_back(os.str());
            return "";
        }
        return lit_obj;
    }
    if (base_op == "NOBASE") {
        base_active = false;
        return "";
    }
    if (base_op == "BASE") {
        int value = 0;
        if (symtab.lookup(line.operand, value)) {
            base_address = value;
            base_active = true;
        } else {
            std::ostringstream os;
            os << "Line " << line.line_number << ": Undefined BASE symbol '" << line.operand << "'";
            errors.push_back(os.str());
        }
        return "";
    }
    if (base_op == "WORD") {
        bool ok = true;
        int value = parse_constant(line.operand, ok);
        if (!ok) {
            std::ostringstream os;
            os << "Line " << line.line_number << ": Invalid WORD operand '" << line.operand << "'";
            errors.push_back(os.str());
            return "";
        }
        return hex(value & 0xFFFFFF, 6);
    }
    if (base_op == "BYTE") {
        bool ok = true;
        int bits = 0;
        int value = parse_byte_value(line.operand, ok, bits);
        if (!ok) {
            std::ostringstream os;
            os << "Line " << line.line_number << ": Invalid BYTE operand '" << line.operand << "'";
            errors.push_back(os.str());
            return "";
        }
        return hex(value, bits / 4);
    }

    if (!optab.has(base_op)) {
        return "";
    }

    OpEntry e = optab.get(base_op);
    int format = 3;
    if (is_extended) format = 4;
    else if (e.valid_formats & (1 << 1)) format = 1;
    else if (e.valid_formats & (1 << 2)) format = 2;
    else format = 3;

    if (format == 1) {
        return hex(e.opcode_byte, 2);
    }

    if (format == 2) {
        int r1 = 0;
        int r2 = 0;
        std::string opr = line.operand;
        std::string left = opr;
        std::string right = "";
        std::size_t comma = opr.find(',');
        if (comma != std::string::npos) {
            left = opr.substr(0, comma);
            right = opr.substr(comma + 1);
        }
        r1 = get_register_code(upper(left));
        r2 = right.empty() ? 0 : get_register_code(upper(right));
        if (r1 < 0 || r2 < 0) {
            std::ostringstream os;
            os << "Line " << line.line_number << ": Invalid register operand '" << line.operand << "'";
            errors.push_back(os.str());
            return "";
        }
        int code = (e.opcode_byte << 8) | ((r1 & 0xF) << 4) | (r2 & 0xF);
        return hex(code, 4);
    }

    int n = 1, i = 1, x = 0, b = 0, p = 0, ext = (format == 4 ? 1 : 0);
    std::string operand = line.operand;
    if (!operand.empty() && operand[0] == '#') {
        n = 0; i = 1; operand = operand.substr(1);
    } else if (!operand.empty() && operand[0] == '@') {
        n = 1; i = 0; operand = operand.substr(1);
    }

    std::size_t comma = operand.find(",X");
    if (comma != std::string::npos && comma + 2 == operand.size()) {
        x = 1;
        operand = operand.substr(0, comma);
    }

    int first = (e.opcode_byte & 0xFC) | ((n << 1) | i);
    if (base_op == "RSUB") {
        if (format == 4) {
            int code4 = (first << 24) | (1 << 20);
            return hex(code4, 8);
        }
        return hex((first << 16), 6);
    }

    int target = 0;
    bool immediate_const = false;
    bool ok_const = false;
    if (n == 0 && i == 1) {
        target = parse_constant(operand, ok_const);
        immediate_const = ok_const;
    }
    if (!immediate_const) {
        if (!symtab.lookup(operand, target)) {
            std::map<std::string, int>::const_iterator lit_it = literal_addresses.find(operand);
            if (lit_it != literal_addresses.end()) {
                target = lit_it->second;
            } else {
                std::ostringstream os;
                os << "Line " << line.line_number << ": Undefined symbol '" << operand << "'";
                errors.push_back(os.str());
                return "";
            }
        }
    }
    if (format == 4) {
        int flags = (x << 3) | (b << 2) | (p << 1) | ext;
        int addr = immediate_const ? target : target;
        int code = (first << 24) | (flags << 20) | (addr & 0xFFFFF);
        return hex(code, 8);
    }

    int disp = 0;
    if (immediate_const) {
        b = 0; p = 0;
        disp = target & 0xFFF;
    } else {
        int next_addr = (next != NULL && next->address >= 0) ? next->address : (line.address + 3);
        int rel = target - next_addr;
        if (rel >= -2048 && rel <= 2047) {
            p = 1;
            b = 0;
            disp = rel & 0xFFF;
        } else if (base_active) {
            int brel = target - base_address;
            if (brel >= 0 && brel <= 4095) {
                b = 1;
                p = 0;
                disp = brel;
            } else {
                std::ostringstream os;
                os << "Line " << line.line_number << ": Target out of range for format 3 '" << operand << "'";
                errors.push_back(os.str());
                return "";
            }
        } else {
            std::ostringstream os;
            os << "Line " << line.line_number << ": Cannot fit format 3 displacement for '" << operand << "'";
            errors.push_back(os.str());
            return "";
        }
    }

    int flags = (x << 3) | (b << 2) | (p << 1) | ext;
    int code = (first << 16) | (flags << 12) | (disp & 0xFFF);
    return hex(code, 6);
}

int Pass2::parse_constant(const std::string& text, bool& ok) const {
    ok = false;
    if (text.empty()) return 0;
    char* end = NULL;
    long value = std::strtol(text.c_str(), &end, 10);
    if (end != NULL && *end == '\0') {
        ok = true;
        return static_cast<int>(value);
    }
    return 0;
}

int Pass2::parse_byte_value(const std::string& operand, bool& ok, int& width_bits) const {
    ok = false;
    width_bits = 0;
    if (operand.size() < 3) return 0;
    char kind = operand[0];
    if (operand[1] != '\'' || operand[operand.size() - 1] != '\'') return 0;
    std::string body = operand.substr(2, operand.size() - 3);
    if (kind == 'C' || kind == 'c') {
        int value = 0;
        width_bits = static_cast<int>(body.size() * 8);
        for (std::size_t i = 0; i < body.size(); ++i) {
            value = (value << 8) | static_cast<unsigned char>(body[i]);
        }
        ok = true;
        return value;
    }
    if (kind == 'X' || kind == 'x') {
        for (std::size_t i = 0; i < body.size(); ++i) {
            if (!std::isxdigit(static_cast<unsigned char>(body[i]))) return 0;
        }
        width_bits = static_cast<int>(body.size() * 4);
        int value = static_cast<int>(std::strtol(body.c_str(), NULL, 16));
        ok = true;
        return value;
    }
    return 0;
}

std::string Pass2::encode_literal_token(const std::string& literal_token, bool& ok) const {
    ok = false;
    if (literal_token.size() < 2 || literal_token[0] != '=') return "";
    int bits = 0;
    int value = parse_byte_value(literal_token.substr(1), ok, bits);
    if (!ok) return "";
    return hex(value, bits / 4);
}

int Pass2::get_register_code(const std::string& name) const {
    static std::map<std::string, int> regs;
    if (regs.empty()) {
        regs["A"] = 0; regs["X"] = 1; regs["L"] = 2; regs["B"] = 3; regs["S"] = 4;
        regs["T"] = 5; regs["F"] = 6; regs["PC"] = 8; regs["SW"] = 9;
    }
    std::map<std::string, int>::const_iterator it = regs.find(name);
    if (it == regs.end()) return -1;
    return it->second;
}

std::string Pass2::upper(const std::string& s) const {
    std::string out = s;
    for (std::size_t i = 0; i < out.size(); ++i) {
        out[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(out[i])));
    }
    return out;
}

std::string Pass2::hex(int value, int width) const {
    std::ostringstream os;
    os << std::uppercase << std::hex << std::setw(width) << std::setfill('0');
    if (width <= 6) os << (value & 0xFFFFFF);
    else os << (value & 0xFFFFFFFF);
    return os.str();
}
