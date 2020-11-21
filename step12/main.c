#include "9cc.h"

int main(int argc, char **argv) {
    if (argc != 2)
        error("引数の個数が正しくありません");
    user_input = argv[1];
    token = tokenize();
    locals = calloc(1, sizeof(LVar));
    program();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf(" push rbp\n");
    printf(" mov rbp, rsp\n");
    printf(" sub rsp, %d\n", locals->offset);

    for (size_t i = 0; code[i]; i++) {
        gen(code[i]);
        printf(" pop rax\n");
    }

    printf(" mov rsp, rbp\n");
    printf(" pop rbp\n");
    printf(" ret\n");
    return 0;
}