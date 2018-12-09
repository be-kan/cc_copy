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

noreturn void error(char *fmt, ...) __attribute__((format(printf, 1, 2)));
char *format(char *fmt, ...) __attribute__((format(printf, 1, 2)));

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

Vector *new_vec(void);
void vec_push(Vector *v, void *elem);
void vec_pop(Vector *v);

typedef struct {
    Vector *keys;
    Vector *vals;
} Map;

Map *new_map(void);
void map_put(Map *map, char *key, void *val);
void map_puti(Map *map, char *key, int val);
void *map_get(Map *map, char *key);
int map_geti(Map *map, char *key, int default_);
bool map_exists(Map *map, char *key);

typedef struct {
    char *data;
    int capacity;
    int len;
} StringBuilder;

StringBuilder *new_sb(void);
void sb_add(StringBuilder *sb, char c);
void sb_append(StringBuilder *sb, char *s);
void sb_append_n(StringBuilder *sb, char *s, int len);
char *sb_get(StringBuilder *sb);

typedef struct Type {
    int ty;
    int size;
    int align;
    bool is_extern;
    struct Type *ptr_to;
    struct Type *ary_of;
    int len;
    Vector *members;
    int offset;
    struct Type *returning;
} Type;

Type *ptr_to(Type *base);
Type *ary_of(Type *base, int len);
bool same_type(Type *x, Type *y);
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
    TK_BREAK,
    TK_CONTINUE,
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
    TK_MUL_EQ,
    TK_DIV_EQ,
    TK_MOD_EQ,
    TK_ADD_EQ,
    TK_SUB_EQ,
    TK_SHL_EQ,
    TK_SHR_EQ,
    TK_BITAND_EQ,
    TK_XOR_EQ,
    TK_BITOR_EQ,
    TK_RETURN,
    TK_SIZEOF,
    TK_ALIGNOF,
    TK_TYPEOF,
    TK_PARAM,
    TK_EOF,
};

typedef struct {
    int ty;
    int val;
    char *name;
    char *str;
    int len; // fix char -> int
    bool stringize;
    char *buf;
    char *path;
    char *start;
    char *end;
} Token;

Vector *tokenize(char *path, bool add_eof);
noreturn void bad_token(Token *t, char *msg);
void warn_token(Token *t, char *msg);
char *tokstr(Token *t);
int get_line_number(Token *t);

Vector *preprocess(Vector *tokens);

extern int nlabel;

enum {
    ND_NUM = 256,
    ND_STR,
    ND_STRUCT,
    ND_DECL,
    ND_VARDEF,
    ND_VAR,
    ND_IF,
    ND_FOR,
    ND_DO_WHILE,
    ND_BREAK,
    ND_CONTINUE,
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
    ND_POST_INC,
    ND_POST_DEC,
    ND_MUL_EQ,
    ND_DIV_EQ,
    ND_MOD_EQ,
    ND_ADD_EQ,
    ND_SUB_EQ,
    ND_SHL_EQ,
    ND_SHR_EQ,
    ND_BITAND_EQ,
    ND_XOR_EQ,
    ND_BITOR_EQ,
    ND_RETURN,
    ND_CALL,
    ND_FUNC,
    ND_COMP_STMT,
    ND_EXPR_STMT,
    ND_STMT_EXPR,
    ND_NULL,
};

enum {
    INT = 1,
    CHAR,
    VOID,
    PTR,
    ARY,
    STRUCT,
    FUNC,
    TYPEOF,
};

typedef struct {
    Type *ty;
    bool is_local;
    int offset;
    char *name;
    char *data;
    int len;
} Var;

typedef struct Node {
    int op;
    Type *ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    struct Node *expr;
    Vector *stmts;
    char *name;
    Var *var;
    struct Node *cond;
    struct Node *then;
    struct Node *els;
    struct Node *init;
    struct Node *inc;
    struct Node *body;
    int break_label;
    int continue_label;
    struct Node *target;
    int stacksize;
    Vector *globals;
    Vector *lvars;
    Vector *args;
    Token *token;
} Node;

typedef struct {
    char *name;
    int stacksize;
    Node *node;
    Vector *ir;
} Function;

typedef struct {
    Vector *gvars;
    Vector *funcs;
} Program;

Program *parse(Vector *tokens);

Node *new_int_node(int val, Token *t);

Type *get_type(Node *node);
void sema(Program *prog);

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
    IR_JMP,
    IR_IF,
    IR_UNLESS,
    IR_LOAD,
    IR_STORE,
    IR_STORE_ARG,
    IR_KILL,
    IR_NOP,
};

typedef struct {
    int op;
    int lhs;
    int rhs;
    int size;
    bool is_imm;
    char *name;
    int nargs;
    int args[6];
} IR;

enum {
    IR_TY_NOARG,
    IR_TY_BINARY,
    IR_TY_REG,
    IR_TY_IMM,
    IR_TY_MEM,
    IR_TY_JMP,
    IR_TY_LABEL,
    IR_TY_LABEL_ADDR,
    IR_TY_REG_REG,
    IR_TY_REG_IMM,
    IR_TY_STORE_ARG,
    IR_TY_REG_LABEL,
    IR_TY_CALL,
};

void gen_ir(Program *prog);

void alloc_regs(Program *prog);

extern char *regs[];
extern char *regs8[];
extern char *regs32[];
extern int num_regs;

void gen_x86(Program *prog);

void util_test();