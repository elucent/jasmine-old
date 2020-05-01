#include "core.h"

u8 parsereg(stream& input, u64 pc) {
    string name;
    read(input, name);
    auto it = nameregs.find(name);
    if (it == nameregs.end()) return 0;
    else return it->second;
}

i16 parseimm(stream& input, u64 pc, const map<string, u64>& labels) {
    while (isspace(input.peek())) input.read();
    if (input.peek() == ':') {
        string label;
        read(input, label);
        return i64(labels[label]) - i64(pc) - 2;
    }
    i16 imm;
    read(input, imm);
    return imm;
}

u16 parseb(stream& input, opcode* code, u64 pc, const map<string, u64>& labels) {
    u8 dst = parsereg(input, pc);
    return code->ser()
        | dst << 4
        | parsereg(input, pc);
}

u16 parseu(stream& input, opcode* code, u64 pc, const map<string, u64>& labels) {
    return code->ser()
        | parsereg(input, pc);
}

u16 parseib(stream& input, opcode* code, u64 pc, const map<string, u64>& labels) {
    u8 dst = parsereg(input, pc);
    return code->ser()
        | dst << 8
        | i8(parseimm(input, pc, labels));
}

u16 parseiu(stream& input, opcode* code, u64 pc, const map<string, u64>& labels) {
    return code->ser()
        | i8(parseimm(input, pc, labels));
}

u16 parsec(stream& input, opcode* code, u64 pc, const map<string, u64>& labels) {
    return code->ser()
        | (parseimm(input, pc, labels) & 0xFF);
}

u16 parser(stream& input, opcode* code, u64 pc, const map<string, u64>& labels) {
    u8 dst = parsereg(input, pc);
    return code->ser()
        | dst << 4
        | (parsereg(input, pc) & 0xF);
}

u16 parseit(stream& input, opcode* code, u64 pc, const map<string, u64>& labels) {
    u8 dst = parsereg(input, pc);
    i16 imm = parseimm(input, pc, labels);
    return code->ser()
        | dst << 8
        | (imm & 0xF) << 4
        | parsereg(input, pc);
}

void writeu16(stream& output, u16 i) {
    output.write(i >> 8), output.write(i & 0xFF);
}

void assemble(stream& input, stream& output, u64& pc, map<string, u64>& labels) {
    if (input.peek() == '.') {
        string macro;
        i64 imm;
        read(input, macro);
        if (macro == ".b") 
            read(input, imm), 
            output.write(imm), 
            output.write(0),
            pc += 2;
        else if (macro == ".w")
            read(input, imm),
            output.write(imm >> 8),
            output.write(imm),
            pc += 2;
        else if (macro == ".d")
            read(input, imm),
            output.write(imm >> 24), output.write(imm >> 16),
            output.write(imm >> 8), output.write(imm),
            pc += 4;
        else if (macro == ".q")
            read(input, imm),
            output.write(imm >> 56), output.write(imm >> 48),
            output.write(imm >> 40), output.write(imm >> 32),
            output.write(imm >> 24), output.write(imm >> 16),
            output.write(imm >> 8), output.write(imm),
            pc += 8;
        else if (macro == ".s") {
            string literal;
            while (isspace(input.peek())) input.read();
            if (input.peek() != '"') return;
            input.read();
            while (input.peek() != '"') {
                if (input.peek() == '\\') {
                    input.read();
                    if (input.peek() == 'n') literal += '\n';
                    else if (input.peek() == 't') literal += '\t';
                    else if (input.peek() == 'b') literal += '\b';
                    else if (input.peek() == '0') literal += '\0';
                    else if (input.peek() == 'v') literal += '\v';
                    else if (input.peek() == 'r') literal += '\r';
                    else if (input.peek() == '\\') literal += '\\';
                    else if (input.peek() == '"') literal += '"';
                    else if (input.peek() == '\'') literal += '\'';
                    else return;
                }
                else literal += input.read();
            }
            input.read();
            literal += '\0';
            if (literal.size() % 2 != 0) literal += '\0';
            for (u64 i = 0; i < literal.size(); i ++) 
                output.write(literal[i]);
            pc += literal.size();
        }
    }
    else if (input.peek() == ':') {
        string label;
        read(input, label);
        auto it = labels.find(label);
        if (it == labels.end()) labels[label] = pc;
    }
    else {
        i16 tmp;
        string op;
        read(input, op);
        auto it = nameops.find(op);
        if (it == nameops.end()) return;
        opcode* code = it->second;
        insntype type = bytetypes[code->a << 4 | code->b];
        if (type == B_OR_U) type = code->c ? B : U;
        switch (type) {
            case B: 
                writeu16(output, parseb(input, code, pc, labels));
                break;
            case U: 
                writeu16(output, parseu(input, code, pc, labels));
                break;
            case IB: 
                writeu16(output, parseib(input, code, pc, labels));
                break;
            case IU: 
                writeu16(output, parseiu(input, code, pc, labels));
                break;
            case C: 
                writeu16(output, parsec(input, code, pc, labels));
                break;
            case R: 
                writeu16(output, parser(input, code, pc, labels));
                pc += 2;
                read(input, tmp);
                writeu16(output, u16(tmp));
                break;
            case IT: 
                writeu16(output, parseit(input, code, pc, labels));
                break;
            default: break;
        }
        pc += 2;
    }
        
    while (input.peek() && input.peek() != '\n') input.read();
    input.read();
}

void findlabels(map<string, u64>& labels, const char* fname) {
    file input(fname);
    u64 pc = 0;
    string s;
    while (input) {
        if (input.peek() == '.') {
            fread(input, s);
            if (s == ".b") pc += 2;
            else if (s == ".w") pc += 2;
            else if (s == ".d") pc += 4;
            else if (s == ".q") pc += 8;
        }
        else if (input.peek() == ':') {
            fread(input, s);
            labels[s] = pc;
        }
        else {
            string op;
            fread(input, op);
            auto it = nameops.find(op);
            if (it != nameops.end()) {
                opcode* code = it->second;
                insntype type = bytetypes[code->a << 4 | code->b];
                if (type == R) pc += 2;
            }
            pc += 2;
        }
        
        while (input.peek() && input.peek() != '\n') input.read();
        input.read();
    }
}

int main(int argc, char** argv) {
    if (argc > 2) {
        println("usage: jasmine-as [input file]"); 
        return -1;
    }
    if (argc == 2 && string(argv[1]) == "--help") {
        println("usage: jasmine-as [input file]"); 
        return 0;
    }
    map<string, u64> labels;
    u64 pc = 0;
    if (argc == 2) {
        findlabels(labels, argv[1]);
        file input(argv[1]);
        while (input) assemble(input, _stdout, pc, labels);
    }
    else while(_stdin) assemble(_stdin, _stdout, pc, labels);
    return 0;
}