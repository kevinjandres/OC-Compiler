// Jeffrey Deng (jkdeng) & Kevin Andres (kandres)
//  main.cpp
//  Project 1 - String Set ADT and C Preprocessor
//
//  Created by Kevin Andres & Jeffrey Deng.
//  Copyright (c) 2013 Kevin Andres. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <string.h>
using namespace std;
#include "auxlib.h"
#include "stringset.h"
#include "astree.h"
#include "lyutils.h"
#include "symtable.h"
#include <fstream>
#include <unordered_set>
using namespace std;

int exit_status = 0;
const string CPP = "/usr/bin/cpp";
const size_t LINESIZE = 1024;
extern int yy_flex_debug;
extern int yydebug;
extern FILE *yyin;
extern int yylex(void);
extern int yyparse(void);
extern const char *get_yytname (int symbol);
FILE *tokfile;
FILE *astfile;
FILE *symfile;
void run_scanner(void){
    while(yylex()!=0){
        yyparse();
        // yyprint(astfile,program,
    }
    
}
void run_parser(void){
    while(yyparse()==0){
        printf("1");
    }
}
int main(int argc,  char * argv[])
{
    
    int strings = 0;
    int opt;
    char buffer[LINESIZE];
    char bufferr[LINESIZE];
    string line;
    string work;
    string tempp;
    ofstream test;
    FILE * temp;
    FILE * piipe;
    char *fgets_rcc;
    char *bufptrr;
    char *savee = NULL;
    yy_flex_debug=0;
    while((opt = getopt(argc , argv, "D:@:ly"))!=-1){
        switch(opt) {
            case 'D':
                line = optarg;
                temp=fopen("test.txt","w");
                tempp="test.txt";
                fprintf(temp,"-D");
                fprintf(temp,"%s",line.c_str());
                work = CPP+ " " +tempp;
                piipe = popen(work.c_str(), "r");
                for(;;){
                    fgets_rcc=fgets(bufferr, LINESIZE, piipe);
                    if(fgets_rcc == NULL){
                        break;
                    }
                }
                
                bufptrr = bufferr;
                for(int token_ctt = 1;;++token_ctt){
                    char *tokenn = strtok_r(bufptrr , " \t\n", &savee);
                    bufptrr = NULL;
                    if(tokenn == NULL) break;
                    intern_stringset(tokenn);
                }
                
                
                break;
                
                
            case '@':
                set_debugflags(optarg);
                strings = 1;
                break;
            case 'l':
                yy_flex_debug=1;
                break;
            case 'y':
                yydebug =1;
                break;
                
            default:
                fprintf(stderr,"option:[-ly][-@flag...][-D string]\n");
                exit_status=1;
                exit(exit_status);
        }
    }
    if (optind>=argc){
        fprintf(stderr, "Expected argument after options\n");
        exit_status=1;
        exit(exit_status);
    }
    
    char *filename = argv[optind];
    string name = filename;
    set_execname(filename);
    string correctname = ".oc";
    string ending;
    string id;
    string f_id;
    string f2_id;
    string f3_id;
    string f4_id;
    string suffix = ".str";
    string suffix2 = ".tok";
    string suffix3 = ".ast";
    string suffix4 = ".sym";
    string yyin_name;
    unsigned x,y;
    y = id.find_last_of(".");
    int z = name.find_last_of(".");
    ending = name.substr(z);
    if(strcmp(ending.c_str(),correctname.c_str()) !=0){
        fprintf(stderr, "Unexpected file type. Usage: filename.oc\n");
        exit_status=1;
        exit(exit_status);
    }
    string one;
    string command = CPP + " " + filename;
    ifstream file(filename);
    if(!file){
        exit_status=1;
        exit(exit_status);
    }
    yyin = popen(command.c_str(), "r");
    if(yyin == NULL){
        fprintf(stderr,"can't open file");
        exit_status=1;
        exit(exit_status);
    }else{
        
        x = name.find_last_of("/")+1;
        id = name.substr(x);
        y = id.find_last_of(".");
        f_id = id.substr(0,y);
        f2_id = f_id;
        f3_id = f_id;
        f4_id = f_id;
        f_id.append(suffix);
        f2_id.append(suffix2);
        f3_id.append(suffix3);
        f4_id.append(suffix4);
        
        tokfile= fopen(f2_id.c_str(),"w");
        astfile= fopen(f3_id.c_str(),"w");
        symfile = fopen(f4_id.c_str(),"w");
        //run_scanner();
        int parsecheck = yyparse();
        if(parsecheck){
            printf("wronnnngggg\n");
            exit_status=1;
            exit(exit_status);
        }
        
        
        //  dump_node(astfile, program);
        visitTreeStart(symfile, yyparse_astree);
        
        dump_astree(astfile, yyparse_astree); // prints astree for .ast file

        for(;;){
            char *fgets_rc=fgets(buffer, LINESIZE, yyin);
            if(fgets_rc == NULL){
                break;
            }
        }
        char *bufptr = buffer;
        char *save = NULL;
        for(int token_ct = 1;;++token_ct){
            char *token = strtok_r(bufptr , " \t\n", &save);
            bufptr = NULL;
            if(token == NULL) break;
            intern_stringset(token);
        }
        
    }
    FILE * pFile;
    pFile = fopen (f_id.c_str(),"w");
    dump_stringset(pFile);
    exit(exit_status);
    
}










