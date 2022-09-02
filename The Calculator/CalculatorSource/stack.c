// by stevenso aka v3ssel
#include "stack.h"

_Bool entryPoint(char *exp, long double *value, long double x) {
  _Bool return_value = true;
  stack *node = NULL;
  *value = 0.0;
  char *new_exp = exp;
  if (exp) new_exp = remove_spaces(exp);
  if (__parser(new_exp, &node, x)) {
    stack *tmp = NULL;
    stackReverse(&node, &tmp);
    node = NULL;
    shuntingYard(&tmp, &node);
    tmp = NULL;
    stackReverse(&node, &tmp);
    node = NULL;
    calcus(&tmp, &node);
    *value = node->value;
    popUp(&node);
  } else {
    if (node) popUp(&node);
    return_value = false;
  }
  free(new_exp);
  return return_value;
}

void pushUp(stack **node, long double value, int64_t priority, int64_t type) {
  stack *tmp = malloc(sizeof(stack));
  tmp->value = value;
  tmp->priority = priority;
  tmp->next = *node;
  tmp->type = type;
  *node = tmp;
}

void popUp(stack **node) {
  stack *tmp = NULL;
  tmp = (*node)->next;
  free(*node);
  (*node) = tmp;
}

void doublePop(stack **node) {
  stack *tmp = NULL;
  tmp = (*node)->next->next->next;
  free((*node)->next->next);
  free((*node)->next);
  (*node)->next = tmp;
}

void stackReverse(stack **node, stack **reversed) {
  _Bool check = false;
  while (1) {
    pushUp(reversed, (*node)->value, (*node)->priority, (*node)->type);
    popUp(node);
    if (check) break;
    if (!(*node)->next) check = true;
  }
}

char *remove_spaces(char *exp) {
  char *return_exp = calloc(strlen(exp), sizeof(char));
  for (size_t i = 0, j = 0; i <= strlen(exp); i++) {
    if (exp[i] != ' ') return_exp[j++] = exp[i];
  }
  return return_exp;
}

void getFullNumber(char *exp, size_t *i, stack **node, size_t len) {
  char *num = NULL;
  num = (char *)malloc(sizeof(char));
  if (num) {
    size_t cnt = 0, dot_counter = 0;
    for (; *i < len; *i += 1) {
      if (exp[*i] == '.') dot_counter++;
      if (isNumber(exp[*i]) || exp[*i] == '.') {
        char *tmp = (char *)realloc(num, sizeof(char));
        if (tmp)
          num = tmp, num[cnt++] = exp[*i];
        else
          *i = NUMBER_ERR_CODE;
      } else {
        (*i)--;
        break;
      }
    }
    if (dot_counter > 1) {
      *i = NUMBER_ERR_CODE;
      free(num);
      return;
    }
    num[cnt] = '\0';
    pushUp(node, strtold(num, NULL), 0, NUMBER);
    free(num);
  } else {
    *i = NUMBER_ERR_CODE;
  }
}

_Bool __parser(char *exp, stack **node, long double x) {
  _Bool err_code = true;
  size_t rBrCn = 0, lBrCn = 0, func_cnt = 0;
  if (exp != NULL && *exp != '\0') {
    size_t iOfL = 0, iOfR = 0, len = strlen(exp);
    for (size_t i = 0; i < len; i++) {
      if (isNumber(exp[i])) {
        if (!i || (*node && ((*node)->type == OPEN_BRACKET ||
                             ((*node)->type >= 4 && (*node)->type <= 9)))) {
          getFullNumber(exp, &i, node, len);
        } else {
          err_code = false;
        }
      } else {
        switch (exp[i]) {
          case 'x':
            easyCases('x', exp, node, &i, &func_cnt, &err_code, x);
            break;
          case '(':
            lBrCn++;
            if (*node && (((*node)->type == NUMBER || (*node)->type == X)))
              pushUp(node, 0, 2, MULT);
            pushUp(node, 0, -1, OPEN_BRACKET);
            iOfL = i;
            break;
          case ')':
            rBrCn++;
            if (*node && (*node)->type == OPEN_BRACKET) err_code = false;
            pushUp(node, 0, -1, CLOSED_BRACKET);
            iOfR = i;
            break;
          case '+':
            plusMinusMultDiv(node, exp, &i, &err_code, 0);
            break;
          case '-':
            plusMinusMultDiv(node, exp, &i, &err_code, 1);
            break;
          case '*':
            plusMinusMultDiv(node, exp, &i, &err_code, 2);
            break;
          case '/':
            plusMinusMultDiv(node, exp, &i, &err_code, 3);
            break;
          case 'm':
            easyCases('m', exp, node, &i, &func_cnt, &err_code, 0);
            break;
          case '^':
            easyCases('^', exp, node, &i, &func_cnt, &err_code, 0);
            break;
          case 's':
            hardCases('s', exp, node, &i, &func_cnt, &err_code);
            break;
          case 'c':
            easyCases('c', exp, node, &i, &func_cnt, &err_code, 0);
            break;
          case 't':
            easyCases('t', exp, node, &i, &func_cnt, &err_code, 0);
            break;
          case 'a':
            hardCases('a', exp, node, &i, &func_cnt, &err_code);
            break;
          case 'l':
            hardCases('l', exp, node, &i, &func_cnt, &err_code);
            break;
          default:
            err_code = false;
            break;
        }
      }
      if (iOfR < iOfL && (lBrCn + rBrCn) % 2 == 0 && (lBrCn == rBrCn))
        err_code = false;
      if (i == NUMBER_ERR_CODE) err_code = false;
      if (!err_code) break;
    }
    pushUp(node, 0, 1, PLUS), pushUp(node, 0, 0, NUMBER);
  } else {
    err_code = false;
  }
  lastChecker(node, lBrCn, rBrCn, func_cnt, &err_code);
  return err_code;
}

void plusMinusMultDiv(stack **node, const char *exp, const size_t *i,
                      _Bool *err_code, short sign) {
  if (sign < 2) {
    if (((*node && !((*node)->type >= 4 && (*node)->type <= 9)) || !(*i)) &&
        exp[*i + 1] != '\0') {
      if (!(*i) || exp[*i - 1] == '(') pushUp(node, 0, 0, NUMBER);

      if (sign)
        pushUp(node, 0, 1, MINUS);
      else
        pushUp(node, 0, 1, PLUS);
    } else {
      *err_code = false;
    }
  } else {
    if (*node &&
        (!isPrevNonOp((*node)->type) || (*node)->type == CLOSED_BRACKET) &&
        exp[*i + 1] != '\0') {
      if (sign == 2)
        pushUp(node, 0, 2, MULT);
      else
        pushUp(node, 0, 2, DIV);
    } else {
      *err_code = false;
    }
  }
}

void lastChecker(stack **node, size_t lBrCn, size_t rBrCn, size_t func_cnt,
                 _Bool *err_code) {
  if (lBrCn != rBrCn ||
      (*node && ((*node)->type != CLOSED_BRACKET && (*node)->type != NUMBER &&
                 (*node)->type != X)) ||
      ((func_cnt * 2) > (lBrCn + rBrCn)))
    *err_code = false;
}

void easyCases(char caseType, const char *exp, stack **node, size_t *i,
               size_t *func_cnt, _Bool *err_code, long double x) {
  if (caseType == 'c') {
    if (exp[*i + 1] == 'o' && exp[*i + 2] == 's') {
      if (*node && ((*node)->type == NUMBER)) pushUp(node, 0, 2, MULT);
      pushUp(node, 0, 4, COS);
      *i += 2;
      (*func_cnt)++;
    } else {
      *err_code = false;
    }
  }
  if (caseType == 't') {
    if (exp[*i + 1] == 'a' && exp[*i + 2] == 'n') {
      if (*node && (*node)->type == NUMBER) pushUp(node, 0, 2, MULT);
      pushUp(node, 0, 4, TAN);
      *i += 2;
      (*func_cnt)++;
    } else {
      *err_code = false;
    }
  }
  if (caseType == 'x') {
    if ((!isRight(exp[*i + 1]) && !isRight(exp[*i - 1])) ||
        exp[*i + 1] == 'm' || exp[*i - 1] == 'd') {
      if ((*node) && (((*node)->type == NUMBER || (*node)->type == X)))
        pushUp(node, 0, 2, MULT);
      pushUp(node, x, 0, X);
    } else {
      *err_code = false;
    }
  }
  if (caseType == '^') {
    if (*node &&
        (!isPrevNonOp((*node)->type) || (*node)->type == CLOSED_BRACKET) &&
        exp[*i + 1] != '\0')
      pushUp(node, 0, 3, POWER);
    else
      *err_code = false;
  }
  if (caseType == 'm') {
    if (exp[*i + 1] == 'o' && exp[*i + 2] == 'd' && *node &&
        exp[*i + 1] != '\0')
      pushUp(node, 0, 2, MOD);
    else
      *err_code = false;
    *i += 2;
  }
}

void hardCases(char caseType, const char *exp, stack **node, size_t *i,
               size_t *func_cnt, _Bool *err_code) {
  if (caseType == 'l') {
    if (exp[*i + 1] == 'n') {
      if (*node && (*node)->type == NUMBER) pushUp(node, 0, 2, MULT);
      pushUp(node, 0, 4, LN);
      *i += 1;
      func_cnt++;
    } else if (exp[*i + 1] == 'o' && exp[*i + 2] == 'g') {
      if (*node && (*node)->type == NUMBER) pushUp(node, 0, 2, MULT);
      pushUp(node, 0, 4, LOG);
      *i += 2;
      (*func_cnt)++;
    } else {
      *err_code = false;
    }
  }
  if (caseType == 'a') {
    if (exp[*i + 1] == 's' && exp[*i + 2] == 'i' && exp[*i + 3] == 'n') {
      if (*node && (*node)->type == NUMBER) pushUp(node, 0, 2, MULT);
      pushUp(node, 0, 4, ASIN);
      *i += 3;
      (*func_cnt)++;
    } else if (exp[*i + 1] == 'c' && exp[*i + 2] == 'o' && exp[*i + 3] == 's') {
      if (*node && (*node)->type == NUMBER) pushUp(node, 0, 2, MULT);
      pushUp(node, 0, 4, ACOS);
      *i += 3;
      (*func_cnt)++;
    } else if (exp[*i + 1] == 't' && exp[*i + 2] == 'a' && exp[*i + 3] == 'n') {
      if (*node && (*node)->type == NUMBER) pushUp(node, 0, 2, MULT);
      pushUp(node, 0, 4, ATAN);
      *i += 3;
      (*func_cnt)++;
    } else {
      *err_code = false;
    }
  }
  if (caseType == 's') {
    if (exp[*i + 1] == 'i' && exp[*i + 2] == 'n') {
      if (*node && (*node)->type == NUMBER) pushUp(node, 0, 2, MULT);
      pushUp(node, 0, 4, SIN);
      *i += 2;
      (*func_cnt)++;
    } else if (exp[*i + 1] == 'q' && exp[*i + 2] == 'r' && exp[*i + 3] == 't') {
      pushUp(node, 0, 4, SQRT);
      *i += 3;
      (*func_cnt)++;
    } else {
      *err_code = false;
    }
  }
}

void shuntingYard(stack **node, stack **output) {
  _Bool check = false;
  stack *operators = NULL;
  while (1) {
    if (*node) {
      if ((*node)->type == CLOSED_BRACKET) {
        popUp(node);
        while (operators->type != OPEN_BRACKET) {
          pushUp(output, operators->value, operators->priority,
                 operators->type);
          popUp(&operators);
        }
        popUp(&operators);
        if (operators && operators->priority == 4) {
          pushUp(output, operators->value, operators->priority,
                 operators->type);
          popUp(&operators);
        }
      } else {
        if ((*node)->type == NUMBER || (*node)->type == X) {
          pushUp(output, (*node)->value, (*node)->priority, (*node)->type);
          popUp(node);
        } else if ((*node)->type != NUMBER && (*node)->type != CLOSED_BRACKET) {
          while (operators) {
            if ((*node)->priority != -1 &&
                (((*node)->priority == 3 &&
                  (*node)->priority < operators->priority) ||
                 ((*node)->priority != 3 &&
                  (*node)->priority <= operators->priority))) {
              pushUp(output, operators->value, operators->priority,
                     operators->type);
              popUp(&operators);
            } else {
              break;
            }
          }
          pushUp(&operators, (*node)->value, (*node)->priority, (*node)->type);
          popUp(node);
        }
      }
    }
    if (!(*node) || check) break;
    if (!(*node)->next) check = true;
  }
  if (operators) {
    check = false;
    while (1) {
      pushUp(output, operators->value, operators->priority, operators->type);
      popUp(&operators);
      if (!operators || check) break;
      if (!operators->next) check = true;
    }
  }
}

void calcus(stack **node, stack **calculated) {
  _Bool check = false;
  while (1) {
    switch ((*node)->type) {
      case PLUS:
        popUp(node);
        pushUp(calculated, (*calculated)->next->value + (*calculated)->value, 0,
               NUMBER);
        doublePop(calculated);
        break;
      case MINUS:
        popUp(node);
        pushUp(calculated, (*calculated)->next->value - (*calculated)->value, 0,
               NUMBER);
        doublePop(calculated);
        break;
      case MULT:
        popUp(node);
        pushUp(calculated, (*calculated)->next->value * (*calculated)->value, 0,
               NUMBER);
        doublePop(calculated);
        break;
      case DIV:
        popUp(node);
        pushUp(calculated, (*calculated)->next->value / (*calculated)->value, 0,
               NUMBER);
        doublePop(calculated);
        break;
      case MOD:
        popUp(node);
        pushUp(calculated,
               fmodl((*calculated)->next->value, (*calculated)->value), 0,
               NUMBER);
        doublePop(calculated);
        break;
      case POWER:
        popUp(node);
        pushUp(calculated,
               powl((*calculated)->next->value, (*calculated)->value), 0,
               NUMBER);
        doublePop(calculated);
        break;
      case SQRT:
        popUp(node);
        pushUp(calculated, sqrtl((*calculated)->value), 0, NUMBER);
        popUp(&(*calculated)->next);
        break;
      case COS:
        popUp(node);
        pushUp(calculated, cosl((*calculated)->value), 0, NUMBER);
        popUp(&(*calculated)->next);
        break;
      case SIN:
        popUp(node);
        pushUp(calculated, sinl((*calculated)->value), 0, NUMBER);
        popUp(&(*calculated)->next);
        break;
      case TAN:
        popUp(node);
        pushUp(calculated, tanl((*calculated)->value), 0, NUMBER);
        popUp(&(*calculated)->next);
        break;
      case ACOS:
        popUp(node);
        pushUp(calculated, acosl((*calculated)->value), 0, NUMBER);
        popUp(&(*calculated)->next);
        break;
      case ASIN:
        popUp(node);
        pushUp(calculated, asinl((*calculated)->value), 0, NUMBER);
        popUp(&(*calculated)->next);
        break;
      case ATAN:
        popUp(node);
        pushUp(calculated, atanl((*calculated)->value), 0, NUMBER);
        popUp(&(*calculated)->next);
        break;
      case LN:
        popUp(node);
        pushUp(calculated, logl((*calculated)->value), 0, NUMBER);
        popUp(&(*calculated)->next);
        break;
      case LOG:
        popUp(node);
        pushUp(calculated, log10l((*calculated)->value), 0, NUMBER);
        popUp(&(*calculated)->next);
        break;
      default:
        break;
    }
    if (*node && ((*node)->type == NUMBER || (*node)->type == X)) {
      pushUp(calculated, (*node)->value, (*node)->priority, (*node)->type);
      popUp(node);
    }
    if (!(*node) || check) break;
    if (!(*node)->next) check = true;
  }
}
