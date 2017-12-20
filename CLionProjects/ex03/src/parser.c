#include "compiler.h"

int token;
int iteration_flag = 0;

char *tokenstr[NUMOFTOKEN+1] = {
		"",
		"NAME", "program", "var", "array", "of", "begin", "end", "if", "then",
		"else", "procedure", "return", "call", "while", "do", "not", "or",
		"div", "and", "char", "integer", "boolean", "readln", "writeln", "true",
		"false", "NUMBER", "STRING", "+", "-", "*", "=", "<>", "<", "<=", ">",
		">=", "(", ")", "[", "]", ":=", ".", ",", ":", ";", "read","write", "break"
};

int parse_program(){//ahead_scan
    init_namelist();
    init_globalidtab();
    init_localidtab();

	/*progarm 名前 ; ブロック.*/
	token = scan();
	pretty_print();
	if(token != TPROGRAM)
		return error_parse("[Parse_program]Keyword 'program' is not found.");
	token = scan();
	pretty_print();
	if(token != TNAME)
		return error_parse("[Parse_program]Program name is not found.");
	token = scan();
	pretty_print();
	if(token != TSEMI)
		return error_parse("[Parse_program]';' is not found.");
	token = scan();
	pretty_print();
	if(block() == ERROR){
		printf("[INFO]Block Final read:'%s'\n",tokenstr[token]);
		return ERROR;
	}
	if(token != TDOT)
		return error_parse("[Parse_program]'.' is not found at the end of program.");
	token = scan();
	pretty_print();
	if(token > 0){
		printf("\n\n[Result]Final read:'%s'\n",tokenstr[token]);
	}
	else{
		printf("\n\n[Result]Final read:EOF\n");
	}
    print_idlist(local);
    print_idlist(global);
	return NORMAL;
}

int block(){
	/*{変数宣言部 | 副プログラム宣言}複合文*/
	while((variable_declaration() != ERROR) || (subprogram_declaration() != ERROR)){
        reset_flags();
    }
    if (compound_statement() == ERROR)
		return error_parse("[Block]Compound_statement is not found.");
	return NORMAL;
}
//todo
int is_para = var;
int scope = global;
int array_size;
char procname[MAXSTRSIZE];

int variable_declaration(){//ahead_scan
	/*var 変数名の並び : 型 ; {変数名の並び : 型 ;}*/
    int mem_type;

	if(token != TVAR)
		return ERROR;
	token = scan();
	pretty_print();
	do{
		if(variable_names() == ERROR)
			return error_parse("[Variable_declation]Variable_names is not found.");
		if(token != TCOLON)
			return error_parse("[Variable_declation]':' is not found.");
		token = scan();
		pretty_print();
        mem_type = type();
		if(mem_type == ERROR)
			return error_parse("[Variable_declation]Type is not found.");
		if(token != TSEMI)
			return error_parse("[Variable_declation]';' is not found.");

        if((insert_idlist(procname,mem_type,is_para,scope,array_size)) == ERROR){
            return error_variable("[Variable_declation]Overload.");
        }
        token = scan();
        pretty_print();

        reset_array();
        free_namelist();
	}while(token == TNAME);
    return NORMAL;
}

int variable_names(){//ahead_scan
	/*変数名 {, 変数名}*/
	if(token != TNAME)
		return ERROR;
    if(insert_namelist(string_attr,get_linenum())==ERROR){
        return error_variable("[Variable_names]Overload.");
    }
	token = scan();
	pretty_print();
	while(token == TCOMMA){
		token = scan();
		pretty_print();
		if(token != TNAME)
			return error_parse("[Variable_names]Varible names is not found.");
        if(insert_namelist(string_attr,get_linenum())==ERROR){
            return error_variable("[Variable_names]Overload.");
        }
		token = scan();
		pretty_print();
	}
	return NORMAL;
}

int type(){//ahead_scan
	/*標準型|配列型*/
    int return_num;
    if((return_num = standard_type()) == ERROR) {
        if ((return_num = array_type()) == ERROR) {
            return ERROR;
        }
    }
    token = scan();
    pretty_print();
    return return_num;
}

int standard_type(){
	/*integer | boolean | char*/
    if(token == TINTEGER){
        return TPINT;
    }
    else if(token == TBOOLEAN){
        return TPBOOL;
    }
    else if(token == TCHAR){
        return TPCHAR;
    }
	else return ERROR;
}

int array_type(){
	/*array '[' 符号なし整数 ']' of 標準型*/
	int return_num;
    if(token != TARRAY)
		return ERROR;
	token = scan();
	pretty_print();
	if(token != TLSQPAREN)
		return error_parse("[Array_type]'[' is not found.");
	token = scan();
	pretty_print();
	if(token != TNUMBER)
		return error_parse("[Array_type]Number is not found.");
	array_size = num_attr;
    if(array_size < 1){
        return error_variable("[ARRAY_SIZE]Number is too small.");
    }
    token = scan();
	pretty_print();
	if(token != TRSQPAREN)
		return error_parse("[Array_type]']' is not found.");
	token = scan();
	pretty_print();
	if(token != TOF)
		return error_parse("[Array_type]'of' is not found.");
	token = scan();
	pretty_print();
    return_num = standard_type();
    switch(return_num){
        case TPINT:
            return_num = TPARRAYINT;
            break;
        case TPCHAR:
            return_num = TPARRAYCHAR;
            break;
        case TPBOOL:
            return_num = TPARRAYBOOL;
    }
	return return_num;
}

int subprogram_declaration(){//ahead_scan
	//"procedure" 手続き名 [ 仮引数部 ] ";" [ 変数宣言部 ] 複合文 ";"
    if(token != TPROCEDURE)
		return ERROR;
    scope = local;
    token = scan();
	pretty_print();
	if(token != TNAME)
		return error_parse("[Subprogram_declation]Procedure name is not found.");
	strcpy(procname,string_attr);
    add_proc(procname,get_linenum());
    token = scan();
	pretty_print();
	if(token != TSEMI && formal_parameters()!= ERROR){
		token = scan();
		pretty_print();
	}
	if(token != TSEMI)
		return error_parse("[Subprogram_declation]';' is not found.");
	token = scan();
	pretty_print();
	if(token == TVAR){
		if(variable_declaration() == ERROR)
			return error_parse("[Subprogram_declation]Variable_declation error.");;
	}
	if(compound_statement()== ERROR)
		return error_parse("[Subprogram_declation]Compound_statement is not found.");
	if(token != TSEMI)
		return error_parse("[Subprogram_declation]';' is not found.");
	token = scan();
	pretty_print();
    copy_locallist();
    reset_flags();
	return NORMAL;
}

int formal_parameters(){//ahead_scan
	/*'('変数名の並び　: 型 { ; 変数名の並び : 型} ')'*/
	int mem_type = 0;
    int is_iterate = 0;

    if(token != TLPAREN)
		return ERROR;
    token = scan();
	pretty_print();
    is_para = formal;
	do {
        if(is_iterate){
            if(token != TSEMI)
                return error_parse("[Formal_parameters]';' is not found.");
            token = scan();
            pretty_print();
        }
        if (variable_names() == ERROR)
            return error_parse("[Formal_parameters]Variable_names is not found.");
        if (token != TCOLON)
            return error_parse("[Formal_parameters]':' is not found.");
        token = scan();
        pretty_print();
        mem_type = type();
        if (mem_type == ERROR)
            return error_parse("[Formal_parameters]Type is not found.");
        if(!(mem_type < TPARRAY))
            return error_variable("[Formal_parameters]Can`t use arraytype.");
        if((insert_idlist(procname, mem_type, is_para, scope, array_size))==ERROR){
            return error_variable("[Formal_parameters]Overload.");
        }
        reset_array();
        free_namelist();
        if(is_iterate == 0)is_iterate = 1;
    }while(token != TRPAREN);
    is_para = 0;
	return NORMAL;
}

int compound_statement(){
	//"begin" 文 { ";" 文 } "end"
	if(token != TBEGIN)
		return ERROR;
	token = scan();
	pretty_print();
	if(statement() == ERROR)
		return error_parse("[Compound_Statement]Statement is not found.");
	while(token != TEND){
		if(token != TSEMI)
			return error_parse("[Compound_Statement]';' is not found.");
		token = scan();
		pretty_print();
		if(statement() == ERROR)
			return error_parse("[Compound_Statement]Statement is not found.");
	}
	token = scan();
	pretty_print();
	return NORMAL;
}

int statement(){//ahead_scan
	//代入文 | 分岐文 | 繰り返し文 | 脱出文 | 手続き呼び出し文 | 戻り文 | 入力文 | 出力文 |複合文 | 空文
	if(assignment_statement() == ERROR
			&& condition_statement() == ERROR
			&& iteration_statement() == ERROR
			&& exit_statement() == ERROR
			&& call_statement() == ERROR
			&& return_statement() == ERROR
			&& input_statement() == ERROR
			&& output_statement() == ERROR
			&& compound_statement() == ERROR){
		if(token == TSEMI || token == TEND){//空
			return NORMAL;
		}
		else {
			return ERROR;
		}
	}
	return NORMAL;
}

int condition_statement(){//ahead_scan
	//"if" 式 "then" 文 [ "else" 文 ]
	int exp_type = 0;
    if(token != TIF)
		return ERROR;
	token = scan();
	pretty_print();
    exp_type = expression();
	if(exp_type == ERROR)
		return error_parse("[Condition_statement]Expression is not found.");
    else if(exp_type != TPBOOL)
        return error_variable("[Condition_statement]Expression_type must be boolean.");
	if(token != TTHEN)
		return error_parse("[Condition_statement]'then' is not found.");
	token = scan();
	pretty_print();
	if(statement() == ERROR)
		return error_parse("[Condition_statement]Statement is not found.");
	if(token == TELSE){
		token = scan();
		pretty_print();
		if(statement() == ERROR)
			return error_parse("[Condition_statement]Statement is not found.");
	}
	return NORMAL;
}

int iteration_statement(){//ahead_scan
	//"while" 式 "do" 文
	int exp_type = 0;

    if(token != TWHILE)
		return ERROR;
	iteration_flag ++;
	token = scan();
	pretty_print();
    exp_type = expression();
	if(exp_type == ERROR)
		return error_parse("[Iteration_statement]Expression is not found.");
    else if(exp_type != TPBOOL)
        return error_variable("[Iteration_statement]Expression_type must be boolean.");
	if(token != TDO)
		return error_parse("[Iteration_statement]'do' is not found.");
	token = scan();
	pretty_print();
	if(statement() == ERROR)
		return error_parse("[Iteration_statement]Statement is not found.");
	iteration_flag --;
	return NORMAL;
}

int exit_statement(){//ahead_scan
	if(token != TBREAK)
		return ERROR;
	if(iteration_flag <= 0)
		return error_parse("[Exit_statement]Found 'break' but not iteration.");
	token = scan();
	pretty_print();
	return NORMAL;
}

int call_statement(){//ahead_scan
	//"call" 手続き名 [ "(" 式の並び ")" ]
    int search_result;
	if(token != TCALL)
		return ERROR;
	token = scan();
	pretty_print();
	if(token != TNAME)
		return error_parse("[Call_statement]Procedure_name is not found.");
    search_result = id_count(string_attr,scope,get_linenum());
    if(search_result == ELSEERROR){
        return error_variable("[Call_statement]Can`t recursion call at same procedure.");
    }else if(search_result != ERROR){
        return error_variable("[Call_statement]Not Registration Procedure_name.");
    }
    if(is_null_proc_type() != ERROR)
        return error_variable("[Call_statement]No match num exp_type and formal");
    token = scan();
	pretty_print();
	if(token == TLPAREN){
		token = scan();
		pretty_print();
		if(expressions() == ERROR)
			return error_parse("[Call_statement]Expressions is not found.");
		if(is_null_proc_type() == ERROR)
            return error_variable("[Call_statement]No match num exp_type and formal");
        if(token != TRPAREN)
			return error_parse("[Call_statement]')' is not found.");
		token = scan();
		pretty_print();
	}
    return NORMAL;
}

int expressions(){//ahead_scan
	// 式 { "," 式 }
    int return_num = 0;

    return_num = expression();
	if(return_num == ERROR)
		return ERROR;
    else if(check_proc_type(return_num) == ERROR)
        return error_variable("[Expressions]No match type exp_type and formal_type.");
	while(token == TCOMMA){
		token = scan();
		pretty_print();
        return_num = expression();
		if(return_num == ERROR)
			return error_parse("[Expressions]Expression is not found.");
        else if(check_proc_type(return_num) == ERROR)
            return error_variable("[Expressions]No match type exp_type and formal_type.");
	}
	return NORMAL;
}

int return_statement(){//ahead_scan
	if(token != TRETURN)
		return ERROR;
	token = scan();
	pretty_print();
	return NORMAL;
}

int assignment_statement(){//ahead_scan
	//左辺部(＝変数) ":=" 式
    int val_type = 0;
    int exp_type = 0;

    val_type = variable();
	if(val_type == ERROR)
		return ERROR;
    else if(val_type > TPBOOL )
        return error_variable("[Assignment_statement]Left side type must be standard_type.");
	if(token != TASSIGN)
		return error_parse("[Assignment_statement]':=' is not found.");
	token = scan();
	pretty_print();
    exp_type = expression();
	if(exp_type == ERROR)
		return error_parse("[Assignment_statement]Expression is not found.");
    else if(exp_type > TPBOOL)
        return error_variable("[Assignment_statement]Right side type must be standard_type.");
    else if(val_type != exp_type)
        return error_variable("[Assignment_statement]No match left side and right side.");
    return NORMAL;
}

int variable(){//ahead_scan
	//変数名 [ "[" 式 "]" ]
    int val_type = 0;
	if(token != TNAME)
		return ERROR;
    val_type = id_count(string_attr,scope,get_linenum());
    if(val_type == ERROR){
        return error_variable("[Variable]undefined variable.");
    }
	token = scan();
	pretty_print();

    int exp_type = 0;
	if(token == TLSQPAREN){
        if(val_type < TPARRAY || val_type == TPPROC)
            return error_variable("[Variable]val_name is not array");
		token = scan();
		pretty_print();
        exp_type = expression();
		if(exp_type == ERROR)
			return error_parse("[Variable]Expression is not found.");
        else if(exp_type != TPINT)
            return error_variable("[Variable]Expression must be integer type.");
        //todo arraysize 0~size-1
		if(token != TRSQPAREN)
			return error_parse("[Variable]']' is not found.");
		token = scan();
		pretty_print();
        val_type -= 4;
	}
    return val_type;
}

int expression(){//ahead_scan
	//単純式 { 関係演算子 単純式 }
    int return_num[2] = {0};
    int ope_num = 0;

    return_num[0] = simple_expression();
	if(return_num[0] == ERROR)
		return ERROR;
	ope_num = relational_operator();
    while(ope_num != ERROR){
        if(return_num[0] < TPARRAY)
            error_variable("[Expression]When operator exist,type must be standard_type.");
		token = scan();
		pretty_print();
        return_num[1] = simple_expression();
		if(return_num[1] == ERROR)
			return error_parse("[Expression]Simple_expression is not found.");
        else if(return_num[0] != return_num[1])
            return  error_variable("[Expression]When operator exist,all type must be same.");
        ope_num = relational_operator();
	}
    if(return_num[1] == 0)
        return return_num[0];
    else return TPBOOL;
}

int simple_expression(){//ahead_scan
	// [ "+" | "-" ] 項 { 加法演算子 項 }
    int return_num = 0;
    int is_operator = 0;
    int ope_num = 0;
	if(token == TPLUS || token == TMINUS){
		token = scan();
		pretty_print();
        is_operator = 1;
	}
    return_num = term();
	if(return_num == ERROR)
		return error_parse("[Simple_expression]Term is not found.");
    else if(is_operator && return_num != TPINT)
        return  error_variable("[Simple_expression]When operator exist,type must be integer.");

    ope_num = addictive_operator();
    while(ope_num != ERROR){
        if(ope_num == TPLUS || ope_num == TMINUS){
            if(return_num != TPINT)
                error_variable("[Simple_expression]Operand must be integer.");
            token = scan();
            pretty_print();
            return_num = term();
            if(return_num == ERROR)
                return error_parse("[Simple_expression]Term is not found.");
            else if(return_num != TPINT)
                error_variable("[Simple_expression]Operand must be integer.");
        }
        else if(ope_num == TOR) {
            if(return_num != TPBOOL)
                error_variable("[Simple_expression]Operand must be boolean.");
            token = scan();
            pretty_print();
            return_num = term();
            if(return_num == ERROR)
                return error_parse("[Simple_expression]Term is not found.");
            else if(return_num != TPBOOL)
                error_variable("[Simple_expression]Operand must be boolean.");
        }
        ope_num = addictive_operator();
	}
	return return_num;
}

int term(){//ahead_scan
	// 因子 { 乗法演算子 因子 }
    int return_num = 0;
    int ope_num = 0;

    return_num = factor();
	if(return_num == ERROR)
		return ERROR;
    ope_num = multiplicative_operator();
	while(ope_num != ERROR){
        if(ope_num == TSTAR || ope_num == TDIV){
            if(return_num != TPINT)
                return error_variable("[Term][* or /]Operand must be integer.");
            token = scan();
            pretty_print();
            return_num = factor();
            if(return_num == ERROR)
                return error_parse("[Term]Factor is not found.");
            else if(return_num != TPINT)
                return  error_variable("[Term][* or /]Operand must be integer.");
        }
        else if(ope_num == TAND) {
            if(return_num != TPBOOL)
                return error_variable("[Term][* or /]Operand must be boolean.");
            token = scan();
            pretty_print();
            return_num = factor();
            if(return_num == ERROR)
                return error_parse("[Term]Factor is not found.");
            else if(return_num != TPBOOL)
                return  error_variable("[Term][* or /]Operand must be boolean.");
        }
        ope_num = multiplicative_operator();
	}
	return return_num;
}

int factor(){//ahead_scan
	// 変数 | 定数 | "(" 式 ")" | "not" 因子 | 標準型 "(" 式 ")"
    int return_num = 0;
    int std_type = 0;
    return_num = variable();
	if(return_num == ERROR){
        return_num = constant();
		if(return_num == ERROR){
			if(token == TLPAREN){
				token = scan();
				pretty_print();
                return_num = expression();
				if(return_num == ERROR)
					return error_parse("[Factor]Expression is not found.");
				if(token != TRPAREN)
					return error_parse("[Factor]')'is not found.");
				token = scan();
				pretty_print();
			}
			else if(token == TNOT){
				token = scan();
				pretty_print();
                return_num = factor();
				if(return_num == ERROR)
					return error_parse("[Factor]Factor is not found.");
                else if(return_num !=TPBOOL)
                    return error_variable("[Factor]'Not factor' is must be boolean.");
			}
			else if((return_num = standard_type()) == NORMAL){
				token = scan();
				pretty_print();
				if(token != TLPAREN)
					return error_parse("[Factor]'('is not found.");
				token = scan();
				pretty_print();
                std_type = expression();
				if(std_type == ERROR)
					return error_parse("[Factor]Expression is not found.");
                else if(std_type >TPBOOL)
                    return error_variable("[Factor]'Standard_type(expression)'is must be standard_type.");
				if(token != TRPAREN)
					return error_parse("[Factor]')'is not found.");
				token = scan();
				pretty_print();
			}
			else return ERROR;
		}
	}
	return return_num;
}

int constant(){//ahead_scan
	//符号なし整数" | "false" | "true" | "文字列"
    int return_num = 0;

    char string_copy[MAXSTRSIZE];
    int j = 0;
    while(1){
        if(string_attr[j+2] == '\0'){
            string_copy[j] = '\0';
            break;
        }
        string_copy[j] = string_attr[j+1];
        j++;
    }

    if(token == TNUMBER)return_num =  TPINT;
    else if(token == TFALSE || token == TTRUE)return_num = TPBOOL;
    else if(token == TSTRING){
        if(strlen(string_copy) != 1)error_variable("[Constant]String length must be 1");
        else return_num = TCHAR;
    }
    else return ERROR;
	token = scan();
	pretty_print();
	return return_num;
}

int multiplicative_operator(){
	//"*" | "div" | "and"
    if(token == TSTAR)return TSTAR;
    else if(token == TDIV)return TDIV;
    else if(token == TAND)return TAND;
    else return ERROR;
}

int addictive_operator(){
	//"+" | "-" | "or"
    if(token == TPLUS)return TPLUS;
    else if(token == TMINUS)return TMINUS;
    else if(token == TOR)return TOR;
    else return ERROR;
}

int relational_operator(){
	//"=" | "<>" | "<" | "<=" | ">" | ">="
    if(token == TEQUAL)return TEQUAL;
    else if(token == TNOTEQ)return TNOTEQ;
    else if(token == TLE)return TLE;
    else if(token == TLEEQ)return TLEEQ;
    else if(token == TGR)return TGR;
    else if(token == TGREQ)return TGREQ;
    else return ERROR;
}

int input_statement(){//ahead_scan
	//("read" | "readln") [ "(" 変数 { "," 変数 } ")" ]
    int val_type = 0;
	if(token != TREAD && token != TREADLN)
		return ERROR;
	token = scan();
	pretty_print();
	if(token == TLPAREN){
		token = scan();
		pretty_print();
        val_type = variable();
		if(val_type == ERROR)
			return error_parse("[Input_statement]Variable is not found.");
        else if(!(val_type == TPINT || val_type == TPCHAR))
            return error_variable("[Input_statement]Variable must be integer or char.");
		while(token != TRPAREN){
			if(token != TCOMMA)
				return error_parse("[Input_statement]',' is not found.");
			token = scan();
			pretty_print();
            val_type = variable();
			if(val_type == ERROR)
				return error_parse("[Input_statement]Variable is not found.");
            else if(!(val_type == TPINT || val_type == TPCHAR))
                return error_variable("[Input_statement]Variable must be integer or char.");
		}
		token = scan();
		pretty_print();
	}
	return NORMAL;
}

int output_statement(){//ahead_scan
	//("write" | "writeln") [ "(" 出力指定 { "," 出力指定 } ")" ]
	if(token != TWRITE && token != TWRITELN)
		return ERROR;
	token = scan();
	pretty_print();
	if(token == TLPAREN){
		token = scan();
		pretty_print();
		if(output_format() == ERROR)
			return error_parse("[Output_statement]Output_format is not found.");
		while(token != TRPAREN){
			if(token != TCOMMA)
				return error_parse("[Output_statement]',' is not found.");
			token = scan();
			pretty_print();
			if(output_format() == ERROR)
				return error_parse("[Output_statement]Output_format is not found.");
		}
		token = scan();
		pretty_print();
	}
	return NORMAL;
}

int output_format(){//ahead_scan
	//式 [ ":" "符号なし整数" ] | "文字列"
	/*string_attrから''を削除*/
	char string_copy[MAXSTRSIZE];
	int j = 0;
    int exp_type = 0;
	while(1){
		if(string_attr[j+2] == '\0'){
			string_copy[j] = '\0';
			break;
		}
		string_copy[j] = string_attr[j+1];
		j++;
	}

	/*文字列の長さが1なら「式」であるとする*/
	if(token == TSTRING && strlen(string_copy) != 1){
		token = scan();
		pretty_print();
		return NORMAL;
	}
	else if((exp_type = expression()) != ERROR){
        if(exp_type > TPBOOL)return error_variable("[Output_format]Expression type must be standard_type.");
		if(token == TCOLON){
			token = scan();
			pretty_print();
			if(token != TNUMBER)
				return error_parse("[Output_format]Output_format number is not found.");
			token = scan();
			pretty_print();
		}
		return NORMAL;
	}
	return ERROR;

}

void pretty_print(){
	static int compound_flag = 0;
	static int begin_flag = 0;
	static int indention_flag = 0;
	static int procedure_flag = 0;
	static int var_flag = 0;
	static int var_semi_flag = 0;

	/*変数宣言部内の;では改行しない*/
	if(var_semi_flag == 1){
		if(token != TNAME){
			printf("\n");
			var_flag = 0;
		}
		var_semi_flag = 0;
	}

	/*段付け*/
	int i = 0;
	if(compound_flag > 0) {
		if(begin_flag == 1 || indention_flag == 1){
			while(i<compound_flag){
				printf("    ");//SP*4
				i++;
			}
			if(begin_flag == 1) begin_flag = 0;
			if(indention_flag == 1) indention_flag = 0;
		}
	}

	switch(token){
	/*NAME or STRING*/
	case TNAME:
	case TSTRING:
		printf("%s ",string_attr);
		break;
	/*NUMBER*/
	case TNUMBER:
		printf("%d ",num_attr);
		break;
	/*;*/
	case TSEMI:
		/*複合文内の;の時、改行後の段付用のフラグを立てる*/
		if(compound_flag > 0 ) indention_flag = 1;
		/*変数宣言部内の;のときは改行しない*/
		if(var_flag == 1){
			var_semi_flag = 1;
			printf("\b%s",tokenstr[token]);
		}
		else{
			printf("\b%s\n",tokenstr[token]);
		}
		break;
	/*begin*/
	case TBEGIN:
		compound_flag ++;
		begin_flag = 1;
		printf("%s\n",tokenstr[token]);
		break;
	/*end*/
	case TEND:
		compound_flag --;
		printf("\n");

		i = 0;
		while(i<compound_flag){
			printf("    ");//SP*4
			i++;
		}

		/*副プログラム宣言の段付解除*/
		if(compound_flag == 1 && procedure_flag){
			compound_flag--;
			procedure_flag = 0;
		}
		printf("%s ",tokenstr[token]);
		break;
	/*else*/
	case TELSE:
		printf("\n");
		i = 0;
		while(i<compound_flag){
			printf("    ");//SP*4
			i++;
		}
		printf("%s ",tokenstr[token]);
		break;
    /*procedure*/
	case TPROCEDURE:
		compound_flag ++;
		procedure_flag = 1;
		printf("    %s ",tokenstr[token]);
		break;
    /*.*/
	case TDOT:
		printf("\b%s",tokenstr[token]);
		break;
    /*var*/
	case TVAR:
		printf("    %s ",tokenstr[token]);
		var_flag = 1;
		break;

	default:
		if(token > 0){
			printf("%s ",tokenstr[token]);
		}
		break;
	}

}

int error_parse(char *mes){
	printf("\n[PARSE_ERROR]%s\n",mes);
	return ERROR;
}

int error_variable(char *mes){
    printf("\n[VARIABLE_ERROR]%s\n",mes);
    return ERROR;
}

void reset_flags(){
    array_size = 0;
    is_para = var;
    scope = global;
    memset(procname,'\0',sizeof(procname));
}

void reset_array(){
    array_size = 0;
}