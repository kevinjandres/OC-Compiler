// Jeffrey Deng (jkdeng) & Kevin Andres (kandres)
#ifndef __LYUTILS_H__
#define __LYUTILS_H__

// Lex and Yacc interface utility.

#include <stdio.h>

#include "astree.h"
#include "auxlib.h"

#define YYEOF 0

extern FILE* yyin;
extern astree* yyparse_astree;
extern int yyin_linenr;
extern char* yytext;
extern int yy_flex_debug;
extern int yydebug;
extern int yyleng;

int yylex (void);
int yyparse (void);
void yyerror (const char* message);
int yylex_destroy (void);
const char* get_yytname (int symbol);
bool is_defined_token (int symbol);

const string* scanner_filename (int filenr);
void scanner_newfilename (const char* filename);
void scanner_badchar (unsigned char bad);
void scanner_badtoken (char* lexeme);
void scanner_newline (void);
void scanner_setecho (bool echoflag);
void scanner_useraction (void);
astree* new_parseroot (void);
astree* new_function (void);
astree* new_leave(void);
astree* new_protonode (void);
astree* new_tokenStruct (void);
astree* new_node (string name);
astree* new_vardecl(void);
astree* new_binop(void);
astree* new_while(void);
astree* new_if(void);
astree* new_else(void);
int yylval_token (int symbol);
void error_destructor (astree*);

void scanner_include (void);

typedef astree* astree_pointer;
#define YYSTYPE astree_pointer
#include "yyparse.h"

RCSH("$Id: lyutils.h,v 1.5 2013-10-10 18:17:45-07 - - $")
#endif
