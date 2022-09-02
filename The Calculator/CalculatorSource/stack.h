// by stevenso aka v3ssel
#ifndef STACK_H
#define STACK_H

#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_ERR_CODE 301
#define isNumber(x) ((x) >= '0' && (x) <= '9')
#define isPrevNonOp(x) ((x) >= 2 && (x) <= 9)
#define isRight(x) (((x) >= 'A' && (x) <= 'Z') || ((x) >= 'a' && (x) <= 'z'))

enum types {
  NUMBER,
  X,
  OPEN_BRACKET,
  CLOSED_BRACKET,
  PLUS,
  MINUS,
  MULT,
  DIV,
  MOD,
  POWER,
  SQRT,
  COS,
  SIN,
  TAN,
  ACOS,
  ASIN,
  ATAN,
  LN,
  LOG
};

typedef struct stack_s {
  long double value;
  int64_t priority;
  int64_t type;
  struct stack_s *next;
} stack;

_Bool entryPoint(char *exp, long double *value, long double x);
void pushUp(stack **node, long double value, int64_t priority, int64_t type);
void popUp(stack **node);
void stackReverse(stack **node, stack **reversed);
char *remove_spaces(char *exp);
void getFullNumber(char *exp, size_t *i, stack **node, size_t len);
_Bool __parser(char *exp, stack **node, long double x);
void hardCases(char caseType, const char *exp, stack **node, size_t *i,
               size_t *func_cnt, _Bool *err_code);
void easyCases(char, const char *, stack **, size_t *, size_t *, _Bool *,
               long double x);
void plusMinusMultDiv(stack **node, const char *exp, const size_t *i,
                      _Bool *err_code, short sign);
void lastChecker(stack **, size_t, size_t, size_t, _Bool *);
void shuntingYard(stack **node, stack **output);
void calcus(stack **node, stack **calculated);

void calc_monthly(long double sum, int64_t, double, _Bool, double *, double *,
                  double *);
void calc_loan(long double sum, int64_t, double, double *, double *, double *);
void calc_term(long double sum, double, double, int64_t *, double *, double *);

void deposit_calc(long double amount, int64_t, _Bool, int64_t *, size_t, size_t,
                  long double *, long double taxRate, short, _Bool, int64_t *,
                  long double *, long double *, long double *, long double *);

#endif  //  STACK_H
