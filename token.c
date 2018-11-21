#include "9cc.h"

static Token *add_token(Vector *v, int ty, char *input) {
    Token *t = calloc(1, sizeof(Token));
    t->ty = ty;
    t->input = input;
    vec_push(v, t);
    return t;
}

static struct {
    char *name;
    int ty;
} symbols[] = {
        {"else", TK_ELSE},
        {"for", TK_FOR},
        {"if", TK_IF},
        {"int", TK_INT},
        {"return", TK_RETURN},
        {"&&", TK_LOGAND},
        {"||", TK_LOGOR},
        {NULL, 0},
};

Vector *tokenize(char *p) {
    Vector *v = new_vec();

loop:
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        for (int i = 0; symbols[i].name; i++) {
            char *name = symbols[i].name;
            int len = strlen(name);
            if (strncmp(p, name, len)) {
                continue;
            }
            add_token(v, symbols[i].ty, p);
            p += len;
            goto loop;
        }

        if (strchr("+-*/;=(),{}<>[]&", *p)) {
            add_token(v, *p, p);
            p++;
            continue;
        }

        if (isalpha(*p) || *p == '_') {
            int len = 1;
            while (isalpha(p[len]) || isdigit(p[len]) || p[len] == '_') {
                len++;
            }
            Token *t = add_token(v, TK_IDENT, p);
            t->name = strndup(p, len);
            p += len;
            continue;
        }

        if (isdigit(*p)) {
            Token *t = add_token(v, TK_NUM, p);
            for (; isdigit(*p); p++) {
                t->val = t->val * 10 + *p - '0';
            }
            continue;
        }

        error("cannot tokenize: %s", p);
    }

    add_token(v, TK_EOF, p);
    return v;
}
