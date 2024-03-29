#include "50cc.h"

void gen_lval(Node *node) {
  if (node->kind == ND_DEREF) {
    gen(node->lhs);
    return;
  }
  if (node->kind != ND_LVAR) error("代入の左辺値が変数ではありません");
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

int genId = 0;
char *argRegs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen(Node *node) {
  if (!node) return;
  genId += 1;
  int id = genId;
  int argCount = 0;
  switch (node->kind) {
    case ND_NUM:
      printf("  push %d\n", node->val);
      return;
    case ND_LVAR:
      gen_lval(node);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;
    case ND_RETURN:
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      return;
    case ND_IF:
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lelse%d\n", id);
      if (node->rhs->kind == ND_ELSE) {
        gen(node->rhs->lhs);
      } else {
        gen(node->rhs);
      }
      printf("  jmp .Lend%d\n", id);
      printf(".Lelse%d:\n", id);
      if (node->rhs->kind == ND_ELSE) {
        gen(node->rhs->rhs);
      }
      printf(".Lend%d:\n", id);
      return;
    case ND_WHILE:
      printf(".Lbegin%d:\n", id);
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .Lend%d\n", id);
      gen(node->rhs);
      printf("  jmp .Lbegin%d\n", id);
      printf(".Lend%d:\n", id);
      return;
    case ND_FOR:
      gen(node->lhs->lhs);
      printf(".Lbegin%d:\n", id);
      gen(node->lhs->rhs);
      if (!node->lhs->rhs) {
        printf("  push 1\n");
      }
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je  .Lend%d\n", id);
      gen(node->rhs->rhs);
      gen(node->rhs->lhs);
      printf("  jmp .Lbegin%d\n", id);
      printf(".Lend%d:\n", id);
      return;
    case ND_BLOCK:
      for (int i = 0; node->block[i]; i++) {
        gen(node->block[i]);
      }
      return;
    case ND_FUNC_CALL:
      for (int i = 0; node->block[i]; i++) {
        gen(node->block[i]);
        argCount++;
      }
      for (int i = argCount - 1; i >= 0; i--) {
        printf("  pop %s\n", argRegs[i]);
      }
      printf("  mov rax, rsp\n");
      printf("  and rax, 15\n");
      printf("  jnz .L.call.%d\n", id);
      printf("  mov rax, 0\n");
      printf("  call %s\n", node->funcname);
      printf("  jmp .L.end.%d\n", id);
      printf(".L.call.%d:\n", id);
      printf("  sub rsp, 8\n");
      printf("mov rax, 0\n");
      printf("  call %s\n", node->funcname);
      printf("  add rsp, 8\n");
      printf(".L.end.%d:\n", id);
      printf("  push rax\n");
      return;
    case ND_FUNC_DEF:
      printf("%s:\n", node->funcname);
      printf("  push rbp\n");
      printf("  mov rbp, rsp\n");
      for (int i = 0; node->args[i]; i++) {
        printf("push %s\n", argRegs[i]);
        argCount++;
      }

      if (locals[cur_func]) {
        int offset = locals[cur_func][0].offset;
        offset -= argCount * 8;
        printf("  sub rsp, %d\n", offset);
      }

      gen(node->lhs);
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      return;
    case ND_ADDR:
      gen_lval(node->lhs);
      return;
    case ND_DEREF:
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NE:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
  }
  printf("  push rax\n");
}