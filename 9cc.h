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
void sb_lappend(StringBuilder *sb, char *s, int len);
char *sb_get(StringBuilder *sb);

typedef struct Type {
    int ty;
    int size;
    int align;
    struct Type *ptr_to;
    struct Type *ary_of;
    int len;
    Vector *members;
    int offset;
} Type;

Type *ptr_to(Type *base);
Type *ary_of(Type *base, int len);
int roundup(int x, int align);

enum {
    TK_NUM = 256,
    TK_STR,
    TK_IDENT,
    TK_ARROW,
    TK_EXTERN,
    TK_TYPEDEF,
    TK_INT,
    TK_CHAR,
    TK_VOID,
    TK_STRUCT,
    TK_IF,
    TK_ELSE,
    TK_FOR,
    TK_DO,
    TK_WHILE,
    TK_EQ,
    TK_NE,
    TK_LE,
    TK_GE,
    TK_LOGAND,
    TK_LOGOR,
    TK_SHL,
    TK_SHR,
    TK_INC,
    TK_DEC,
    TK_RETURN,
    TK_SIZEOF,
    TK_ALIGNOF,
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
    ND_STRUCT,
    ND_VARDEF,
    ND_LVAR,
    ND_GVAR,
    ND_IF,
    ND_FOR,
    ND_DO_WHILE,
    ND_ADDR,
    ND_DEREF,
    ND_DOT,
    ND_EQ,
    ND_NE,
    ND_LE,
    ND_LOGAND,
    ND_LOGOR,
    ND_SHL,
    ND_SHR,
    ND_MOD,
    ND_NEG,
    ND_PRE_INC,
    ND_PRE_DEC,
    ND_POST_INC,
    ND_POST_DEC,
    ND_RETURN,
    ND_SIZEOF,
    ND_ALIGNOF,
    ND_CALL,
    ND_FUNC,
    ND_COMP_STMT,
    ND_EXPR_STMT,
    ND_STMT_EXPR,
    ND_NULL,
};

enum {
    INT,
    CHAR,
    VOID,
    PTR,
    ARY,
    STRUCT,
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
    bool is_extern;
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

typedef struct {
    Type *ty;
    bool is_local;
    int offset;
    char *name;
    bool is_extern;
    char *data;
    int len;
} Var;

Vector *sema(Vector *nodes);

typedef struct {
    char *name;
    int ty;
} IRInfo;

extern IRInfo irinfo[];

void dump_ir(Vector *irv);

enum {
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_IMM,
    IR_BPREL,
    IR_MOV,
    IR_RETURN,
    IR_CALL,
    IR_LABEL,
    IR_LABEL_ADDR,
    IR_EQ,
    IR_NE,
    IR_LE,
    IR_LT,
    IR_AND,
    IR_OR,
    IR_XOR,
    IR_SHL,
    IR_SHR,
    IR_MOD,
    IR_NEG,
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
    int stacksize;
    Vector *globals;
    Vector *ir;
} Function;

Vector *gen_ir(Vector *fns);

extern char *regs[];
extern char *regs8[];
extern char *regs32[];
void alloc_regs(Vector *irv);

void gen_x86(Vector *globals, Vector *fns);

void util_test();