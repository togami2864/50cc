#include "50cc.h"

Node *code[100];
LVar *locals;

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

Token *consume_ident() {
  if (token->kind != TK_IDENT) return NULL;
  Token *tok = token;
  token = token->next;
  return tok;
}

bool consume_return() {
  if (token->kind != TK_RETURN) return false;
  token = token->next;
  return true;
}

bool consume_if() {
  if (token->kind != TK_IF) return false;
  token = token->next;
  return true;
}

bool consume_else() {
  if (token->kind != TK_ELSE) return false;
  token = token->next;
  return true;
}

bool consume_while() {
  if (token->kind != TK_WHILE) return false;
  token = token->next;
  return true;
}

bool consume_for() {
  if (token->kind != TK_FOR) return false;
  token = token->next;
  return true;
}

void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "expected \"%s\"", op);
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM) {
    fprintf(stderr, "%d\n", token->kind);
    error_at(token->str, "数ではありません");
  }
  int val = token->val;
  token = token->next;

  return val;
}

LVar *find_lvar(Token *tok) {
  int i = 0;
  for (LVar *var = locals; var; var = var->next) {
    i++;
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  }
  return NULL;
}

void program() {
  int i = 0;
  while (!at_eof()) code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node;
  if (consume("{")) {
    node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
    node->block = calloc(10, sizeof(Node));
    for (int i = 0; !consume("}"); i++) {
      node->block[i] = stmt();
    }
    return node;
  }
  if (consume_if()) {
    expect("(");
    node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    node->lhs = expr();
    expect(")");
    node->rhs = stmt();
    node->els = NULL;
    if (consume_else()) {
      Node *els = calloc(1, sizeof(Node));
      els->kind = ND_ELSE;
      els->lhs = node->rhs;
      els->rhs = stmt();
      node->rhs = els;
    }
    return node;
  }

  if (consume_while()) {
    expect("(");
    node = calloc(1, sizeof(Node));
    node->kind = ND_WHILE;
    node->lhs = expr();
    expect(")");
    node->rhs = stmt();
    return node;
  }

  if (consume_for()) {
    expect("(");
    node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;
    Node *left = calloc(1, sizeof(Node));
    left->kind = ND_FOR_LEFT;
    Node *right = calloc(1, sizeof(Node));
    right->kind = ND_FOR_RIGHT;

    if (!consume(";")) {
      left->lhs = expr();
      expect(";");
    }

    if (!consume(";")) {
      left->rhs = expr();
      expect(";");
    }

    if (!consume(")")) {
      right->lhs = expr();
      expect(")");
    }

    right->rhs = stmt();

    node->lhs = left;
    node->rhs = right;

    return node;
  }

  if (consume_return()) {
    node = new_node(ND_RETURN, expr(), NULL);
  } else {
    node = expr();
  }

  expect(";");
  return node;
}

Node *expr() { return assign(); }

Node *assign() {
  Node *node = equality();
  if (consume("=")) node = new_node(ND_ASSIGN, node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();
  for (;;) {
    if (consume("==")) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume("!=")) {
      node = new_node(ND_NE, node, relational());
    } else {
      return node;
    }
  }
}

Node *relational() {
  Node *node = add();
  for (;;) {
    if (consume("<=")) {
      node = new_node(ND_LE, node, add());
    } else if (consume("<")) {
      node = new_node(ND_LT, node, add());
    } else if (consume(">")) {
      node = new_node(ND_LT, add(), node);
    } else if (consume(">=")) {
      node = new_node(ND_LE, add(), node);
    } else {
      return node;
    }
  }
}

Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume("+")) {
      node = new_node(ND_ADD, node, mul());
    } else if (consume("-")) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

Node *mul() {
  Node *node = unary();
  for (;;) {
    if (consume("*")) {
      node = new_node(ND_MUL, node, unary());
    } else if (consume("/")) {
      node = new_node(ND_DIV, node, unary());
    } else {
      return node;
    }
  }
}

Node *unary() {
  if (consume("+")) return unary();
  if (consume("-")) return new_node(ND_SUB, new_node_num(0), unary());
  return primary();
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);

    if (lvar) {
      node->offset = lvar->offset;
    } else {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      if (locals == NULL) {
        lvar->offset = 8;
      } else {
        lvar->offset = locals->offset + 8;
      }
      node->offset = lvar->offset;
      locals = lvar;
    }
    return node;
  }

  return new_node_num(expect_number());
}