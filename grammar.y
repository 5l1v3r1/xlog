
%{
#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"

extern "C" int yylex();
extern "C" int yyparse();
extern "C" FILE *yyin;

Scanner *scanner;
map<string,string> identiValueMap ;
list<IDENTIFIER_KV *> identiValueList ;
list<META_KV*> metaList ;

void yyerror(const char *s, ...);

%}

%union 
{
  struct ast *a;
  char* c_string;
  char* c_string1;
  YR_META *meta; 
  int request_method;
  int64_t  integer;
}


%token EOL
%token  _RULE_ _META_ _STRINGS_ _CONDITION_ _NONE_  _REQUEST_ _AGENT_ _REFER_ _STATUS_ _REMOTE_USER_ _URL_DECODE_
%token <c_string1>	_TEXTSTRING_
%token <c_string> _STRING_IDENTIFIER_
%token <c_string>  _IDENTIFIER_
%left _AND_ _OR_ _NOT_

%token <c_string> _DIGIT_NUMBER_
%token <c_string> _REMOTEADDR_
%token <c_string> _TIMELOCAL_
%token <c_string> _REGEXP_  
%token <request_method> _REQUSET_METHOD_


%type <c_string> request_url
%type <c_string> response_size
%type <c_string> refer
%type <c_string> user_agent
%type <c_string> remote_user
%type <integer> string_modifier
%type <integer> string_modifiers
%type <integer> type_modifier
%type <integer> url_oprate
	   
	   
%type <meta> meta
%type <meta> meta_declarations 
%type <meta> meta_declaration 
%type <a> strings                
%type <a> string_declarations
%type <a> string_declaration
%type <a> boolean_expression
%type <a> condition

%start rules

%%

rules : /* empty */
      | rules rule { 
						
		}
      | rules error rule    
	  ;


rule  :  _RULE_   _IDENTIFIER_   '{' meta strings  condition'}'  
		{	
			//cout<<"Rule_IDENTIFIER = "<<$2<<endl;
			 _RULE * rule=new _RULE();
			rule->identifier=$2;
			rule->conditions=$6;
			rule->identiValueList=identiValueList;
			
		    rule->metaList=metaList;
			rule->chained_to=NULL;	
		
			RULES * rules=scanner->GetRules();
			if(rules->rule==NULL) 
			{	
				rules->rule=rule;		
			}
			else
			{
				_RULE * rule1=rules->rule;
				while(rule1->chained_to!=NULL)
				{			
					rule1=rule1->chained_to;
				}
				
				rule1->chained_to=rule;
			}
		
			ClearMapValue();		
		}
		|_REMOTEADDR_  _NONE_ remote_user  _TIMELOCAL_ request_url _DIGIT_NUMBER_ response_size refer user_agent
		{
		
			//cout<<$1<<" - - "<<$4<<" "<<$5<<" "<<$6<<" "<<$7<<" "<<$8<<endl;	
			
			
			
			WEBLOG * log=new WEBLOG();
			log->remote_addr=$1;
			log->time_local=$4;
			log->request=$5;
			log->http_status=atoi($6);
			if($6)
				free($6);
			
			
			
			if($7==NULL)		 /*为NULL,即为-*/
				log->responsize=0;
			else
				log->responsize=atoi($7);
				free($7);				
			log->refer=$8;;
			
			log->user_agent=$9;
			log->chained_to=NULL;	

			
			WEBLOGS *logs=scanner->GetLogs();  
			if(logs->weblog==NULL) 
			{	
				logs->weblog=log;		
			}
			else
			{
				WEBLOG * log1=(WEBLOG *)scanner->GetLinkTail(); /*获得尾节点位置,让新log记录直接追加在链表结尾*/	
								
				log1->chained_to=log;
			}
			scanner->SetLinkTail((int *)log);					/*保存尾节点位置*/
		
		
		
		}
		;

remote_user:	_NONE_				{ $$=NULL;}
				|_IDENTIFIER_ 		{	
										$$=NULL; 
										free($1);				/*不用remote_user字段*/
								    }		
request_url: 	_TEXTSTRING_		{ $$=$1;}
response_size: 	_NONE_				{ $$=NULL;}
				|_DIGIT_NUMBER_	{ $$=$1;}
				
refer: _TEXTSTRING_{ $$=$1;}
user_agent: _TEXTSTRING_{ $$=$1;}



meta : /*empty*/	 { $$=NULL;}
	 | _META_ ':'  meta_declarations  {  $$=$3; }
	 ;

meta_declarations :meta_declaration { $$=$1;}
				  | meta_declarations meta_declaration { $$=$1;}
				  ;

meta_declaration :_IDENTIFIER_ '=' _TEXTSTRING_  
				{
					//cout<<"meta_declaration "<<$1<<" = "<<$3<<endl; 
                    InsertMetaValue($1,$3);

				};

				
//strings
strings : /*empty*/	 { $$=NULL;}
		| _STRINGS_ ':' string_declarations { $$=$3;}
		
string_declarations : string_declaration                      { $$ = $1; }
                    | string_declarations string_declaration  { $$ = $1; }
                    ;

string_declaration : _STRING_IDENTIFIER_ '=' _TEXTSTRING_ type_modifier  url_oprate{
				
						 //cout<<"string_declaration "<<$1<<" = "<<$3<<endl; 
						 InsertIdentifierValue($1,$3,TYPE_MATCH_NORMAL,$4,$5);
					
					}
					|_STRING_IDENTIFIER_ '=' _REGEXP_ type_modifier url_oprate
					{	 //cout<<"regex string_declaration "<<$1<<" = "<<$3<<endl; 
						 InsertIdentifierValue($1,$3,TYPE_MATCH_REGEX,$4,$5);
							
					}

					


type_modifier :	/* empty */       { $$ = 0;  }
				| _REQUEST_       { $$ = TYPE_GFLAGS_REQUEST; }
                | _REFER_         { $$ = TYPE_GFLAGS_REFER; }
                | _AGENT_      	  { $$ = TYPE_GFLAGS_AGENT; }
                ;				 


url_oprate :	/* empty */       { $$ = 0;  }
				| _URL_DECODE_    { $$ = TYPE_GFLAGS_URL_DECODE; }
                ;				 

				
				

//condition
condition : _CONDITION_ ':'	 boolean_expression { $$=$3;}
	
boolean_expression :  _STRING_IDENTIFIER_ {
					
						//cout<<"boolean_expression _STRING_IDENTIFIER_ = "<<$1<<endl;
						$$=NewIdentifier($1);
					}
					| boolean_expression _AND_ boolean_expression {
						
						//cout<<"boolean_expression and  "<<endl;
						$$=NewLogic_Op(7,$1,$3);
					}
					|boolean_expression _OR_ boolean_expression {
						
						//cout<<"boolean_expression or  "<<endl;
						$$=NewLogic_Op(8,$1,$3);
					}
					|'(' boolean_expression ')' { 
						$$=$2; 
					}
					| _NOT_ boolean_expression{				
						$$=NewLogic_Op(9,$2,NULL);		
					}
					|_REQUSET_METHOD_ { 
					
						$$=NewMethod('M',$1);
					}
					| _STATUS_ '=' _DIGIT_NUMBER_ {
					
						$$=NewMethod('S',atoi($3));				
						free($3);
					}
					
					
%%
