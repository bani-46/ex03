#include "compiler.h"

#define NORMAL 1
#define ERROR -1

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
	return NORMAL;
}

//todo not scan
int block(){
	/*{変数宣言部 | 副プログラム宣言}複合文*/
	while((variable_declaration() != ERROR) || (subprogram_declaration() != ERROR));
	if (compound_statement() == ERROR)
		return error_parse("[Block]Compound_statement is not found.");
	return NORMAL;
}

int variable_declaration(){//ahead_scan
	/*var 変数名の並び : 型 ; {変数名の並び : 型 ;}*/
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
		if(type() == ERROR)
			return error_parse("[Variable_declation]Type is not found.");
		if(token != TSEMI)
			return error_parse("[Variable_declation]';' is not found.");
		token = scan();
		pretty_print();
	}while(token == TNAME);
	return NORMAL;
}

int variable_names(){//ahead_scan
	/*変数名 {, 変数名}*/
	if(token != TNAME)
		return ERROR;
	token = scan();
	pretty_print();
	while(token == TCOMMA){
		token = scan();
		pretty_print();
		if(token != TNAME)
			return error_parse("[Variable_names]Varible names is not found.");
		token = scan();
		pretty_print();
	}
	return NORMAL;
}

int type(){//ahead_scan
	/*標準型|配列型*/
	if(standard_type() == ERROR){
		if(array_type() == ERROR)
			return ERROR;
	}
	token = scan();
	pretty_print();
	return NORMAL;
}

int standard_type(){
	/*integer | boolean | char*/
	if(token != TINTEGER && token != TBOOLEAN && token != TCHAR)
		return ERROR;
	return NORMAL;
}

int array_type(){
	/*array '[' 符号なし整数 ']' of 標準型*/
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
	if(standard_type() == ERROR)
		return error_parse("[Array_type]Standard_type is not found.");
	return NORMAL;
}

int subprogram_declaration(){//ahead_scan
	//"procedure" 手続き名 [ 仮引数部 ] ";" [ 変数宣言部 ] 複合文 ";"
	if(token != TPROCEDURE)
		return ERROR;
	token = scan();
	pretty_print();
	if(token != TNAME)
		return error_parse("[Subprogram_declation]Procedure name is not found.");
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
	return NORMAL;
}

int formal_parameters(){//ahead_scan
	/*'('変数名の並び　: 型 { ; 変数名の並び : 型} ')'*/
	if(token != TLPAREN)
		return ERROR;
	token = scan();
	pretty_print();
	if(variable_names() == ERROR)
		return error_parse("[Formal_parameters]Variable_names is not found.");
	if(token != TCOLON)
		return error_parse("[Formal_parameters]':' is not found.");
	token = scan();
	pretty_print();
	if(type() == ERROR)
		return error_parse("[Formal_parameters]Type is not found.");
	while(token != TRPAREN){
		if(token != TSEMI)
			return error_parse("[Formal_parameters]';' is not found.");;
		token = scan();
		pretty_print();
		if(variable_names() == ERROR)
			return error_parse("[Formal_parameters]Variable_names is not found.");
		if(token != TCOLON)
			return error_parse("[Formal_parameters]':' is not found.");
		token = scan();
		pretty_print();
		if(type() == ERROR)
			return error_parse("[Formal_parameters]Type is not found.");
	}
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
	if(token != TIF)
		return ERROR;
	token = scan();
	pretty_print();
	if(expression() == ERROR)
		return error_parse("[Condition_statement]Expression is not found.");
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
	if(token != TWHILE)
		return ERROR;
	iteration_flag ++;
	token = scan();
	pretty_print();
	if(expression() == ERROR)
		return error_parse("[Iteration_statement]Expression is not found.");
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
	if(token != TCALL)
		return ERROR;
	token = scan();
	pretty_print();
	if(token != TNAME)
		return error_parse("[Call_statement]Procedure_name is not found.");
	token = scan();
	pretty_print();
	if(token == TLPAREN){
		token = scan();
		pretty_print();
		if(expressions() == ERROR)
			return error_parse("[Call_statement]Expressions is not found.");
		if(token != TRPAREN)
			return error_parse("[Call_statement]')' is not found.");
		token = scan();
		pretty_print();
	}
	return NORMAL;
}

int expressions(){//ahead_scan
	// 式 { "," 式 }
	if(expression() == ERROR)
		return ERROR;
	while(token == TCOMMA){
		token = scan();
		pretty_print();
		if(expression() == ERROR)
			return error_parse("[Expressions]Expression is not found.");
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
	if(variable() == ERROR)
		return ERROR;
	if(token != TASSIGN)
		return error_parse("[Assinment_statement]':=' is not found.");
	token = scan();
	pretty_print();
	if(expression() == ERROR)
		return error_parse("[Assinment_statement]Expression is not found.");
	return NORMAL;
}

int variable(){//ahead_scan
	//変数名 [ "[" 式 "]" ]
	if(token != TNAME)
		return ERROR;
	token = scan();
	pretty_print();
	if(token == TLSQPAREN){
		token = scan();
		pretty_print();
		if(expression() == ERROR)
			return error_parse("[Variable]Expression is not found.");
//		token = scan();//todo
//		pretty_print();
		if(token != TRSQPAREN)
			return error_parse("[Variable]']' is not found.");
		token = scan();
		pretty_print();
	}
	return NORMAL;
}

int expression(){//ahead_scan
	//単純式 { 関係演算子 単純式 }
	if(simple_expression() == ERROR)
		return ERROR;
	while(relational_operator() != ERROR){
		token = scan();
		pretty_print();
		if(simple_expression() == ERROR)
			return error_parse("[Expression]Simple_expression is not found.");
	}
	return NORMAL;
}

int simple_expression(){//ahead_scan
	// [ "+" | "-" ] 項 { 加法演算子 項 }
	if(token == TPLUS || token == TMINUS){
		token = scan();
		pretty_print();
	}
	if(term() == ERROR)
		return error_parse("[Simple_expression]Term is not found.");
	while(addictive_operator() != ERROR){
		token = scan();
		pretty_print();
		if(term() == ERROR)
			return error_parse("[Simple_expression]Term is not found.");
	}
	return NORMAL;
}

int term(){//ahead_scan
	// 因子 { 乗法演算子 因子 }
	if(factor() == ERROR)
		return ERROR;
	while(multiplicative_operator() != ERROR){
		token = scan();
		pretty_print();
		if(factor() == ERROR)
			return error_parse("[Term]Factor is not found.");
	}
	return NORMAL;
}

int factor(){//ahead_scan
	// 変数 | 定数 | "(" 式 ")" | "not" 因子 | 標準型 "(" 式 ")"
	if(variable() == ERROR){
		if(constant() == ERROR){
			if(token == TLPAREN){
				token = scan();
				pretty_print();
				if(expression() == ERROR)
					return error_parse("[Factor]Expression is not found.");
				if(token != TRPAREN)
					return error_parse("[Factor]')'is not found.");
				token = scan();
				pretty_print();
			}
			else if(token == TNOT){
				token = scan();
				pretty_print();
				if(factor() == ERROR)
					return error_parse("[Factor]Factor is not found.");
			}
			else if(standard_type() == NORMAL){
				token = scan();
				pretty_print();
				if(token != TLPAREN)
					return error_parse("[Factor]'('is not found.");
				token = scan();
				pretty_print();
				if(expression() == ERROR)
					return error_parse("[Factor]Expression is not found.");
				if(token != TRPAREN)
					return error_parse("[Factor]')'is not found.");
				token = scan();
				pretty_print();
			}
			else return ERROR;
		}
	}
	return NORMAL;
}

int constant(){//ahead_scan
	//符号なし整数" | "false" | "true" | "文字列"
	if(token != TNUMBER && token != TFALSE && token != TTRUE && token != TSTRING)
		return ERROR;
	token = scan();
	pretty_print();
	return NORMAL;
}

int multiplicative_operator(){
	//"*" | "div" | "and"
	if(token != TSTAR && token != TDIV && token !=TAND)
		return ERROR;
	return NORMAL;
}

int addictive_operator(){
	//"+" | "-" | "or"
	if(token != TPLUS && token != TMINUS && token !=TOR)
		return ERROR;
	return NORMAL;
}

int relational_operator(){
	//"=" | "<>" | "<" | "<=" | ">" | ">="
	if(token != TEQUAL &&
			token != TNOTEQ &&
			token !=TLE &&
			token !=TLEEQ &&
			token !=TGR &&
			token !=TGREQ)
		return ERROR;
	return NORMAL;
}

int input_statement(){//ahead_scan
	//("read" | "readln") [ "(" 変数 { "," 変数 } ")" ]
	if(token != TREAD && token != TREADLN)
		return ERROR;
	token = scan();
	pretty_print();
	if(token == TLPAREN){
		token = scan();
		pretty_print();
		if(variable() == ERROR)
			return error_parse("[Input_statement]Variable is not found.");
		while(token != TRPAREN){
			if(token != TCOMMA)
				return error_parse("[Input_statement]',' is not found.");
			token = scan();
			pretty_print();
			if(variable() == ERROR)
				return error_parse("[Input_statement]Variable is not found.");
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
	else if(expression() != ERROR){
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
