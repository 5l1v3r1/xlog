
%option noyywrap  yylineno 

%{
#include "scanner.h"
#include "grammar.h"
//

#define YY_DECL extern "C" int yylex()
#define MAX_INCLUDE_DEPTH 100
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
string str_input="";//存放双引号之间的字符串


%}

/* float exponent */
EXP	([Ee][-+]?[0-9]+)
digit			[0-9]
letter			[a-zA-Z]
hexdigit      [a-fA-F0-9]
/*定义一个状态读取双引号之间的内容*/
%x _STR_
/*定义一个状态读取[]之间的内容*/
%x _TIME_ 
/*定义一个状态读取//之间的内容,匹配正则*/
%x regexp

%%
 
"and"                   { return _AND_; }
"or"                    { return _OR_;  }
"rule"   				{ return _RULE_;}
"meta"                  { return _META_;}
"strings"               { return _STRINGS_;}
"condition"             { return _CONDITION_;}
"not"            		{ return _NOT_;}


"url"            		{ return _REQUEST_;}
"refer"            		{ return _REFER_;}
"agent"            		{ return _AGENT_;}
"status"            	{ return _STATUS_;}  
"-" 					{return _NONE_;}
"decode" 				{return _URL_DECODE_;}

	
"GET"  { 	yylval.request_method=1;
			return _REQUSET_METHOD_;
}

"POST"  { 	yylval.request_method=2;
			return _REQUSET_METHOD_;
}






({letter}|_)({letter}|{digit}|_)*  {	
	
	if (strlen(yytext) > 128) 
	{
		yyerror("indentifier too long");
	}
	yylval.c_string =yr_strdup(yytext);
 
	return _IDENTIFIER_;
}







$({letter}|{digit}|_)*  {

  yylval.c_string = yr_strdup(yytext);
  return _STRING_IDENTIFIER_;

}








\[  { 
		BEGIN(_TIME_);//遇到左双引号，开始启动str状态
}

<_TIME_>[^\]]+   {	
		str_input+=yytext;//未遇到]，继续收集字符串
}

<_TIME_>\] {	//遇到]

	BEGIN(INITIAL);	// BEGIN(INITIAL)类似于BEGIN(0),表示状态从开头解析（0表示不带状态的解析，也就说规则前没有<>这个标记的状态
	yylval.c_string1=yr_strdup((char *)(str_input.c_str()));
	//cout<<"c_string ="<< yylval.c_string1<<endl;
	str_input="";
	return _TIMELOCAL_;
}

\"  { 
		BEGIN(_STR_);//遇到左双引号，开始启动str状态
}
\\\\ { cout<<"bingo"<<endl;}



<_STR_>[^\\\n\"]+   {	
		str_input+=yytext;//未遇到回车和双引号，继续收集字符串
}

<_STR_>\\t  {	
		str_input+=yytext;//遇到\t，继续收集字符串
}

<_STR_>\\\"   { 

	str_input+=yytext;//遇到\，继续收集字符串
}

<_STR_>\\\   { 

	str_input+=yytext;//遇到\，继续收集字符串
}


<_STR_>\\\\   {

	str_input+=yytext;//遇到\\，继续收集字符串
}

<_STR_>\\   {

	str_input+=yytext;//遇到\，继续收集字符串
}


<_STR_>\" {	//遇到右双引号结束符号，此时的yytext是双引号

	BEGIN(INITIAL);	// BEGIN(INITIAL)类似于BEGIN(0),表示状态从开头解析（0表示不带状态的解析，也就说规则前没有<>这个标记的状态
	yylval.c_string1=yr_strdup((char *)(str_input.c_str()));
	//cout<<"c_string ="<< yylval.c_string1<<endl;
	str_input="";
	return _TEXTSTRING_;
}


<_STR_>\n  {	//如果做双引号最后直接为回车，即没有字符串结束的双引号,则提示错误

	yyerror("unterminated strings");
	yyterminate();
}

<_STR_>\\x{hexdigit}{2} {

   //int result;

   //sscanf( yytext + 2, "%x", &result );
   
 
   str_input+=yytext;//未遇到回车和双引号，继续收集字符串

}



"/"  {

  BEGIN(regexp);
}

<regexp>\n  {

  yyerror("unterminated regular expression");
  yyterminate();
}


<regexp>[^/\n\\]+ {
 str_input+=yytext; 
}

<regexp>\\\/  {
	str_input+="/";
}


<regexp>\\. {

  str_input+="\\.";

}


<regexp>\/  { //正则匹配结束
  BEGIN(INITIAL);
  yylval.c_string=yr_strdup((char *)(str_input.c_str()));
  str_input="";
  return _REGEXP_;
}






{digit}+ { 

  yylval.c_string = yr_strdup(yytext);
  return _DIGIT_NUMBER_;

}


{digit}?{digit}?{digit}?"."{digit}?{digit}?{digit}?"."{digit}?{digit}?{digit}?"."{digit}?{digit}?{digit}? {	

	if (strlen(yytext) > 16) 
	{
		yyerror("ip too long");
	}
	yylval.c_string =yr_strdup(yytext);
	
	return _REMOTEADDR_;
}






[ \t\r\n]  /*忽略空格等字符,防止规则读取到有问题 */

			/*<<EOF>> 	匹配文件结尾，如果去掉注释，则读取完文件不会退出*/
"//".*  	/*忽略注释行*/

.	{ //匹配所有其他一个字节的字符串
	
	if (yytext[0] >= 32 && yytext[0] < 127)
	{
		return yytext[0];
	}
	else
	{
		yyerror("non-ascii character %c\n", *yytext);
		yyterminate();
	}
}
%%
