

%{
// Jeffrey Deng (jkdeng) & Kevin Andres (kandres)
// $Id: scanner.l,v 1.3 2013-10-15 16:37:56-07 - - $

#include "auxlib.h"
#include "lyutils.h"
#define YY_USER_ACTION  { scanner_useraction (); }
#define IGNORE(THING)   { }

%}

%option 8bit
%option debug
%option ecs
%option nodefault
%option nounput
%option noyywrap
%option perf-report
%option verbose
%option warn
LETTER          [a-zA-Z_]
DIGIT           [0-9]
IDENT           ({LETTER}({LETTER}|{DIGIT})*)
BADID           ({DIGIT}({LETTER}|{DIGIT})*)
NUMBER          {DIGIT}({DIGIT})*
STRING      (\"([^\\"\n]|\\[\\'"0nt])*\")
BADSTR      (\"([^\\"\n]|\\[\\'"0nt])*\n)
CHAR        (\' ([^\\'\n]|\\[\\'"0nt])')
BADCHAR     (\'(([^\n\'\\][^\n\'\\]+)|(\\[\\\'\"n0t][^\n\'\\]+))\')
COMMENT         (\/\/.*)

%%


"#".*           { scanner_include();}
[\040\t]+       { IGNORE(witespace);}
\n              { scanner_newline();}
"[]"            { return yylval_token (TOK_ARRAY); }
"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
"["             { return yylval_token ('['); }
"]"             { return yylval_token (']'); }
"{"             { return yylval_token ('{'); }
"}"             { return yylval_token ('}'); }
";"             { return yylval_token (';'); }
","             { return yylval_token (','); }
"."             { return yylval_token ('.'); }
"="             { return yylval_token ('='); }
"=="            { return yylval_token (TOK_EQ); }
"!="            { return yylval_token (TOK_NE); }
"<"             { return yylval_token (TOK_LT); }
"<="            { return yylval_token (TOK_LE); }
">"             { return yylval_token (TOK_GT); }
">="            { return yylval_token (TOK_GE); }
"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"%"             { return yylval_token ('%'); }
"!"             { return yylval_token ('!'); }

"void"          { return yylval_token (TOK_VOID);}
"bool"          { return yylval_token (TOK_BOOL);}
"char"          { return yylval_token (TOK_CHAR);}
"int"           { return yylval_token (TOK_INT);}
"string"        { return yylval_token (TOK_STRING);}
"struct"        { return yylval_token (TOK_STRUCT);}
"if"            { return yylval_token (TOK_IF);}
"else"          { return yylval_token (TOK_ELSE);}
"while"         { return yylval_token (TOK_WHILE);}
"return"        { return yylval_token (TOK_RETURN);}
"new"           { return yylval_token (TOK_NEW);}
"false"         { return yylval_token (TOK_FALSE);}
"true"          { return yylval_token (TOK_TRUE);}
"null"          { return yylval_token (TOK_NULL);}
"ord"           { return yylval_token (TOK_ORD);}
"chr"           { return yylval_token (TOK_CHR);}

{NUMBER}        { return yylval_token (TOK_INTCON);}
{IDENT}         { return yylval_token (TOK_IDENT);}
{BADID}         { scanner_badtoken(yytext);
                    return yylval_token (TOK_IDENT);}
{STRING}        { return yylval_token (TOK_STRINGCON);}
{BADSTR}        { scanner_badtoken(yytext); 
                    return yylval_token (TOK_STRINGCON);}
{CHAR}          { return yylval_token (TOK_CHARCON);}
{BADCHAR}       { scanner_badtoken(yytext); 
                    return yylval_token (TOK_CHARCON);}
{COMMENT}       { IGNORE(comment);}


.               { scanner_badchar(*yytext);}

%%





