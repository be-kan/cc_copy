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
char *format(char *fmt, ...);

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

typedef struct {
    char *data;
    int capacity;
    int len;
} StringBuilder;

StringBuilder *new_sb(void);
void sb_append(StringBuilder *sb, char *s);
char *sb_get(StringBuilder *sb);

enum {
    TK_NUM = 256,
    TK_IDENT,
    TK_INT,
    TK_IF,
    TK_ELSE,
    TK_FOR,
    TK_LOGAND,
    TK_LOGOR,
    TK_RETURN,
    TK_EOF,
};

typedef struct {
    int ty;
    int val;
    char *name;
    char *input;
} Token;

Vector *tokenize(char *p);

enum {
    ND_NUM = 256,
    ND_IDENT,
    ND_VARDEF,
    ND_IF,
    ND_FOR,
    ND_LOGAND,
    ND_LOGOR,
    ND_RETURN,
    ND_CALL,
    ND_FUNC,
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
    char *name;
    struct Node *cond;
    struct Node *then;
    struct Node *els;
    struct Node *init;
    struct Node *inc;
    struct Node *body;
    Vector *args;
} Node;

Vector *parse(Vector *tokens);

enum {
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_IMM,
    IR_SUB_IMM,
    IR_MOV,
    IR_RETURN,
    IR_CALL,
    IR_LABEL,
    IR_LT,
    IR_JMP,
    IR_UNLESS,
    IR_LOAD,
    IR_STORE,
    IR_KILL,
    IR_SAVE_ARGS,
    IR_NOP,
};

typedef struct {
    int op;
    int lhs;
    int rhs;
    char *name;
    int nargs;
    int args[6];
} IR;

enum {
    IR_TY_NOARG,
    IR_TY_REG,
    IR_TY_IMM,
    IR_TY_JMP,
    IR_TY_LABEL,
    IR_TY_REG_REG,
    IR_TY_REG_IMM,
    IR_TY_REG_LABEL,
    IR_TY_CALL,
};

typedef struct {
    char *name;
    int ty;
} IRInfo;

typedef struct {
    char *name;
    int stacksize;
    Vector *ir;
} Function;

extern IRInfo irinfo[];

Vector *gen_ir(Vector *nodes);
void dump_ir(Vector *irv);

extern char *regs[];
extern char *regs8[];
void alloc_regs(Vector *irv);

void gen_x86(Vector *fns);

void util_test();