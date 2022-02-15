/* Complete recursive descent parser for the calculator language.
   Builds on figure 2.16 in the text.  Prints a trace of productions
   predicted and tokens matched.  Does no error recovery: prints
   "syntax error" and dies on invalid input.
   Michael L. Scott, 2008-2021.
*/

#include <iostream>
#include <vector>
#include <regex>

#include "scan.h"
using namespace std;

const string names[] = {"read", "write", "if", "do", "fi", "od" , "check",
                        "id", "literal", "gets", "eq", "neq",
                        "less", "great", "less_eq", "great_eq",
                        "add", "sub", "mul", "div", "lparen", "rparen", "eof"};

static token upcoming_token;
string syntax_tree = "";

vector<token> P = {t_id, t_read, t_write, t_if, t_do, t_check, t_eof};
vector<token> SL {t_id, t_read, t_write, t_if, t_do, t_check};
vector<token> S {t_id, t_read, t_write, t_if, t_do, t_check};
vector<token> R {t_lparen, t_id, t_literal};
vector<token> E {t_lparen, t_id, t_literal};
vector<token> T {t_lparen, t_id, t_literal};
vector<token> F {t_lparen, t_id, t_literal};
vector<token> ET {t_eq, t_neq, t_less, t_great, t_less_eq, t_great_eq};
vector<token> TT {t_add, t_sub};
vector<token> FT {t_mul, t_div};
vector<token> ro {t_eq, t_neq, t_less, t_great, t_less_eq, t_great_eq};
vector<token> ao {t_add, t_sub};
vector<token> mo {t_mul, t_div};

bool epsP = false;
bool epsSL = true;
bool epsS = false;
bool epsR = false;
bool epsE = false;
bool epsT = false;
bool epsF = false;
bool epsET = true;
bool epsTT = true;
bool epsFT = true;
bool eps_ro = false;
bool eps_ao = false;
bool eps_mo = false;

vector<token> P_follow {};
vector<token> SL_follow {t_fi, t_od, t_eof};
vector<token> S_follow {t_id, t_read, t_write, t_if, t_do, t_check, t_fi, t_od, t_eof};
vector<token> R_follow {t_rparen, t_id, t_read, t_write, t_if, t_do, t_check, t_fi, t_od, t_eof};
vector<token> E_follow {t_eq, t_neq, t_less, t_great, t_less_eq, t_great_eq, t_rparen,
                    t_id, t_read, t_write, t_if, t_do, t_check, t_fi, t_od, t_eof};
vector<token> T_follow {t_add, t_sub, t_eq, t_neq, t_less, t_great, t_less_eq, t_great_eq, t_rparen,
                    t_id, t_read, t_write, t_if, t_do, t_check, t_fi, t_od, t_eof};
vector<token> F_follow {t_mul, t_div, t_add, t_sub, t_eq, t_neq, t_less, t_great, t_less_eq, t_great_eq, t_rparen,
                    t_id, t_read, t_write, t_if, t_do, t_check, t_fi, t_od, t_eof};
vector<token> ET_follow {t_rparen, t_id, t_read, t_write, t_if, t_do, t_check, t_fi, t_od, t_eof};
vector<token> TT_follow {t_literal,t_eq, t_neq, t_less, t_great, t_less_eq, t_great_eq, t_rparen,
                    t_id, t_read, t_write, t_if, t_do, t_check, t_fi, t_od, t_eof};
vector<token> FT_follow {t_add, t_sub, t_eq, t_neq, t_less, t_great, t_less_eq, t_great_eq, t_rparen,
                    t_id, t_read, t_write, t_if, t_do, t_check, t_fi, t_od, t_eof};
vector<token> ro_follow {t_lparen, t_id, t_literal};
vector<token> ao_follow {t_lparen, t_id, t_literal};
vector<token> mo_follow {t_lparen, t_id, t_literal};

void error () {
    throw new exception();
}

void make_error_report (token a) {
    cout << "symbol not valid: "<< names[a] << "\n";
}

void match (token expected) {
    if (upcoming_token == expected) {        
        cout << "matched" << names[upcoming_token];
        if (upcoming_token == t_id || upcoming_token == t_literal)
            cout << ": " << token_image;
        cout <<"\n";
        syntax_tree += names[expected] + " ";
        if (expected != t_eof){
            upcoming_token = scan ();
        }
    }
    else error ();
}


bool in (token given_token, vector<token> l) {
    int a = l.size();
    for (int i=0; i < a; i++) {
        if (given_token == l[i]) {
            return true;
        }
    }
    return false;
}

bool has_error (vector<token> first, bool eps, vector<token> follow) {
    // 'match' epsilon
    if (!in(upcoming_token, first) && eps) {
        return true;
    }

    if (in(upcoming_token, first)) {
        return false;
    }

    make_error_report(upcoming_token);
    do {
        upcoming_token = scan();
    }
    while (!(in(upcoming_token, first) || in(upcoming_token, follow)));

    if (in(upcoming_token, first)) {
        return false;
    } else {
        return true;
    }
}

void error_recovery (vector<token> follow) {
    do {
        upcoming_token = scan();
    } while (!(in(upcoming_token,follow)));
}

void program ();
void stmt_list ();
void stmt ();
void rel ();
void expr ();
void expr_tail ();
void term_tail ();
void term ();
void factor_tail ();
void factor ();
void r_op();
void add_op ();
void mul_op ();

void program () {
    if (has_error (P, epsP, P_follow)){
        program();
    }
    syntax_tree += "(";

    switch (upcoming_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_do:
        case t_if:
        case t_check:
        case t_eof:
            cout << "predict program --> stmt_list eof\n";
            stmt_list ();
            
            try{
            match (t_eof); 
            } catch(exception* e) {
                    if (has_error(vector<token> {t_eof}, false, P_follow))
                        return;
                }
            break;
        default: return;
    }

    syntax_tree += ")";
    regex pattern("\\(\\)");
    syntax_tree = regex_replace(syntax_tree, pattern, "");
    cout << syntax_tree << endl;
}

void stmt_list () {
    if (has_error (SL, epsSL, SL_follow)){
        return;
    }
    syntax_tree += "(";
    switch (upcoming_token) {
        case t_id:
        case t_read:
        case t_if:
        case t_do:
        case t_check:
        case t_write:
            cout << "predict stmt_list --> stmt stmt_list\n";
            stmt ();
            stmt_list ();
            break;
        default: return;

        // epsilon
    }
    syntax_tree += ")";
}

void stmt () {
    if (has_error (S, epsS, S_follow)){
        return;
    }
    syntax_tree += "(";
    switch (upcoming_token) {
        case t_id:
            cout << "predict stmt --> id gets expr\n";
            
            try{
                match (t_id);
            } catch(exception* e) {
                if (has_error(vector<token> {t_id}, false, S_follow))
                    return;
                else
                    match (t_id);
            }

            try{
                match (t_gets);
            } catch(exception* e) {
                if (has_error(vector<token> {t_gets}, false, S_follow))
                    return;
                else
                    match (t_gets);
            }

            expr ();
            break;
        case t_read:
            cout << "predict stmt --> read id\n";
            
            try{
                match (t_read);
            } catch(exception* e) {
                if (has_error(vector<token> {t_read}, false, S_follow))
                    return;
                else
                    match (t_read);
            }

            try{
                match (t_id);
            } catch(exception* e) {
                if (has_error(vector<token> {t_id}, false, S_follow))
                    return;
                else
                    match (t_id);
            }

            break;
        case t_write:
            cout << "predict stmt --> write rel\n";
            
            try{
                match (t_write);
            } catch(exception* e) {
                if (has_error(vector<token> {t_write}, false, S_follow))
                    return;
                else
                    match (t_write);
            }

            rel ();
            break;
        case t_if:
            cout << "predict stmt --> if rel stmt_list fi\n";
            
            try{
                match (t_if);
            } catch(exception* e) {
                if (has_error(vector<token> {t_if}, false, S_follow))
                    return;
                else
                    match (t_if);
            }

            rel ();
            stmt_list ();
            
            try{
                match (t_fi);
            } catch(exception* e) {
                if (has_error(vector<token> {t_fi}, false, S_follow))
                    return;
                else
                    match (t_fi);
            }

            break;
        case t_do:
            cout << "predict stmt --> do stmt_list od\n";
            
            try{
                match (t_do);
            } catch(exception* e) {
                if (has_error(vector<token> {t_do}, false, S_follow))
                    return;
                else
                    match (t_do);
            }

            stmt_list ();
            
            try{
                match (t_od);
            } catch(exception* e) {
                if (has_error(vector<token> {t_od}, false, S_follow))
                    return;
                else
                    match (t_od);
            }

            break;
        case t_check:
            cout << "predict stmt --> check rel\n";
            
            try{
                match (t_check);
            } catch(exception* e) {
                if (has_error(vector<token> {t_check}, false, S_follow))
                    return;
                else
                    match (t_check);
            }

            rel ();
            break;
        default: return;
    }
    syntax_tree += ")";
}

void rel () {
    if(has_error (R, epsR, R_follow)){
        return;
    }
    syntax_tree += "(";
    cout << "predict rel --> expr expr_tail\n";
    expr ();
    expr_tail ();
    syntax_tree += ")";
}

void expr () {
    if (has_error (E, epsE, E_follow)){
        return;
    }
    syntax_tree += "(";
    cout << "predict expr --> term term_tail\n";
    term ();
    term_tail ();
    syntax_tree += ")";
}

void term () {
    if (has_error (T, epsT, T_follow)){
        return;
    }
    syntax_tree += "(";
    cout << "predict term --> factor factor_tail\n";
    factor ();
    factor_tail ();
    syntax_tree += ")";
}

void factor () {
    if (has_error (F, epsF, F_follow)){
        return;
    }
    syntax_tree += "(";

    switch (upcoming_token) {
        case t_lparen:
            cout << "predict factor --> lparen rel rparen\n";
            
            try{
                match (t_lparen);
            } catch(exception* e) {
                if (has_error(vector<token> {t_lparen}, false, F_follow))
                    return;
                else
                    match (t_lparen);
            }
            
            rel ();
            
            try{
                match (t_rparen);
            } catch(exception* e) {
                if (has_error(vector<token> {t_rparen}, false, F_follow))
                    return;
                else
                    match (t_rparen);
            }
            
            break;
        case t_id:
            cout << "predict factor --> id\n";
            
            try{
                match (t_id);
            } catch(exception* e) {
                if (has_error(vector<token> {t_id}, false, F_follow))
                    return;
                else
                    match (t_id);
            }

            break;
        case t_literal:
            cout << "predict factor --> literal\n";
            
            try{
                match (t_literal);
            } catch(exception* e) {
                if (has_error(vector<token> {t_literal}, false, F_follow))
                    return;
                else
                    match (t_literal);
            }

            break;
        default: return;
    }
    syntax_tree += ")";
}

void expr_tail () {
    if (has_error (ET, epsET, ET_follow)){
        return;
    }
    syntax_tree += "(";
    switch (upcoming_token) {
        case t_eq:
        case t_neq:
        case t_less:
        case t_great:
        case t_less_eq:
        case t_great_eq:
            cout << "predict expr_tail --> relational_op expr\n";
            r_op ();
            expr ();
            break;
        default: return;
        // epsilon

    }
    syntax_tree += ")";
}

void term_tail () {
    if (has_error (TT, epsTT, TT_follow)){
        return;
    }
    syntax_tree += "(";
    switch (upcoming_token) {
        case t_add:
        case t_sub:
            cout << "predict term_tail --> add_op term term_tail\n";
            add_op ();
            term ();
            term_tail ();
            break;
        default: return;
        // epsilon
    }
    syntax_tree += ")";
}

void factor_tail () {
    if (has_error (FT, epsFT, FT_follow)){
        return;
    }
    syntax_tree += "(";
    switch (upcoming_token) {
        case t_mul:
        case t_div:
            cout << "predict factor_tail --> mul_op factor factor_tail\n";
            mul_op ();
            factor ();
            factor_tail ();
            break;
        default: return;
        // epsilon
    }
    syntax_tree += ")";
}

void r_op () {
    if (has_error (ro, eps_ro, ro_follow)){
        return;
    }
    syntax_tree += "(";

    switch (upcoming_token) {
        case t_eq:
            cout << "predict r_op --> eq\n";
            
            try{
                match (t_eq);
            } catch(exception* e) {
                    if (has_error(vector<token> {t_eq}, false, ro_follow))
                        return;
                    else
                    match (t_eq);
                }

            break;
        case t_neq:
            cout << "predict r_op --> neq\n";
            
            try{
                match (t_neq);
            } catch(exception* e) {
                    if (has_error(vector<token> {t_neq}, false, ro_follow))
                        return;
                    else
                    match (t_neq);
                }

            break;
        case t_less:
            cout << "predict r_op --> less\n";
            
            try{
                match (t_less);
            } catch(exception* e) {
                    if (has_error(vector<token> {t_less}, false, ro_follow))
                        return;
                    else
                    match (t_less);
                }

            break;
        case t_great:
            cout << "predict r_op --> great\n";
            
            try{
                match (t_great);
            } catch(exception* e) {
                    if (has_error(vector<token> {t_great}, false, ro_follow))
                        return;
                    else
                    match (t_great);
                }

            break;
        case t_less_eq:
            cout << "predict r_op --> less_eq\n";
            
            try{
                match (t_less_eq);
            } catch(exception* e) {
                    if (has_error(vector<token> {t_less_eq}, false, ro_follow))
                        return;
                    else
                    match (t_less_eq);
                }

            break;
        case t_great_eq:
            cout << "predict r_op --> great_eq\n";
            
            try{
                match (t_great_eq);
            } catch(exception* e) {
                    if (has_error(vector<token> {t_great_eq}, false, ro_follow))
                        return;
                    else
                    match (t_great_eq);
                }

            break;
        default: return;
    }
    
    syntax_tree += ")";
}

void add_op () {
    if (has_error (ao, eps_ao, ao_follow)){
        return;
    }
    syntax_tree += "(";
    switch (upcoming_token) {
        case t_add:
            cout << "predict add_op --> add\n";
            
            try{
                match (t_add);
            } catch(exception* e) {
                    if (has_error(vector<token> {t_add}, false, ao_follow))
                        return;
                }

            break;
        case t_sub:
            cout << "predict add_op --> sub\n";
            
            try{
                match (t_sub);
            } catch(exception* e) {
                    if (has_error(vector<token> {t_sub}, false, ao_follow))
                        return;
                }

            break;
        default: return;
    }

    syntax_tree += ")";
}

void mul_op () {
    if (has_error (mo, eps_mo, mo_follow)){
        return;
    }
    syntax_tree += "(";
    switch (upcoming_token) {
        case t_mul:
            cout << "predict mul_op --> mul\n";
            
            try{
                match (t_mul);
            } catch(exception* e) {
                    if (has_error(vector<token> {t_mul}, false, mo_follow))
                        return;
                }

            break;
        case t_div:
            cout << "predict mul_op --> div\n";
            
            try{
                match (t_div);
            } catch(exception* e) {
                    if (has_error(vector<token> {t_div}, false, mo_follow))
                        return;
                }

            break;
        default: return;
    }
    
    syntax_tree += ")";
}

int main () {
    upcoming_token = scan ();
    program ();    
}
