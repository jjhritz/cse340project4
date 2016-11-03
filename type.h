//
// Created by student on 11/2/16.
//

#ifndef CSE340PROJECT4_TYPE_H
#define CSE340PROJECT4_TYPE_H

#include <vector>
#include <string>

using namespace std;

typedef enum
{
    IMPLICIT,
    EXPLICIT
}plicit_t;

typedef struct vari_type
{
    string name;
    vector<string> eq_types;
    vector<string> eq_names;
    plicit_t plicity;
    bool printed = false;

    vari_type(string name)
    {
        this->name = name;
    }

    vari_type(string name, plicit_t plicity)
    {
        this->name = name;
        this->plicity = plicity;
    }

}vari_type_t;

typedef struct vari
{
    string name;
    string type;
    bool printed = false;

    vari(string name)
    {
        this->name = name;
    }

    vari(string name, string type)
    {
        this->name = name;
        this->type = type;
    }

}vari_t;

vector<vari_type_t> types;
vector<vari_t> variables;

/**Scan parse tree**/
void scan_parse_tree(struct programNode* program);

void scan_decl(struct declNode* dec);
void scan_body(struct bodyNode* body);

void scan_type_decl_section(struct type_decl_sectionNode* typeDeclSection);
void scan_type_decl_list(struct type_decl_listNode* typeDeclList);
void scan_type_decl(struct type_declNode* typeDecl);

void scan_var_decl_section(struct var_decl_sectionNode* varDeclSection);
void scan_var_decl_list(struct var_decl_listNode* varDeclList);
void scan_var_decl(struct var_declNode* varDecl);
void scan_type_name(struct type_nameNode* typeName);
void scan_type_id_list(struct id_listNode* idList);
void scan_var_id_list(struct id_listNode* idList);

void scan_stmt_list(struct stmt_listNode* stmt_list);
void scan_stmt(struct stmtNode* stmt);
void scan_assign_stmt(struct assign_stmtNode* assign_stmt);
void scan_expression_prefix(struct exprNode* expr);
void scan_while_stmt(struct while_stmtNode* while_stmt);
void scan_do_stmt(struct while_stmtNode* do_stmt);
void scan_condition(struct conditionNode* condition);
void scan_case(struct caseNode* cas);
void scan_case_list(struct case_listNode* case_list);
void scan_switch_stmt(struct switch_stmtNode* switc);


/**Check parse tree for type errors**/
void check_parse_tree(struct programNode* program);

void check_decl(struct declNode* dec);
void check_body(struct bodyNode* body);

void check_type_decl_section(struct type_decl_sectionNode* typeDeclSection);
void check_type_decl_list(struct type_decl_listNode* typeDeclList);
void check_type_decl(struct type_declNode* typeDecl);

void check_var_decl_section(struct var_decl_sectionNode* varDeclSection);
void check_var_decl_list(struct var_decl_listNode* varDeclList);
void check_var_decl(struct var_declNode* varDecl);
void check_type_name(struct type_nameNode* typeName);
void check_id_list(struct id_listNode* idList);

void check_stmt_list(struct stmt_listNode* stmt_list);
void check_stmt(struct stmtNode* stmt);
void check_assign_stmt(struct assign_stmtNode* assign_stmt);
void check_expression_prefix(struct exprNode* expr);
void check_while_stmt(struct while_stmtNode* while_stmt);
void check_do_stmt(struct while_stmtNode* do_stmt);
void check_condition(struct conditionNode* condition);
void check_case(struct caseNode* cas);
void check_case_list(struct case_listNode* case_list);
void check_switch_stmt(struct switch_stmtNode* switc);

#endif //CSE340PROJECT4_TYPE_H
