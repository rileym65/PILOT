#define MAIN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define OP_MUL    0x42
#define OP_DIV    0x41
#define OP_ADD    0x32
#define OP_SUB    0x31
#define OP_GT     0x26
#define OP_LT     0x25
#define OP_GTE    0x24
#define OP_LTE    0x23
#define OP_EQ     0x22
#define OP_NE     0x21
#define OP_AND    0x13
#define OP_OR     0x12
#define OP_XOR    0x11
#define OP_OP     0x08
#define OP_CP     0x09
#define OP_NUM    0x00

void show_tokens() {
  int i;
  for (i=0; i<numTokens; i++)
    printf("%d   %d\n",types[i], tokens[i]);
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

void reduce() {
  if (numTokens < 3) return;
  switch (types[numTokens-2]) {
    case OP_MUL:tokens[numTokens-3] *= tokens[numTokens-1]; break;
    case OP_DIV:tokens[numTokens-3] /= tokens[numTokens-1]; break;
    case OP_ADD:tokens[numTokens-3] += tokens[numTokens-1]; break;
    case OP_SUB:tokens[numTokens-3] -= tokens[numTokens-1]; break;
    case OP_GT:tokens[numTokens-3] =
               tokens[numTokens-3] > tokens[numTokens-1]; break;
    case OP_LT:tokens[numTokens-3] =
               tokens[numTokens-3] < tokens[numTokens-1]; break;
    case OP_GTE:tokens[numTokens-3] =
                tokens[numTokens-3] >= tokens[numTokens-1]; break;
    case OP_LTE:tokens[numTokens-3] =
                tokens[numTokens-3] <= tokens[numTokens-1]; break;
    case OP_EQ:tokens[numTokens-3] =
               tokens[numTokens-3] == tokens[numTokens-1]; break;
    case OP_NE:tokens[numTokens-3] =
               tokens[numTokens-3] != tokens[numTokens-1]; break;
    case OP_AND:tokens[numTokens-3] &= tokens[numTokens-1]; break;
    case OP_OR:tokens[numTokens-3] |= tokens[numTokens-1]; break;
    case OP_XOR:tokens[numTokens-3] ^= tokens[numTokens-1]; break;
    }
  numTokens -= 2;
  }

void add(int op) {
  while (numTokens > 2 && (op & 0xf0) <= (types[numTokens-2] & 0xf0)) {
    reduce();
    }
  types[numTokens++] = op;
  }

int evaluate(char* buffer) {
  int p;
  char token[64];
  int flag;
  int parens;
  parens = 0;
  numTokens = 0;
  flag = 1;
  while (*buffer != 0 && flag) {
    if (*buffer >= '0' && *buffer <= '9') {
      types[numTokens] = OP_NUM;
      tokens[numTokens] = *buffer - '0';
      buffer++;
      while (*buffer >= '0' && *buffer <= '9') {
        tokens[numTokens] *= 10;
        tokens[numTokens] += *buffer - '0';
        buffer++;
        }
      numTokens++;
      }
    else if ((*buffer >= 'a' && *buffer <= 'z') ||
             (*buffer >= 'A' && *buffer <= 'Z') ||
             *buffer == '_' || *buffer == '#') {
      if (*buffer == '#') buffer++;
      p = 0;
      while ((*buffer >= 'a' && *buffer <= 'z') ||
             (*buffer >= 'A' && *buffer <= 'Z') ||
             (*buffer >= '0' && *buffer <= '9') ||
             *buffer == '_') {
        token[p++] = *buffer++;
        }
      token[p] = 0;
      types[numTokens] = OP_NUM;
      tokens[numTokens] = getIntegerVar(token);
      numTokens++;
      }
    else if (*buffer == '+') { add(OP_ADD); buffer++; }
    else if (*buffer == '-') { add(OP_SUB); buffer++; }
    else if (*buffer == '*') { add(OP_MUL); buffer++; }
    else if (*buffer == '/') { add(OP_DIV); buffer++; }
    else if (*buffer == '&') { add(OP_AND); buffer++; }
    else if (*buffer == '|') { add(OP_OR); buffer++; }
    else if (*buffer == '^') { add(OP_XOR); buffer++; }
    else if (*buffer == '<' && *(buffer+1) == '=') { add(OP_LTE); buffer+=2; }
    else if (*buffer == '>' && *(buffer+1) == '=') { add(OP_GTE); buffer+=2; }
    else if (*buffer == '<' && *(buffer+1) == '>') { add(OP_NE); buffer+=2; }
    else if (*buffer == '<') { add(OP_LT); buffer++; }
    else if (*buffer == '>') { add(OP_GT); buffer++; }
    else if (*buffer == '=') { add(OP_EQ); buffer++; }
    else if (*buffer == '(') { types[numTokens++] = OP_OP; parens++; buffer++; }
    else if (*buffer == ')' && parens > 0) {
      parens--;
      while (numTokens >= 2 &&
             types[numTokens-2] != OP_OP &&
             types[numTokens-2] >= 0x10) reduce();
      if (numTokens < 2) {
        printf("Expression Error\n");
        exit(1);
        }
//      while (types[numTokens-2] != OP_OP &&
//             types[numTokens-2] >= 0x10) reduce();
      types[numTokens-2] = OP_NUM;
      tokens[numTokens-2] = tokens[numTokens-1];
      numTokens--;
      buffer++;
      }
    else if (*buffer == ' ') buffer++;
    else flag = 0;
    }
  while (numTokens > 2) reduce();
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
//  tokenize(line,start,end);
//  evaluate(0);
  line = trim(line);
  evaluate(line+1);
  if (numTokens > 1 || types[0] != OP_NUM) {
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
//  tokenize(line, 0, x);
//  x = evaluate(0);
  x = evaluate(line);
  comma++;
  comma = trim(comma);
//  tokenize(comma, 0, strlen(comma)-1);
//  y = evaluate(0);
  y = evaluate(comma);
  printf("\e[%d;%dH",y,x);
  }

void command_k(char* line) {
  int   x,y;
  char* comma;
  comma = strchr(line, ',');
  while (comma != NULL) {
    x = comma - line;
    x--;
//    tokenize(line, 0, x);
//    x = evaluate(0);
    x = evaluate(line);
    printf("%c",x);
    line = comma + 1;
    comma = strchr(line, ',');
    }
//  tokenize(line, 0, strlen(line)-1);
//  x = evaluate(0);
  x = evaluate(line);
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
//    tokenize(line, 0, i);
//    evaluate(0);
    evaluate(line);
    if (numTokens > 1 || types[0] != OP_NUM) {
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

