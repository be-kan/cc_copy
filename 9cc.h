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
void sb_add(StringBuilder *sb, char s);
void sb_append(StringBuilder *sb, char *s);
char *sb_get(StringBuilder *sb);

typedef struct Type {
    int ty;
    struct Type *ptr_of;
    struct Type *ary_of;
    int len;
} Type;

Type *ptr_of(Type *base);
Type *ary_of(Type *base, int len);
int size_of(Type *ty);

enum {
    TK_NUM = 256,
    TK_STR,
    TK_IDENT,
    TK_INT,
    TK_CHAR,
    TK_IF,
    TK_ELSE,
    TK_FOR,
    TK_DO,
    TK_WHILE,
    TK_EQ,
    TK_NE,
    TK_LOGAND,
    TK_LOGOR,
    TK_RETURN,
    TK_SIZEOF,
    TK_EOF,
};

typedef struct {
    int ty;
    int val;
    char *name;
    char *input;
    char *str;
    int len; // fix char -> int
} Token;

Vector *tokenize(char *p);

enum {
    ND_NUM = 256,
    ND_STR,
    ND_IDENT,
    ND_VARDEF,
    ND_LVAR,
    ND_GVAR,
    ND_IF,
    ND_FOR,
    ND_DO_WHILE,
    ND_ADDR,
    ND_DEREF,
    ND_EQ,
    ND_NE,
    ND_LOGAND,
    ND_LOGOR,
    ND_RETURN,
    ND_SIZEOF,
    ND_CALL,
    ND_FUNC,
    ND_COMP_STMT,
    ND_EXPR_STMT,
};

enum {
    INT,
    CHAR,
    PTR,
    ARY,
};

typedef struct Node {
    int op;
    Type *ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    struct Node *expr;
    Vector *stmts;
    char *name;
    char *data;
    int len;
    struct Node *cond;
    struct Node *then;
    struct Node *els;
    struct Node *init;
    struct Node *inc;
    struct Node *body;
    int stacksize;
    Vector *globals;
    int offset;
    Vector *args;
} Node;

Vector *parse(Vector *tokens);
int size_of(Type *ty);

typedef struct {
    Type *ty;
    bool is_local;
    int offset;
    char *name;
    char *data;
    int len;
} Var;

Vector *sema(Vector *nodes);

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
    IR_LABEL_ADDR,
    IR_EQ,
    IR_NE,
    IR_LT,
    IR_JMP,
    IR_IF,
    IR_UNLESS,
    IR_LOAD8,
    IR_LOAD32,
    IR_LOAD64,
    IR_STORE8,
    IR_STORE32,
    IR_STORE64,
    IR_STORE8_ARG,
    IR_STORE32_ARG,
    IR_STORE64_ARG,
    IR_KILL,
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
    IR_TY_LABEL_ADDR,
    IR_TY_REG_REG,
    IR_TY_REG_IMM,
    IR_TY_IMM_IMM,
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
    Vector *globals;
    Vector *ir;
} Function;

extern IRInfo irinfo[];

Vector *gen_ir(Vector *fns);
void dump_ir(Vector *irv);

extern char *regs[];
extern char *regs8[];
extern char *regs32[];
void alloc_regs(Vector *irv);

void gen_x86(Vector *globals, Vector *fns);

void util_test();