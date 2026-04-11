# SIC/XE OPTAB Reference
## CS530 Spring 2025 — Project #2: LXE Assembler
**Team:** Alan Chavarin (cssc2513) | Amir Ali (cssc2503)

---

## Instruction Table

| Mnemonic | Opcode (hex) | Format |
|----------|-------------|--------|
| ADD      | 18          | 3/4    |
| ADDF     | 58          | 3/4    |
| ADDR     | 90          | 2      |
| AND      | 40          | 3/4    |
| CLEAR    | B4          | 2      |
| COMP     | 28          | 3/4    |
| COMPF    | 88          | 3/4    |
| COMPR    | A0          | 2      |
| DIV      | 24          | 3/4    |
| DIVF     | 64          | 3/4    |
| DIVR     | 9C          | 2      |
| FIX      | C4          | 1      |
| FLOAT    | C0          | 1      |
| HIO      | F4          | 1      |
| J        | 3C          | 3/4    |
| JEQ      | 30          | 3/4    |
| JGT      | 34          | 3/4    |
| JLT      | 38          | 3/4    |
| JSUB     | 48          | 3/4    |
| LDA      | 00          | 3/4    |
| LDB      | 68          | 3/4    |
| LDCH     | 50          | 3/4    |
| LDF      | 70          | 3/4    |
| LDL      | 08          | 3/4    |
| LDS      | 6C          | 3/4    |
| LDT      | 74          | 3/4    |
| LDX      | 04          | 3/4    |
| LPS      | D0          | 3/4    |
| MUL      | 20          | 3/4    |
| MULF     | 60          | 3/4    |
| MULR     | 98          | 2      |
| NORM     | C8          | 1      |
| OR       | 44          | 3/4    |
| RD       | D8          | 3/4    |
| RMO      | AC          | 2      |
| RSUB     | 4C          | 3/4    |
| SHIFTL   | A4          | 2      |
| SHIFTR   | A8          | 2      |
| SIO      | F0          | 1      |
| SSK      | EC          | 3/4    |
| STA      | 0C          | 3/4    |
| STB      | 78          | 3/4    |
| STCH     | 54          | 3/4    |
| STF      | 80          | 3/4    |
| STI      | D4          | 3/4    |
| STL      | 14          | 3/4    |
| STS      | 7C          | 3/4    |
| STSW     | E8          | 3/4    |
| STT      | 84          | 3/4    |
| STX      | 10          | 3/4    |
| SUB      | 1C          | 3/4    |
| SUBF     | 5C          | 3/4    |
| SUBR     | 94          | 2      |
| SVC      | B0          | 2      |
| TD       | E0          | 3/4    |
| TIO      | F8          | 1      |
| TIX      | 2C          | 3/4    |
| TIXR     | B8          | 2      |
| WD       | DC          | 3/4    |

---

## Format Summary

| Format | Size    | Notes |
|--------|---------|-------|
| 1      | 1 byte  | Opcode only, no operand |
| 2      | 2 bytes | Opcode + two 4-bit register fields |
| 3      | 3 bytes | Default. 6-bit opcode + n,i,x,b,p,e flags + 12-bit disp |
| 4      | 4 bytes | Triggered by `+` prefix. e=1, 20-bit address field |

---

## Addressing Mode Flag Bits (Format 3/4)

| Bit | Name | Meaning |
|-----|------|---------|
| n   | indirect | `@` prefix on operand |
| i   | immediate | `#` prefix on operand |
| x   | indexed | `,X` suffix on operand |
| b   | base-relative | use BASE register for addressing |
| p   | PC-relative | use PC for addressing |
| e   | extended | Format 4 instruction (`+` prefix) |

**Common ni combinations:**
| ni | Mode |
|----|------|
| 11 | simple (default) |
| 10 | indirect (`@`) |
| 01 | immediate (`#`) |

---

## Register Codes (Format 2)

| Register | Code |
|----------|------|
| A        | 0    |
| X        | 1    |
| L        | 2    |
| B        | 3    |
| S        | 4    |
| T        | 5    |
| F        | 6    |
| PC       | 8    |
| SW       | 9    |

---

## Assembler Directives

| Directive | Purpose |
|-----------|---------|
| `START`   | Define program name and starting address |
| `END`     | Mark end of program, specify first executable instruction |
| `BYTE`    | Reserve and initialize 1 byte (char `C'x'` or hex `X'xx'`) |
| `WORD`    | Reserve and initialize one 3-byte word |
| `RESB`    | Reserve n bytes (uninitialized) |
| `RESW`    | Reserve n words (uninitialized) |
| `BASE`    | Set base register value for base-relative addressing |
| `NOBASE`  | Disable base-relative addressing |

> **Not implemented:** `EQU`, `ORG`

---

## Addressing Mode Selection (Format 3 Priority Order)

1. **PC-relative** — if target fits in signed 12-bit offset from next instruction address (`-2048` to `+2047`)
2. **Base-relative** — if BASE is set and offset fits in unsigned 12-bit value (`0` to `4095`)
3. **Format 4** — use `+` prefix for addresses that don't fit Format 3 (20-bit direct address)
4. **Error** — if none of the above work, flag as assembly error

---

## BYTE Directive Size Rules

| Operand form | Size |
|-------------|------|
| `C'ABC'`    | 1 byte per character (3 bytes for `C'ABC'`) |
| `X'F1'`     | 1 byte per 2 hex digits (`X'F1'` = 1 byte) |

---

## Quick Reference: Instruction Encoding

### Format 1
```
[ opcode (8 bits) ]
```

### Format 2
```
[ opcode (8 bits) ] [ r1 (4 bits) ] [ r2 (4 bits) ]
```

### Format 3
```
[ opcode (6 bits) | n | i ] [ x | b | p | e | disp (12 bits) ]
```

### Format 4
```
[ opcode (6 bits) | n | i ] [ x | b | p | e | address (20 bits) ]
```
