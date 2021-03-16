#define MAIN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define OP_END     0
#define OP_MUL     1
#define OP_DIV     2
#define OP_ADD     3
#define OP_SUB     4
#define OP_LT      5
#define OP_GT      6
#define OP_LTE     7
#define OP_GTE     9
#define OP_EQ     10
#define OP_NE     11
#define OP_AND    12
#define OP_OR     13
#define OP_XOR    14
#define OP_OP     15
#define OP_CP     16

void show_tokens() {
  int i;
  for (i=0; i<numTokens; i++)
    printf("%d   %d\n",tokenTypes[i], tokens[i]);
  printf("-------\n");
  }

char* trim(char* line) {
  while (*line == ' ' || *line == '\t') line++;
  return line;
  }

int check_label(char* label,char *line) {
  line = trim(line);
  while (*label == *line) {
    label++;
    line++;
    }
  if (*label != ' ' && *label != 0) return 0;
  if (*line != ' ' && *line != 0) return 0;
  return -1;
  }

char* getVariable(char* line,char* var) {
  while ( (*line >= 'a' && *line <= 'z') ||
          (*line >= 'A' && *line <= 'Z') ||
          (*line >= '0' && *line <= '9') ||
          *line == '_') *var++ = *line++;
  *var = 0;
  return line;
  }

int getIntegerVar(char* var) {
  int i;
  for (i=0; i<numIntegerVars; i++)
    if (strcasecmp(integerVars[i], var) == 0) {
      return integerValues[i];
      }
  printf("Variable not found: %s\n",var);
  exit(1);
  }

void setIntegerVar(char* var, int value) {
  int i;
  for (i=0; i<numIntegerVars; i++) {
    if (strcasecmp(integerVars[i], var) == 0) {
      integerValues[i] = value;
      return;
      }
    }
  numIntegerVars++;
  if (numIntegerVars == 1) {
    integerVars = (char**)malloc(sizeof(char*));
    integerValues = (int*)malloc(sizeof(int));
    }
  else {
    integerVars = (char**)realloc(integerVars,sizeof(char*) * numIntegerVars);
    integerValues = (int*)realloc(integerValues,sizeof(int) * numIntegerVars);
    }
  integerVars[numIntegerVars-1] = (char*)malloc(sizeof(var)+1);
  strcpy(integerVars[numIntegerVars-1], var);
  integerValues[numIntegerVars-1] = value;
  }

char* getStringVar(char* var) {
  int i;
  for (i=0; i<numStringVars; i++)
    if (strcasecmp(stringVars[i], var) == 0) {
      return stringValues[i];
      }
  printf("Variable not found: %s\n",var);
  exit(1);
  }

void setStringVar(char* var, char* value) {
  int i;
  for (i=0; i<numStringVars; i++) {
    if (strcasecmp(stringVars[i], var) == 0) {
      free(stringValues[i]);
      stringValues[i] = (char*)malloc(strlen(value) + 1);
      strcpy(stringValues[i],value); 
      return;
      }
    }
  numStringVars++;
  if (numStringVars == 1) {
    stringVars = (char**)malloc(sizeof(char*));
    stringValues = (char**)malloc(sizeof(char*));
    }
  else {
    stringVars = (char**)realloc(stringVars,sizeof(char*) * numStringVars);
    stringValues = (char**)realloc(stringValues,sizeof(char*) * numStringVars);
    }
  stringVars[numStringVars-1] = (char*)malloc(sizeof(var)+1);
  stringValues[numStringVars-1] = (char*)malloc(sizeof(value)+1);
  strcpy(stringVars[numStringVars-1], var);
  strcpy(stringValues[numStringVars-1], value);
  }

void tokenize(char* line, int start, int end) {
  char token[64];
  int  p;
  numTokens = 0;
  while (start <= end) {
    if (line[start] >= '0' && line[start] <= '9') {
      tokenTypes[numTokens] = ' ';
      tokens[numTokens] = 0;
      while (line[start] >= '0' && line[start] <= '9') {
        tokens[numTokens] *= 10;
        tokens[numTokens] += (line[start++] - '0');
        }
      numTokens++;
      }
    else if (line[start] == '#' ||
             (line[start] >= 'a' && line[start] <= 'z') ||
             (line[start] >= 'A' && line[start] <= 'Z')) {
      if (line[start] == '#') start++;
      p = 0;
      while ((line[start] >= 'a' && line[start] <= 'z') ||
             (line[start] >= 'A' && line[start] <= 'Z') ||
             (line[start] >= '0' && line[start] <= '9') ||
             line[start] == '_') {
        token[p++] = line[start++];
        }
      token[p] = 0;
      tokenTypes[numTokens] = ' ';
      tokens[numTokens] = getIntegerVar(token);
      numTokens++;
      }
    else if (line[start] == '*') { tokenTypes[numTokens++] = OP_MUL; start++; }
    else if (line[start] == '/') { tokenTypes[numTokens++] = OP_DIV; start++; }
    else if (line[start] == '+') { tokenTypes[numTokens++] = OP_ADD; start++; }
    else if (line[start] == '-') { tokenTypes[numTokens++] = OP_SUB; start++; }
    else if (line[start] == '&') { tokenTypes[numTokens++] = OP_AND; start++; }
    else if (line[start] == '|') { tokenTypes[numTokens++] = OP_OR; start++; }
    else if (line[start] == '^') { tokenTypes[numTokens++] = OP_XOR; start++; }
    else if (line[start] == '(') { tokenTypes[numTokens++] = OP_OP; start++; }
    else if (line[start] == ')') { tokenTypes[numTokens++] = OP_CP; start++; }
    else if (line[start] == '<' && line[start+1] == '=')
      { tokenTypes[numTokens++] = OP_LTE; start+=2; }
    else if (line[start] == '<' && line[start+1] == '>')
      { tokenTypes[numTokens++] = OP_NE; start+=2; }
    else if (line[start] == '>' && line[start+1] == '=')
      { tokenTypes[numTokens++] = OP_GTE; start+=2; }
    else if (line[start] == '<') { tokenTypes[numTokens++] = OP_LT; start++; }
    else if (line[start] == '>') { tokenTypes[numTokens++] = OP_GT; start++; }
    else if (line[start] == '=') { tokenTypes[numTokens++] = OP_EQ; start++; }
    else if (line[start] == ' ') start++;
    else {
      printf("Expression error: %s\n",program[pc]);
      exit(1);
      }
    }
  tokenTypes[numTokens++] = OP_END;
  }

int evaluate(int start) {
  int i,j;
  int op,cp;
  int flag;
  flag = 1;
  while (flag) {
    flag = 0;
    i = start;
    op = -1;
    while (tokenTypes[i] != OP_END && tokenTypes[i] != OP_CP) {
      if (tokenTypes[i] == OP_OP) op = i;
      i++;
      }
    if (tokenTypes[i] == OP_CP && op >= 0) {
      cp = i;
      if (op < 0) {
        printf("Expression error: %s\n",program[pc]);
        exit(1);
        }
      evaluate(op+1);
      tokenTypes[op] = tokenTypes[op+1];
      tokens[op] = tokens[op+1];
      op++;
      cp = op;
      while (tokenTypes[cp] != OP_CP) cp++;
      cp++;
      while (tokenTypes[op] != OP_END) {
        tokenTypes[op] = tokenTypes[cp];
        tokens[op] = tokens[cp];
        op++;
        cp++;
        }
      tokenTypes[op] = OP_END;
      numTokens -= 2;
      flag = -1;
      i = 0;
      }
    }

  i = start;
  while (tokenTypes[i] != OP_END && tokenTypes[i] != OP_CP) {
    if (tokenTypes[i] == OP_SUB) {
      if (tokenTypes[i+1] == OP_END) {
        printf("Expression error: %s\n",program[pc]);
        exit(1);
        }
      if ((i == start || tokenTypes[i-1] != ' ') && tokenTypes[i+1] == ' ') {
        tokens[i] = -tokens[i+1];
        tokenTypes[i] = ' ';
        for (j=i+1; j<numTokens-1; j++) {
          tokenTypes[j] = tokenTypes[j+1];
          tokens[j] = tokens[j+1];
          }
        numTokens -= 1;
        }
      else i++;
      }
    else i++;
    }

  i = start;
  while (tokenTypes[i] != OP_END && tokenTypes[i] != OP_CP) {
    if (tokenTypes[i] == OP_MUL) {
      if (i == start || tokenTypes[i-1] != ' ' || tokenTypes[i+1] != ' ') {
        printf("Expression error: %s\n",program[pc]);
        exit(1);
        }
      tokens[i-1] = tokens[i-1] * tokens[i+1];
      i--;
      for (j=i+1; j<numTokens-2; j++) {
        tokenTypes[j] = tokenTypes[j+2];
        tokens[j] = tokens[j+2];
        }
      numTokens -= 2;
      }
    else if (tokenTypes[i] == OP_DIV) {
      if (i == start || tokenTypes[i-1] != ' ' || tokenTypes[i+1] != ' ') {
        printf("Expression error: %s\n",program[pc]);
        exit(1);
        }
      tokens[i-1] = tokens[i-1] / tokens[i+1];
      i--;
      for (j=i+1; j<numTokens-2; j++) {
        tokenTypes[j] = tokenTypes[j+2];
        tokens[j] = tokens[j+2];
        }
      numTokens -= 2;
      }
    else i++;
    }

  i = start;
  while (tokenTypes[i] != OP_END && tokenTypes[i] != OP_CP) {
    if (tokenTypes[i] == OP_ADD) {
      if (i == start || tokenTypes[i-1] != ' ' || tokenTypes[i+1] != ' ') {
        printf("Expression error: %s\n",program[pc]);
        exit(1);
        }
      tokens[i-1] = tokens[i-1] + tokens[i+1];
      i--;
      for (j=i+1; j<numTokens-2; j++) {
        tokenTypes[j] = tokenTypes[j+2];
        tokens[j] = tokens[j+2];
        }
      numTokens -= 2;
      }
    else if (tokenTypes[i] == OP_SUB) {
      if (i == start || tokenTypes[i-1] != ' ' || tokenTypes[i+1] != ' ') {
        printf("Expression error: %s\n",program[pc]);
        exit(1);
        }
      tokens[i-1] = tokens[i-1] - tokens[i+1];
      i--;
      for (j=i+1; j<numTokens-2; j++) {
        tokenTypes[j] = tokenTypes[j+2];
        tokens[j] = tokens[j+2];
        }
      numTokens -= 2;
      }
    else i++;
    }

  i = start;
  while (tokenTypes[i] != OP_END && tokenTypes[i] != OP_CP) {
    if (tokenTypes[i] == OP_LT) {
      if (i == start || tokenTypes[i-1] != ' ' || tokenTypes[i+1] != ' ') {
        printf("Expression error: %s\n",program[pc]);
        exit(1);
        }
      tokens[i-1] = (tokens[i-1] < tokens[i+1]) ? -1 : 0;
      i--;
      for (j=i+1; j<numTokens-2; j++) {
        tokenTypes[j] = tokenTypes[j+2];
        tokens[j] = tokens[j+2];
        }
      numTokens -= 2;
      }
    else if (tokenTypes[i] == OP_GT) {
      if (i == start || tokenTypes[i-1] != ' ' || tokenTypes[i+1] != ' ') {
        printf("Expression error: %s\n",program[pc]);
        exit(1);
        }
      tokens[i-1] = (tokens[i-1] > tokens[i+1]) ? -1 : 0;
      i--;
      for (j=i+1; j<numTokens-2; j++) {
        tokenTypes[j] = tokenTypes[j+2];
        tokens[j] = tokens[j+2];
        }
      numTokens -= 2;
      }
    if (tokenTypes[i] == OP_LTE) {
      if (i == start || tokenTypes[i-1] != ' ' || tokenTypes[i+1] != ' ') {
        printf("Expression error: %s\n",program[pc]);
        exit(1);
        }
      tokens[i-1] = (tokens[i-1] <= tokens[i+1]) ? -1 : 0;
      i--;
      for (j=i+1; j<numTokens-2; j++) {
        tokenTypes[j] = tokenTypes[j+2];
        tokens[j] = tokens[j+2];
        }
      numTokens -= 2;
      }
    else if (tokenTypes[i] == OP_GTE) {
      if (i == start || tokenTypes[i-1] != ' ' || tokenTypes[i+1] != ' ') {
        printf("Expression error: %s\n",program[pc]);
        exit(1);
        }
      tokens[i-1] = (tokens[i-1] >= tokens[i+1]) ? -1 : 0;
      i--;
      for (j=i+1; j<numTokens-2; j++) {
        tokenTypes[j] = tokenTypes[j+2];
        tokens[j] = tokens[j+2];
        }
      numTokens -= 2;
      }
    if (tokenTypes[i] == OP_EQ) {
      if (i == start || tokenTypes[i-1] != ' ' || tokenTypes[i+1] != ' ') {
        printf("Expression error: %s\n",program[pc]);
        exit(1);
        }
      tokens[i-1] = (tokens[i-1] == tokens[i+1]) ? -1 : 0;
      i--;
      for (j=i+1; j<numTokens-2; j++) {
        tokenTypes[j] = tokenTypes[j+2];
        tokens[j] = tokens[j+2];
        }
      numTokens -= 2;
      }
    else if (tokenTypes[i] == OP_NE) {
      if (i == start || tokenTypes[i-1] != ' ' || tokenTypes[i+1] != ' ') {
        printf("Expression error: %s\n",program[pc]);
        exit(1);
        }
      tokens[i-1] = (tokens[i-1] != tokens[i+1]) ? -1 : 0;
      i--;
      for (j=i+1; j<numTokens-2; j++) {
        tokenTypes[j] = tokenTypes[j+2];
        tokens[j] = tokens[j+2];
        }
      numTokens -= 2;
      }
    else i++;
    }

  i = start;
  while (tokenTypes[i] != OP_END && tokenTypes[i] != OP_CP) {
    if (tokenTypes[i] == OP_AND) {
      if (i == start || tokenTypes[i-1] != ' ' || tokenTypes[i+1] != ' ') {
        printf("Expression error: %s\n",program[pc]);
        exit(1);
        }
      tokens[i-1] = tokens[i-1] & tokens[i+1];
      i--;
      for (j=i+1; j<numTokens-2; j++) {
        tokenTypes[j] = tokenTypes[j+2];
        tokens[j] = tokens[j+2];
        }
      numTokens -= 2;
      }
    else if (tokenTypes[i] == OP_OR) {
      if (i == start || tokenTypes[i-1] != ' ' || tokenTypes[i+1] != ' ') {
        printf("Expression error: %s\n",program[pc]);
        exit(1);
        }
      tokens[i-1] = tokens[i-1] | tokens[i+1];
      i--;
      for (j=i+1; j<numTokens-2; j++) {
        tokenTypes[j] = tokenTypes[j+2];
        tokens[j] = tokens[j+2];
        }
      numTokens -= 2;
      }
    else if (tokenTypes[i] == OP_XOR) {
      if (i == start || tokenTypes[i-1] != ' ' || tokenTypes[i+1] != ' ') {
        printf("Expression error: %s\n",program[pc]);
        exit(1);
        }
      tokens[i-1] = tokens[i-1] ^ tokens[i+1];
      i--;
      for (j=i+1; j<numTokens-2; j++) {
        tokenTypes[j] = tokenTypes[j+2];
        tokens[j] = tokens[j+2];
        }
      numTokens -= 2;
      }
    else i++;
    }
  return tokens[0];
  }


int find_label(char* line) {
  int i;
  line = trim(line);
  if (*line != '*') {
    printf("Invalid label: %s\n",line);
    exit(1);
    }
  for (i=0; i<numLines; i++) {
    if (check_label(line,program[i])) return i;
    }
  printf("Label not found: %s\n",line);
  exit(1);
  }

void command_a(char* line) {
  char var[32];
  line = trim(line);
  if (strlen(line) == 0) {
    printf("? ");
    fgets(acceptBuffer,255,stdin);
    return;
    }
  while (*line != 0) {
    if (*line == '$') {
      line++;
      line = getVariable(line, var);
      printf("? ");
      fgets(acceptBuffer,255,stdin);
      if (strlen(acceptBuffer) > 0) {
        while (acceptBuffer[strlen(acceptBuffer)-1] == 0x0a ||
               acceptBuffer[strlen(acceptBuffer)-1] == 0x0c)
          acceptBuffer[strlen(acceptBuffer)-1] = 0x00;
        }
      setStringVar(var, acceptBuffer);
      }
    else if (*line == '#' || (*line >= 'a' && *line <= 'z') ||
                             (*line >= 'A' && *line <= 'Z')) {
      if (*line == '#') line++;
      line = getVariable(line, var);
      printf("? ");
      fgets(acceptBuffer,255,stdin);
      if (strlen(acceptBuffer) > 0) {
        while (acceptBuffer[strlen(acceptBuffer)-1] == 0x0a ||
               acceptBuffer[strlen(acceptBuffer)-1] == 0x0c)
          acceptBuffer[strlen(acceptBuffer)-1] = 0x00;
        }
      setIntegerVar(var, atoi(acceptBuffer));
      }
    else {
      printf("Syntax error: %s\n",program[pc]);
      exit(1);
      }
    line = trim(line);
    if (*line == ',') line++;
    else if (*line != 0) {
      printf("Syntax error: %s\n",program[pc]);
      exit(1);
      }
    }
  }

void command_c(char* line) { 
  int i;
  int start;
  int end;
  char var[32];
  line = trim(line);
  if (*line == '#' ||
      (*line >= 'a' && *line <= 'z') ||
      (*line >= 'A' && *line <= 'Z')) {
    if (*line == '#') line++;
    i = 0;
    while ((*line >= 'a' && *line <= 'z') ||
           (*line >= 'A' && *line <= 'Z') ||
           (*line >= '0' && *line <= '9') ||
           *line == '_') var[i++] = *line++;
    var[i] = 0;
    }
  else {
    printf("Syntax error: %s\n",program[pc]);
    exit(1);
    }
  start = -1;
  end = -1;
  i = 0;
  while (line[i] != 0 && line[i] != '=') i++;
  if (line[i] == 0) {
    printf("Expression error: %s\n",line);
    exit(1);
    }
  start = i+1;
  end = start;
  while (line[end] != 0) end++;
  end--;
  tokenize(line,start,end);
  evaluate(0);
  if (tokenTypes[0] != ' ' || tokenTypes[1] != OP_END) {
    printf("Expression error: %s\n",program[pc]);
    exit(1);
    }
  setIntegerVar(var, tokens[0]);
  }

void command_j(char* line) {
  int i;
  line = trim(line);
  jumped = -1;
  pc = find_label(line);
  }

void command_m(char* line) {
  int  i;
  char *pchar;
  char match[128];
  char var[32];
  char left[128];
  char right[128];
  char middle[128];
  line = trim(line);
  while (*line != 0) {
    if (*line == '$') {
      i = 0;
      while (*line != ',' && *line != 0) var[i++] = *line++;
      var[i] = 0;
      strcpy(match, getStringVar(var));
      }
    else {
      i = 0;
      while (*line != ',' && *line != 0) match[i++] = *line++;
      match[i] = 0;
      }
    pchar = strstr(acceptBuffer, match);
    if (pchar != NULL) {
      strncpy(middle,pchar,strlen(match));
      middle[strlen(match)] = 0;
      strncpy(left,acceptBuffer,(pchar-acceptBuffer));
      left[pchar-acceptBuffer] = 0;
      strcpy(right,pchar+strlen(match));
      setStringVar("match",middle);
      setStringVar("right",right);
      setStringVar("left",left);
      matched = -1;
      return;
      }
    line = trim(line);
    if (*line == ',') {
      line++;
      line = trim(line);
      }
    }
  matched = 0;
  }

void command_t(char* line) {
  char var[32];
  while (*line != 0) {
    if (*line == '$') {
      line++;
      line = getVariable(line,var);
      printf("%s",getStringVar(var));
      }
    if (*line == '#') {
      line++;
      line = getVariable(line,var);
      printf("%d",getIntegerVar(var));
      }
    else if (*line == '\\') {
      return;
      }
    else {
      printf("%c",*line++);
      }
    }
  printf("\n");
  }

void command_u(char* line) {
  int i;
  line = trim(line);
  stack[sp++] = pc;
  pc = find_label(line);
  jumped = -1;
  }

/* ***************************** */
/* ***** Extended commands ***** */
/* ***************************** */

void command_g(char* line) {
  int   x,y;
  char* comma;
  comma = strchr(line, ',');
  if (comma == NULL) {
    printf("Syntax error: %s\n",program[pc]);
    exit(1);
    }
  x = comma-line;
  x--;
  tokenize(line, 0, x);
  x = evaluate(0);
  comma++;
  comma = trim(comma);
  tokenize(comma, 0, strlen(comma)-1);
  y = evaluate(0);
  printf("\e[%d;%dH",y,x);
  }

void command_k(char* line) {
  int   x,y;
  char* comma;
  comma = strchr(line, ',');
  while (comma != NULL) {
    x = comma - line;
    x--;
    tokenize(line, 0, x);
    x = evaluate(0);
    printf("%c",x);
    line = comma + 1;
    comma = strchr(line, ',');
    }
  tokenize(line, 0, strlen(line)-1);
  x = evaluate(0);
  printf("%c",x);
  }

void command_l(char* line) {
  printf("\e[H\e[2J");
  }

void command_p(char* line) {
  FILE *file;
  char var[32];
  file = fopen("printer.txt","a");
  while (*line != 0) {
    if (*line == '$') {
      line++;
      line = getVariable(line,var);
      fprintf(file,"%s",getStringVar(var));
      }
    if (*line == '#') {
      line++;
      line = getVariable(line,var);
      fprintf(file,"%d",getIntegerVar(var));
      }
    else if (*line == '\\') {
      fclose(file);
      return;
      }
    else {
      fprintf(file,"%c",*line++);
      }
    }
  fprintf(file,"\n");
  fclose(file);
  }


void execute(char* line) {
  int i;
  int ct;
  char cmd;
  line = trim(line);
  if (*line == '*') {
    while (*line != 0 && *line != ' ') line++;
    line = trim(line);
    }
  cmd = *line++;
  if (cmd >= 'a' && cmd <= 'z') cmd -= 32;
  line = trim(line);
  if (*line == 'n' || *line == 'N') {
    if (matched) return;
    line++;
    }
  else if (*line == 'y' || *line == 'Y') {
    if (matched == 0) return;
    line++;
    }
  else if (*line == '(') {
    line++;
    i = 0;
    ct = 1;
    while (line[i] != ':' && line[i] != 0 && ct != 0) {
      if (line[i] == '(') { ct++; i++; }
      else if (line[i] == ')') { ct--; i++; }
      else i++;
      }
    if (line[i] == 0) {
      printf("Syntax error: [%d] %s\n",pc,program[pc]);
      exit(1);
      }
    i -= 2;
    tokenize(line, 0, i);
    evaluate(0);
    if (tokenTypes[0] != ' ' || tokenTypes[1] != OP_END) {
      printf("Expression error: [%d] %s\n",pc,program[pc]);
      exit(1);
      }
    if (tokens[0] == 0) return;
    while (*line != 0 && *line != ':') line++;
    }
  line = trim(line);
  if (*line != ':') {
    printf("Syntax error: [%d] %s\n",pc,program[pc]);
    exit(1);
    }
  line++;
  switch (cmd) {
    case 'A': command_a(line); break;
    case 'C': command_c(line); break;
    case 'E': if (sp == 0) pc = numLines+1;
                else pc = stack[--sp];
              break;
    case 'J': command_j(line); break;
    case 'M': command_m(line); break;
    case 'N': if (matched == 0) command_t(line);
              break;
    case 'R': break;
    case 'T': command_t(line); break;
    case 'U': command_u(line); break;
    case 'Y': if (matched != 0) command_t(line);
              break;

    case 'G': command_g(line); break;
    case 'K': command_k(line); break;
    case 'L': command_l(line); break;
    case 'P': command_p(line); break;
    default : printf("Unknown command: %c\n",cmd); exit(1);
    }
  }

int main(int argc, char** argv) {
  FILE *source;
  char  buffer[1024];
  printf("Rc/Pilot v1.0\n");
  numLines = 0;
  if (argc != 2) {
    printf("Usage: pilot filename\n");
    exit(1);
    }
  source = fopen(argv[1],"r");
  if (source == NULL) {
    printf("Could not open %s\n",argv[1]);
    exit(1);
    }
  while (fgets(buffer, 1023, source) != NULL) {
    if (strlen(buffer) > 0) {
      while (buffer[strlen(buffer)-1] == 0x0a ||
             buffer[strlen(buffer)-1] == 0x0c)
        buffer[strlen(buffer)-1] = 0x00;
      }
    if (strlen(buffer) > 0) {
      if (numLines == 0)
        program = (char**)malloc(sizeof(char*));
      else
        program = (char**)realloc(program,sizeof(char*) * (numLines+1));
       program[numLines] = (char*)malloc(strlen(buffer) + 1);
       strcpy(program[numLines], buffer);
       numLines++;
      }
    }
  fclose(source);
  pc = 0;
  sp = 0;
  matched = 0;
  numStringVars = 0;
  numIntegerVars = 0;
  setStringVar("match","");
  setStringVar("right","");
  setStringVar("left","");
  while (pc < numLines) {
    jumped = 0;
    execute(program[pc]);
    if (jumped == 0) pc++;
    }
  }

