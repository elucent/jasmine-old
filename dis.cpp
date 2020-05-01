#include "core.h"

u16 readu16(stream& input) {
    return u16(input.read()) << 8 | input.read();
}

void formatb(stream& output, u16 insn) {
    opcode op(insn >> 12, insn >> 8 & 0xF);
    println(output, opnames[op], " ", regnames[insn >> 4 & 0xF], " ", 
            regnames[insn & 0xF]);
}

void formatu(stream& output, u16 insn) {
    opcode op(insn >> 12, insn >> 8 & 0xF, insn >> 4 & 0xF);
    println(output, opnames[op], " ", regnames[insn & 0xF]);
}

void formatib(stream& output, u16 insn) {
    opcode op(insn >> 12);
    println(output, opnames[op], " ", regnames[insn >> 8 & 0xF], " ", 
            i16(i8(insn & 0xFF)));
}

void formatiu(stream& output, u16 insn) {
    opcode op(insn >> 12, insn >> 8 & 0xF);
    println(output, opnames[op], " ", i16(i8(insn & 0xFF)));
}

void formatc(stream& output, u16 insn) {
    opcode op(insn >> 12, insn >> 8 & 0xF);
    println(output, opnames[op], " ", i16(insn & 0xFF));
}

void formatr(stream& output, u16 insn) {
    opcode op(insn >> 12, insn >> 8 & 0xF);
    print(output, opnames[op], " ", regnames[insn >> 4 & 0xF], " ", 
            regnames[insn & 0xF]);
}

void formatit(stream& output, u16 insn) {
    opcode op(insn >> 12);
    println(output, opnames[op], " ", regnames[insn >> 8 & 0xF], " ", 
            i16(i8(insn >> 4 & 0xF)), " ", regnames[insn & 0xF]);
}

void disassemble(stream& input, stream& output) {
    u16 insn = readu16(input);
    insntype type = bytetypes[insn >> 8];
    if (type == B_OR_U) type = (insn >> 4 & 0xF) ? B : U;
    switch (type) {
        case B: return formatb(output, insn);
        case U: return formatu(output, insn);
        case IB: return formatib(output, insn);
        case IU: return formatiu(output, insn);
        case C:
            formatc(output, insn);
            if ((insn & 0xFF) == 2) {
                i16 i = input.read() << 8;
                i |= input.read();
                println(output, ".w ", i);
            }
            else if ((insn & 0xFF) == 4) {
                i32 i = 0;
                for (u8 j = 0; j < 4; j ++) i <<= 8, i |= input.read();
                read(input, i);
                println(output, ".d ", i);
            }
            else if ((insn & 0xFF) == 8) {
                i64 i = 0;
                for (u8 j = 0; j < 8; j ++) i <<= 8, i |= input.read();
                println(output, ".q ", i);
            }
            else {
                print(output, ".s \"");
                for (i64 i = 0; i < (insn & 0xFF); i += 1)
                    output.write(input.read());
                println(output, '"');    
            } // dump constants
            break;
        case R: 
            formatr(output, insn);
            println(output, " ", i16(readu16(input))); // offset
            break;
        case IT: return formatit(output, insn);
        default: return;
    }
}

int main(int argc, char** argv) {
    if (argc > 2) {
        println("usage: jasmine-dis [input file]"); 
        return -1;
    }
    if (argc == 2 && string(argv[1]) == "--help") {
        println("usage: jasmine-dis [input file]");
        return -1;
    }
    if (argc == 2) {
        file input(argv[1]);
        while (input) disassemble(input, _stdout);
    }
    else while(_stdin) disassemble(_stdin, _stdout);
    return 0;
}