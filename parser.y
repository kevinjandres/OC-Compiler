// Jeffrey Deng (jkdeng) & Kevin Andres (kandres)
%{

// Dummy parser for scanner project.

#include "lyutils.h"
#include <assert.h>
#include "astree.h"

%}

%debug
%defines
%error-verbose
%token-table
%verbose

%token TOK_VOID TOK_BOOL TOK_CHAR TOK_INT TOK_STRING
%token TOK_IF TOK_ELSE TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_FALSE TOK_TRUE TOK_NULL TOK_NEW TOK_ARRAY
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%token TOK_FUNCTION TOK_PROTOTYPE TOK_RETURNVOID TOK_PARAMLIST
%token TOK_NEWSTRING TOK_INDEX
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON

%token TOK_BLOCK TOK_CALL TOK_IFELSE TOK_INITDECL
%token TOK_POS TOK_NEG TOK_NEWARRAY TOK_TYPEID TOK_FIELD
%token TOK_ORD TOK_CHR TOK_ROOT TOK_NONTERMINAL TOK_VARDECL
%token TOK_LEAVE TOK_WHILE TOK_BINOP

%right     TOK_IF TOK_ELSE
%right     '='
%left      TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%left      '+' '-'
%left      '*' '/' '%'
%right     TOK_POS TOK_NEG '!' TOK_ORD TOK_CHR
%left      '[' '.'
%nonassoc  TOK_NEW



%start start

%%

start       : program             {yyparse_astree = $1; }
            ;

program     : program structdef   { $$ = adopt1 ($1, $2); }
            | program statement   { $$ = adopt1 ($1, $2); }
            | program function    { $$ = adopt1 ($1, $2); }
            | program error '}'   { $$ = $1; }
            | program error ';'   { $$ = $1; }
            |                     { $$ = new_parseroot ();  }
            ;

structdef   : TOK_STRUCT TOK_IDENT '{' moredecl ';' '}'
            { $$ = adopt2(new_tokenStruct(),$2,$3); }
            | TOK_STRUCT TOK_IDENT '{' '}'
            { $$=adopt1(new_tokenStruct(),$2);}
            ;

type        : basetype '[]'         { $$ = adopt2(new_node("type"),$1, $2); }
            | basetype              { $$ = adopt1(new_node("type"), $1 ); }
            ;

            
decl        : type TOK_ARRAY TOK_IDENT
            { $$ = adopt3(new_node("decl"), $1, $2,$3); }
            | type TOK_IDENT     { $$ = adopt2 (new_node("decl"),$1, $2); }
            ;

moredecl    : decl                  {$$=$1}
            | moredecl ';' decl     {$$=adopt1($1,$3); free_ast($2); }
            ;


basetype    : TOK_VOID     { $$ = adopt1(new_node("basetype"),$1); }
            | TOK_BOOL     { $$ = adopt1(new_node("basetype"),$1); }
            | TOK_CHAR     { $$ = adopt1(new_node("basetype"),$1); }
            | TOK_INT      { $$ = adopt1(new_node("basetype"),$1); }   
            | TOK_STRING   { $$ = adopt1(new_node("basetype"),$1); }
            | TOK_IDENT    { $$ = $1; }
            ;
            
paramlist   : '(' decl
            { $$ = adopt1sym ($1, $2, TOK_PARAMLIST); }
            | paramlist ',' decl
            { $$ = adopt1 ($1, $3); free_ast ($2); }
            | '('                   { 
                $$=new_node("TOK_PARAMALIST") ;}
            ;

function    : decl paramlist ')' block { $$ = adopt3(new_function(),$1, $2, $4); }
            ;
            
block       : '{' state_cont '}'            {free_ast($3);
                                                        $$ = adopt2(new_node("block"),$2, new_leave());}
            | '{' '}'                       {free_ast($2);
                                            $$ = adopt1(new_node("block"), new_leave());}
            | ';'                           {$$ = adopt1(new_node("block"),new_leave());}
            ;

state_cont  : state_cont statement      {$$ = adopt1($1,$2); }//go back to parent
            | statement                 { $$ = $1; }
            ;
            
statement   : block               { $$ = $1; }
            | vardecl	          { $$ = $1; }
            | while               { $$ = $1; }
            | ifelse              { $$ = $1; }
            | return              { $$ = $1; }
            | expr ';'            { free_ast($2); $$ = $1;  }
            ;

vardecl	    : decl '=' expr ';' { free_ast($4);
            $$ = adopt2(new_vardecl(), $1, $3) ;}
            ;


while       : TOK_WHILE '(' expr ')' statement {free_ast2($2, $4);
                                                $$ = adopt2(new_while(),$3,$5);}
            ;

ifelse    : TOK_IF '(' expr ')' statement %prec TOK_ELSE
          { $$ = adopt2 (new_if(), $3, $5); free_ast2 ($2, $4); }
          | TOK_IF '(' expr ')' statement TOK_ELSE statement
          { $$ = adopt2 (new_node("IfElse"), adopt2(new_if(),$3,$5),adopt1(new_else(),$7));
            free_ast2 ($2, $4); free_ast($6);}
          ;
return      : TOK_RETURN ';'        {$$ = adopt1sym($1,$2,
                                        TOK_RETURNVOID); }
            | TOK_RETURN expr ';'   { free_ast($3);
                                        $$ = adopt1($1, $2) ;}
            ;

expr        : binop                   { $$ = $1 ; }
            | unop                    { $$ = $1 ; }
            | allocator               { $$ = $1 ; }
            | call                    { $$ = $1 ; }
            | variable                { $$ = $1 ; }
            | constant                { $$ = $1 ; }
            | '(' expr ')'            { free_ast2($1,$3); $$ = $2;}
            ;
binop	    :  expr '=' expr          { $$ = adopt3 (new_binop(), $1,$2, $3); }
            |  expr TOK_EQ expr       { $$ = adopt3 (new_binop(), $1,$2, $3); }
            |  expr TOK_NE expr       { $$ = adopt3 (new_binop(), $1,$2, $3); }
            |  expr TOK_LT expr       { $$ = adopt3 (new_binop(), $1,$2, $3); }
            |  expr TOK_GT expr       { $$ = adopt3 (new_binop(), $1,$2, $3); }
            |  expr TOK_LE expr       { $$ = adopt3 (new_binop(), $1,$2, $3); }
            |  expr TOK_GE expr       { $$ = adopt3 (new_binop(), $1,$2, $3); }
            |  expr '+' expr          { $$ = adopt3 (new_binop(), $1,$2, $3); }
            |  expr '-' expr          { $$ = adopt3 (new_binop(), $1,$2, $3); }
            |  expr '*' expr          { $$ = adopt3 (new_binop(), $1,$2, $3); }
            |  expr '/' expr          { $$ = adopt3 (new_binop(), $1,$2, $3); }
            |  expr '%' expr          { $$ = adopt3 (new_binop(), $1,$2, $3); }
            ;

unop	    :  '+' expr %prec TOK_POS { $$ = adopt1sym ($1, $2, TOK_POS); }
            |  '-' expr %prec TOK_NEG { $$ = adopt1sym ($1, $2, TOK_NEG); }
            |  '!' expr               { $$ = adopt1 ($1, $2); }
            |  TOK_ORD expr           { $$ = adopt1 ($1, $2); }
            |  TOK_CHR expr           { $$ = adopt1 ($1, $2); }
            ;

allocator   : TOK_NEW TOK_IDENT '('')' { free_ast2($3,$4);
                                        $2 = adoptsym($2,TOK_TYPEID);
                                        $$ = adopt1($1, $2) ;}
            | TOK_NEW TOK_STRING '(' expr ')' { free_ast2($3,$5);
                                                $$ = adopt1sym($1,$4,
                                                TOK_NEWSTRING);}
            | basetype '[' expr ']' { free_ast2($2,$4);
                                                $$ = adopt2(new_node("allocator"),$1,$3 );}
            | TOK_NEW basetype '[' expr ']' { free_ast2($3,$5);
                                                $$ = adopt3(new_node("allocator"),$1,$2,$4 );}
            ;

call        : TOK_IDENT callque ')'   
            { $$ = adopt2(new_node("call"), $1, $2); free_ast ($3);}
            | TOK_IDENT '(' ')'
            { $$ = adopt1(new_node("call"), $1); free_ast2($2, $3);}
            ;
          
          
callque     : '(' expr        { $$ = adopt1 (new_node("callque"), $2); }
            | callque ',' expr      { free_ast($2);
                                    $$ = adopt1(new_node("callque"),$3); }
            ;

variable    : TOK_IDENT              { $$ = adopt1(new_node("variable"), $1); }
            | expr '[' expr ']'      { $$ = adopt1 (adopt1sym ($2, $1, TOK_INDEX), $3); free_ast ($4); }
            | expr '.' TOK_IDENT     { free_ast($2); $$=adopt1(new_node("variable"), $1);}
            ;
            
constant    : TOK_INTCON        { $$ = adopt1(new_node("constant"), $1) ; }
            | TOK_CHARCON       { $$ = $1 ; }
            | TOK_STRINGCON     { $$ = $1 ; }
            | TOK_FALSE         { $$ = $1 ; }
            | TOK_TRUE          { $$ = $1 ; }
            | TOK_NULL          { $$ = $1 ; }
            ;


%%

const char *get_yytname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}


bool is_defined_token (int symbol) {
   return YYTRANSLATE (symbol) > YYUNDEFTOK;
}
