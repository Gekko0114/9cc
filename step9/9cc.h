#define _GNU_SOURCE
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_EOF,    
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;
    Token *next;
    int val;
    char *str;
    int len;
};

Token *token;
void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number(void);
bool at_eof(void);
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
Token *tokenize();

/* parse.c */

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_ASSIGN,
    ND_LVAR,    
    ND_NUM,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE     
} NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
    int offset;
};

Node *code[100];
Node *new_node(NodeKind kind);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_num(int val);
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
Node *assign();
Node *stmt();
void *program();

/* codegen.c */
void gen(Node *node);
void gen_lval(Node *node);

/* main.c */
char *user_input;
int main(int argc, char **argv);