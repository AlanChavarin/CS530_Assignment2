#include "optab.h"

// CS530 Assignment 2 - LXE Assembler
// Team: Alan Chavarin (cssc2513, RedID 827690364)
//       Amir Ali (cssc2503, RedID 132395455)

#include <stdexcept>

namespace {
const int F1 = 1 << 1;
const int F2 = 1 << 2;
const int F34 = (1 << 3) | (1 << 4);
}  // namespace

OpTab::OpTab() {
    add("ADD", 0x18, F34); add("ADDF", 0x58, F34); add("ADDR", 0x90, F2);
    add("AND", 0x40, F34); add("CLEAR", 0xB4, F2); add("COMP", 0x28, F34);
    add("COMPF", 0x88, F34); add("COMPR", 0xA0, F2); add("DIV", 0x24, F34);
    add("DIVF", 0x64, F34); add("DIVR", 0x9C, F2); add("FIX", 0xC4, F1);
    add("FLOAT", 0xC0, F1); add("HIO", 0xF4, F1); add("J", 0x3C, F34);
    add("JEQ", 0x30, F34); add("JGT", 0x34, F34); add("JLT", 0x38, F34);
    add("JSUB", 0x48, F34); add("LDA", 0x00, F34); add("LDB", 0x68, F34);
    add("LDCH", 0x50, F34); add("LDF", 0x70, F34); add("LDL", 0x08, F34);
    add("LDS", 0x6C, F34); add("LDT", 0x74, F34); add("LDX", 0x04, F34);
    add("LPS", 0xD0, F34); add("MUL", 0x20, F34); add("MULF", 0x60, F34);
    add("MULR", 0x98, F2); add("NORM", 0xC8, F1); add("OR", 0x44, F34);
    add("RD", 0xD8, F34); add("RMO", 0xAC, F2); add("RSUB", 0x4C, F34);
    add("SHIFTL", 0xA4, F2); add("SHIFTR", 0xA8, F2); add("SIO", 0xF0, F1);
    add("SSK", 0xEC, F34); add("STA", 0x0C, F34); add("STB", 0x78, F34);
    add("STCH", 0x54, F34); add("STF", 0x80, F34); add("STI", 0xD4, F34);
    add("STL", 0x14, F34); add("STS", 0x7C, F34); add("STSW", 0xE8, F34);
    add("STT", 0x84, F34); add("STX", 0x10, F34); add("SUB", 0x1C, F34);
    add("SUBF", 0x5C, F34); add("SUBR", 0x94, F2); add("SVC", 0xB0, F2);
    add("TD", 0xE0, F34); add("TIO", 0xF8, F1); add("TIX", 0x2C, F34);
    add("TIXR", 0xB8, F2); add("WD", 0xDC, F34);
}

void OpTab::add(const std::string& mnem, int opcode, int mask) {
    OpEntry e;
    e.mnemonic = mnem;
    e.opcode_byte = opcode;
    e.valid_formats = mask;
    table_[mnem] = e;
}

bool OpTab::has(const std::string& mnemonic) const {
    return table_.find(mnemonic) != table_.end();
}

OpEntry OpTab::get(const std::string& mnemonic) const {
    std::unordered_map<std::string, OpEntry>::const_iterator it = table_.find(mnemonic);
    if (it == table_.end()) {
        throw std::runtime_error("Unknown mnemonic: " + mnemonic);
    }
    return it->second;
}
