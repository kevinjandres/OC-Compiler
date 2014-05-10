#include "auxlib.h"
#include "symtable.h"
#include <string.h>

// Creates and returns a new symbol table.
//
// Use "new SymbolTable(NULL)" to create the global table
SymbolTable::SymbolTable(SymbolTable* parent) {
    // Set the parent (this might be NULL)
    this->parent = parent;
    // Assign a unique number and increment the global N
    this->number = SymbolTable::N++;
}

// Creates a new empty table beneath the current table and returns it.
SymbolTable* SymbolTable::enterBlock() {
    //printf("enterblock\n");
    // Create a new symbol table beneath the current one
    SymbolTable* child = new SymbolTable(this);
    // Convert child->number to a string stored in buffer
    char buffer[10];
    sprintf(&buffer[0], "%d", child->number);
    // Use the number as ID for this symbol table
    // to identify all child symbol tables
    this->subscopes[buffer] = child;
    // Return the newly created symbol table
    return child;
}



// Adds the function name as symbol to the current table
// and creates a new empty table beneath the current one.
//
// Example: To enter the function "void add(int a, int b)",
//          call "currentSymbolTable->enterFunction("add", "void(int,int)");
SymbolTable* SymbolTable::enterFunction(string name, string signature,int filenr, int linenr, int offset) {
    // Add a new symbol using the signature as type
    this->addSymbol(name, signature,filenr,linenr,offset);
    // Create the child symbol table
    SymbolTable* child = new SymbolTable(this);
    // Store the symbol table under the name of the function
    // This allows us to retrieve the corresponding symbol table of a function
    // and the coresponding function of a symbol table.
    this->subscopes[name] = child;
    return child;
}

// Add a symbol with the provided name and type to the current table.
//
// Example: To add the variable declaration "int i = 23;"
//          use "currentSymbolTable->addSymbol("i", "int");
void SymbolTable::addSymbol(string name, string type, int filenr,int linenr, int offset) {
    // Use the variable name as key for the identifier mapping
    //printf("in addSymbol  %s, %s %p\n",name.c_str(), type.c_str(), &this->mapping);
    this->mapping[name] = type;
    this->mapfile[name] = filenr;
    this->mapline[name] = linenr;
    this->mapoffset[name] = offset;
}
void SymbolTable::addSymbol1(string name, string type) {
    // Use the variable name as key for the identifier mapping
    //printf("in addSymbol  %s, %s %p\n",name.c_str(), type.c_str(), &this->mapping);
    this->mapping[name] = type;
}


// Dumps the content of the symbol table and all its inner scopes
// depth denotes the level of indention.


//
// Example: "global_symtable->dump(symfile, 0)"
void SymbolTable::dump(FILE* symfile, int depth) {
    // Create a new iterator for <string,string>
    std::map<string,string>::iterator it;
    // Iterate over all entries in the identifier mapping
    for (it = this->mapping.begin(); it != this->mapping.end(); ++it) {
        // The key of the mapping entry is the name of the symbol
        const char* name = it->first.c_str();
        // The value of the mapping entry is the type of the symbol
        const char* type = it->second.c_str();
        // Print the symbol as "name {blocknumber} type"
        // indented by 3 spaces for each level
        fprintf(symfile, "%*s%s (%d, %d, %d){%d} %s\n", 3*depth, "", name, this->mapfile[name]-3, this->mapline[name], this->mapoffset[name], this->number, type);
        
        // If the symbol we just printed is actually a function
        // then we can find the symbol table of the function by the name
        if (this->subscopes.count(name) > 0) {
            // And recursively dump the functions symbol table
            // before continuing the iteration
            this->subscopes[name]->dump(symfile, depth + 1);
        }
    }
    // Create a new iterator for <string,SymbolTable*>
    std::map<string,SymbolTable*>::iterator i;
    // Iterate over all the child symbol tables
    for (i = this->subscopes.begin(); i != this->subscopes.end(); ++i) {
        // If we find the key of this symbol table in the symbol mapping
        // then it is actually a function scope which we already dumped above
        if (this->mapping.count(i->first) < 1) {
            // Otherwise, recursively dump the (non-function) symbol table
            i->second->dump(symfile, depth + 1);
        }
    }
}

// Look up name in this and all surrounding blocks and return its type.
//
// Returns the empty string "" if variable was not found
string SymbolTable::lookup(string name) {
    // Look up "name" in the identifier mapping of the current block
    if (this->mapping.count(name) > 0) {
        // If we found an entry, just return its type
        return this->mapping[name];
    }
    // Otherwise, if there is a surrounding scope
    if (this->parent != NULL) {
        // look up the symbol in the surrounding scope
        // and return its reported type
        return this->parent->lookup(name);
    } else {
        // Return "" if the global symbol table has no entry
        errprintf("Unknown identifier: %s\n", name.c_str());
        return "";
    }
}

// Looks through the symbol table chain to find the function which
// surrounds the scope and returns its signature
// or "" if there is no surrounding function.
//
// Use parentFunction(NULL) to get the parentFunction of the current block.
string SymbolTable::parentFunction(SymbolTable* innerScope) {
    // Create a new <string,SymbolTable*> iterator
    std::map<string,SymbolTable*>::iterator it;
    // Iterate over all the subscopes of the current scopes
    for (it = this->subscopes.begin(); it != this->subscopes.end(); ++it) {
        // If we find the requested inner scope and if its name is a symbol
        // in the identifier mapping
        if (it->second == innerScope && this->mapping.count(it->first) > 0) {
            // Then it must be the surrounding function, so return its type/signature
            return this->mapping[it->first];
        }
    }
    // If we did not find a surrounding function
    if (this->parent != NULL) {
        // Continue the lookup with the parent scope if there is one
        return this->parent->parentFunction(this);
    }
    // If there is no parent scope, return ""
    errprintf("Could not find surrounding function\n");
    return "";
}


SymbolTable* SymbolTable::leave(SymbolTable* innerScope){
    if(innerScope->parent != NULL){
    innerScope= innerScope->parent;
    return innerScope;
    }
    else{
        return innerScope;
    }
    
}

// initialize running block ID to 0
int SymbolTable::N(0);

// Parses a function signature and returns all types as vector.
// The first element of the vector is always the return type.
//
// Example: "SymbolTable::parseSignature("void(int,int)")
//          returns ["void", "int", "int"]
vector<string> SymbolTable::parseSignature(string sig) {
    // Initialize result string vector
    vector<string> results;
    // Find first opening parenthesis
    size_t left = sig.find_first_of('(');
    if (left == string::npos) {
        // Print error and return empty results if not a function signature
        errprintf("%s is not a function\n", sig.c_str());
        return results;
    }
    // Add return type
    results.push_back(sig.substr(0, left));
    // Starting with the position of the left parenthesis
    // Find the next comma or closing parenthesis at each step
    // and stop when the end is reached.
    for (size_t i = left + 1; i < sig.length()-1; i = sig.find_first_of(",)", i) + 1) {
        // At each step, get the substring between the current position
        // and the next comma or closing parenthesis
        results.push_back(sig.substr(i, sig.find_first_of(",)", i) - i));
    }
    return results;
}
