// Jeffrey Deng (jkdeng) & Kevin Andres (kandres)
#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <string>
#include <vector>
using namespace std;
#include "auxlib.h"

//typedef struct astree_rep *astree;

struct astree {
    int symbol;               // token code
    size_t filenr;            // index into filename stack
    size_t linenr;            // line number from source code
    size_t offset;            // offset of token with current line
    const string* lexinfo;    // pointer to lexical information
    vector<astree*> children; // children of this n-way node
};


astree* new_astree (int symbol, int filenr, int linenr, int offset,
                    const char* lexinfo);
astree* adopt1 (astree* root, astree* child);
astree* adopt2 (astree* root, astree* left, astree* right);
astree* adopt3 (astree* root, astree* left, astree* right, astree* middle);
astree* adopt1sym (astree* root, astree* child, int symbol);
astree* adopt2sym (astree* root, astree* left, astree* right, int symbol);
astree* adoptsym (astree* root, int symbol);
void dump_astree (FILE* outfile, astree* root);
void yyprint (FILE* outfile, unsigned short toknum, astree* yyvaluep);
void free_ast (astree* tree);
void free_ast2 (astree* tree1, astree* tree2);
void printfffff(string x);
void visitTree (astree* node);
void checkDECL(astree* node);
void checkFunction(astree* node);
void checkType(string type1, string type2, string opt);
//void printTree(SymbolTable* table);
void visitTreeStart(FILE* outfile, astree* node);
void checkStruct(astree* node);

RCSH("$Id: astree.h,v 1.3 2013-09-20 12:23:31-07 - - $")
#endif
