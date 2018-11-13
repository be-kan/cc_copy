#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

Vector *new_vec() {
    Vector *v = malloc(sizeof(Vector));
    v->data = malloc(sizeof(void *) * 16);
    v->capacity = 16;
    v->len = 0;
    return v;
}

void vec_push(Vector *v, void *elem) {
    if (v->len == v->capacity) {
        v->capacity *= 2;
        v->data = realloc(v->data, sizeof(void *) * v->capacity);
    }
    v->data[v->len++] = elem;
}

enum {
    TK_NUM = 256,
    TK_EOF,
};

typedef struct {
    int ty;
    int val;
    char *input;
} Token;

Token *add_token(Vector *v, int ty, char *input) {
    Token *t = malloc(sizeof(Token));
    t->ty = ty;
    t->input = input;
    vec_push(v, t);
    return t;
}

Vector *tokenize(char *p) {
    Vector *v = new_vec();

    int i = 0;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            add_token(v, *p, p);
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            Token *t = add_token(v, TK_NUM, p);
            t->val = strtol(p, &p, 10);
            i++;
            continue;
        }

        fprintf(stderr, "cannot tokenize: %s", p);
        exit(1);
    }

    add_token(v, TK_EOF, p);
    return v;
}

enum {
    ND_NUM = 256,
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

Vector *tokens;
int pos;

Node *new_node(int op, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->ty = op;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num (int val) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

noreturn void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

Node *number() {
    Token *t = tokens->data[pos];
    if (t->ty != TK_NUM) {
        error("number expected, but got %s", t->input);
    }
    pos++;
    return new_node_num(t->val);
}

Node *expr() {
    Node *lhs = number();
    for (;;) {
        Token *t = tokens->data[pos];
        int op = t->ty;
        if (op != '+' && op != '-') {
            break;
        }
        pos++;
        lhs = new_node(op, lhs, number());
    }

    Token *t = tokens->data[pos];
    if (t->ty != TK_EOF) {
        error("stray token: %s", t->input);
    }
    return lhs;
}

enum {
    IR_IMM,
    IR_MOV,
    IR_RETURN,
    IR_KILL,
    IR_NOP,
};

typedef struct {
    int op;
    int lhs;
    int rhs;
} IR;

IR *new_ir(int op, int lhs, int rhs) {
    IR *ir = malloc(sizeof(IR));
    ir->op = op;
    ir->lhs = lhs;
    ir->rhs = rhs;
    return ir;
}

int gen_ir_sub(Vector *v, Node *node) {
    static int regno;
    if (node->ty == ND_NUM) {
        int r = regno++;
        vec_push(v, new_ir(IR_IMM, r, node->val));
        return r;
    }

    assert(node->ty == '+' || node->ty == '-');

    int lhs = gen_ir_sub(v, node->lhs);
    int rhs = gen_ir_sub(v, node->rhs);

    vec_push(v, new_ir(node->ty, lhs, rhs));
    vec_push(v, new_ir(IR_KILL, rhs, 0));
    return lhs;
}

Vector *gen_ir(Node *node) {
    Vector *v = new_vec();
    int r = gen_ir_sub(v, node);
    vec_push(v, new_ir(IR_RETURN, r, 0));
    return v;
}

char *regs[] = {"rdi", "rsi", "r10", "r11", "r12", "r13", "r14", "r15"};
bool used[sizeof(regs) / sizeof(*regs)];
int *reg_map;

int alloc(int ir_reg) {
    if (reg_map[ir_reg] != -1) {
        int r = reg_map[ir_reg];
        assert(used[r]);
        return r;
    }

    for (int i = 0; i < sizeof(regs) / sizeof(*regs); i++) {
        if (used[i]) continue;
        used[i] = true;
        reg_map[ir_reg] = i;
        return i;
    }

    error("register exhausted");
    return -1;
}

void kill(int r) {
    assert(used[r]);
    used[r] = false;
}

void alloc_regs(Vector *irv) {
    reg_map = malloc(sizeof(int) * irv->len);
    for (int i = 0; i < irv->len; i++) {
        reg_map[i] = -1;
    }
    for (int i = 0; i < irv->len; i++) {
        IR *ir = irv->data[i];

        switch (ir->op) {
            case IR_IMM:
                ir->lhs = alloc(ir->lhs);
                break;
            case IR_MOV:
            case '+':
            case '-':
                ir->lhs = alloc(ir->lhs);
                ir->rhs = alloc(ir->rhs);
                break;
            case IR_RETURN:
                kill(reg_map[ir->lhs]);
                break;
            case IR_KILL:
                kill(reg_map[ir->lhs]);
                ir->op = IR_NOP;
                break;
            default:
                assert(0 && "unknown operator");
        }
    }
}

void gen_x86(Vector *irv) {
    for (int i = 0; i < irv->len; i++) {
        IR *ir = irv->data[i];
        switch (ir->op) {
            case IR_IMM:
                printf("  mov %s, %d\n", regs[ir->lhs], ir->rhs);
                break;
            case IR_MOV:
                printf("  mov %s, %s\n", regs[ir->lhs], regs[ir->rhs]);
                break;
            case IR_RETURN:
                printf("  mov rax, %s\n", regs[ir->lhs]);
                printf("  ret\n");
            case '+':
                printf("  add %s, %s\n", regs[ir->lhs], regs[ir->rhs]);
                break;
            case '-':
                printf("  sub %s, %s\n", regs[ir->lhs], regs[ir->rhs]);
                break;
            case IR_NOP:
                break;
            default:
                assert(0 && "unknwon operator");
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: 9cc <code>\n");
        return 1;
    }

    tokens = tokenize(argv[1]);
    Node *node = expr();

    Vector *irv = gen_ir(node);
    alloc_regs(irv);

    printf(".intel_syntax noprefix\n");
    printf(".global _main\n");
    printf("_main:\n");

    gen_x86(irv);

    return 0;
}