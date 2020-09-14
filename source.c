#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#define VSTACK_SIZE 8000
#define CSTACK_SIZE 800
#define TIMEOUT 180
#define CODE_SIZE 3200
#define LOCAL_CNT 16
//#define TUX_DEBUG
typedef struct Locals
{
    struct Locals *prev;
    long long int locals[LOCAL_CNT];
} Locals;

char *valuestack, *code, *cntlstack;
int valueptr, valuebase;
int codeptr;
int cntlptr, cntlbase;
Locals *locals;
inline char fetch_byte(void);
inline short fetch_word(void);
inline int fetch_dword(void);
inline long long int fetch_qword(void);

enum opcodes
{
    nop = 1,
    drop,
    i64_const = 16,
    i64_add,
    i64_sub,
    i64_mul,
    i64_div,
    i64_mod,
    i64_i32,
    i64_get,
    i64_set,
    i64_eq = 32,
    i64_ne,
    i64_lt,
    i64_gt,
    i64_le,
    i64_ge,
    i64_and = 48,
    i64_xor,
    i64_or,
    i64_not,
    i64_lsh,
    i64_rsh,
    i32_const = 64,
    i32_add,
    i32_sub,
    i32_mul,
    i32_div,
    i32_mod,
    i32_i64,
    i32_eq = 80,
    i32_ne,
    i32_lt,
    i32_gt,
    i32_le,
    i32_ge,
    i32_and = 96,
    i32_xor,
    i32_or,
    i32_not,
    i32_lsh,
    i32_rsh,
    call = 0x3,
    call1,
    call2,
    call3,
    call4,
    retn,
    retnv,
    end,
    block,
    loop,
    br,
    br_if,
    sys,
};

enum syscalls
{
    sys_read = 0,
    sys_write,
    sys_tuxcall,
    sys_sleep,
};

void __attribute__((noreturn)) fail(char *err)
{
    printf("Failure At 0x%x:0x%x, SP: %d, SB: %d, CP:%d, CB:%d\n", codeptr, code[codeptr], valueptr, valuebase, cntlptr, cntlbase);
    exit(puts(err));
}

void handler()
{
    puts("Timeeeeeed Out!!!!!!");
    return fail("Get Out!");
}

void setup()
{
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    valuestack = malloc(VSTACK_SIZE);
    cntlstack = malloc(CSTACK_SIZE);
    code = calloc(CODE_SIZE, 1);
    alarm(TIMEOUT);
    signal(SIGALRM, handler);
    return;
}

void push(long long int n)
{
    if (__glibc_likely(valueptr < VSTACK_SIZE && valueptr >= valuebase))
    {
        *(long long int *)(&valuestack[valueptr]) = n;
        valueptr += 8;
    }
    else
        fail("Failed to Handle Value Reference : Stack Overflow");
}

long long int pop(void)
{
    if (__glibc_likely(valueptr > 0 && valueptr > valuebase))
    {
        valueptr -= 8;
        long long int ret = *(long long int *)(&valuestack[valueptr]);
        return ret;
    }
    else
        fail("Failed to Handle Value Reference : Nothing in Stack");
}

void push32(int n)
{
    if (__glibc_likely(valueptr < VSTACK_SIZE && valueptr >= valuebase))
    {
        *(int *)(&valuestack[valueptr]) = n;
        valueptr += 4;
    }
    else
        fail("Failed to Handle Value Reference : Stack Overflow");
}

int pop32(void)
{
    if (__glibc_likely(valueptr > 0 && valueptr > valuebase))
    {
        valueptr -= 4;
        int ret = *(int *)(&valuestack[valueptr]);
        return ret;
    }
    else
        fail("Failed to Handle Value Reference : Nothing in Stack");
}

void cpush(int n)
{
    if (__glibc_likely(cntlptr < CSTACK_SIZE && cntlptr >= cntlbase))
    {
        *(int *)(&cntlstack[cntlptr]) = n;
        cntlptr += 4;
    }
    else
        fail("Invalid Branching or Overcomplicated Routine : Halting");
}

int cpop()
{
    if (__glibc_unlikely(cntlptr > 0 && cntlptr >= cntlbase))
    {
        cntlptr -= 4;
        int ret = *(int *)(&cntlstack[cntlptr]);
        return ret;
    }
    else
        fail("Invalid Branching : Halting");
}

/*
void pushf32(float n)
{
    if (__glibc_likely(valueptr < 8000))
    {
        *(float *)(&valuestack[valueptr]) = n;
        valueptr += 4;
    }
    else
        fail("Failed to Handle Value Reference : Stack Overflow");
}

float popf32(void)
{
    if (__glibc_unlikely(valueptr > 0))
    {
        int ret = *(float *)(&valuestack[valueptr]);
        valueptr += 4;
        return ret;
    }
    else
        fail("Failed to Handle Value Reference : Stack Overflow");
}

void pushf(double n)
{
    if (__glibc_likely(valueptr < 8000))
    {
        *(double *)(&valuestack[valueptr]) = n;
        valueptr += 8;
    }
    else
        fail("Failed to Handle Value Reference : Stack Overflow");
}

double popf(void)
{
    if (__glibc_unlikely(valueptr > 0))
    {
        double ret = *(double *)(&valuestack[valueptr]);
        valueptr += 8;
        return ret;
    }
    else
        fail("Failed to Handle Value Reference : Stack Overflow");
}
*/

char fetch_byte(void)
{
    return code[codeptr++];
}

short fetch_word(void)
{
    short ret = *(short *)(&code[codeptr]);
    codeptr += 2;
    return ret;
}

int fetch_dword(void)
{
    int ret = *(int *)(&code[codeptr]);
    codeptr += 4;
    return ret;
}

long long int fetch_qword(void)
{
    long long int ret = *(long long int *)(&code[codeptr]);
    codeptr += 8;
    return ret;
}

long long int handle_syscall(char request)
{
    long long int result = -1;
    long long int x, y;
    switch (request)
    {
    case sys_read:
        x = pop();
        y = pop();
        if (valueptr - y - x >= 0 && x >= 0 && valueptr - y - x < VSTACK_SIZE)
            result = read(STDIN_FILENO, &valuestack[valueptr - y - x], x);
        break;
    case sys_write:
        x = pop();
        y = pop();
        if (valueptr - y - x >= 0 && x >= 0 && valueptr - y - x < VSTACK_SIZE)
            result = write(STDOUT_FILENO, &valuestack[valueptr - y - x], x);
        break;
    case sys_tuxcall:
        result = puts("Tux Says HI!");
#ifdef TUX_DEBUG
        printf("Checking At 0x%x:0x%x, SP: %d (*SP %llX:%lld), SB: %d, CP:%d, CB:%d\n", codeptr, code[codeptr], valueptr, *(long long int *)(&valuestack[valueptr - 8]), *(long long int *)(&valuestack[valueptr - 8]), valuebase, cntlptr, cntlbase);
        //__asm__ __volatile__("int3\n\t");
#endif
        break;
    case sys_sleep:
        x = pop();
        result = sleep(x);
        break;
    default:
        puts("That Syscall is Not Supported.");
        kill(getpid(), SIGSYS);
    }
    return result;
}

int vm()
{
    int result = 1;
    register int i;
    register long long int lx, ly;
    Locals *L;
    switch (fetch_byte())
    {
    case 0:
        return 0;
    case nop:
        break;
    case drop:
        pop();
        break;
    case i32_const:
        push32(fetch_dword());
        break;
    case i32_add:
        push32(pop32() + pop32());
        break;
    case i32_sub:
        push32(pop32() - pop32());
        break;
    case i32_mul:
        push32(pop32() * pop32());
        break;
    case i32_div:
        push32(pop32() / pop32());
        break;
    case i32_mod:
        push32(pop32() % pop32());
        break;
    case i32_eq:
        push32(pop32() == pop32());
        break;
    case i32_ne:
        push32(pop32() != pop32());
        break;
    case i32_lt:
        push32(pop32() < pop32());
        break;
    case i32_gt:
        push32(pop32() > pop32());
        break;
    case i32_le:
        push32(pop32() <= pop32());
        break;
    case i32_ge:
        push32(pop32() >= pop32());
        break;
    case i64_const:
        push(fetch_qword());
        break;
    case i64_add:
        lx = pop();
        ly = pop();
        push(lx + ly);
        break;
    case i64_sub:
        lx = pop();
        ly = pop();
        push(lx - ly);
        break;
    case i64_mul:
        lx = pop();
        ly = pop();
        push(lx * ly);
        break;
    case i64_div:
        lx = pop();
        ly = pop();
        push(lx / ly);
        break;
    case i64_mod:
        lx = pop();
        ly = pop();
        push(lx % ly);
        break;
    case i64_eq:
        push(pop() == pop());
        break;
    case i64_ne:
        push(pop() != pop());
        break;
    case i64_lt:
        push(pop() < pop());
        break;
    case i64_gt:
        push(pop() > pop());
        break;
    case i64_le:
        push(pop() <= pop());
        break;
    case i64_ge:
        push(pop() >= pop());
        break;
    case i32_i64:
        push(pop32());
        break;
    case i64_i32:
        push32((int)pop());
        break;
    case i32_and:
        push32(pop32() & pop32());
        break;
    case i32_or:
        push32(pop32() | pop32());
        break;
    case i32_xor:
        push32(pop32() ^ pop32());
        break;
    case i32_not:
        push32(~pop32());
        break;
    case i64_get:
        lx = fetch_byte();
        if (lx >= LOCAL_CNT)
            fail("Failed To Handle Local Value Reference : Halting");
        push(locals->locals[lx]);
        break;
    case i64_set:
        lx = fetch_byte();
        if (lx >= LOCAL_CNT)
            fail("Failed To Handle Local Value Reference : Halting");
        locals->locals[lx] = pop();
        break;
    case i64_and:
        push(pop() & pop());
        break;
    case i64_or:
        push(pop() | pop());
        break;
    case i64_xor:
        push(pop() ^ pop());
        break;
    case i64_not:
        push(~pop());
        break;
    case i64_lsh:
        push(pop() << pop());
        break;
    case i64_rsh:
        push(pop() >> pop());
        break;
    case call:
        L = malloc(sizeof(Locals));
        L->prev = locals;
        locals = L;
        cpush(cntlbase);
        cntlbase = cntlptr;
        cpush(codeptr + 4);
        cpush(valuebase);
        valuebase = valueptr;
        codeptr = fetch_dword();
        if (__glibc_unlikely(codeptr < 0 || codeptr >= CODE_SIZE))
            kill(getpid(), SIGSEGV);
        break;
    case call1:
        L = malloc(sizeof(Locals));
        L->prev = locals;
        locals = L;
        lx = pop();
        cpush(cntlbase);
        cntlbase = cntlptr;
        cpush(codeptr + 4);
        cpush(valuebase);
        valuebase = valueptr;
        codeptr = fetch_dword();
        if (__glibc_unlikely(codeptr < 0 || codeptr >= CODE_SIZE))
            kill(getpid(), SIGSEGV);
        push(lx);
        break;
    case call2:
        L = malloc(sizeof(Locals));
        L->prev = locals;
        locals = L;
        lx = pop();
        ly = pop();
        cpush(cntlbase);
        cntlbase = cntlptr;
        cpush(codeptr + 4);
        cpush(valuebase);
        valuebase = valueptr;
        codeptr = fetch_dword();
        if (__glibc_unlikely(codeptr < 0 || codeptr >= CODE_SIZE))
            kill(getpid(), SIGSEGV);
        push(ly);
        push(lx);
        break;
    case call3:
        L = malloc(sizeof(Locals));
        L->prev = locals;
        locals = L;
        puts("Do you Really neeeeed That many arguments?");
        break;
    case call4:
        puts("You Reaaaaaaaaally neeeeded that many arguments? Get out.");
        return 0;
    case retn:
        if (__glibc_unlikely(valueptr != valuebase + 8))
            fail("Returning Nothing in a non-void function.");
        if (__glibc_unlikely(cntlptr != cntlbase + 8))
            fail("ReTuRn...........");
        lx = pop();
        valuebase = cpop();
        codeptr = cpop();
        cntlbase = cpop();
        push(lx);
        L = locals;
        locals = locals->prev;
        free(L);
        break;
    case retnv:
        if (__glibc_unlikely(valueptr != valuebase))
            fail("Returning Something in a void function.");
        if (__glibc_unlikely(cntlptr != cntlbase + 8))
            fail("ReTuRn...........");
        valuebase = cpop();
        codeptr = cpop();
        cntlbase = cpop();
        L = locals;
        locals = locals->prev;
        free(L);
        break;
    case sys:
        lx = handle_syscall(fetch_byte());
        if (lx < 0)
            fail("PANIC!!!!!!!!!!!");
        push(lx);
        break;
    case block:
        cpush(fetch_word() + codeptr);
        break;
    case loop:
        cpush(codeptr - 1);
        break;
    case br_if:
        if (!pop())
        {
            fetch_byte();
            break;
        }
    case br:
        lx = fetch_byte();
        if (lx < 0)
            fail("Invalid Branching");
        while (lx-- >= 0)
        {
            ly = cpop();
        }
        codeptr = ly;
        break;
    case end:
        cpop();
        break;
    }

    return result;
}

int __attribute__((noreturn)) main(int argc, char *argv[])
{
    setup();
    int fd = 1337;
    if (__glibc_unlikely(argc == 1))
    {
        fd = STDIN_FILENO;
        printf("Code\n>>> ");
    }
    else
    {
        fd = open(argv[1], O_RDONLY);
        if (__glibc_unlikely(fd < 0))
            fail("open() Failed");
    }
    if (read(fd, code, CODE_SIZE) < 0)
        fail("read() Failed");
    locals = malloc(sizeof(Locals));
    locals->prev = NULL;
    while (vm())
        ;
    puts("VM Exited Normally");
    exit(0);
}