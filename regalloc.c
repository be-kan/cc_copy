#include "9cc.h"

static bool *used;

static void alloc(Reg *r) {
    if (!r || r->rn != -1) {
        return;
    }

    for (int i = 0; i < num_regs; i++) {
        if (used[i]) continue;
        used[i] = true;
        r->rn = i;
        return;
    }

    error("register exhausted");
}

static void visit(IR *ir) {
    alloc(ir->r0);
    alloc(ir->r2);

    if (ir->op == IR_CALL) {
        for (int i = 0; i < ir->nargs; i++) {
            alloc(ir->args[i]);
        }
    }

    for (int i = 0; i < ir->kill->len; i++) {
        Reg *r = ir->kill->data[i];
        assert(r->rn != -1);
        used[r->rn] = false;
    }
}

void alloc_regs(Program *prog) {
    used = calloc(1, num_regs);

    for (int i = 0; i < prog->funcs->len; i++) {
        Function *fn = prog->funcs->data[i];
        for (int i = 0; i < fn->bbs->len; i++) {
            BB *bb = fn->bbs->data[i];
            for (int i = 0; i < bb->ir->len; i++) {
                IR *ir = bb->ir->data[i];
                visit(ir);
            }
        }
    }
}