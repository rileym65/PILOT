#ifndef _HEADER_H
#define _HEADER_H

#ifdef MAIN
#define LINK
#else
#define LINK extern
#endif

LINK char  **program;
LINK int     numLines;
LINK int     pc;
LINK int     jumped;
LINK int     matched;
LINK int     stack[256];
LINK int     sp;
LINK char  **stringVars;
LINK char  **stringValues;
LINK int     numStringVars;
LINK char  **integerVars;
LINK int    *integerValues;
LINK int     numIntegerVars;
LINK char    acceptBuffer[256];
LINK int     tokens[64];
LINK char    tokenTypes[64];
LINK int     numTokens;

#endif

