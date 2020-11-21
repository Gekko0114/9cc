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
    TK_RETURN    
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
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool start_with(char *p, char *q);
bool is_alpha(char c);
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
    ND_LE,
    ND_RETURN,
    ND_IF   
} NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    Node *cond;
    Node *then;
    Node *els;
    int val;
    int offset;
};

bool consume(char *op);
Token *consume_ident(void);
void expect(char *op);
int expect_number(void);
bool at_eof(void);

Node *new_node(NodeKind kind);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);
Node *new_num(int val);

Node *code[100];
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

typedef struct LVar LVar;
struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
};
LVar *locals;
LVar *find_lvar(Token *tok);

/* codegen.c */
void gen(Node *node);
void gen_lvar(Node *node);

/* main.c */
char *user_input;
int main(int argc, char **argv);