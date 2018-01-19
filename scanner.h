#include <iostream>
#include<vector>
#include<set>
#include <regex.h>  
#include <string.h>
#include <map>
#include <list>
#include <iostream>

using namespace std;


#define TYPE_GFLAGS_REQUEST  0x01
#define TYPE_GFLAGS_REFER 0x02
#define TYPE_GFLAGS_AGENT 0x04

#define TYPE_MATCH_NORMAL  0x01
#define TYPE_MATCH_REGEX   0x02


#define TYPE_GFLAGS_URL_DECODE  0x01

/*抽象语法树结构 */
struct ast {
  int nodetype;
  struct ast *l;
  struct ast *r;
};

/*存放规则变量$str*/
struct IdentifierVal {
  int nodetype;			
  char* str;
};

/*存放规则变量$str*/
struct RequestMethodVal {
  int nodetype;			
  int type;
};


/*存放规则变量和对应的值*/
struct IdentifierStrVal {
  int nodetype;	
  struct IdentifierStrVal* chaint_to;
  string identifier;
  string value;
};


/* 构建语法树 */
struct ast *NewLogic_Op(int cmptype, struct ast *l, struct ast *r);
struct ast *NewIdentifier(char * str);
struct ast *NewMethod(char nodetype,int type);

/*插入和清空 规则变量和对应的值*/
void InsertIdentifierValue(char * identifier,char* value,int mode,int64_t match_type,int64_t oprate_type);
void InsertMetaValue(char* identifier,char* value);
void ClearMapValue();

/* define a function */
char* yr_strdup(const char *s);
void yr_free(char *str);

/*lexer的接口函数，分别为行号、错误 */
extern int yylineno;
void yyerror(const char *s, ...);

/*规则key,value结构*/
struct IDENTIFIER_KV
{
	char * key;
	char * value;
	int mode; //1 表示 字符串匹配 2表示正则匹配
	int type;// 匹配的内容是_REQUEST_ 或  _REFER_ 或 _AGENT_
	int oprate_type;//0默认值,0x2 url解码
	  
};       
         
struct META_KV
{
	char * key;
	char * value;
};

/*扫描规则定义*/
typedef struct _YR_META
{
  int32_t type;
  int32_t integer;
  char* identifier;
  char* string;
} YR_META;

typedef struct _RULE
{
	char*  identifier;/*存放rule 名称*/
	ast * conditions;/*存放rule 条件*/
	list<IDENTIFIER_KV *> identiValueList ;/*存放规则变量和对应的值*/
	
	list<META_KV*> metaList ;/*存放规则变量和对应的值*/
	struct _RULE* chained_to;/*指向下一条rule*/

}RULE;

typedef struct _RULES
{
 RULE * rule;


}RULES;

/*日志结构定义*/
typedef struct _WEBLOG
{

	char * remote_addr;
	char * time_local;
	char * request;
	int http_status;
	long int responsize;
	char * refer;
	char * user_agent;
	struct _WEBLOG* chained_to;//指向下一条rule
	
}WEBLOG;

typedef struct _WEBLOGS
{
	WEBLOG * weblog;
	
}WEBLOGS;

/*结构结构体定义*/

typedef struct _RESULTS
{
 WEBLOG * weblog;
 RULE * rule;
}RESULTS;

/*主要逻辑函数Scanner*/
class Scanner{
    public:
		Scanner();						/*初始化*/
		~Scanner();						/*析构函数，释放内存*/
		void LogFree(WEBLOG * log);		/*释放log链表*/
		int Eval(struct ast *a, list<IDENTIFIER_KV *> kv,WEBLOG * log );
		int Match(int mode,char *str,char * match_value);	/*匹配:字符串包含匹配、正则匹配*/
		void Treefree(struct ast *a);
		int Run();						/*执行语法*/
		RULES* GetRules();
		WEBLOGS * GetLogs();
		void SetLinkTail(int *addr);	/*保存rules链表的尾节点*/
		int * GetLinkTail();			/*获得rules链表的尾节点*/	
		void PrintResult();				/*输出结果*/
		void AddResult( WEBLOG * weblog,RULE * rule);
		tm MonthParse(string time_str);/*时间字符串转时间tm*/
		void SetLogTime(string start_time,string end_time);/*设置扫描日志的时间段*/
		int compare(struct tm tm_time1,struct tm tm_time2);/*时间比较*/
	private:
		int * tailAddr;					/*rules链表的尾节点地址*/
		RULES *rules;
		WEBLOGS *logs;				
		list<RESULTS *> resultList; 	/*存放命中后的rule和log*/
		struct tm tm_starttime;			/*日志扫描开始时间*/
		struct tm tm_endtime;			/*日志扫描结束时间*/
};





