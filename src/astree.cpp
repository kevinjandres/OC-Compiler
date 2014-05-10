// Jeffrey Deng (jkdeng) & Kevin Andres (kandres)
#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "astree.h"
#include "stringset.h"
#include "lyutils.h"
#include "symtable.h"
#include <fstream>
//
SymbolTable* globalT = new SymbolTable(NULL);
SymbolTable* currentT = globalT;
extern int exit_status;

astree* new_astree (int symbol, int filenr, int linenr, int offset,
                    const char* lexinfo) {
    astree* tree = new astree();
    // size_t size = sizeof(struct astree_rep);
    tree->symbol = symbol;
    tree->filenr = filenr;
    tree->linenr = linenr;
    tree->offset = offset;
    tree->lexinfo = intern_stringset (lexinfo);
    DEBUGF ('f', "astree %p->{%d:%d.%d: %s: \"%s\"}\n",
            tree, tree->filenr, tree->linenr, tree->offset,
            get_yytname (tree->symbol), tree->lexinfo->c_str());
    return tree;
}


astree* adopt1 (astree* root, astree* child) {
    root->children.push_back (child);
    DEBUGF ('a', "%p (%s) adopting %p (%s)\n",
            root, root->lexinfo->c_str(),
            child, child->lexinfo->c_str());
   // printf(" (%s) adopting (%s)\n",
         //  root->lexinfo->c_str(),
           //child->lexinfo->c_str());
    return root;
}

astree* adopt2 (astree* root, astree* left, astree* right) {
    adopt1 (root, left);
    adopt1 (root, right);
    return root;
}

astree* adopt3 (astree* root, astree* left, astree* right, astree* middle) {
    adopt1 (root, left);
    adopt1 (root, right);
    adopt1 (root, middle);
    return root;
}
astree* adopt1sym (astree* root, astree* child, int symbol) {
    root = adopt1 (root, child);
    root->symbol = symbol;
    return root;
}
astree* adopt2sym (astree* root, astree* left, astree* right, int symbol) {
    root = adopt2 (root, left, right);
    root->symbol = symbol;
    return root;
}
astree* adoptsym (astree* root, int symbol) {
    root->symbol = symbol;
    return root;
}
static void dump_node (FILE* outfile, astree* node) {
    fprintf (outfile, "%s (%s)",
             get_yytname (node->symbol), node->lexinfo->c_str());
    bool need_space = false;
    int child;
    
    
    for (child = 0; child < node->children.size(); ++child) {
        
        
        
        
        if (need_space){
         //   fprintf (outfile, " ");
        }
        need_space = true;
       // fprintf (outfile, "%p", node->children.at(child));
    }
    //fprintf (outfile, "]}");
}

static void dump_astree_rec (FILE* outfile, astree* root, int depth) {
    if (root == NULL) return;
    if(strcmp(get_yytname(root->symbol),"TOK_LEAVE")==0){
    }
    else if(strcmp(get_yytname(root->symbol), "TOK_NONTERMINAL")==0 || strcmp(get_yytname(root->symbol), "TOK_ROOT")==0 ||  strcmp(get_yytname(root->symbol), "TOK_FUNCTION")==0 ||  strcmp(get_yytname(root->symbol), "TOK_STRUCT")==0 || strcmp(get_yytname(root->symbol), "TOK_VARDECL")==0||strcmp(get_yytname(root->symbol), "TOK_WHILE")==0){
            fprintf (outfile, "%*s%s ", depth * 2, "", root->lexinfo->c_str());
        fprintf (outfile, "\n");
    }
    
    else{
        fprintf (outfile, "%*s%s (%s)", depth * 2,"",
                 get_yytname (root->symbol), root->lexinfo->c_str());
        fprintf (outfile, "\n");
    }
    
    for (size_t child = 0; child < root->children.size(); ++child) {
        dump_astree_rec (outfile, root->children[child], depth + 1);
        
    }
}

void dump_astree (FILE* outfile, astree* root) {
    dump_astree_rec (outfile, root, 0);
    fflush (NULL);
}

void visitTreeStart(FILE* outfile, astree* node){
    visitTree(node);
    globalT->dump(outfile, 0);
}

void yyprint (FILE* outfile, unsigned short toknum, astree* yyvaluep) {
    DEBUGF ('f', "toknum = %d, yyvaluep = %p\n", toknum, yyvaluep);
    if (is_defined_token (toknum)) {
        dump_node (outfile, yyvaluep);
    }else {
        fprintf (outfile, "%s(%d)\n", get_yytname (toknum), toknum);
    }
    fflush (NULL);
}

void printfffff(string x){
    printf("%s \n", x.c_str());
}
//void printTree(SymbolTable* table){
 //   table->dump(stdout,0);
//}
void visitTree (astree* node){
    
    switch(node->symbol){
        case TOK_FUNCTION:
        {
            
            astree* type = node->children[0]->children[0]->children[0];
            
            if(strcmp(type->lexinfo->c_str(), "basetype")==0){
                type = type->children[0];
            }
            string signature_type= *type->lexinfo;
            astree* paramm = node->children[1];
            astree* param;
            string signature ;
            
            astree* tempp = node->children[0];
            if(tempp->children.size() == 3){
                if(strcmp(signature_type.c_str(), "void")==0){
                    fprintf(stderr,"can't have void []\n");
                    exit_status = 1;
                    //currentT=currentT->enterBlock();
                    break;
                }
            }
            
            if(paramm->children.size() == 0 ){
            signature = signature_type+ "( )";
            }
            else{
                astree* temp = node->children[1]->children[0]->children[0]->children[0];
                if(strcmp(temp->lexinfo->c_str(), "basetype")==0){
            param = node->children[1]->children[0]->children[0]->children[0]->children[0];
                }
                else{
                    param = temp;
                }
            
            string signature_param = *param->lexinfo;
            signature = signature_type + "(" + signature_param + ")";
            }
            
            astree* namee = node-> children[0]->children[1];
            string name = *namee->lexinfo;
            //printf("%s, %s\n",name.c_str(), signature.c_str());
            currentT = currentT->enterFunction(name, signature,namee->filenr,namee->linenr,namee->offset);
           // globalT->dump(stdout, 0);
            
          //  printf("%s (%zd,%zd,%zd) %s\n", name.c_str(), namee->filenr, namee->linenr, namee-> offset, signature.c_str());
            //printf("function\n");
           // printf("%zd\n", node->linenr);
            break;
        }
            
        case TOK_STRUCT:
            //checkStruct(node);
           //printf("Struct\n");
            
           //     printf("%zd\n", node->linenr);
            break;
            
        case TOK_VARDECL:
        {
          //  printf("in vardecl");
            
            
            
            astree* decl = node->children[0];
            if(decl->children.size() == 3){
                astree* name = decl->children[2];
                string namee = *name->lexinfo;

                
                astree* type = decl->children[0]->children[0];
                if(strcmp(type->lexinfo->c_str(), "basetype")==0){
                    type = type->children[0];
                    //printf("this should prtinf");
                }
                string typee = *type->lexinfo;
                //printf("typee = %s\n", typee.c_str());
                if(strcmp(typee.c_str(), "void")==0){
                    fprintf(stderr,"can't have void vars\n");
                    exit_status=1;
                    break;
                }
                string typeF = typee + " []" ;
                currentT->addSymbol(namee, typeF,name->filenr,name->linenr,name->offset);
            }
            //else if(decl->children.size() == 2){
            else{
            astree* name = decl->children[1];
                string namee = *name->lexinfo;
                astree* type = decl->children[0]->children[0];
                if(strcmp(type->lexinfo->c_str(),"basetype")==0){
                    type= type->children[0];
                }
                    if(strcmp(type->lexinfo->c_str(),"void")==0){
                        fprintf(stderr,"can't have void vars\n");
                        exit_status=1;
                        break;
                    }
                
                string typeF = *type->lexinfo;
               // printf("%s, %s\n",namee.c_str(), typeF.c_str());
                currentT->addSymbol(namee, typeF,name->filenr,name->linenr,name->offset);
            }
            //printf("Vardecl\n");
           // globalT->dump(stdout, 0);
            break;
            
        }
        case TOK_PARAMLIST:
        {
            for(int i = 0; i<node->children.size(); i++){
                astree* name;
            astree* decl = node->children[i];
                if(decl->children.size()!=2){
                name = decl->children[2];
                }
                else{
            name = decl->children[1];
                }
                astree* type;
            string namee = *name->lexinfo;
                if(strcmp(decl->children[0]->children[0]->lexinfo->c_str(), "basetype")==0){
            type = decl->children[0]->children[0]->children[0];
                }
                    else{
                        type = decl->children[0]->children[0];
                    }
            string typee= *type->lexinfo;
            currentT->addSymbol(namee, typee,name->filenr,name->linenr,name->offset);
            
            }
            break;
        }
            
        case TOK_IF:
            currentT=currentT->enterBlock();
            break;
        
        case TOK_ELSE:
            currentT=currentT->enterBlock();
            break;
            
        case TOK_LEAVE:
            currentT= currentT->leave(currentT);
            break;
            
        case TOK_WHILE:
            currentT=currentT->enterBlock();
            break;
            
        case TOK_BINOP:
        {
           // printf("in BINOP\n");
            /*string type1, type2, opt;
            astree* name = node->children[0];
            if(strcmp(name->lexinfo->c_str() ,"allocator")==0){
               // string type1 = currentT->lookup(*node->children[0]->children[0]->lexinfo);
            }
            else{
            //string type1 = currentT->lookup(*node->children[0]->children[0]->lexinfo);
            }
            if(strcmp(node->children[2]->lexinfo->c_str(),"null")==0){
                string type2 = "NULL";
            }
            else{
           // string type2 = currentT->lookup(*node->children[2]->children[0]->lexinfo);
            }
            // string opt = *node->children[1]->lexinfo;
            //checkType(type1,type2,opt);*/
            astree* opt = node->children[1];
            if(strcmp(opt->lexinfo->c_str(), "<")==0){

            astree* type1 = node->children[0]->children[0];
            astree* type2 = node->children[2]->children[0];

               // printf("%s\n",currentT->lookup(type2->lexinfo->c_str()).c_str());
                if(strcmp(currentT->lookup(type2->lexinfo->c_str()).c_str(), "int []")==0){
                    fprintf(stderr,"can't compare pointer\n");
                    exit_status=1;
                    break;
            }
            }
            
            break;
    }
    }
            for (size_t child = 0; child < node->children.size(); ++child) {
                visitTree (node->children[child]);
            }
    
   // printf("return %s\n", node->lexinfo->c_str());
}

/*void checkFunction(astree* node){
    astree* decl = node->children[0];
    astree* paramlist = node->children[1];
    astree* blocl = node->children[2];
    checkDECL(decl);
}*/


/*void checkType(string type1, string type2, string opt){
    if(strcmp(opt.c_str(),"=")==0||strcmp(opt.c_str(),"==")==0||strcmp(opt.c_str(),"!=")==0||strcmp(opt.c_str(),"<")==0||strcmp(opt.c_str(),"<=")==0||strcmp(opt.c_str(),">")==0||strcmp(opt.c_str(),">=")==0){
        if(type1=="NULL"|| type2 =="NULL"){
            printf("Typecheck is okay for NULL");
        }
        
        else if(strcmp(type1.c_str(),type2.c_str())!=0){
            fprintf(stderr, "Incompatible variable types of %s & %s\n", type1.c_str(), type2.c_str());
        } else{
            printf("Typecheck is okay");
        }
    
    } 
}*/

/*void checkStruct(astree* node){
    
}*/

/*void checkDECL(astree* node){
    astree* type = node->children[0]->children[0];
    const string* typee = type->lexinfo;
    
    for(int i =0;i<=type->children.size(); ++i){
    if(type->children.size()==2){
    //addSymbol(name, type);
    }
    if(type->children.size()==3){
        astree* name = node->children[2];
        astree* namee = node->children[1];
        const string* name1 = name->lexinfo;   //name
        const string* name2 = namee->lexinfo;
        }
        }
}*/


void free_ast (astree* root) {
    while (not root->children.empty()) {
        astree* child = root->children.back();
        root->children.pop_back();
        free_ast (child);
    }
    DEBUGF ('f', "free [%X]-> %d:%d.%d: %s: \"%s\")\n",
            (uintptr_t) root, root->filenr, root->linenr, root->offset,
            get_yytname (root->symbol), root->lexinfo->c_str());
    delete root;
}

void free_ast2 (astree* tree1, astree* tree2) {
    free_ast (tree1);
    free_ast (tree2);
}

RCSC("$Id: astree.cc,v 1.14 2013-10-10 18:48:18-07 - - $")

