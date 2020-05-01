#include "core.h"

u16 readu16(stream& input) {
    return u16(input.read()) << 8 | input.read();
}

float abs(float f) {
    return f < 0 ? -f : f;
}

i64 abs(i64 i) {
    return i < 0 ? -i : i;
}

double abs(double d) {
    return d < 0 ? -d : d;
}

void add_or_abs(u16 insn, i64* regfile, i8* memory) {
    if (!(insn >> 4 & 0xF))
        regfile[insn & 0xF] = abs(regfile[insn & 0xF]);
    else 
        regfile[insn >> 4 & 0xF] += regfile[insn & 0xF];
}

void addi(u16 insn, i64* regfile, i8* memory) {
    regfile[insn >> 8 & 0xF] += i64(i8(insn & 0xFF));
}

void fadd_or_fabs(u16 insn, i64* regfile, i8* memory) {
    if (!(insn >> 4 & 0xF)) 
        *(float*)(regfile + (insn & 0xF)) = abs(*(float*)(regfile + (insn & 0xF)));
    else 
        *(float*)(regfile + (insn >> 4 & 0xF)) += *(float*)(regfile + (insn & 0xF));
}

void sub_or_neg(u16 insn, i64* regfile, i8* memory) {
    if (!(insn >> 4 & 0xF))
        regfile[insn & 0xF] *= -1;
    else 
        regfile[insn >> 4 & 0xF] -= regfile[insn & 0xF];
}

void subi(u16 insn, i64* regfile, i8* memory) {
    regfile[insn >> 8 & 0xF] -= i64(i8(insn & 0xFF));
}

void fsub_or_fneg(u16 insn, i64* regfile, i8* memory) {
    if (!(insn >> 4 & 0xF)) 
        *(float*)(regfile + (insn & 0xF)) *= -1;
    else 
        *(float*)(regfile + (insn >> 4 & 0xF)) -= *(float*)(regfile + (insn & 0xF));
}

void mul(u16 insn, i64* regfile, i8* memory) {
    regfile[insn >> 4 & 0xF] *= regfile[insn & 0xF];
}

void muli(u16 insn, i64* regfile, i8* memory) {
    regfile[insn >> 8 & 0xF] *= i64(i8(insn & 0xFF));
}

void fmul(u16 insn, i64* regfile, i8* memory) {
    *(float*)(regfile + (insn >> 4 & 0xF)) *= *(float*)(regfile + (insn & 0xF));
}

void div(u16 insn, i64* regfile, i8* memory) {
    regfile[insn >> 4 & 0xF] /= regfile[insn & 0xF];
}

void divi(u16 insn, i64* regfile, i8* memory) {
    regfile[insn >> 8 & 0xF] /= i64(i8(insn & 0xFF));
}

void fdiv(u16 insn, i64* regfile, i8* memory) {
    *(float*)(regfile + (insn >> 4 & 0xF)) /= *(float*)(regfile + (insn & 0xF));
}

void and_or_not(u16 insn, i64* regfile, i8* memory) {
    if (!(insn >> 4 & 0xF))
        regfile[insn & 0xF] = ~regfile[insn & 0xF];
    else 
        regfile[insn >> 4 & 0xF] &= regfile[insn & 0xF];
}

void andi(u16 insn, i64* regfile, i8* memory) {
    regfile[insn >> 8 & 0xF] &= i64(i8(insn & 0xFF));
}

void or_(u16 insn, i64* regfile, i8* memory) {
    regfile[insn >> 4 & 0xF] |= regfile[insn & 0xF];
}

void ori(u16 insn, i64* regfile, i8* memory) {
    regfile[insn >> 8 & 0xF] |= i64(i8(insn & 0xFF));
}

void xor_(u16 insn, i64* regfile, i8* memory) {
    regfile[insn >> 4 & 0xF] ^= regfile[insn & 0xF];
}

void xori(u16 insn, i64* regfile, i8* memory) {
    regfile[insn >> 8 & 0xF] ^= i64(i8(insn & 0xFF));
}

void mov(u16 insn, i64* regfile, i8* memory) {
    regfile[insn >> 4 & 0xF] = regfile[insn & 0xF];
}

void movi(u16 insn, i64* regfile, i8* memory) {
    regfile[insn >> 8 & 0xF] = i64(i8(insn & 0xFF));
}

void int_(u16 insn, i64* regfile, i8* memory) {
    switch (insn & 0xFF) {
        case 0: // halt with code t0
            memory[0x1] = 1;
            memory[0x2] = regfile[T0];
            break;
        default:
            break;
    }
}

void noop(u16 insn, i64* regfile, i8* memory) {
    //
}

void (*ops[256])(u16, i64*, i8*) = {
    add_or_abs, addi, addi, addi, addi, addi, addi, addi, addi, addi, addi, addi, addi, addi, addi, fadd_or_fabs,
    sub_or_neg, subi, subi, subi, subi, subi, subi, subi, subi, subi, subi, subi, subi, subi, subi, fsub_or_fneg,
    mul, muli, muli, muli, muli, muli, muli, muli, muli, muli, muli, muli, muli, muli, muli, fmul,
    div, divi, divi, divi, divi, divi, divi, divi, divi, divi, divi, divi, divi, divi, divi, fdiv,
    and_or_not, andi, andi, andi, andi, andi, andi, andi, andi, andi, andi, andi, andi, andi, andi, noop,
    or_, ori, ori, ori, ori, ori, ori, ori, ori, ori, ori, ori, ori, ori, ori, noop,
    xor_, xori, xori, xori, xori, xori, xori, xori, xori, xori, xori, xori, xori, xori, xori, noop,
    mov, movi, movi, movi, movi, movi, movi, movi, movi, movi, movi, movi, movi, movi, movi, int_,
    noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, 
    noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop,
    noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop,
    noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop,
    noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop,
    noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop,
    noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop,
    noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop, noop,
};

void printh(stream& output, u16 raw) {
    u16 mask = 15 << 12;
    for (u8 i = 0; i < 4; i ++) 
        output.write("0123456789ABCDEF"[(raw & mask) >> 12]), raw = raw << 4;
    output.write(' ');
}

void execute(u16 insn, i64* regfile, i8* memory) {
    ops[insn >> 8 & 0xFF](insn, regfile, memory);
    regfile[IP] += 2;
}

i8 interpret(const vector<u16>& program) {
    i64 regfile[16] = {
        // null, t0 - t3, s0 - s2
        0, 0, 0, 0, 0, 0, 0, 0,
        // s3 - s7, bp, sp, ip
        0, 0, 0, 0, 0, 0, 0, 0, 
    };
    i8* stack = new i8[0x100000]; // 1MB stack
    u64 i = 0, j = 256;
    for (; i < program.size(); i ++) { // load program into memory, starting at address 128
        *(u16*)(stack + j) = program[i];
        j += 2;
    }
    regfile[BP] = j;
    regfile[SP] = j; // position stack beneath program

    stack[0x1] = 0; // halted flag
    stack[0x2] = 0; // exit code
    regfile[IP] = i64(stack + 256);
    while (!stack[0x1]) {
        execute(*(u16*)(regfile[IP]), regfile, stack);
    }
    return stack[0x2];
}

int main(int argc, char** argv) {
    if (argc > 2) {
        println("usage: jasmine-sim [input file]"); 
        return -1;
    }
    if (argc == 2 && string(argv[1]) == "--help") {
        println("usage: jasmine-sim [input file]");
        return -1;
    }
    vector<u16> program;
    if (argc == 2) {
        file input(argv[1]);
        while (input) program.push(readu16(input));
    }
    else while(_stdin) program.push(readu16(_stdin));

    return interpret(program);
}