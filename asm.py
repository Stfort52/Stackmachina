#!/usr/bin/python3

import sys


def p8(n: int):
    return n.to_bytes(1, 'little', signed=True)


def p16(n: int):
    return n.to_bytes(2, 'little', signed=True)


def p32(n: int):
    return n.to_bytes(4, 'little', signed=True)


def p64(n: int):
    return n.to_bytes(8, 'little', signed=True)


i64_ops = ['i64_const', 'i64_add', 'i64_sub',
           'i64_mul', 'i64_div', 'i64_mod', 'i64_i32', 'i64_get', 'i64_set']
i64_cmp = ['i64_eq', 'i64_ne', 'i64_lt', 'i64_gt', 'i64_le', 'i64_ge']
i64_bin = ['i64_and', 'i64_xor', 'i64_or', 'i64_not', 'i64_lsh', 'i64_rsh']

i32_ops = ['i32_const', 'i32_add', 'i32_sub',
           'i32_mul', 'i32_div', 'i32_mod', 'i32_i64']
i32_cmp = ['i32_eq', 'i32_ne', 'i32_lt', 'i32_gt', 'i32_le', 'i32_ge']
i32_bin = ['i32_and', 'i32_xor', 'i32_or', 'i32_not', 'i32_lsh', 'i32_rsh']

arb_ops = ['exit', 'nop', 'drop', 'call', 'call1', 'call2', 'call3',
           'call4', 'retn', 'retnv', 'end', 'block', 'loop', 'br', 'br_if', 'sys']


def Asm(instr: str):
    #instr = ";".join(instr.split(";")[:-1])
    if len(instr.split(" ", 1)) == 2:
        op, instr = instr.split(" ", 1)
    else:
        op = instr
        instr = None
    if 'i64_' in op:
        try:
            asm = bytes([i64_ops.index(op)+0x10])
            if op == 'i64_const':
                if 'x' in instr:
                    asm += p64(int(instr, 16))
                else:
                    asm += p64(int(instr))
            if op == 'i64_get' or op == 'i64_set':
                asm += p8(int(instr))
            return asm
        except ValueError:
            pass
        try:
            return bytes([i64_cmp.index(op)+0x20])
        except ValueError:
            pass
        try:
            return bytes([i64_bin.index(op)+0x30])
        except ValueError:
            raise NotImplementedError("Dunno how to asm that")
    elif 'i32_' in op:
        try:
            asm = bytes([i32_ops.index(op)+0x40])
            if op == 'i32_const':
                if 'x' in instr:
                    asm += p64(int(instr, 16))
                else:
                    asm += p64(int(instr))
            return asm
        except ValueError:
            pass
        try:
            return bytes([i32_cmp.index(op)+0x50])
        except ValueError:
            pass
        try:
            return bytes([i64_bin.index(op)+0x60])
        except ValueError:
            raise NotImplementedError("Dunno how to asm that")
    elif 'str' in op:
        asm = bytes()
        instr = instr.replace("\\n", "\n")
        if len(instr) % 8:
            instr = instr.ljust(len(instr)-(len(instr) % 8)+8, "\0")
        for i in range(len(instr)//8):
            asm += bytes([16]+list(map(ord, instr[8*i:8*i+8])))
        return asm

    else:
        try:
            asm = bytes([arb_ops.index(op)])
            if 'br' in op or op == 'sys':
                asm += p8(int(instr))
            elif 'block' == op or 'call' in op:
                if 'x' in instr:
                    asm += p32(int(instr, 16))
                else:
                    asm += p32(int(instr))
            elif 'drop' == op:
                if instr:
                    asm = bytes([arb_ops.index(op)]) * int(instr)
            return asm
        except ValueError:
            raise NotImplementedError("Dunno how to asm {}, {}".format(op, instr))


if __name__ == "__main__":
    code = bytes()
    if(len(sys.argv) == 2):
        with open(sys.argv[1]) as f:
            assembly = f.read().split("\n")
        for a in assembly:
            code += Asm(a)
    else:
        assembly = input(">> ")
        while assembly:
            code += Asm(assembly)
            assembly = input(">> ")

    with open('output.sm', 'wb') as f:
        f.write(code)

    print("done!")
