#define _GNU_SOURCE
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Type Type;

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
void error_tok(Token *tok, char *fmt, ...);
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool start_with(char *p, char *q);
bool is_alpha(char c);
Token *tokenize();

/* parse.c */

typedef enum {
    ND_ADD,
    ND_PTR_ADD,
    ND_SUB,
    ND_PTR_SUB,
    ND_PTR_DIFF,
    ND_MUL,
    ND_DIV,
    ND_ADDR,
    ND_DEREF,
    ND_ASSIGN,
    ND_LVAR,    
    ND_NUM,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_RETURN,
    ND_IF,
    ND_WHILE,
    ND_FOR,
    ND_BLOCK,
    ND_NULL,
    ND_FUNCALL,
    ND_EXPR_STMT
} NodeKind;

typedef struct LVar LVar;
struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
    Type *ty;
};

typedef struct LVarList LVarList;

struct LVarList
{
    LVarList *next;
    LVar *var;
};

typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node *lhs;
    Node *rhs;

    Node *init;
    Node *cond;
    Node *then;
    Node *els;
    Node *inc;

    Node *body;
    Node *next;
    Type *ty;
    Token *tok;

    char *funcname;
    Node *args;

    int val;
    LVar *var;
};

Token *peek(char *op);
Token *consume(char *op);
Token *consume_ident(void);
void expect(char *op);
int expect_number(void);
char *expect_ident(void);
bool at_eof(void);

Node *new_node(NodeKind kind, Token *tok);
Node *new_binary(NodeKind kind, Node *lhs, Node *rhs, Token *tok);
Node *new_unary(NodeKind kind, Node *lhs, Token *tok);
Node *new_num(int val, Token *tok);
Node *new_node_lvar(LVar *lvar, Token *tok);
LVar *new_lvar(char *name, Type *ty);
LVar *find_lvar(Token *tok);

typedef struct Function Function;

struct Function
{
    Function *next;
    char *name;
    LVarList *params;
    Node *node;
    LVarList *locals;
    int stack_size;
};

Function *program(void);
void codegen(Function *prog);
Function *function(void);

Node *expr(void);
Node *equality(void);
Node *relational(void);
Node *add(void);
Node *mul(void);
Node *unary(void);
Node *primary(void);
Node *assign(void);
Node *declaration(void);
Node *stmt(void);
Node *stmt2(void);

LVarList *locals;

/* type.c */

typedef enum
{
    TY_INT,
    TY_PTR
} TypeKind;

struct Type
{
    TypeKind kind;
    Type *base;
};

extern Type *int_type;

bool is_integer(Type *ty);
Type *pointer_to(Type *base);
void add_type(Node *node);

/* codegen.c */
void gen(Node *node);
void gen_lvar(Node *node);

/* main.c */
char *user_input;
int main(int argc, char **argv);