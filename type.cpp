//
// Created by student on 11/2/16.
//

#include "syntax.h"
#include "type.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <string.h>

using namespace std;

vector<vari_type_t> types;
vector<vari_t> variables;
int found_vari;
int found_type;
int name_order = 0;

string ANONPREFIX = "__anon__";

int main()
{
    freopen("/home/student/ClionProjects/cse340project4/tests/semantic_test23.txt", "r", stdin);
    struct programNode* parseTree;
    parseTree = program();
    print_parse_tree(parseTree); // This is just for debugging purposes
    // TODO: do type checking & print output according to project specification

    //build premade types
    types.push_back(create_type("BOOLEAN", "BOOLEAN", EXPLICIT));
    types.push_back(create_type("INT", "INT", EXPLICIT));
    types.push_back(create_type("LONG", "LONG", EXPLICIT));
    types.push_back(create_type("REAL", "REAL", EXPLICIT));
    types.push_back(create_type("STRING", "STRING", EXPLICIT));

    //typecheck tree
    scan_parse_tree(parseTree);

    //print type list
    print_type_list();

    return 0;
}


/**Scan parse tree to build symbol table**/
void scan_parse_tree(struct programNode* program)
{
    //scan declarations
    scan_decl(program->decl);
    //scan body
    scan_body(program->body);
}

void scan_decl(struct declNode* dec)
{
    //if type declaration section is not NULL
    if(dec->type_decl_section != NULL)
    {
        //scan type declaration section
        scan_type_decl_section(dec->type_decl_section);
    }
    //endif

    //if variable declaration section is not NULL
    if(dec->var_decl_section != NULL)
    {
        //scan variable declaration section
        scan_var_decl_section(dec->var_decl_section);
    }
    //endif
}
void scan_body(struct bodyNode* body)
{
    //scan statement list
    scan_stmt_list(body->stmt_list);
}

/**Type declaration section**/
void scan_type_decl_section(struct type_decl_sectionNode* typeDeclSection)
{
    //if type declaration list is not NULL
    if(typeDeclSection->type_decl_list != NULL)
    {
        //scan type declaration list
        scan_type_decl_list(typeDeclSection->type_decl_list);
    }
    //endif
}

void scan_type_decl_list(struct type_decl_listNode* typeDeclList)
{
    //scan type declaration
    scan_type_decl(typeDeclList->type_decl);

    //if type declaration list is not NULL
    if(typeDeclList->type_decl_list != NULL)
    {
        //scan type declaration list
        scan_type_decl_list(typeDeclList->type_decl_list);
    }
    //endif
}

void scan_type_decl(struct type_declNode* typeDecl)
{
    //generate a temporary anonymous type name
    string anon_type = gen_anon_type();

    //scan type list
    scan_type_id_list(typeDecl->id_list, anon_type);

    //scan type name
    string type_name = scan_type_name(typeDecl->type_name);

    //if type name does not exist
    if(!find_type(type_name))
    {
        //create type with name ID, implicit
        vari_type_t new_type = create_type(type_name, type_name, IMPLICIT);

        //add type to types vector
        types.push_back(new_type);
    }
    //endif

    /**get the base type of the declaration**/
    //find the type
    find_type(type_name);
    string base_type = types[found_type].base_type;

    //change the types and base types of all new types created in this declaration
    for(int type = 0; type < types.size(); type++)
    {
        //if the current type's base type is the anonymous type
        if(types[type].base_type.compare(anon_type) == 0)
        {
            //change the base type
            types[type].base_type = type_name;

            //add the type to the base_type's eq_names list
            find_type(base_type);
            types[found_type].eq_names.push_back(types[type].name);
        }
    }

    //delete the anonymous type from the types vector
    if(find_type(anon_type))
    {
        types.erase(types.begin() + found_type);
    }

}

string scan_type_name(struct type_nameNode* typeName)
{
    //if type name is not an ID
    if(typeName->type != ID)
    {
        //return the reserved string as the type name
        return string(reserved[typeName->type]);
    }
    //else, type is just an ID
    else
    {
        //return the ID as the type name
        return string(typeName->id);
    }
    //endif
}

vari_type_t create_type(string type_name, string base_type, plicit_t plicity)
{
    //if type name is not a variable
    if(!find_vari(type_name))
    {
        //construct vari_type_t
        vari_type_t new_type = vari_type(type_name, base_type, plicity);
        new_type.order_pos = name_order;
        name_order++;

        /**add this type to the base type's eq_names vector**/
        //if type name is not the same as the base type
        if(type_name.compare(base_type) != 0)
        {
            //find base type in types vector
            find_type(base_type);
            //add type_name to end of base type's eq_names vector
            types[found_type].eq_names.push_back(type_name);
        }
        //endif

        //return new type
        return new_type;
    }
    //else, type name is already being used as a variable
    else
    {
        //throw error 2.2
        error_code("2.2", type_name);
    }
    //endif
}

void scan_type_id_list(struct id_listNode* idList, string base_type)
{
    //get type name ID
    string type_name = idList->id;

    //if type does not exist
    if(!find_type(type_name))
    {
        //create type with ID and base type as explicit
        vari_type_t new_type = create_type(type_name, base_type, EXPLICIT);
        //add new type to types vector
        types.push_back(new_type);
    }
    //else, type exists already; throw error
    else
    {
        //if found type is explicit, explicit type redeclared explicitly
        if(types[found_type].plicity == EXPLICIT)
        {
            //throw error 1.1
            error_code("1.1", type_name);
        }
        //else, found type is implicit, implicit type redeclared explicitly
        else
        {
            //throw error 1.2
            error_code("1.2", type_name);
        }
        //endif
    }
    //endif

    //if type ID list is not NULL
    if(idList->id_list != NULL)
    {
        //scan type ID list
        scan_type_id_list(idList->id_list, base_type);
    }
    //endif
}


/**Variable declaration section**/
void scan_var_decl_section(struct var_decl_sectionNode* varDeclSection)
{
    //if variable declaration list is not NULL
    if(varDeclSection->var_decl_list != NULL)
    {
        //scan variable declaration list
        scan_var_decl_list(varDeclSection->var_decl_list);
    }
    //endif
}

void scan_var_decl_list(struct var_decl_listNode* varDeclList)
{
    //scan var declaration
    scan_var_decl(varDeclList->var_decl);

    //if var declaration list is not NULL
    if(varDeclList->var_decl_list != NULL)
    {
        //scan var declaration list
        scan_var_decl_list(varDeclList->var_decl_list);
    }
    //endif
}

void scan_var_decl(struct var_declNode* varDecl)
{
    //generate anonymous type
    string anon_type = gen_anon_type();

    //scan variable IDs
    scan_var_id_list(varDecl->id_list, anon_type);

    //scan type name
    string var_type = scan_type_name(varDecl->type_name);

    //if type name does not exist
    if(!find_type(var_type))
    {
        //create type with name ID, implicit
        vari_type_t new_type = create_type(var_type, var_type, IMPLICIT);

        //add type to types vector
        types.push_back(new_type);
    }
    //endif

    //change the types of all new variables created in this declaration
    for(int vari = 0; vari < variables.size(); vari++)
    {
        //if the current variable's type is the anonymous type
        if(variables[vari].type.compare(anon_type) == 0)
        {
            //change the type
            variables[vari].type = var_type;

            //add the variable to the base_type's eq_names list
            find_type(var_type);
            find_type(types[found_type].base_type);
            types[found_type].eq_names.push_back(variables[vari].name);
        }
    }

    //delete the anonymous type from the types vector
    if(find_type(anon_type))
    {
        types.erase(types.begin() + found_type);
    }
}

void scan_var_id_list(struct id_listNode* idList, string type)
{
    //get variable ID
    string vari_name = string(idList->id);

    //if variable ID is not a type name
    if(!find_type(vari_name))
    {
        //if the variable type does not exist, create it implicitly
        if(!find_type(type))
        {
            //create the type
            vari_type_t new_type = create_type(type, type, IMPLICIT);
            //push it onto the types vector
            types.push_back(new_type);
        }

        //create new variable
        vari_t new_vari = create_vari(vari_name, type);

        //add new variable to variables vector
        variables.push_back(new_vari);

        //if variable ID list is not NULL
        if(idList->id_list != NULL)
        {
            //scan variable ID list
            scan_var_id_list(idList->id_list, type);
        }
        //endif
    }
    //else, type name is being declared as a variable name
    else
    {
        //throw throw error 1.3
        error_code("1.3", vari_name);
    }
    //endif
}

vari_t create_vari(string vari_name, string vari_type)
{
    //if variable name not already declared
    if(!find_vari(vari_name))
    {
        //construct vari_t
        vari_t new_vari = vari(vari_name, vari_type);
        new_vari.order_pos = name_order;
        name_order++;

        /**add this variable to the type's eq_names vector**/
        find_type(vari_type);
        types[found_type].eq_names.push_back(vari_name);

        /**add this variable to the base type's eq_names vector**/
        //if type name is not the same as the base type
        if(vari_type.compare(types[found_type].base_type) != 0)
        {
            //find base type in types vector
            find_type(types[found_type].base_type);
            //add vari_name to end of base type's eq_names vector
            types[found_type].eq_names.push_back(vari_name);
        }
        //endif

        //return new type
        return new_vari;
    }
    //else, variable is already declared
    else
    {
        //throw error 2.1
        error_code("2.1", vari_name);
    }
    //endif

}


/**Program body**/
void scan_stmt_list(struct stmt_listNode* stmt_list)
{
    //scan statement
    scan_stmt(stmt_list->stmt);

    //if statement list is not NULL
    if(stmt_list->stmt_list != NULL)
    {
        //scan statement list
        scan_stmt_list(stmt_list->stmt_list);
    }
    //endif
}

void scan_stmt(struct stmtNode* stmt)
{
    //switch stmt
    switch(stmt->stmtType)
    {
        //assign statement
        case ASSIGN:
            //scan assign statement
            scan_assign_stmt(stmt->assign_stmt);
            break;
        //endcase
        //while statement
        case WHILE:
            //scan while statement
            scan_while_stmt(stmt->while_stmt);
            break;
        //endcase
        //do statement
        case DO:
            //scan do statement
            scan_do_stmt(stmt->while_stmt);
            break;
        //endcase
        //switch statement
        case SWITCH:
            //scan switch statement
            scan_switch_stmt(stmt->switch_stmt);
            break;
        //endcase
    }
    //endswitch
}

void scan_assign_stmt(struct assign_stmtNode* assign_stmt)
{
    //get left hand side variable ID
    string left_vari = string(assign_stmt->id);

    //if variable does not exist
    if(!find_vari(left_vari))
    {
        string anon_type = gen_anon_type();
        //create variable with ID and unknown type
        vari_t new_vari = create_vari(left_vari, anon_type);
        //add variable to variables vector
        variables.push_back(new_vari);

        //scan expression to determine type
        scan_expression_prefix(assign_stmt->expr);
        string expr_type = assign_stmt->expr->type;

        //update the variable's type
        find_vari(left_vari);
        variables[found_vari].type = expr_type;

        //update the base type's eq_names list
        find_type(expr_type);
        find_type(types[found_type].base_type);
        types[found_type].eq_names.push_back(left_vari);

        //if new variables appeared in expression
        if(assign_stmt->expr->expr_varis.size() > 0)
        {
            //union expression variables vector with program variables vector
            variables = vector_union(variables, assign_stmt->expr->expr_varis);
        }
        //endif

    }
    //else, variable exists and we must check its type
    else
    {
        //scan expression to determine type
        scan_expression_prefix(assign_stmt->expr);
        string expr_type = assign_stmt->expr->type;

        //union expression variables vector with program variables vector, if any
        if(assign_stmt->expr->expr_varis.size() > 0)
        {
            variables = vector_union(variables, assign_stmt->expr->expr_varis);
        }

        //get base type of both types
        find_vari(left_vari);
        find_type(variables[found_vari].type);
        string vari_base_type = types[found_type].base_type;

        find_type(expr_type);
        string expr_base_type = types[found_type].base_type;

        //if the the base types are not the same and it's an assignment type violation
        if(vari_base_type.compare(expr_base_type) != 0)
        {
            //type mismatch C1
            type_mismatch(assign_stmt->line_num, "C1");
        }
        //endif
    }
    //endif
}

void scan_expression_prefix(struct exprNode* expr)
{
    //if expression is tagged as expression
    if(expr->tag == EXPR)
    {
        //scan expression prefix left operand
        scan_expression_prefix(expr->leftOperand);
        //scan expression prefix right operand
        scan_expression_prefix(expr->rightOperand);

        //if both have anonymous types
        if( (expr->leftOperand->type.find(ANONPREFIX) != string::npos)
            && (expr->rightOperand->type.find(ANONPREFIX) != string::npos)
            )
        {
            /**try to resolve the types**/
            //if they can't be resolved, types are incompatible
            if(!resolve_types(expr->leftOperand->type, expr->rightOperand->type))
            {
                //throw type mismatch C2
                type_mismatch(expr->line_num,"C2");
            }
        }
        //else if left operand type is anonymous, type could not be identified from context
        else if(expr->leftOperand->type.find(ANONPREFIX) != string::npos)
        {
            //set left operand type to right operand type
            expr->leftOperand->type = expr->rightOperand->type;

            //set all variables in the left expression to right operand's type
            for(int vari = 0; vari < expr->leftOperand->expr_varis.size(); vari++)
            {
                //if the variable is an anonymous type
                if(expr->leftOperand->expr_varis[vari].type.find(ANONPREFIX) != string::npos)
                {
                    //find and delete the anonymous type
                    if(find_type(expr->leftOperand->expr_varis[vari].type))
                    {
                        types.erase(types.begin() + found_type);
                    }

                    //set the variable's type to the type of the expression
                    expr->leftOperand->expr_varis[vari].type = expr->rightOperand->type;

                    //update variable in the variable vector
                    find_vari(expr->leftOperand->expr_varis[vari].name);
                    variables[found_vari].type = expr->rightOperand->type;

                    //add it to the type's eq_names list
                    find_type(expr->rightOperand->type);
                    types[found_type].eq_names.push_back(expr->leftOperand->expr_varis[vari].name);

                    //add it to the type's base type's eq_names list
                    find_type(types[found_type].base_type);
                    types[found_type].eq_names.push_back(expr->leftOperand->expr_varis[vari].name);
                }
                //endif
            }
            //endfor
        }
        //else if right operand type is anonymous, type could not be identified from context
        else if(expr->rightOperand->type.find(ANONPREFIX) != string::npos)
        {
            //set right operand type to left operand type
            expr->rightOperand->type = expr->leftOperand->type;

            //set all variables in the right expression to left operand's type
            for(int vari = 0; vari < expr->rightOperand->expr_varis.size(); vari++)
            {
                //if the variable is an anonymous type
                if(expr->rightOperand->expr_varis[vari].type.find(ANONPREFIX) != string::npos)
                {
                    //find and delete the anonymous type
                    if(find_type(expr->rightOperand->expr_varis[vari].type))
                    {
                        types.erase(types.begin() + found_type);
                    }

                    //set its type to the type of the expression
                    expr->rightOperand->expr_varis[vari].type = expr->leftOperand->type;

                    //update variable in the variable vector
                    find_vari(expr->rightOperand->expr_varis[vari].name);
                    variables[found_vari].type = expr->leftOperand->type;

                    //add it to the type's eq_names list
                    find_type(expr->leftOperand->type);
                    types[found_type].eq_names.push_back(expr->rightOperand->expr_varis[vari].name);

                    //add it to the type's base type's eq_names list
                    find_type(types[found_type].base_type);
                    types[found_type].eq_names.push_back(expr->rightOperand->expr_varis[vari].name);
                }
                //endif
            }
            //endfor
        }
        //else, the types are already defined and we need to check the base types
        else
        {
            string l_op_base_type;
            string r_op_base_type;

            /**get left operand base type**/
            //find operand type
            find_type(expr->leftOperand->type);
            //find operand base type
            find_type(types[found_type].base_type);
            //store base type
            l_op_base_type = types[found_type].name;

            /**get right operand base type**/
            //find operand type
            find_type(expr->rightOperand->type);
            //find operand base type
            find_type(types[found_type].base_type);
            //store base type
            r_op_base_type = types[found_type].name;

            //if the base types are different, figure out if they can be resolved
            if(l_op_base_type.compare(r_op_base_type) != 0)
            {
                //if types can't be resolved
                if(!resolve_types(expr->leftOperand->type, expr->rightOperand->type))
                {
                    //throw type mismatch error C2
                    type_mismatch(expr->line_num, "C2");
                }
                //endif
            }
        }
        //endif

        /**cleanup before returning up the tree**/
        //We should only get this far if all the types are the same

        /**union left and right operand variables vectors
        *set this expression's variable vector to the union**/
        //if left expression didn't contain any variables, use the right variables
        if(expr->leftOperand->expr_varis.size() == 0)
        {
            expr->expr_varis = expr->rightOperand->expr_varis;
        }
        //else if the right expression didn't contain any variables, use the left variables
        else if(expr->rightOperand->expr_varis.size() == 0)
        {
            expr->expr_varis = expr->leftOperand->expr_varis;
        }
        //else, both sides contain variables and we need to union the vectors
        else
        {
            expr->expr_varis = vector_union(expr->leftOperand->expr_varis, expr->rightOperand->expr_varis);
        }
        //endif

        //set expression type to left operand type
        expr->type = expr->leftOperand->type;
    }
    //else if expression is tagged as primary
    else if(expr->tag == PRIMARY)
    {
        //if primary tag is ID
        if(expr->primary->tag == ID)
        {
            string vari_name = string(expr->primary->id);
            //if var name is a type, type is being used as a variable
            if(find_type(vari_name))
            {
                //throw error 1.4
                error_code("1.4", vari_name);
            }
            //else if variable is already declared
            if(find_vari(vari_name))
            {
                //set expression node's type to variable's type
                expr->type = variables[found_vari].type;
            }
            //else, variable does not exist
            else
            {
                //create variable with ID and anonymous type
                string anon_type = gen_anon_type();
                vari_type_t new_type = create_type(anon_type, anon_type, IMPLICIT);
                types.push_back(new_type);

                //create variable with ID and anonymous type
                vari_t new_vari = create_vari(vari_name, anon_type);
                //set expression type to this anon type
                expr->type = anon_type;
                //add variable to expression variables vector
                expr->expr_varis.push_back(new_vari);
                //add variable to variables vector
                variables.push_back(new_vari);
            }
            //endif
        }
        //else if primary is a NUM
        else if(expr->primary->tag == NUM)
        {
            //set expression node's type to INT
            expr->type = "INT";
        }
        //else if primary is a REALNUM
        else if(expr->primary->tag == REALNUM)
        {
            //set expression node's type to REAL
            expr->type = "REAL";
        }
        //endif
    }
    //endif
}

void scan_while_stmt(struct while_stmtNode* while_stmt)
{
    //scan condition
    scan_condition(while_stmt->condition);
    //scan body
    scan_body(while_stmt->body);
}

void scan_do_stmt(struct while_stmtNode* do_stmt)
{
    //scan body
    scan_body(do_stmt->body);
    //scan condition
    scan_condition(do_stmt->condition);
}

void scan_condition(struct conditionNode* condition)
{
    //if condition relop is NOOP, condition is just an ID
    if(condition->relop == NOOP)
    {
        string cond_id = condition->left_operand->id;
        //if left operand's ID is a type name, type name is used as variable
        if(find_type(cond_id))
        {
            //throw error 1.4
            error_code("1.4", cond_id);
        }
        //else if ID name is variable and base type is not BOOLEAN
        else if(find_vari(cond_id))
        {
            find_type(variables[found_vari].type);
            if(types[found_type].base_type.compare("BOOLEAN") != 0)
            {
                //type mismatch C4
                type_mismatch(condition->line_num, "C4");
            }
        }
        //else, left operand's ID is not a type or variable name
        else
        {
            //create variable with name ID and type BOOLEAN
            vari_t new_vari = create_vari(cond_id, "BOOLEAN");
            //add variable to variables vector
            variables.push_back(new_vari);
        }
        //endif
    }
    //else, condition is a comparison
    else
    {
        string left_op_id;
        string right_op_id;
        string left_op_type;
        string right_op_type;
        string left_op_base_type;
        string right_op_base_type;

        /**Get left operand type**/
        //if left operand's tag is ID
        if(condition->left_operand->tag == ID)
        {
            left_op_id = condition->left_operand->id;
            //if left operand's ID is a type name, type name is used as variable
            if(find_type(left_op_id))
            {
                //throw error 1.4
                error_code("1.4", left_op_id);
            }
            //else if left operand is a variable
            else if(find_vari(left_op_id))
            {
                //get the type of the variable
                left_op_type = variables[found_vari].type;
                //find the base type of the variable
                find_type(left_op_type);
                left_op_base_type = types[found_type].base_type;
            }
            //else left operand's ID is not declared
            else
            {
                //create anonymous type
                string anon_type = gen_anon_type();
                vari_type_t new_type = create_type(anon_type, anon_type, IMPLICIT);
                types.push_back(new_type);
                left_op_type = anon_type;
                left_op_base_type = anon_type;

                //create variable with name ID and anonymous type
                vari_t new_vari = create_vari(left_op_id, anon_type);
                variables.push_back(new_vari);
            }
            //endif
        }
        //else if left operand's tag is NUM
        else if(condition->left_operand->tag == NUM)
        {
            //left operand type is INT
            left_op_type = "INT";
            left_op_base_type = "INT";
        }
        //else if left operand's tag is REALNUM
        else if(condition->left_operand->tag == NUM)
        {
            //left operand type is REAL
            left_op_type = "REAL";
            left_op_base_type = "REAL";
        }
        //endif

        /**Get right operand type**/
        if(condition->right_operand->tag == ID)
        {
            right_op_id = condition->right_operand->id;
            //if right operand's ID is a type name, type name is used as variable
            if(find_type(right_op_id))
            {
                //throw error 1.4
                error_code("1.4", right_op_id);
            }
            //else if right operand is a variable
            else if(find_vari(right_op_id))
            {
                //get the type of the variable
                right_op_type = variables[found_vari].type;
                //find the base type of the variable
                find_type(right_op_type);
                right_op_base_type = types[found_type].base_type;
            }
            //else right operand's ID is not declared
            else
            {
                //create anonymous type
                string anon_type = gen_anon_type();
                vari_type_t new_type = create_type(anon_type, anon_type, IMPLICIT);
                types.push_back(new_type);
                right_op_type = anon_type;
                right_op_base_type = anon_type;

                //create variable with name ID and anonymous type
                vari_t new_vari = create_vari(left_op_id, anon_type);
                variables.push_back(new_vari);
            }
            //endif
        }
        //else if right operand's tag is NUM
        else if(condition->right_operand->tag == NUM)
        {
            //left operand type is INT
            right_op_type = "INT";
            right_op_base_type = "INT";
        }
        //else if right operand's tag is REALNUM
        else if(condition->left_operand->tag == NUM)
        {
            //left operand type is REAL
            right_op_type = "REAL";
            right_op_base_type = "REAL";
        }
        //endif

        /**check for type equivalence**/
        //if both left and right operands are anonymous types
        if( (left_op_type.find(ANONPREFIX) != string::npos)
            &&(right_op_type.find(ANONPREFIX) != string::npos)
            )
        {
            //try to resolve the types
            if(!resolve_types(left_op_type, right_op_type))
            {
                //throw type mismatch C3
                type_mismatch(condition->line_num,"C3");
            }
        }
        //endif
        //else if left operand type is an anonymous type
        else if(left_op_type.find(ANONPREFIX) != string::npos)
        {
            //set left operand type to right operand type
            left_op_type = right_op_type;

            //find variable in variables vector and changes its type
            find_vari(left_op_id);
            variables[found_vari].type = left_op_type;
        }
        //else if right operand type is an anonymous type
        else if(right_op_type.find(ANONPREFIX) != string::npos)
        {
            //set right operand type to left operand type
            right_op_type = left_op_type;

            //find variable in variables vector and changes its type
            find_vari(right_op_id);
            variables[found_vari].type = right_op_type;
        }
        //else, try to resolve the types
        else
        {
            //if left operand base type is not equal to the right operand base type
            if(left_op_base_type.compare(right_op_base_type) != 0)
            {
                if (!resolve_types(left_op_type, right_op_type))
                {
                    //throw type mismatch error C3
                    type_mismatch(condition->line_num, "C3");
                }
            }
        }
        //endif
    }
    //endif
}

void scan_case(struct caseNode* cas)
{
    //scan body
    scan_body(cas->body);
}

void scan_case_list(struct case_listNode* case_list)
{
    //scan case
    scan_case(case_list->cas);
    //if case list is not null
    if(case_list->case_list != NULL)
    {
        //scan case list
        scan_case_list(case_list->case_list);
    }
    //endif
}

void scan_switch_stmt(struct switch_stmtNode* switc)
{
    string switch_id = switc->id;

    //if switch ID is a type name, type name is used as variable
    if(find_type(switch_id))
    {
        //throw error 1.4
        error_code("1.4", switch_id);
    }
    //else switch ID is a variable name
    else if(find_vari(switch_id))
    {
        //find variable's base type
        find_type(variables[found_vari].type);
        string base_type = types[found_type].base_type;

        //if base type is not INT, try to resolve type
        if(base_type.compare("INT") != 0)
        {
            //if types are incompatible
            if(!resolve_types("INT", types[found_type].name))
            {
                //throw type mismatch C5
                type_mismatch(switc->line_num, "C5");
            }
        }
    }
    //else, ID is a new variable that must be type INT
    else
    {
        //create variable with name ID and type INT
        vari_t new_vari = create_vari(switch_id, "INT");
        variables.push_back(new_vari);
    }
    //endif

    //scan case list
    scan_case_list(switc->case_list);
}

/**utility functions**/
void print_type_list()
{
    //for all types in type vector
    for(int type = 0; type < types.size(); type++)
    {
        //if type has not been printed
        if(!types[type].printed)
        {
            //print type
            print_type(&types[type]);
        }
        //endif
    }
    //endfor
}

void print_type(vari_type_t* type)
{
    //if type is not anonymous
    if(type->name.find(ANONPREFIX) == string::npos)
    {
        //print type name
        cout << type->name << " ";
    }
    //endif

    //sort eq_names based on appearance order
    sort(type->eq_names.begin(), type->eq_names.end(), compare_name_by_order);

    //for all equivalent names in type.eq_names
    for(int name = 0; name < type->eq_names.size(); name++)
    {
        //make sure symbol hasn't already been printed
        if((find_type(type->eq_names[name]) && !types[found_type].printed)
           || (find_vari(type->eq_names[name]) && !variables[found_vari].printed)
            )
        {
            //print name + " "
            cout << type->eq_names[name] << " ";
            //mark name as printed
            flag_printed(type->eq_names[name]);
        }
        //endif
    }
    //endfor

    //print terminating character
    cout << "#" << endl;

    //mark type as printed
    type->printed = true;
}

void flag_printed(string symbol)
{
    //if symbol is type
    if(find_type(symbol))
    {
        //flag type as printed
        types[found_type].printed = true;
    }
    //else if symbol is a variable
    else if(find_vari(symbol))
    {
        //flag variable as printed
        variables[found_vari].printed = true;
    }
    //else, symbol not recognized
    else
    {
        //print error; you done fucked up
        cout << "ERROR: Tried to flag an unrecognized symbol as printed." << endl;
    }
    //endif
}

bool find_type(string symbol)
{
    //lambda function predicate for searching types vector
    auto is_type_symbol = [symbol](const vari_type_t& type)
    {
        //if the symbol proviced from the argument is the same as the name stored in type
        if(symbol.compare(type.name) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    };

    //search types using std::find_if and is_type_symbol
    vector<vari_type_t>::iterator it = find_if(types.begin(), types.end(), is_type_symbol);
    //if the returned iterator does not equal the end of the vector, the symbol was found
    if(it != types.end())
    {
        //set found_type to position of found iterator
        found_type = distance(types.begin(), it);

        return true;
    }
    //else, symbol was not found and the symbol is not a known type
    else
    {
        return false;
    }
}

bool find_vari(string symbol)
{
    //lambda function predicate for searching varables vector
    auto is_vari_symbol = [symbol](const vari_t& variable)
    {
        //if the symbol proviced from the argument is the same as the name stored in type
        if(symbol.compare(variable.name) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    };

    //search types using std::find_if and is_type_symbol
    vector<vari_t>::iterator it = find_if(variables.begin(), variables.end(), is_vari_symbol);
    //if the returned iterator does not equal the end of the vector, the symbol was found
    if(it != variables.end())
    {
        //set found_vari to position of found iterator
        found_vari = distance(variables.begin(), it);

        return true;
    }
        //else, symbol was not found and the symbol is not a known variable
    else
    {
        return false;
    }
}

void error_code(string error, string symbol)
{
    cout << "ERROR CODE " << error << " " << symbol << endl;
    exit(1);
}

void type_mismatch(int line, string constraint)
{
    cout << "TYPE MISMATCH " << line << " " << constraint << endl;
    exit(1);
}

bool find_element(string symbol, vector<vari_t> vector1)
{
    //lambda function predicate for searching varables vector
    auto is_symbol = [symbol](const vari_t& element)
    {
        //if the symbol proviced from the argument is the same as the name stored in type
        if(symbol.compare(element.name) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    };

    //search types using std::find_if and is_type_symbol
    vector<vari_t>::iterator it = find_if(vector1.begin(), vector1.end(), is_symbol);

    //if the returned iterator does not equal the end of the vector, the symbol was found
    if(it != vector1.end())
    {
        return true;
    }
    //else, symbol was not found
    else
    {
        return false;
    }
}


vector<vari_t> vector_union(vector<vari_t> vector1, vector<vari_t> vector2)
{
    vector<vari_t> union_vector;     //vector that will store union of vector1 and vector2;

    //load vector 1 into the union vector
    union_vector.assign(vector1.begin(), vector1.end());

    //for all elements in vector2
    for(int element = 0; element < vector2.size(); element++)
    {
        //if element does not exist in union vector
        if(!find_element(vector2[element].name, union_vector))
        {
            //add element to end of union vector
            union_vector.push_back(vector2[element]);
        }
        //endif
    }
    //endfor

    return union_vector;
}

vector<string> vector_union(vector<string> vector1, vector<string> vector2)
{
    vector<string> union_vector(1);     //vector that will store union of vector1 and vector2;

    //load vector 1 into the union vector
    union_vector.assign(vector1.begin(), vector1.end());

    //for all elements in vector2
    for(int element = 0; element < vector2.size(); element++)
    {
        //if element does not exist in union vector
        if(find(union_vector.begin(), union_vector.end(), vector2[element]) == union_vector.end())
        {
            //add element to end of union vector
            union_vector.push_back(vector2[element]);
        }
        //endif
    }
    //endfor

    return union_vector;
}

//Helper function for sorting names
bool compare_name_by_order(const string a, const string b)
{
    int a_pos;
    int b_pos;

    if(find_type(a))
    {
        a_pos = types[found_type].order_pos;
    }
    else if(find_vari(a))
    {
        a_pos = variables[found_vari].order_pos;
    }

    if(find_type(b))
    {
        b_pos = types[found_type].order_pos;
    }
    else if(find_vari(b))
    {
        b_pos = variables[found_vari].order_pos;
    }

    if (a_pos < b_pos)
    {
        return true;
    }
    else
    {
        return false;
    }
}

string gen_anon_type()
{
    return ANONPREFIX + to_string(rand());
}

bool resolve_types(string lt, string rt)
{
    //store positions of types and base types
    find_type(lt);
    int lti = found_type;
    string lbt = types[lti].base_type;
    find_type(lbt);
    int lbti = found_type;

    find_type(rt);
    int rti = found_type;
    string rbt = types[rti].base_type;
    find_type(rbt);
    int rbti = found_type;

    //if left operand base type is BOOLEAN, INT, LONG, REAL, or STRING
    //and right operand is not
    if(     (  (lbt.compare("BOOLEAN") == 0)
               || (lbt.compare("INT") == 0)
               || (lbt.compare("LONG") == 0)
               || (lbt.compare("REAL") == 0)
               || (lbt.compare("STRING") == 0)
            )
            &&
            (  (rbt.compare("BOOLEAN") != 0)
               && (rbt.compare("INT") != 0)
               && (rbt.compare("LONG") != 0)
               && (rbt.compare("REAL") != 0)
               && (rbt.compare("STRING") != 0)
            )
            )
    {

        /**add right operand's base type and right operand's base type's eq_names
        * to left operand's base type's eq_names**/
        //ad
        types[lbti].eq_names.push_back(types[rti].base_type);


        //if left operator's base type eq_names is empty
        if(types[lbti].eq_names.size() == 0)
        {
            //copy the right operand's vector
            types[lbti].eq_names.assign(types[rbti].eq_names.begin(), types[rbti].eq_names.end());
        }
        //else if the right operator's base type eq_names is not empty
        else if(types[rbti].eq_names.size() != 0)
        {
            //union the vectors
            types[lbti].eq_names = vector_union(types[lbti].eq_names, types[rbti].eq_names);
        }

        //make right operand's base type left operand's base type
        types[rbti].base_type = lbt;

        //TODO: index through all names and make sure to set base types of types
        /**index through all names and make sure to set base types of types**/

        /**index through all names and make sure to set base types of types**/
        for(int name = 0; name < types[lbti].eq_names.size(); name++)
        {
            //if name is a type
            if(find_type(types[lbti].eq_names[name]))
            {
                types[found_type].base_type = lbt;
            }
        }


        return true;
    }
    //else if right operand base type is BOOLEAN, INT, LONG, REAL, STRING
    //and left operand is not
    else if((  (rbt.compare("BOOLEAN") == 0)
               || (rbt.compare("INT") == 0)
               || (rbt.compare("LONG") == 0)
               || (rbt.compare("REAL") == 0)
               || (rbt.compare("STRING") == 0)
            )
            &&
            (  (lbt.compare("BOOLEAN") != 0)
               && (lbt.compare("INT") != 0)
               && (lbt.compare("LONG") != 0)
               && (lbt.compare("REAL") != 0)
               && (lbt.compare("STRING") != 0)
            )
            )
    {

        /**add left operand's base type and left operand's base type's eq_names
         * to right operand's base type's eq_names**/
        types[rbti].eq_names.push_back(types[lti].base_type);

        //if right operator's base type eq_names is empty
        if(types[rbti].eq_names.size() == 0)
        {
            //copy the left operand's vector
            types[rbti].eq_names.assign(types[lbti].eq_names.begin(),
                                                      types[lbti].eq_names.end());
        }
        //else if the left operator's base type eq_names is not empty
        else if(types[lbti].eq_names.size() != 0)
        {
            //union the vectors
            types[rbti].eq_names = vector_union(types[rbti].eq_names,
                                                types[lbti].eq_names);
        }

        //make left operand's base type right operand's base type
        types[lti].base_type = rbt;

        //TODO: index through all names and make sure to set base types of types
        /**index through all names and make sure to set base types of types**/

        for(int name = 0; name < types[rbti].eq_names.size(); name++)
        {
            //if name is a type
            if(find_type(types[rbti].eq_names[name]))
            {
                types[found_type].base_type = rbt;
            }
        }

        return true;
    }
    //else, if neither type can be resolved to a built-in type, resolve to the "older" type
        //this should help condense anonymous types
    else if((  !(rbt.compare("BOOLEAN") == 0)
               || (rbt.compare("INT") == 0)
               || (rbt.compare("LONG") == 0)
               || (rbt.compare("REAL") == 0)
               || (rbt.compare("STRING") == 0)
            )
            &&
            (  (lbt.compare("BOOLEAN") != 0)
               && (lbt.compare("INT") != 0)
               && (lbt.compare("LONG") != 0)
               && (lbt.compare("REAL") != 0)
               && (lbt.compare("STRING") != 0)
            )
            )
    {
        //if left base type came first, resolve to left base type
        if(types[lbti].order_pos < types[rbti].order_pos)
        {
            /**add right operand's base type and right operand's base type's eq_names
            * to left operand's base type's eq_names**/
            //ad
            types[lbti].eq_names.push_back(types[rti].base_type);


            //if left operator's base type eq_names is empty
            if(types[lbti].eq_names.size() == 0)
            {
                //copy the right operand's vector
                types[lbti].eq_names.assign(types[rbti].eq_names.begin(), types[rbti].eq_names.end());
            }
                //else if the right operator's base type eq_names is not empty
            else if(types[rbti].eq_names.size() != 0)
            {
                //union the vectors
                types[lbti].eq_names = vector_union(types[lbti].eq_names, types[rbti].eq_names);
            }

            //make right operand's base type left operand's base type
            types[rbti].base_type = lbt;

            //TODO: index through all names and make sure to set base types of types
            /**index through all names and make sure to set base types of types**/
            for(int name = 0; name < types[lbti].eq_names.size(); name++)
            {
                //if name is a type
                if(find_type(types[lbti].eq_names[name]))
                {
                    types[found_type].base_type = lbt;
                }
            }


            return true;
        }
        //else, right base type came first, resolve to right base type
        else
        {
            /**add left operand's base type and left operand's base type's eq_names
            * to right operand's base type's eq_names**/
            types[rbti].eq_names.push_back(types[lti].base_type);

            //if right operator's base type eq_names is empty
            if(types[rbti].eq_names.size() == 0)
            {
                //copy the left operand's vector
                types[rbti].eq_names.assign(types[lbti].eq_names.begin(),
                                            types[lbti].eq_names.end());
            }
                //else if the left operator's base type eq_names is not empty
            else if(types[lbti].eq_names.size() != 0)
            {
                //union the vectors
                types[rbti].eq_names = vector_union(types[rbti].eq_names,
                                                    types[lbti].eq_names);
            }

            //make left operand's base type right operand's base type
            types[lti].base_type = rbt;

            //TODO: index through all names and make sure to set base types of types
            /**index through all names and make sure to set base types of types**/
            for(int name = 0; name < types[rbti].eq_names.size(); name++)
            {
                //if name is a type
                if(find_type(types[rbti].eq_names[name]))
                {
                    types[found_type].base_type = rbt;
                }
            }

            return true;
        }
    }
    //else, base type mismatch
    else
    {
        return false;
    }
}