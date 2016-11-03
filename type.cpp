//
// Created by student on 11/2/16.
//

extern "C"
{
    #include "syntax.h"
}

#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

int main()
{
    freopen("/home/student/ClionProjects/cse340project4/tests/parser-test", "r", stdin);
    struct programNode* parseTree;
    parseTree = program();
    print_parse_tree(parseTree); // This is just for debugging purposes
    // TODO: do type checking & print output according to project specification

    //build premade types

    //Check for type errors

    return 0;
}


/**Scan parse tree to build symbol table**/
void scan_parse_tree(struct programNode* program)
{
    //scan declarations
    //scan body
}

void scan_decl(struct declNode* dec)
{
    //if type declaration section is not NULL
        //scan type declaration section
    //endif

    //if variable declaration section is not NULL
        //scan variable declaration section
    //endif
}
void scan_body(struct bodyNode* body)
{
    //scan statement list
}

/**Type declaration section**/
void scan_type_decl_section(struct type_decl_sectionNode* typeDeclSection)
{
    //if type declaration list is not NULL
        //scan type declaration list
    //endif
}

void scan_type_decl_list(struct type_decl_listNode* typeDeclList)
{
    //scan type declaration

    //if type declaration list is not NULL
        //scan type declaration list
    //endif
}

void scan_type_decl(struct type_declNode* typeDecl)
{
    //scan type name

    //if type name does not exist
        //create type with name ID, implicit
    //endif

    //scan type list
}

void scan_type_name(struct type_nameNode* typeName)
{
    //if type name is not an ID
        //get the reserved string
    //else, type is just an ID
        //get the ID as the type
    //endif

    //return type name
}

void scan_type_id_list(struct id_listNode* idList, string base_type)
{
    //get type name ID

    //if type does not exist
        //create type with ID and base type as explicit
        //add type to base type's equivalent types
    //else, type exists already; throw error
        //if found type is explicit, explicit type redeclared explicitly
            //throw error 1.1
        //else, found type is implicit, implicit type redeclared explicitly
            //throw error 1.2
        //endif
    //endif

    //if type ID list is not NULL
        //scan type ID list
    //endif
}


/**Variable declaration section**/
void scan_var_decl_section(struct var_decl_sectionNode* varDeclSection)
{
    //if variable declaration list is not NULL
        //scan variable declaration list
    //endif
}

void scan_var_decl_list(struct var_decl_listNode* varDeclList)
{
    //scan var declaration

    //if type declaration list is not NULL
        //scan type declaration list
    //endif
}

void scan_var_decl(struct var_declNode* varDecl)
{
    //scan type name

    //scan variable IDs
}

void scan_var_id_list(struct id_listNode* idList, string type)
{
    //get variable ID

    //if variable ID is not a type name
        //if variable does not exist
            //create variable with ID and type
            //add ID to type's equivalent names
        //else, variable is being declared more than once
            //throw error 2.1
        //endif

        //if variable ID list is not NULL
            //scan variable ID list
        //endif
    //else, type name is being declared as a variable name
        //throw throw error 1.3
    //endif
}

/**Program body**/
void scan_stmt_list(struct stmt_listNode* stmt_list)
{
    //scan statement

    //if statement list is not NULL
        //scan statement list
    //endif
}

void scan_stmt(struct stmtNode* stmt)
{
    //switch stmt
        //assign statement
            //scan assign statement
        //endcase
        //while statement
            //scan while statement
        //endcase
        //do statement
            //scan do statement
        //endcase
        //switch statement
            //scan switch statement
        //endcase
    //endswitch
}

void scan_assign_stmt(struct assign_stmtNode* assign_stmt)
{
    //get left hand side variable ID

    //if variable does not exist
        //scan expression to determine type
        //create type with ID and type
    //endif
}

void scan_expression_prefix(struct exprNode* expr)
{
    //if expression is tagged as expression
        //scan expression prefix left operand
        //scan expression prefix right operand

        //if left operand type is *%#%
            //set left operand type to right operand type
        //else if right operand type is *%#%
            //set right operand type to left operand type
        //else if left operand type is not equal to the right operand type
            //throw type mismatch error C2
        //endif

        //return left operand type as expression type
    //else if expression is tagged as primary
        //if primary tag is ID
            //if ID is not type or variable name
                //create variable with ID and type *%#%
            //endif
            //return variable type
        //else if primary is a NUM
            //return "INT"
        //else if primary is a REALNUM
            //return "REAL"
        //endif
    //endif
}

void scan_while_stmt(struct while_stmtNode* while_stmt)
{
    //scan condition
    //scan body
}

void scan_do_stmt(struct while_stmtNode* do_stmt)
{
    //scan body
    //scan condition
}

void scan_condition(struct conditionNode* condition)
{
    //if condition relop is NOOP, condition is just an ID
        //if left operand's ID is not a type or variable name
            //create variable with name ID and type BOOLEAN
        //else if left operand's ID is a type name, type name is used as variable
            //throw error 1.4
        //endif
    //else, condition is a comparison
        /**Get left operand type**/
        //if left operand's tag is ID
            //if left operand's ID is not a type or variable name
                //create variable with name ID and type *%#%
            //else if left operand's ID is a type name, type name is used as variable
                //throw error 1.4
            //endif
        //else if left operand's tag is NUM
            //left operand type is INT
        //else if left operand's tag is REALNUM
            //left operand type is REAL
        //endif

        /**Get right operand type**/
        //if right operand's tag is ID
            //if right operand's ID is not a type or variable name
                //create variable with name ID and type *%#%
            //else if left operand's ID is a type name, type name is used as variable
                //throw error 1.4
            //endif
        //else if right operand's tag is NUM
            //right operand type is INT
        //else if right operand's tag is REALNUM
            //right operand type is REAL
        //endif

        /**check for type equivalence**/
        //if left operand type is *%#%
            //set left operand type to right operand type
        //else if right operand type is *%#%
            //set right operand type to left operand type
        //else if left operand type is not equal to the right operand type
            //throw type mismatch error C2
        //endif
    //endif
}

void scan_case(struct caseNode* cas)
{
    //scan body
}

void scan_case_list(struct case_listNode* case_list)
{
    //scan case
    //if case list is not null
        //scan case list
    //endif
}

void scan_switch_stmt(struct switch_stmtNode* switc)
{
    //if switch ID is not a type or variable name
        //create variable with name ID and type INT
    //else if switch ID is a type name, type name is used as variable
        //throw error 1.4
    //else if switch ID is a variable name AND not an INT
        //throw type mismatch C5
    //endif

    //scan case list
}