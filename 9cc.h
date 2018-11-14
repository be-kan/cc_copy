#define _GNU_SOURCE
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

noreturn void error(char *fmt, ...);

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

Vector *new_vec(void);
void vec_push(Vector *v, void *elem);

typedef struct {
    Vector *keys;
    Vector *vals;
} Map;

Map *new_map(void);
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);
bool map_exists(Map *map, char *key);

enum {
    TK_NUM = 256,
    TK_RETURN,
    TK_EOF,
};

typedef struct {
    int ty;
    int val;
    char *input;
} Token;

Vector *tokenize(char *p);

enum {
    ND_NUM = 256,
    ND_RETURN,
    ND_COMP_STMT,
    ND_EXPR_STMT,
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    struct Node *expr;
    Vector *stmts;
} Node;

Node *parse(Vector *tokens);

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

Vector *gen_ir(Node *node);

extern char *regs[];
void alloc_regs(Vector *irv);

void gen_x86(Vector *irv);