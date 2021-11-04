#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tokenize
typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_IDENT,
  TK_RETURN,
  TK_IF,
  TK_ELSE,
  TK_WHILE,
  TK_FOR,
  TK_TYPE,
  TK_EOF
} TokenKind;

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
bool consume_if();
bool consume_else();
bool consume_while();
bool consume_for();
bool consume_type();
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
  ND_RETURN,
  ND_IF,
  ND_ELSE,
  ND_WHILE,
  ND_FOR,
  ND_FOR_LEFT,
  ND_FOR_RIGHT,
  ND_BLOCK,
  ND_FUNC_CALL,
  ND_FUNC_DEF,
  ND_ADDR,
  ND_DEREF
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  Node *els;
  Node **block;
  char *funcname;
  Node **args;
  int len;
  int val;
  int offset;
};

typedef struct Type Type;
struct Type {
  enum { INT, PTR } ty;
  struct Type *ptr_to;
};

extern Node *code[];

void program();
Node *func();
Node *stmt();
Node *assign();
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
Node *define_variable();
Node *variable();

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
  Type *type;
};

extern LVar *locals[];
extern int cur_func;
