#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tokenize
typedef enum { TK_RESERVED, TK_NUM, TK_IDENT, TK_RETURN, TK_EOF } TokenKind;

typedef struct Token Token;
struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};

extern Token *token;
extern char *user_input;

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

bool consume(char *op);
Token *consume_ident();
bool consume_return();
void expect(char *op);
int expect_number();
bool at_eof();
bool startswith(char *p, char *q);

Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize();

typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM,
  ND_EQ,
  ND_NE,
  ND_LT,
  ND_LE,
  ND_ASSIGN,
  ND_LVAR,
  ND_RETURN
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
  int offset;
};

extern Node *code[];

void program();
Node *stmt();
Node *assign();
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

// gen
void gen_lval(Node *node);
void gen(Node *node);

typedef struct LVar LVar;
struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};

extern LVar *locals;