#include "pass2.h"

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <map>
#include <sstream>

// this runs pass 2 and returns a Pass2Result struct
Pass2Result Pass2::run(const Pass1Result& pass1, const OpTab& optab) const {
    Pass2Result out;
    bool base_active = false; // this is used to check if base addressing is active
    int base_address = 0;
    std::map<std::string, int> literal_addresses; // this map stores literals and their addresses

    // we loop through the lines from pass 1 and add the literals and their addresses
    for (std::size_t i = 0; i < pass1.lines.size(); ++i) {
        const SourceLine& line = pass1.lines[i];
        if (line.label == "*" && !line.opcode.empty() && line.opcode[0] == '=' && line.address >= 0) { // check if the line is a literal
            literal_addresses[line.opcode] = line.address; // add the literal and address to the map
        }
    }

    // we then loop through the lines from pass 1 and add the listing records to the output
    for (std::size_t i = 0; i < pass1.lines.size(); ++i) {
        const SourceLine& line = pass1.lines[i];
        ListingRecord r; // create a new listing record to store the line info
        r.line_number = line.line_number;
        r.loc_str = (line.address >= 0) ? hex(line.address, 4) : "";
        r.label = line.label;
        r.opcode = line.opcode;
        r.operand = line.operand;
        r.object_code = "";

        if (line.is_blank || line.is_comment) { // check if the line is blank or a comment
            out.listing_records.push_back(r); // add the listing record to the result
            continue;
        }

        const SourceLine* next = NULL;
        // we next loop throu the lines and find the next non-blank, non-comment line
        for (std::size_t j = i + 1; j < pass1.lines.size(); ++j) {
            if (!pass1.lines[j].is_blank && !pass1.lines[j].is_comment) {
                next = &pass1.lines[j];
                break;
            }
        }

        // we then encode the line and add the object code to the listing record
        r.object_code = encode_line(line, next, pass1.symtab, literal_addresses, optab,
                                    base_active, base_address, out.errors);
        out.listing_records.push_back(r); // add the listing record to the result
    }
    return out;
}


std::string Pass2::encode_line(const SourceLine& line, const SourceLine* next, const SymTab& symtab,
                               const std::map<std::string, int>& literal_addresses, const OpTab& optab,
                               bool& base_active, int& base_address,
                               std::vector<std::string>& errors) const {
    std::string op = upper(line.opcode); // convert the opcode to uppercase
    bool is_extended = (!op.empty() && op[0] == '+'); // check if the opcode is extended
    std::string base_op = is_extended ? op.substr(1) : op; // get the base opcode

    if (base_op == "START" || base_op == "END" || base_op == "RESW" || base_op == "RESB") return ""; // check if the opcode is a directive
    if (line.label == "*" && !line.opcode.empty() && line.opcode[0] == '=') { // check if the line is a literal
        bool ok_lit = false;
        std::string lit_obj = encode_literal_token(line.opcode, ok_lit); // encode the literal
        if (!ok_lit) { // check if the literal is valid 
            std::ostringstream os;
            os << "line " << line.line_number << ": invalid literal '" << line.opcode << "'";
            errors.push_back(os.str()); // add the error to the result
            return "";
        }
        return lit_obj; // return the literal object
    }

    if (base_op == "NOBASE") { // check if the opcode is NOBASE and if so, set the base active to false
        base_active = false; // 
        return "";
    }
    if (base_op == "BASE") {
        int value = 0;
        if (symtab.lookup(line.operand, value)) { // check if the operand is a valid symbol
            base_address = value;
            base_active = true; // set the base active to true
        } else {
            // if the operand is not a valid symbol, add an error to the result
            std::ostringstream os;
            os << "line " << line.line_number << ": undefined base symbol '" << line.operand << "'";
            errors.push_back(os.str());
        }
        return "";
    }
    if (base_op == "WORD") { // we check for WORD opcode and if so, parse the word value
        bool ok = true;
        int value = parse_constant(line.operand, ok);
        if (!ok) { // check if the operand is valid if not, add the error to errorlist
            std::ostringstream os;
            os << "line " << line.line_number << ": invalid word operand '" << line.operand << "'";
            errors.push_back(os.str());
            return "";
        }
        return hex(value & 0xFFFFFF, 6);
    }
    if (base_op == "BYTE") { // check if the opcode is BYTE and then parse the byte
        bool ok = true;
        int bits = 0;
        int value = parse_byte_value(line.operand, ok, bits); // parse the byte value
        if (!ok) { // check if the operand is valid if not, add the error to errorlist
            std::ostringstream os;
            os << "line " << line.line_number << ": invalid byte operand '" << line.operand << "'";
            errors.push_back(os.str());
            return "";
        }
        return hex(value, bits / 4);
    }

    if (!optab.has(base_op)) { // if the optab does not have that base op, return an empty string
        return "";
    }

    OpEntry e = optab.get(base_op); // get the opcode entry from the optab
    int format = 3;
    // we then set the instruction format 
    if (is_extended) format = 4;
    else if (e.valid_formats & (1 << 1)) format = 1;
    else if (e.valid_formats & (1 << 2)) format = 2;
    else format = 3;

    if (format == 1) { // if the format is 1, return the opcode byte in hex
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
            os << "line " << line.line_number << ": invalid register operand '" << line.operand << "'";
            errors.push_back(os.str());
            return "";
        }
        int code = (e.opcode_byte << 8) | ((r1 & 0xF) << 4) | (r2 & 0xF);
        return hex(code, 4);
    }

    // initialize the bit flags
    int n = 1, i = 1, x = 0, b = 0, p = 0, ext = (format == 4 ? 1 : 0);
    std::string operand = line.operand;
    if (!operand.empty() && operand[0] == '#') { // check if the operand is immediate
        n = 0; i = 1; operand = operand.substr(1);
    } else if (!operand.empty() && operand[0] == '@') { // check if the operand is indirect
        n = 1; i = 0; operand = operand.substr(1);
    }

    std::size_t comma = operand.find(",X"); // check if the operand is indexed
    if (comma != std::string::npos && comma + 2 == operand.size()) {
        x = 1;
        operand = operand.substr(0, comma);
    }

    int first = (e.opcode_byte & 0xFC) | ((n << 1) | i);
    if (base_op == "RSUB") { // check if the opcode is RSUB and if so, return the opcode byte in hex
        if (format == 4) {
            int code4 = (first << 24) | (1 << 20);
            return hex(code4, 8);
        }
        return hex((first << 16), 6);
    }

    int target = 0; // this is the target address
    bool immediate_const = false;
    bool ok_const = false;
    if (n == 0 && i == 1) { // check if the operand is immediate
        target = parse_constant(operand, ok_const);
        immediate_const = ok_const;
    }
    if (!immediate_const) { // if the operand is not immediate, look up the symbol in the symbol table
        if (!symtab.lookup(operand, target)) {
            std::map<std::string, int>::const_iterator lit_it = literal_addresses.find(operand);
            if (lit_it != literal_addresses.end()) {
                target = lit_it->second;
            } else {
                // if the symbol is not found, add error to errorlist
                std::ostringstream os;
                os << "line " << line.line_number << ": undefined symbol '" << operand << "'";
                errors.push_back(os.str());
                return "";
            }
        }
    }
    if (format == 4) { // if the format is 4, return the opcode byte in hex
        int flags = (x << 3) | (b << 2) | (p << 1) | ext; // set the bit flags
        int addr = immediate_const ? target : target;
        int code = (first << 24) | (flags << 20) | (addr & 0xFFFFF);
        return hex(code, 8);
    }

    int disp = 0;
    if (immediate_const) {
        b = 0; p = 0;
        disp = target & 0xFFF;
    } else { // if the operand is not immediate, calculate the displacement
        int next_addr = (next != NULL && next->address >= 0) ? next->address : (line.address + 3);
        int rel = target - next_addr;
        if (rel >= -2048 && rel <= 2047) { // check if the displacement is within range
            p = 1;
            b = 0;
            disp = rel & 0xFFF;
        } else if (base_active) { // check if the base is active and if so, calculate the displacement
            int brel = target - base_address;
            if (brel >= 0 && brel <= 4095) {
                b = 1;
                p = 0;
                disp = brel;
            } else { 
                // if the displacement is not within range, add an error to the errorlist
                std::ostringstream os;
                os << "line " << line.line_number << ": target out of range for format 3 '" << operand << "'";
                errors.push_back(os.str());
                return "";
            }
        } else {
            std::ostringstream os;
            os << "line " << line.line_number << ": cannot fit format 3 displacement for '" << operand << "'";
            errors.push_back(os.str());
            return "";
        }
    }

    int flags = (x << 3) | (b << 2) | (p << 1) | ext; 
    int code = (first << 16) | (flags << 12) | (disp & 0xFFF);
    return hex(code, 6);
}

// this parses the constant and returns the value
int Pass2::parse_constant(const std::string& text, bool& ok) const {
    ok = false;
    if (text.empty()) return 0;
    char* end = NULL;
    long value = std::strtol(text.c_str(), &end, 10);
    if (end != NULL && *end == '\0') { // check if the constant is valid
        ok = true;
        return static_cast<int>(value);
    }
    return 0; // if the constant is not valid, return 0
}

// this parses the byte value and returns the value
int Pass2::parse_byte_value(const std::string& operand, bool& ok, int& width_bits) const {
    ok = false;
    width_bits = 0;
    if (operand.size() < 3) return 0;
    char kind = operand[0];
    if (operand[1] != '\'' || operand[operand.size() - 1] != '\'') return 0; // check if the operand is valid
    std::string body = operand.substr(2, operand.size() - 3); 
    if (kind == 'C' || kind == 'c') { // check if the operand is a character
        int value = 0;
        width_bits = static_cast<int>(body.size() * 8);
        for (std::size_t i = 0; i < body.size(); ++i) { // loop thru the body and convert each character to a value
            value = (value << 8) | static_cast<unsigned char>(body[i]); 
        }
        ok = true;
        return value;
    }
    if (kind == 'X' || kind == 'x') { // check if the operand is a hex value
        // loop thru to check if the hex value is valid
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

// this encodes the literal token and returns the value in hex
std::string Pass2::encode_literal_token(const std::string& literal_token, bool& ok) const {
    ok = false;
    if (literal_token.size() < 2 || literal_token[0] != '=') return ""; // check if the literal token is valid
    int bits = 0;
    int value = parse_byte_value(literal_token.substr(1), ok, bits); // parse the byte value
    if (!ok) return ""; // if the byte value is not valid, return an empty string
    return hex(value, bits / 4); // return the byte value in hex
}

// this gets the register code for a given register name
int Pass2::get_register_code(const std::string& name) const {
    static std::map<std::string, int> regs; // map of register names and their codes
    if (regs.empty()) { // if the map is empty, initialize it
        regs["A"] = 0; regs["X"] = 1; regs["L"] = 2; regs["B"] = 3; regs["S"] = 4;
        regs["T"] = 5; regs["F"] = 6; regs["PC"] = 8; regs["SW"] = 9;
    }
    std::map<std::string, int>::const_iterator it = regs.find(name); // find the register code for the given register name
    if (it == regs.end()) return -1;
    return it->second; // return the register code
}

// this converts a given string to uppercase
std::string Pass2::upper(const std::string& s) const {
    std::string out = s;
    for (std::size_t i = 0; i < out.size(); ++i) { // loop thru the string and convert each character to uppercase
        out[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(out[i])));
    }
    return out;
}

// this converts an int value to hex
std::string Pass2::hex(int value, int width) const {
    std::ostringstream os;
    os << std::uppercase << std::hex << std::setw(width) << std::setfill('0'); // set the width and fill the value with 0
    if (width <= 6) os << (value & 0xFFFFFF);
    else os << (value & 0xFFFFFFFF);
    return os.str();
}
