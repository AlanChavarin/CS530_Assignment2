#include "listing.h"

//cs530 assignment 2 - lxe assembler
//Team: Alan Chavarin (cssc2513, redid 827690364)
//Amir Ali (cssc2503, redid 132395455)

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <set>
#include <sstream>

namespace {

// converts int value to hex string
std::string hex_addr(int value, int width) {
    std::ostringstream os;
    os << std::uppercase << std::hex << std::setw(width) << std::setfill('0') << (value & 0xFFFFFF); // formats the hex val
    return os.str();
}

// Parses BYTE operand (C'..' / X'..') for literal table display; same rules as pass2.
bool parse_byte_operand(const std::string& operand, int& value_out, int& width_bits, std::string& c_inner_out) {
    value_out = 0;
    width_bits = 0;
    c_inner_out.clear();
    if (operand.size() < 3) return false;
    char kind = operand[0];
    if (operand[1] != '\'' || operand[operand.size() - 1] != '\'') return false;
    std::string body = operand.substr(2, operand.size() - 3);
    if (kind == 'C' || kind == 'c') {
        c_inner_out = body;
        int value = 0;
        width_bits = static_cast<int>(body.size() * 8);
        for (std::size_t i = 0; i < body.size(); ++i) {
            value = (value << 8) | static_cast<unsigned char>(body[i]);
        }
        value_out = value;
        return true;
    }
    if (kind == 'X' || kind == 'x') {
        for (std::size_t i = 0; i < body.size(); ++i) {
            if (!std::isxdigit(static_cast<unsigned char>(body[i]))) return false;
        }
        if (body.size() % 2 != 0) return false;
        width_bits = static_cast<int>(body.size() * 4);
        value_out = static_cast<int>(std::strtol(body.c_str(), NULL, 16));
        return true;
    }
    return false;
}

std::string literal_operand_hex(const std::string& literal_token) {
    if (literal_token.size() < 2 || literal_token[0] != '=') return "";
    int value = 0;
    int bits = 0;
    std::string dummy;
    if (!parse_byte_operand(literal_token.substr(1), value, bits, dummy)) return "";
    std::ostringstream os;
    os << std::uppercase << std::hex << std::setw(bits / 4) << std::setfill('0') << (value & 0xFFFFFF);
    return os.str();
}

int literal_length_bytes(const std::string& literal_token) {
    if (literal_token.size() < 2 || literal_token[0] != '=') return 0;
    int value = 0;
    int bits = 0;
    std::string cinner;
    if (!parse_byte_operand(literal_token.substr(1), value, bits, cinner)) return 0;
    return bits / 8;
}

std::string literal_display_name(const std::string& literal_token) {
    if (literal_token.size() < 2 || literal_token[0] != '=') return "";
    int value = 0;
    int bits = 0;
    std::string cinner;
    if (!parse_byte_operand(literal_token.substr(1), value, bits, cinner)) return "";
    if (!cinner.empty()) return cinner;
    // X'..' — use hex body as the name
    std::string op = literal_token.substr(1);
    if (op.size() >= 3 && (op[0] == 'X' || op[0] == 'x') && op[1] == '\'') {
        return op.substr(2, op.size() - 3);
    }
    return "";
}

}  // namespace

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
bool ListingWriter::write_symtab(const std::string& path, const Pass1Result& p1, const Pass2Result& p2,
                                 std::string& error_out) const {
    std::ofstream out(path.c_str());
    if (!out) {
        error_out = "unable to create symbol table file: " + path;
        return false;
    }

    out << "CSect   Symbol  Value   LENGTH  Flags:\n";
    out << "--------------------------------------\n";

    const std::string& csect = p1.program_name;
    if (!csect.empty()) {
        out << std::left << std::setw(16) << csect << hex_addr(p1.start_address, 6) << "  " << hex_addr(p1.program_length, 6)
            << "\n";
    }

    std::set<std::string> seen;
    for (std::size_t i = 0; i < p1.lines.size(); ++i) {
        const SourceLine& line = p1.lines[i];
        if (line.is_comment || line.is_blank) continue;
        if (line.label.empty() || line.label == "*") continue;
        if (!csect.empty() && line.label == csect) continue;
        if (seen.count(line.label)) continue;
        int addr = 0;
        if (!p1.symtab.lookup(line.label, addr)) continue;
        seen.insert(line.label);
        out << "        " << std::left << std::setw(8) << line.label << hex_addr(addr, 6) << "          R\n";
    }

    if (!p2.literal_table.empty()) {
        out << "\nLiteral Table \n";
        out << "Name  Operand   Address  Length:\n";
        out << "--------------------------------\n";
        for (std::size_t i = 0; i < p2.literal_table.size(); ++i) {
            const std::string& tok = p2.literal_table[i].first;
            int lit_addr = p2.literal_table[i].second;
            std::string lname = literal_display_name(tok);
            std::string lhex = literal_operand_hex(tok);
            int llen = literal_length_bytes(tok);
            out << std::left << std::setw(6) << lname << std::setw(8) << lhex << std::setw(8) << hex_addr(lit_addr, 4)
                << "     " << llen << "\n";
        }
    }

    return true;
}
