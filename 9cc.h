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
void vec_pushi(Vector *v, int val);
void *vec_pop(Vector *v);
void *vec_last(Vector *v);

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

typedef struct Type Type;

typedef struct Type {
    int ty;
    int size;
    int align;
    Type *ptr_to;
    Type *ary_of;
    int len;
    Map *members;
    int offset;
    Type *returning;
} Type;

Type *ptr_to(Type *base);
Type *ary_of(Type *base, int len);
Type *void_ty();
Type *bool_ty();
Type *char_ty();
Type *int_ty();
Type *func_ty(Type *returning);
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
    TK_BOOL,
    TK_IF,
    TK_ELSE,
    TK_FOR,
    TK_DO,
    TK_WHILE,
    TK_SWITCH,
    TK_CASE,
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
    TK_AND_EQ,
    TK_XOR_EQ,
    TK_OR_EQ,
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
int get_line_number(Token *t);
char *stringize(Vector *tokens);

Vector *preprocess(Vector *tokens);

extern int nlabel;

enum {
    ND_NUM = 256,
    ND_STRUCT,
    ND_DECL,
    ND_VARDEF,
    ND_VARREF,
    ND_CAST,
    ND_IF,
    ND_FOR,
    ND_DO_WHILE,
    ND_SWITCH,
    ND_CASE,
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
    ND_RETURN,
    ND_CALL,
    ND_FUNC,
    ND_COMP_STMT,
    ND_EXPR_STMT,
    ND_STMT_EXPR,
    ND_NULL,
};

enum {
    VOID = 1,
    BOOL,
    CHAR,
    INT,
    PTR,
    ARY,
    STRUCT,
    FUNC,
};

typedef struct {
    Type *ty;
    char *name;
    bool is_local;
    int offset;
    char *data;
} Var;

typedef struct Node Node;
typedef struct BB BB;

typedef struct Node {
    int op;
    Type *ty;
    Node *lhs;
    Node *rhs;
    int val;
    Node *expr;
    Vector *stmts;
    char *name;
    Var *var;
    Node *cond;
    Node *then;
    Node *els;
    Node *init;
    Node *inc;
    Node *body;
    Vector *cases;
    BB *bb;
    Node *target;
    BB *break_;
    BB *continue_;
    Vector *params;
    Vector *args;
    Token *token;
} Node;

typedef struct {
    char *name;
    Node *node;
    Vector *lvars;
    Vector *bbs;
    int stacksize;
} Function;

typedef struct {
    Vector *gvars;
    Vector *funcs;
} Program;

Program *parse(Vector *tokens);

Node *new_int_node(int val, Token *t);

Type *get_type(Node *node);
void sema(Program *prog);

void dump_ir(Vector *irv);

enum {
    IR_ADD = 1,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_IMM,
    IR_BPREL,
    IR_MOV,
    IR_RETURN,
    IR_CALL,
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
    IR_BR,
    IR_LOAD,
    IR_STORE,
    IR_STORE_ARG,
    IR_NOP,
};

typedef struct BB {
    int label;
    Vector *ir;
} BB;

typedef struct {
    int vn;
    int rn;
} Reg;

typedef struct {
    int op;
    Reg *r0;
    Reg *r2;
    int imm;
    int imm2;
    int label;
    BB *bb1;
    BB *bb2;
    int size;
    char *name;
    int nargs;
    Reg *args[6];
    Vector *kill;
} IR;

void gen_ir(Program *prog);

void alloc_regs(Program *prog);

extern char *regs[];
extern char *regs8[];
extern char *regs32[];
extern int num_regs;

void gen_x86(Program *prog);

void util_test();