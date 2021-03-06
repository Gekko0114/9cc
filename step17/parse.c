#include "9cc.h"

Token *peek(char *op)
{
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return NULL;
    return token;
}

Token *consume(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return NULL;
    Token *t = token;
    token = token->next;
    return t;
}

Token *consume_ident(void)
{
    if (token->kind != TK_IDENT)
        return false;
    Token *tok = token;
    token = token->next;
    return tok;
}

void expect(char *op) {
    if (!peek(op))
        error_tok(token, "expected \"%s\"", op);
    token = token->next;
}

int expect_number(void) {
    if (token->kind != TK_NUM)
        error_tok(token, "expected a number");
    int val = token->val;
    token = token->next;
    return val;
}

char *expect_ident(void)
{
    if (token->kind != TK_IDENT)
        error_tok(token, "識別子ではありません。");
    char *ident = calloc(1, sizeof(token->len));
    strncpy(ident, token->str, token->len);
    ident[token->len] = '\0';
    token = token->next;
    return ident;
}

bool at_eof(void) {
    return token->kind == TK_EOF;
}

Node *new_node(NodeKind kind, Token *tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->tok = tok;
    return node;
};

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs, Token *tok) {
    Node *node = new_node(kind, tok);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
};

Node *new_unary(NodeKind kind, Node *lhs, Token *tok)
{
    Node *node = new_node(kind, tok);
    node->lhs = lhs;
    return node;
};

Node *new_num(int val, Token *tok) {
    Node *node = new_node(ND_NUM, tok);
    node->val = val;
    return node;
};

Node *new_node_lvar(LVar *lvar, Token *tok)
{
    Node *node = new_node(ND_LVAR, tok);
    node->var = lvar;
    return node;
}

LVar *new_lvar(char *name, Type *ty)
{
    LVar *lvar = calloc(1, sizeof(LVar));
    lvar->name = name;
    lvar->ty = ty;
    LVarList *vl = calloc(1, sizeof(LVarList));
    vl->var = lvar;
    vl->next = locals;
    locals = vl;
    return lvar;
}

LVar *find_lvar(Token *tok) {
    for (LVarList *vl = locals; vl; vl = vl->next)
        if (strlen(vl->var->name) == tok->len &&
            !strncmp(tok->str, vl->var->name, tok->len))
            return vl->var;
    return NULL;
}

Function *program(void)
{
    Function head = {};
    Function *cur = &head;
    while (!at_eof())
    {
        cur->next = function();
        cur = cur->next;
    }
    return head.next;
}

Type *basetype(void)
{
    expect("int");
    Type *ty = int_type;
    while (consume("*"))
        ty = pointer_to(ty);
    return ty;
}

LVarList *read_func_param(void)
{
    LVarList *vl = calloc(1, sizeof(LVarList));
    Type *ty = basetype();
    vl->var = new_lvar(expect_ident(), ty);
    return vl;
}

LVarList *read_func_params(void)
{
    if (consume(")"))
        return NULL;
    
    LVarList *head = read_func_param();
    LVarList *cur = head;

    while (!consume(")"))
    {
        expect(",");
        cur->next = read_func_param();
        cur = cur->next;
    }
    return head;
}

Function *function(void)
{
    locals = NULL;
    Function *fn = calloc(1, sizeof(Function));
    basetype();
    fn->name = expect_ident();
    expect("(");
    fn->params = read_func_params();
    expect("{");
    Node head = {};
    Node *cur = &head;
    while (!consume("}"))
    {
        cur->next = stmt();
        cur = cur->next;
    }

    fn->node = head.next;
    fn->locals = locals;
    return fn;
}

Node *declaration(void)
{
    Token *tok = token;
    Type *ty = basetype();
    LVar *var = new_lvar(expect_ident(), ty);

    if (consume(";"))
        return new_node(ND_NULL, tok);

    expect("=");
    Node *lhs = new_node_lvar(var, tok);
    Node *rhs = expr();
    expect(";");
    Node *node = new_binary(ND_ASSIGN, lhs, rhs, tok);
    return new_unary(ND_EXPR_STMT, node, tok);
}

Node *read_expr_stmt(void)
{
    Token *tok = token;
    return new_unary(ND_EXPR_STMT, expr(), tok);
}

Node *stmt(void) {
    Node *node = stmt2();
    add_type(node);
    return node;
}

Node *stmt2(void) {
    Node *node;
    Token *tok;
    if (tok = consume("return"))
    {
        node = new_node(ND_RETURN, tok);
        node->lhs = expr();
        expect(";");
        return node;
    }
    if (tok = consume("if"))
    {
        node = new_node(ND_IF, tok);
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        if (consume("else"))
            node->els = stmt();
        return node;
    }
    if (tok = consume("while"))
    {
        node = new_node(ND_WHILE, tok);
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        return node;
    }
    if (tok = consume("for"))
    {
        node = new_node(ND_FOR, tok);
        expect("(");
        if (!consume(";"))
        {
            node->init = expr();
            expect(";");
        }
        if (!consume(";"))
        {
            node->cond = expr();
            expect(";");
        }
        if (!consume(")"))
        {
            node->inc = expr();
            expect(")");
        }
        node->then = stmt();
        return node;
    }
    if (tok = consume("{"))
    {
        node = new_node(ND_BLOCK, tok);
        Node head = {};
        Node *cur = &head;
        while (!consume("}"))
        {
            cur->next = stmt();
            cur = cur->next;
        }
        node->body = head.next;
        return node;
    }
    if (tok = peek("int"))
        return declaration();
    node = read_expr_stmt();
    expect(";");
    return node;
}

Node *expr() {
    return assign();
}

Node *assign() {
    Node *node = equality();
    Token *tok;
    for (;;)
    {
    if (tok = consume("="))
        node = new_binary(ND_ASSIGN, node, assign(), tok);
    else
        return node;
    }
}

Node *equality() {
    Node *node = relational();
    Token *tok;
    for (;;) {
        if (tok = consume("=="))
            node = new_binary(ND_EQ, node, relational(), tok);
        else if (consume("!="))
            node = new_binary(ND_NE, node, relational(), tok);
        else
            return node;
    }
}

Node *relational() {
    Node *node = add();
    Token *tok;

    for (;;) {
        if (tok = consume("<"))
            node = new_binary(ND_LT, node, add(), tok);
        else if (tok = consume("<="))
            node = new_binary(ND_LE, node, add(), tok);
        else if (tok = consume(">"))
            node = new_binary(ND_LT, add(), node, tok);
        else if (tok = consume(">="))
            node = new_binary(ND_LE, add(), node, tok);
        else
            return node;
    }
}

Node *new_add(Node *lhs, Node *rhs, Token *tok)
{
    add_type(lhs);
    add_type(rhs);

    if (is_integer(lhs->ty) && is_integer(rhs->ty))
        return new_binary(ND_ADD, lhs, rhs, tok);
    if (lhs->ty->base && is_integer(rhs->ty))
        return new_binary(ND_PTR_ADD, lhs, rhs, tok);
    if (is_integer(lhs->ty) && rhs->ty->base)
        return new_binary(ND_ADD, rhs, lhs, tok);
    error_tok(tok, "invalid operands");
}

Node *new_sub(Node *lhs, Node *rhs, Token *tok)
{
    add_type(lhs);
    add_type(rhs);

    if (is_integer(lhs->ty) && is_integer(rhs->ty))
        return new_binary(ND_SUB, lhs, rhs, tok);
    if (lhs->ty->base && is_integer(rhs->ty))
        return new_binary(ND_PTR_SUB, lhs, rhs, tok);
    if (lhs->ty->base && rhs->ty->base)
        return new_binary(ND_PTR_DIFF, lhs, rhs, tok);
    error_tok(tok, "invalid operands");
}

Node *add() {
    Node *node = mul();
    Token *tok;

    for (;;) {
        if (tok = consume("+"))
            node = new_add(node, mul(), tok);
        else if (tok = consume("-"))
            node = new_sub(node, mul(), tok);
        else
            return node;
    }
}

Node *mul() {
    Node *node = unary();
    Token *tok;
    for (;;) {
        if (tok = consume("*"))
            node = new_binary(ND_MUL, node, unary(), tok);
        else if (tok = consume("/"))
            node = new_binary(ND_DIV, node, unary(), tok);
        else
            return node;
    }
}

Node *unary() {
    Token *tok;
    if (tok = consume("+"))
        return unary();
    if (tok = consume("-"))
        return new_binary(ND_SUB, new_num(0, tok), unary(), tok);
    if (tok = consume("&"))
        return new_unary(ND_ADDR, unary(), tok);
    if (tok = consume("*"))
        return new_unary(ND_DEREF, unary(), tok);
    return primary();
}

Node *func_args() {
    if (consume(")"))
        return NULL;

    Node *head = assign();
    Node *cur = head;
    while (consume(","))
    {
        cur->next = assign();
        cur = cur->next;
    }
    expect(")");
    return head;
}

Node *primary() {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }
    Token *tok;
    if (tok = consume_ident()) {
        if (consume("("))
        {
            Node *node = new_node(ND_FUNCALL, tok);
            node->funcname = calloc(1, sizeof(tok->len));
            strncpy(node->funcname, tok->str, tok->len);
            node->funcname[tok->len] = '\0';
            node->args = func_args();
            return node;
        }
        LVar *lvar = find_lvar(tok);
        if (!lvar)
            error_tok(tok, "undefined variable");
        return new_node_lvar(lvar, tok);
    }
    else
    {
        tok = token;
        if (tok->kind != TK_NUM)
            error_tok(tok, "expected expression");
        return new_num(expect_number(), tok);
    }
}

