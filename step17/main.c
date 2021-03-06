#include "9cc.h"

int main(int argc, char **argv) {
    if (argc != 2)
        error("引数の個数が正しくありません");
    user_input = argv[1];
    token = tokenize();
    Function *prog = program();

    for (Function *fn = prog; fn; fn = fn->next)
    {
        int offset = 0;
        for (LVarList *vl = fn->locals; vl; vl = vl->next)
        {
            offset += 8;
            vl->var->offset = offset;
        }
        fn->stack_size = offset;
    };

    codegen(prog);
    return 0;
}