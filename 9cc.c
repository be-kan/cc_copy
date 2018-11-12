#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    TK_NUM = 256,
    TK_EOF,
};

typedef struct {
    int ty;
    int val;
    char *input;
} Token;

Token tokens[100];

void tokenize(char *p) {
    int i = 0;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        fprintf(stderr, "cannot tokenize: %s", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
}

int pos = 0;

enum {
    ND_NUM = 256,
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

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

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

Node *number() {
    if (tokens[pos].ty != TK_NUM) {
        error("number expected, but got %s", tokens[pos].input);
    }
    return new_node_num(tokens[pos++].val);
}

Node *expr() {
    Node *lhs = number();
    for (;;) {
        int op = tokens[pos].ty;
        if (op != '+' && op != '-') {
            break;
        }
        pos++;
        lhs = new_node(op, lhs, number());
    }

    if (tokens[pos].ty != TK_EOF) {
        error("stray token: %s", tokens[pos].input);
    }
    return lhs;
}

char *regs[] = {"rdi", "rsi", "r10", "r11", "r12", "r13", "r14", "r15", NULL};
int cur;

char *gen(Node *node) {
    if (node->ty == ND_NUM) {
        char *reg = regs[cur++];
        if (!reg) {
            error("register exhausted");
        }
        printf("  mov %s, %d\n", reg, node->val);
        return reg;
    }

    char *dst = gen(node->lhs);
    char *src = gen(node->rhs);

    switch (node->ty) {
        case '+':
            printf("  add %s, %s\n", dst, src);
            return dst;
        case '-':
            printf("  sub %s, %s\n", dst, src);
            return dst;
        default:
            assert(0 && "unknown operator");
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: 9cc <code>\n");
        return 1;
    }

    tokenize(argv[1]);
    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".global _main\n");
    printf("_main:\n");
    printf("  mov rax, %s\n", gen(node));
    printf("  ret\n");

    return 0;
}