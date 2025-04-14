#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define MAX(x, y) (((x) >= (y)) ? (x) : (y))

typedef struct BufHdr{
    size_t len;
    size_t cap;
    char buf[];
} BufHdr;

void *ion_realloc(void *ptr, size_t bytes) {
    ptr = (void *)realloc(ptr, bytes);
    if (!ptr) {
        perror("Memory reallocation failed.");
        exit(1);
    }
    return ptr;
} 

void *ion_malloc(size_t bytes) {
    void *ptr = (void *)malloc(bytes);
    if (!ptr) {
        perror("Memory allocation failed.");
        exit(1);
    }
    return ptr;
}

static char err_buf[1024 * 1024];

void fatal (const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("FATAL: ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
    exit(1);
}

#define buf__hdr(b) ((BufHdr *)((char *)b - offsetof(BufHdr, buf)))
#define buf__fit(b, n) (buf_len(b) + (n) <= buf_cap(b) ? 0 : ((b) = buf__grow((b), buf_len(b) + (n), sizeof(*(b)))))

#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b) ((b) ? buf__hdr(b)->cap : 0)
#define buf_push(b, el) (buf__fit(b, 1), b[buf_len(b)] = (el), buf__hdr(b)->len++)
#define buf_free(b) ((b) ? (free(buf__hdr(b)), (b) = NULL) : 0)
#define buf_get(b, i) ((assert((i) < buf_len(b)), (b)[i]))

void *buf__grow(const void *buf, size_t new_len, size_t elem_size) {
    size_t new_cap = MAX(1 + 2*buf_cap(buf), new_len);
    assert(new_len <= new_cap);
    size_t new_size = offsetof(BufHdr, buf) + new_cap * elem_size;
    BufHdr *new_header;
    if (buf) {
        new_header = ion_realloc(buf__hdr(buf), new_size);
        assert(new_header != NULL);
    } else {
        new_header = ion_malloc(new_size);
        assert(new_header != NULL);
        new_header->len = 0;
    }
    new_header->cap = new_cap;
    return new_header->buf;
}

void buf_output(int *buf) {
    for (int i = 0; i < buf_len(buf); ++i) {
        printf("%d\n", buf[i]);
    }
}

void buf_test() {
    int *buf = NULL;
    // some buffer methods like buf_push, free_buf, ...
    buf_push(buf, 42);
    buf_push(buf, 108);
    buf_output(buf);
    printf("Getted value: %d\n", buf_get(buf, 1));
    buf_free(buf);
}

typedef struct InternStr {
    size_t len;
    const char *str;
} InternStr;

static InternStr *intern;

const char *str_intern_range(const char *start, const char *end) {
    size_t len = end - start;
    for (size_t i = 0; i < buf_len(intern); i++) {
        if (intern[i].len == len && strncmp(intern[i].str, start, len) == 0) {
            return intern[i].str;
        }
    }
    char *str = malloc(len + 1);
    memcpy(str, start, len); // Just copies start len to heap var, str;
    str[len] = 0;
    buf_push(intern, ((InternStr){len, str}));
    return str;
}

const char *str_intern(const char *str) {
    return str_intern_range(str, str + strlen(str));
}

void str_intern_test() {
    char x[] = "hello";
    char y[] = "hello";
    assert(x != y);
    assert(str_intern(x) == str_intern(y));
}

// Lexical Analyser
// This is a part of interpreter or compiler, that converts
// character stream into the tokens (lexems).

typedef enum TokenKind {
    TOKEN_INT=128,
    TOKEN_UNARY,
    TOKEN_NAME,
    TOKEN_HIGH_PRIORITY,
    TOKEN_LOW_PRIORITY,
    TOKEN_SHL,
    TOKEN_SHR
}TokenKind;

typedef struct Token {
    TokenKind kind;
    const char *start;
    const char *end;
    union {
        uint64_t val;
        const char *name;
    };
}Token;

Token token;
static TokenKind prev_token_kind = 0;
const char *stream;

const char *keyword_if;
const char *keyword_for;
const char *keyword_while;

void init_keywords() {
    keyword_if = str_intern("if");
    keyword_for = str_intern("for");
    keyword_while = str_intern("while");
    // ...
}

void next_token() {
    while (isspace(*stream)) {
        stream++;
    }
    if (isdigit(*stream)) {
        uint64_t val = 0;
        while (isdigit(*stream)) {
            val = val * 10 + (*stream++ - '0');
        }
        token.kind = TOKEN_INT;
        token.val = val;
        prev_token_kind = TOKEN_INT;
    }
    else if (*stream == '+' || *stream == '|' || *stream == '^') {
        const char *start = stream++;
        token.start = start;
        token.kind = TOKEN_LOW_PRIORITY;
        token.end = stream;
        token.name = str_intern_range(token.start, token.end);
        prev_token_kind = TOKEN_LOW_PRIORITY;
    }
    else if (*stream == '/' || *stream == '*' || *stream == '%' || *stream == '&') {    
        const char *start = stream++;
        token.kind = TOKEN_HIGH_PRIORITY;
        token.start = start;
        token.end = stream;
        token.name = str_intern_range(token.start, token.end);
        prev_token_kind = TOKEN_HIGH_PRIORITY;
    }
    else if (*(stream + 1) != '\0' && ((*stream == '<' && *(stream + 1) == '<') || (*stream == '>' && *(stream + 1) == '>'))) {
        const char *start = stream;
        if (*stream == '<' && *(stream + 1) == '<') {
            token.kind = TOKEN_SHL;
        }
        else {
            token.kind = TOKEN_SHR;
        }
        token.start = start;
        stream += 2;
        token.end = stream;
        token.name = str_intern_range(token.start, token.end);    
        prev_token_kind = TOKEN_HIGH_PRIORITY;
    }
    else if (*stream == '-'  || *stream == '~') {
        const char *start = stream++;
        if (*stream == '~' || prev_token_kind == 0 ||
            prev_token_kind == TOKEN_LOW_PRIORITY || 
            prev_token_kind == TOKEN_HIGH_PRIORITY ||
            prev_token_kind == '(') {
            token.kind = TOKEN_UNARY;
        }
        else {
            token.kind = TOKEN_LOW_PRIORITY;
        }
        token.start = start;
        token.end = stream;
        token.name = str_intern_range(token.start, token.end);
        prev_token_kind = token.kind;
    }
    else if (isalpha(*stream) || *stream == '_') {
        const char *start = stream++;
        while (isalpha(*stream) || *stream == '_') {
            stream++;
        }
        token.start = start;
        token.end = stream;
        token.name = str_intern_range(token.start, token.end);
        token.kind = TOKEN_NAME;
        prev_token_kind = TOKEN_NAME;
    }
    else if (*stream == '('|| *stream == ')') {
        token.kind = *stream++;
        prev_token_kind = token.kind;
    }
    else {
        token.kind = *stream++;
        //prev_token_kind = token.kind;
    }
}

void print_token(Token token) {
    switch (token.kind) {
        case TOKEN_INT:
            printf("%ld", token.val);
            break;
        case TOKEN_UNARY:
            printf("%c", *token.start);
            break;
        case TOKEN_NAME:
            printf(")"); 
            printf("%.*s", (int)(token.end - token.start), token.start);
            break;
        case TOKEN_SHR:
        case TOKEN_SHL:
        case TOKEN_HIGH_PRIORITY:
        case TOKEN_LOW_PRIORITY:
            printf("%c", *token.start);
            break;
        case '(':
        case ')':
            printf("%c", token.kind);
        default:
            if (token.kind < 128)
                printf("%c", token.kind);
            //printf("TOKEN '%c'\n", token.kind);
            break;
    }
}

// Implementation of an expression parser for
// a simple arithmetic language

// Building an expression tree considering the priority of each token
// I'm gonna use EBNF notation

// expr = term {(low priority) term}  <-----  low priority operators  <----- going from left tp right
// term = factor { (high priority) factor}  <-----  high priority operators  <----- going from left to right
// factor = INT | '(' expr ')' | ('~' | '-') factor  <-----  unary operators  <-----going from right to left
// Building a parser for Abstract Syntax Tree (AST)

typedef struct ASTNode {
    TokenKind kind;
    union {
        uint64_t int_val;
        const char *name;
        struct {
            struct ASTNode *left;
            struct ASTNode *right;
        };
        struct ASTNode  *unary_arg;
    };
} ASTNode;

ASTNode *parse_fact();
ASTNode *parse_term();
ASTNode *parse_expr();
// Implement parserr for each lexer

ASTNode *parse_fact() {
    ASTNode *node = NULL;
    if (token.kind == TOKEN_INT) {
        node = (ASTNode *)malloc(sizeof(ASTNode));
        node->kind = TOKEN_INT;
        node->int_val = token.val;
        next_token();
    }
    else if (token.kind == '(') {
        next_token();
        node = parse_expr();
        ASTNode *parent_node = (ASTNode *)malloc(sizeof(ASTNode));
        parent_node->kind = '(';
        parent_node->unary_arg = node;
        node = parent_node;
        //node->kind = '(';
        assert(token.kind == ')');
        next_token();
    }
    else if (token.kind == TOKEN_UNARY) {
        char op = token.start[0];
        next_token();
        ASTNode *operand = parse_fact();
        node = (ASTNode *)malloc(sizeof(ASTNode));
        node->kind = op;
        node->right = operand;
        return node;
    }
    else {
        assert(0 && "Unexpected token in factor!");
    }
    return node;
}

ASTNode *parse_term() {
    ASTNode *node = parse_fact();
    while (token.kind == TOKEN_HIGH_PRIORITY || token.kind == TOKEN_SHR || token.kind == TOKEN_SHL) {
        TokenKind op = token.kind;
        if (token.kind == TOKEN_HIGH_PRIORITY)
            op = (char)token.start[0];
        next_token();
        ASTNode *rhs = parse_fact();
        ASTNode *new_node = (ASTNode *)malloc(sizeof(ASTNode));
        new_node->kind = op;
        new_node->left = node;
        new_node->right = rhs;
        node = new_node;
    }    
    return node;
}

ASTNode *parse_expr() {
    ASTNode *node = parse_term();
    while (token.kind == TOKEN_LOW_PRIORITY) {
        char op = token.start[0];//token.kind;
        next_token();
        ASTNode *rhs = parse_term();
        ASTNode *new_node = (ASTNode *)malloc(sizeof(ASTNode));
        new_node->kind = op;
        new_node->name = token.start;
        new_node->left = node;
        new_node->right = rhs;
        node = new_node;
    }
    return node;
}

// BUILDING THE AST

void print_AST(ASTNode *node) {
    if (!node) {
        return;
    }
    if (node->kind == TOKEN_INT) {
        printf("%lu", node->int_val);
    }
    else if (node->left == NULL && node->right != NULL) {
        printf("(%c ", node->kind);
        print_AST(node->right);
        printf(")");
    }
    else {
        if (node->kind == TOKEN_SHL) printf("(<<");
        else if (node->kind == TOKEN_SHR) printf("(>>");
        else {
            printf("(%c", node->kind);
        }
        printf(" ");
        print_AST(node->left);
        printf(" ");
        print_AST(node->right);
        printf(")");
    }
}

void free_AST(ASTNode *node) {
    if (node == NULL) {
        return;
    }
    if (node->kind == TOKEN_INT) {
        free(node);
    }
    else if (node->kind == '-' || node->kind == '~') {
        free_AST(node->right);
        //free_AST(node->unary_arg);
        free(node);
    }
    else {
        free_AST(node->left);
        free_AST(node->right);
        free(node);
    }
}

// Implementation of a calculator by recursively evaluating the expression as part of the recursive descent
// factor = INT | '(' expr ')' | ('~' | '-') factor  <-----  unary operators  <-----going from right to left
// expr = term {(low priority) term}  <-----  low priority operators  <----- going from left tp right
// term = factor { (high priority) factor}  <-----  high priority operators  <----- going from left to right
// It's basically an interpreter
// An interpreter is a program that reads source code and executes it directly,
//  step by step, without translating it into machine code ahead of time (like a compiler does).


int64_t eval_exp();
int64_t eval_fact();
int64_t eval_term();

int64_t eval_fact() {
    int64_t result = 0;
    if (token.kind == TOKEN_INT) {
        result = token.val;
        next_token();
    }
    else if (token.kind == '(') {
        next_token();
        result = eval_exp();
        if (token.kind != ')') {
            fatal("Expected ')'");
        }
        next_token();
    }
    else if (token.kind == TOKEN_UNARY) {
        char op = *token.start;
        next_token();
        int64_t operand = eval_fact();
        switch (op) {
            case '-':
                result = -operand;
                break;
            case '~':
                result = ~operand;
                break;
            default:
                fatal("Unexpected unary operator '%c'.", op);
        }
    }
    return result;
}

// term = factor { (high priority) factor}  <-----  high priority operators  <----- going from left to right

int64_t eval_term() {
    int64_t result = eval_fact();
    while (token.kind == TOKEN_HIGH_PRIORITY || token.kind == TOKEN_SHL || token.kind == TOKEN_SHR) {    
        if (token.kind == TOKEN_HIGH_PRIORITY) {
            char op = *token.start;
            next_token();
            int64_t rhs = eval_fact();
            switch (op) {
                case '*':
                    result *= rhs;
                    break;
                case '/':
                    result /= rhs;
                    break;
                case '%':
                    result %= rhs;
                    break;
                case '&':
                    result &= rhs;
                    break;
                default:
                    fatal("Unexpected high priority operator '%c'", op);
            }
        }
        else if (token.kind == TOKEN_SHR || token.kind == TOKEN_SHL) {
            TokenKind op = token.kind;
            next_token();
            int64_t rhs = eval_fact();
            if (op == TOKEN_SHL)
                result <<= rhs;
            else 
                result >>= rhs;
        }
    }
    return result;
}

// expr = term {(low priority) term}  <-----  low priority operators  <----- going from left tp right
int64_t eval_exp() {
    int64_t result = eval_term();
    while (token.kind == TOKEN_LOW_PRIORITY) {
        char op = *token.start;
        next_token();
        int64_t rhs = eval_term();
        switch (op) {
            case '+':
                result += rhs;
                break;
            case '-':
                result -= rhs;
                break;
            case '|':
                result |= rhs;
                break;
            case '^':
                result ^= rhs;
                break;
            default:
                fatal("Unexpected low priority operator '%c'", op);
        } 
    }
    return result;
}

// Implementing a small virtual machine
// gonna rely on what I've done

#define POP() (*--top)
#define PUSH(x) (*top++ = (x))
#define POPS(n) assert(top - stack >= (n))
#define PUSHES(n) assert(top + (n) <= stack + MAX_STACK)

// BITWISE'S expl...
// These assert macros are here to help with debugging. A correct compiler cannot trigger stack underflow and
// can only trigger stack overflow if expression depth exceeds MAX_STACK, which can be checked in the compiler.
// The Java Virtual Machine and WebAssembly specifications rely on statically verifying that kind of safety
// property at load time for untrusted code, which lets them eliminate expensive runtime safety checks. That's
// the best of both worlds: high performance without compromises in security or correctness. If you finish
// the other exercises with time to spare, try to implement a static stack-effect checker for our bytecode.
// Hint: Start with your vm_exec, remove the dynamic semantics, and just leave in the stack effect checks,
// tracking only the changing top-of-stack offset, not the actual stack (which can then be removed). This is
// called abstract interpretation. It's not very useful for our expression language, but it generalizes.

enum {
    OP_HALT,
    OP_LIT,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_NEG,
    OP_NOT,
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_SHL,
    OP_SHR
};


int32_t vm_exec(const uint8_t *code) {
    enum { MAX_STACK = 1024 };
    int32_t stack[MAX_STACK];
    int32_t *top = stack;
    for (;;) {
        uint8_t op = *code++;
        switch (op) {
        case OP_SUB: {
            POPS(2);
            int32_t right = POP();
            int32_t left = POP();
            PUSHES(1);
            PUSH(left - right);
            break;
        }
        case OP_ADD: {
            POPS(2);
            int32_t left = POP();
            int32_t right = POP();
            PUSHES(1);
            PUSH(left + right);
            break;
        }
        case OP_OR: {
            POPS(2);
            int32_t left = POP();
            int32_t right = POP();
            PUSHES(1);
            PUSH(left | right);
            break;            
        }
        case OP_XOR: {
            POPS(2);
            int32_t left = POP();
            int32_t right = POP();
            PUSHES(1);
            PUSH(left ^ right);
            break;
        }
        case OP_MUL: {
            POPS(2);
            int32_t left = POP(), right = POP();
            PUSHES(1);
            PUSH(left * right);
            break;
        }
        case OP_DIV: {
            POPS(2);
            int32_t right = POP(), left = POP();
            PUSHES(1);
            if (right == 0)
                fatal("Zero division error");
            PUSH(left / right);
            break;
        }
        case OP_AND:{
            POPS(2);
            int32_t right = POP(), left = POP();
            PUSHES(1);
            PUSH(left & right);
            break;
        }
        case OP_SHL:{
            POPS(2);
            int32_t right = POP(), left = POP();
            PUSHES(1);
            PUSH(left << right);
            break;
        }
        case OP_SHR: {
            POPS(2);
            int32_t right = POP(), left = POP();
            PUSHES(1);
            PUSH(left >> right);
            break;
        }
        case OP_NEG: {
            POPS(1);
            int32_t el = POP();
            PUSHES(1);
            PUSH(-el);
            break;
        }
        case OP_NOT: {
            POPS(1);
            int32_t el = POP();
            PUSHES(1);
            PUSH(~el);
            break;
        }
        case OP_LIT:
            PUSHES(1);
            // Unaligned reads are legal and fast on modern PC-class processors.
            // This assumes the target architecture is little endian; as a bonus
            // exercise, figure out how to read this little endian int32 in a way
            // that's correct regardless of the target computer's native endianness.
            PUSH(*(uint32_t *)code);
            code += sizeof(uint32_t);
            break;
        case OP_HALT:
            POPS(1);
            return POP();
        default:
            fatal("vm_exec: illegal opcode");
            return 0;
        }
    }
}

// Creating a compiler
// 1. Add bytecode generation functions
// 2. Implement the code generator
// 3. Create a compiler entry point

// Function to generate a bytecode from AST nodes;
void emit_byte(uint8_t **code, uint8_t byte) {
    buf_push(*code, byte);
}

void emit_int32(uint8_t **code, int32_t val) {
    for (int i = 0; i < 4; ++i) {
        emit_byte(code, ((uint8_t *)&val)[i]);
    }
}

// Create recursive that generating bytecode

void compile_fact(ASTNode *node, uint8_t **code);
void compile_term(ASTNode *node, uint8_t **code);
void compile_exp(ASTNode *node, uint8_t **code);

void compile_fact(ASTNode *node, uint8_t **code) {
    if (node->kind == TOKEN_INT) {
        emit_byte(code, OP_LIT);
        emit_int32(code, node->int_val);
    }
    else if (node->kind == '-' || node->kind == '~') {
        compile_fact(node->right, code);
        emit_byte(code, node->kind == '-' ? OP_NEG : OP_NOT); 
    }
    else if (node->kind == '(') {
        compile_exp(node->unary_arg, code);
    }
    //else
        //fatal("Unexpected mode in factor '%d'", node->kind);
}
void compile_term(ASTNode *node, uint8_t **code) {
    if (node->kind == TOKEN_HIGH_PRIORITY ||
        node->kind == TOKEN_SHL ||
        node->kind == TOKEN_SHR) {
        compile_term(node->left, code);
        compile_fact(node->right, code);
        
        switch (node->kind) {
            case TOKEN_HIGH_PRIORITY: 
                switch (*node->name) {
                    case '*': emit_byte(code, OP_MUL); break;
                    case '/': emit_byte(code, OP_DIV); break;
                    case '&': emit_byte(code, OP_AND); break;
                    case '%': emit_byte(code, OP_MOD); break;
                    default: fatal("Uknown literal '%c'.", *node->name);
                }
                break;
            case TOKEN_SHL: emit_byte(code, OP_SHL); break;
            case TOKEN_SHR: emit_byte(code, OP_SHR); break;
            default:
                break;
        } 
    }
    else 
        compile_fact(node, code);
}

void compile_exp(ASTNode *node, uint8_t **code) {
    if (node->kind == TOKEN_LOW_PRIORITY) {
        compile_exp(node->left, code);
        compile_term(node->right, code);
        switch (*node->name) {
            case '+': emit_byte(code, OP_ADD); break;
            case '-': emit_byte(code, OP_SUB); break;
            case '|': emit_byte(code, OP_OR); break;
            case '^': emit_byte(code, OP_XOR); break;
            default: fatal("Uknown literal '%c'", *node->name);
        }
    }
    else {
        compile_term(node, code);
    }
}

// Compiler entry point
uint8_t *compile(ASTNode *ast) {
    uint8_t *code = NULL;
    compile_exp(ast, &code);
    emit_byte(&code, OP_HALT);
    return code;
}


// TESTING
#define MAXEXP 100

int getexp(char *expression) {
    int i, c;
    for (i = 0; (c = getchar()) != EOF && c != '\n' && i < MAXEXP - 1; ++i) {
        expression[i] = c;
    }
    expression[i] = '\0';
    return i;
}

void parse_test(char *expression) {
    //char *expression = malloc(MAXEXP * sizeof(char));
    //getexp(expression);
    stream = expression;
    next_token();
    ASTNode *ast = parse_expr();
    print_AST(ast);
    printf("\n");
    free_AST(ast);
}

void lex_test() {
    char *source = "12 * 34 + 45 / 56 + ~25";
    printf("EXAMPLE: %s\n", source);
    stream = source;
    next_token();
    while (token.kind) {
        print_token(token);
        next_token();
    }
    printf("\n");
}

void eval_test(char *expression) {
    //char *expression = malloc(MAXEXP);
    //getexp(expression);
    stream = expression;
    next_token();
    int64_t result = eval_exp();
    printf("RESULT: %ld\n", result);
}
void vm_test() {
    uint8_t *code = NULL;
    int32_t val2 = 2;
    buf_push(code, OP_LIT);
    for (int i = 0; i < 4; i++) buf_push(code, ((uint8_t *)&val2)[i]);
    int32_t val3 = 3;
    buf_push(code, OP_LIT);
    for (int i = 0; i < 4; i++) buf_push(code, ((uint8_t *)&val3)[i]);
    buf_push(code, OP_ADD);
    int32_t val4 = 4;
    buf_push(code, OP_LIT);
    for (int i = 0; i < 4; i++) buf_push(code, ((uint8_t *)&val4)[i]);
    buf_push(code, OP_MUL);
    buf_push(code, OP_HALT);
    int32_t result = vm_exec(code);
    printf("VM result: %d\n", result);
    buf_free(code);
}

void compiler_test(char *expression) {
    stream = expression;
    next_token();
    ASTNode *ast = parse_expr();
    
    printf("AST: ");
    print_AST(ast);
    printf("\n");
    
    uint8_t *bytecode = compile(ast);
    int32_t result = vm_exec(bytecode);
    
    printf("Compiled result: %d\n", result);
    
    free_AST(ast);
    buf_free(bytecode);
}

int main(int argc, char **argv) {

    return 0;
}

