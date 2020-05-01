#include "util/hash.h"
#include "util/vec.h"
#include "util/str.h"
#include "util/io.h"

#include <limits.h>
#include <stdint.h>

opcode ADD(0b0000, 0b0000),
       ADDI(0b0000),
       FADD(0b0000, 0b1111),
       ABS(0b0000, 0b0000, 0b0000),
       FABS(0b0000, 0b1111, 0b0000),
       SUB(0b0001, 0b0000),
       SUBI(0b0001),
       FSUB(0b0001, 0b1111),
       NEG(0b0001, 0b0000, 0b0000),
       FNEG(0b0001, 0b1111, 0b0000),
       MUL(0b0010, 0b0000),
       MULI(0b0010),
       FMUL(0b0010, 0b1111),
       DIV(0b0011, 0b0000),
       DIVI(0b0011),
       FDIV(0b0011, 0b1111),
       AND(0b0100, 0b0000),
       ANDI(0b0100),
       NOT(0b0100, 0b0000, 0b0000),
       OR(0b0101, 0b0000),
       ORI(0b0101),
       XOR(0b0110, 0b0000),
       XORI(0b0110),
       MOV(0b0111, 0b0000),
       MOVI(0b0111),
       INT(0b0111, 0b1111),
       F2I(0b1000, 0b0000),
       I2F(0b1000, 0b0001),
       D2I(0b1000, 0b0010),
       I2D(0b1000, 0b0011),
       F2D(0b1000, 0b0100),
       D2F(0b1000, 0b0101),
       EXTB(0b1000, 0b0110),
       EXTW(0b1000, 0b0111),
       EXTD(0b1000, 0b1000),
       TRB(0b1000, 0b1001),
       TRW(0b1000, 0b1010),
       TRD(0b1000, 0b1011),
       DADD(0b1001, 0b0000),
       DABS(0b1001, 0b0000, 0b0000),
       DSUB(0b1001, 0b0001),
       DNEG(0b1001, 0b0001, 0b0000),
       DMUL(0b1001, 0b0010),
       DDIV(0b1001, 0b0011),
       LDB(0b1011, 0b0000),
       LDW(0b1011, 0b0001),
       LDD(0b1011, 0b0010),
       LDQ(0b1011, 0b0011),
       STB(0b1011, 0b0100),
       STW(0b1011, 0b0101),
       STD(0b1011, 0b0110),
       STQ(0b1011, 0b0111),
       RLDB(0b1011, 0b1000),
       RLDW(0b1011, 0b1001),
       RLDD(0b1011, 0b1010),
       RLDQ(0b1011, 0b1011),
       RSTB(0b1011, 0b1100),
       RSTW(0b1011, 0b1101),
       RSTD(0b1011, 0b1110),
       RSTQ(0b1011, 0b1111),
       SH(0b1101, 0b0000),
       SHI(0b1101),
       CEQ(0b1110, 0b0000),
       CNE(0b1110, 0b0001),
       CL(0b1110, 0b0010),
       CLE(0b1110, 0b0011),
       CG(0b1110, 0b0100),
       CGE(0b1110, 0b0101),
       FALSE(0b1110, 0b0110),
       TRUE(0b1110, 0b0111),
       JMPR(0b1111),
       JMP(0b1111, 0b0000),
       CONST(0b1111, 0b1111),
       ERR;

// registers
u8 NL = 0b0000, T0 = 0b0001, T1 = 0b0010, T2 = 0b0011, 
   T3 = 0b0100, S0 = 0b0101, S1 = 0b0110, S2 = 0b0111, 
   S3 = 0b1000, S4 = 0b1001, S5 = 0b1010, S6 = 0b1011, 
   S7 = 0b1100, BP = 0b1101, SP = 0b1101, IP = 0b1111;

map<string, u8> nameregs = {
    {"nl", NL}, {"t0", T0}, {"t1", T1}, {"t2", T2},
    {"t3", T3}, {"s0", S0}, {"s1", S1}, {"s2", S2},
    {"s3", S3}, {"s4", S4}, {"s5", S5}, {"s6", S6},
    {"s7", S7}, {"bp", BP}, {"sp", SP}, {"ip", IP},
};

map<u8, string> regnames = {
    {NL, "nl"}, {T0, "t0"}, {T1, "t1"}, {T2, "t2"},
    {T3, "t3"}, {S0, "s0"}, {S1, "s1"}, {S2, "s2"},
    {S3, "s3"}, {S4, "s4"}, {S5, "s5"}, {S6, "s6"},
    {S7, "s7"}, {BP, "bp"}, {SP, "sp"}, {IP, "ip"},
};

map<string, opcode*> nameops = {
    {"add", &ADD}, {"addi", &ADDI}, {"fadd", &FADD}, 
    {"abs", &ABS}, {"fabs", &FABS},
    {"sub", &SUB}, {"subi", &SUBI}, {"fsub", &FSUB},
    {"neg", &NEG}, {"fneg", &FNEG},
    {"mul", &MUL}, {"muli", &MULI}, {"fmul", &FMUL},
    {"div", &DIV}, {"divi", &DIVI}, {"fdiv", &FDIV},
    {"and", &AND}, {"andi", &ANDI},
    {"not", &NOT},
    {"or", &OR}, {"ori", &ORI},
    {"xor", &XOR}, {"xori", &XORI},
    {"mov", &MOV}, {"movi", &MOVI},
    {"int", &INT},
    {"f2i", &F2I}, {"i2f", &I2F}, {"d2i", &D2I}, 
    {"i2d", &I2D}, {"f2d", &F2D}, {"d2f", &D2F}, 
    {"extb", &EXTB}, {"extw", &EXTW}, {"extd", &EXTD},
    {"trb", &TRB}, {"trw", &TRW}, {"trd", &TRD},
    {"dadd", &DADD}, {"dabs", &DABS},
    {"dsub", &DSUB}, {"dneg", &DNEG},
    {"dmul", &DMUL},
    {"ddiv", &DDIV},
    {"ldb", &LDB}, {"ldw", &LDW}, {"ldd", &LDD}, {"ldq", &LDQ},
    {"stb", &STB}, {"stw", &STW}, {"std", &STD}, {"stq", &STQ},
    {"rldb", &RLDB}, {"rldw", &RLDW}, {"rldd", &RLDD}, {"rldq", &RLDQ},
    {"rstb", &RSTB}, {"rstw", &RSTW}, {"rstd", &RSTD}, {"rstq", &RSTQ},
    {"sh", &SH}, {"shi", &SHI},
    {"ceq", &CEQ}, {"cne", &CNE}, {"cl", &CL}, {"cle", &CLE},
    {"cg", &CG}, {"cge", &CGE}, {"false", &FALSE}, {"true", &TRUE},
    {"jmpr", &JMPR},
    {"jmp", &JMP},
    {"const", &CONST}
};

map<opcode, string> opnames = {
    {ADD, "add"}, {ADDI, "addi"}, {FADD, "fadd"}, 
    {ABS, "abs"}, {FABS, "fabs"},
    {SUB, "sub"}, {SUBI, "subi"}, {FSUB, "fsub"},
    {NEG, "neg"}, {FNEG, "fneg"},
    {MUL, "mul"}, {MULI, "muli"}, {FMUL, "fmul"},
    {DIV, "div"}, {DIVI, "divi"}, {FDIV, "fdiv"},
    {AND, "and"}, {ANDI, "andi"},
    {NOT, "not"},
    {OR, "or"}, {ORI, "ori"},
    {XOR, "xor"}, {XORI, "xori"},
    {MOV, "mov"}, {MOVI, "movi"},
    {INT, "int"},
    {F2I, "f2i"}, {I2F, "i2f"}, {D2I, "d2i"}, 
    {I2D, "i2d"}, {F2D, "f2d"}, {D2F, "d2f"}, 
    {EXTB, "extb"}, {EXTW, "extw"}, {EXTD, "extd"},
    {TRB, "trb"}, {TRW, "trw"}, {TRD, "trd"},
    {DADD, "dadd"}, {DABS, "dabs"},
    {DSUB, "dsub"}, {DNEG, "dneg"},
    {DMUL, "dmul"},
    {DDIV, "ddiv"},
    {LDB, "ldb"}, {LDW, "ldw"}, {LDD, "ldd"}, {LDQ, "ldq"},
    {STB, "stb"}, {STW, "stw"}, {STD, "std"}, {STQ, "stq"},
    {RLDB, "rldb"}, {RLDW, "rldw"}, {RLDD, "rldd"}, {RLDQ, "rldq"},
    {RSTB, "rstb"}, {RSTW, "rstw"}, {RSTD, "rstd"}, {RSTQ, "rstq"},
    {SH, "sh"}, {SHI, "shi"},
    {CEQ, "ceq"}, {CNE, "cne"}, {CL, "cl"}, {CLE, "cle"},
    {CG, "cg"}, {CGE, "cge"}, {FALSE, "false"}, {TRUE, "true"},
    {JMPR, "jmpr"},
    {JMP, "jmp"},
    {CONST, "const"}
};

enum insntype {
    B,      // binary
    U,      // unary 
    IB,     // binary with immediate
    IU,     // unary with immediate
    C,      // constant
    R,      // relative
    IT,     // ternary with immediate
    B_OR_U, // binary or unary
    E       // error
};

insntype bytetypes[256] = {
    // 00 - 0F (add, abs)
    B_OR_U, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, B,
    // 10 - 1F (sub, neg)
    B_OR_U, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, B,
    // 20 - 2F (mul)
    B, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, B,
    // 30 - 3F (div)
    B, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, B,
    // 40 - 4F (and, not)
    B_OR_U, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB,
    // 50 - 5F (or)
    B, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB,
    // 60 - 6F (xor)
    B, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB,
    // 70 - 7F (mov, int)
    B, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IU,
    // 80 - 8F (cvt, ext, tr)
    B, B, B, B, B, B, B, B, B, B, B, B, E, E, E, E,
    // 90 - 9F (double ops)
    B_OR_U, B_OR_U, B, B, E, E, E, E, E, E, E, E, E, E, E, E,
    // A0 - AF (unused)
    C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C,
    // B0 - BF (load/store)
    IB, IB, IB, IB, IB, IB, IB, IB, R, R, R, R, R, R, R, R,
    // C0 - CF (unused)
    IT, IT, IT, IT, IT, IT, IT, IT, IT, IT, IT, IT, IT, IT, IT, IT,
    // D0 - DF (sh)
    B, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB,
    // E0 - EF (ccc)
    B, B, B, B, B, B, IU, IU, E, E, E, E, E, E, E, E,
    // F0 - FF (jmp)
    IU, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, IB, C,
};
