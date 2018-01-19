#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "scanner.h"
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include <cstring>
#include "google/gflags.h"

extern list<IDENTIFIER_KV *> identiValueList ;
extern list<META_KV*> metaList ;
extern "C" int yyparse();
extern "C" int  yylex();
extern  void yyrestart( FILE * input_file );
extern "C" FILE *yyin;
extern "C" Scanner *scanner;
static unsigned char hexchars[] = "0123456789ABCDEF";



/*gflags*/
DEFINE_bool(a, false, "print all result");
DEFINE_bool(ip, false, "print ip");
DEFINE_bool(time, false, "print time");
DEFINE_bool(request, false, "print all request");
DEFINE_bool(status, false, "print all status");
DEFINE_bool(size, false, "print all size");
DEFINE_bool(refer, false, "print all refer");
DEFINE_bool(useragent, false, "print all useagent");
DEFINE_string(st, "1900/01/01:00:00:00", "the  start time of web log");
DEFINE_string(et, "2999/01/01:00:00:00", "the end time web log");
static int php_htoi(char *s)  
{  
    int value;  
    int c;  
  
    c = ((unsigned char *)s)[0];  
    if (isupper(c))  
        c = tolower(c);  
    value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;  
  
    c = ((unsigned char *)s)[1];  
    if (isupper(c))  
        c = tolower(c);  
    value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;  
  
    return (value);  
}  
string urldecode(string str_source)  
{  
    char const *in_str = str_source.c_str();  
    int in_str_len = strlen(in_str);  
    int out_str_len = 0;  
    string out_str;  
    char *str;  
  
    str = strdup(in_str);  
    char *dest = str;  
    char *data = str;  
  
    while (in_str_len--) {  
        if (*data == '+') {  
            *dest = ' ';  
        }  
        else if (*data == '%' && in_str_len >= 2 && isxdigit((int) *(data + 1))   
            && isxdigit((int) *(data + 2))) {  
                *dest = (char) php_htoi(data + 1);  
                data += 2;  
                in_str_len -= 2;  
        } else {  
            *dest = *data;  
        }  
        data++;  
        dest++;  
    }  
    *dest = '\0';  
    out_str_len =  dest - str;  
    out_str = str;  
    free(str);  
    return out_str;  
}


char* yr_strdup(const char *s)
{
  size_t len = strlen(s);
  char *r = ( char *)malloc(len + 1);
  strcpy(r, s);
  return r;
}

struct ast *newast(int nodetype, struct ast *l, struct ast *r)
{
  struct ast *a =(ast *) malloc(sizeof(struct ast));
  if(!a)
  {
    yyerror("out of space");
    exit(0);
  }

  a->nodetype = nodetype;
  a->l = l;
  a->r = r;
  return a;
}

struct ast *NewIdentifier(char* str1)
{
	struct IdentifierVal * identiNode=(IdentifierVal *)malloc(sizeof(struct IdentifierVal));
	if(!identiNode)
	{
		yyerror("out of space");
		exit(0);		
	}

	identiNode->nodetype='K';
	identiNode->str=str1;
	return  (struct ast *)identiNode;
}


struct ast *NewMethod(char nodetype,int type)
{
	struct RequestMethodVal * identiNode=(RequestMethodVal *)malloc(sizeof(struct RequestMethodVal));
	if(!identiNode)
	{
		yyerror("out of space");
		exit(0);		
	}

	identiNode->nodetype=nodetype;
	identiNode->type=type;
	return  (struct ast *)identiNode;
}

bool existIdentifier(string identifier)
{
	
	list<IDENTIFIER_KV *>::iterator it;
	for (it = identiValueList.begin(); it != identiValueList.end(); ++it)   
	{
			if((*it)->key==identifier)
			{
				return true;
			}
	}	
	
	return false;
}


void InsertIdentifierValue(char * identifier,char* value,int mode,int64_t match_type,int64_t oprate_type)
{
	struct IDENTIFIER_KV * identifier_kv=(IDENTIFIER_KV *)malloc(sizeof(struct IDENTIFIER_KV));
	if(!identifier_kv)
	{
		yyerror("out of space");
		exit(0);		
	}
	if(strlen(value)==0)
	{
		yyerror(("identifier:"+string(identifier)+" value empty").c_str());
		exit(0);	
		
	}
	if(existIdentifier(identifier))
	{	
		yyerror(("exist identifier:"+string(identifier)).c_str());
		exit(0);
		
	}
	identifier_kv->key=identifier;
	identifier_kv->value=value;
	identifier_kv->mode=mode;
	identifier_kv->type=match_type;
	identifier_kv->oprate_type=oprate_type;
	identiValueList.push_back(identifier_kv);	
	

}

void InsertMetaValue(char* identifier,char* value)
{

	 
	 struct META_KV * meta_kv=(META_KV *)malloc(sizeof(struct META_KV));
	if(!meta_kv)
	{
		yyerror("out of space");
		exit(0);		
	}
	if(strlen(value)==0)
	{
		yyerror(("identifier:"+string(identifier)+" value empty").c_str());
		exit(0);	
		
	}
	if(existIdentifier(identifier))
	{	
		yyerror(("exist identifier:"+string(identifier)).c_str());
		exit(0);
		
	}
	meta_kv->key=identifier;
	meta_kv->value=value;	
	metaList.push_back(meta_kv);	
	 
	 
}

void ClearMapValue()
{
	 identiValueList.clear();
     metaList.clear();
}

void yr_free(char *str)
{
	free(str);
}

struct ast *NewLogic_Op(int cmptype, struct ast *l, struct ast *r)
{
  struct ast *a =( ast *) malloc(sizeof(struct ast));
  
  if(!a) {
    yyerror("out of space");
    exit(0);
  }
  a->nodetype = '0' + cmptype;
  a->l = l;
  a->r = r;
  return a;
}

void Scanner::Treefree(struct ast *a)
{
  switch(a->nodetype) 
  {
    case '7': case '8' :
   	{
   		Treefree(a->r);
   		Treefree(a->l);
   		break;
   	}
	case '9':
	{
		Treefree(a->l);
		break;
	}
	case 'M': case 'S':
	{

		break;
	}
    case 'K':
   {
	   free(((IdentifierVal*)a)->str);
	   break;
   }
    default: printf("internal error: free bad node %c\n", a->nodetype);
  }	  
  free(a); /* always free the node itself */
}
void yyerror(const char *s, ...)
{
  va_list ap;
  va_start(ap, s);
  fprintf(stderr, "Line %d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");

}


Scanner::Scanner()
{
	this->rules=(RULES *)malloc(sizeof(RULES));
	if(!rules)
	{
		yyerror("out of space");
		exit(0);
	}
	this->rules->rule=NULL;
	
	this->logs=(WEBLOGS *)malloc(sizeof(WEBLOGS));
	if(!logs)
	{
		yyerror("out of space");
		exit(0);
	}
	this->logs->weblog=NULL;
	
}
void Scanner::LogFree(WEBLOG * log)
{

	if(log->remote_addr) free(log->remote_addr);
	if(log->time_local) free(log->time_local);
	if(log->request) free(log->request);
	if(log->refer) free(log->refer);
	if(log->user_agent) free(log->user_agent);

	if(log->chained_to!=NULL)
	{
		LogFree(log->chained_to);

	}

	delete log;

}


Scanner::~Scanner()
{

	_RULE * rule=this->GetRules()->rule;
	_RULE * tmp=NULL;	
	while(rule!=NULL)	
	{
		free(rule->identifier);
		this->Treefree(rule->conditions);
		
		/*释放meta string，value*/
        list<META_KV *>::iterator iter;
        for(iter=(rule->metaList).begin();iter!=(rule->metaList).end();iter++)
        {

             META_KV * meta_kv=*iter;			  
             if(meta_kv->key)
              	free(meta_kv->key);
             if(meta_kv->value)
              	free(meta_kv->value);
			 if(meta_kv)
				free(meta_kv);			

         }
		 rule->metaList.clear();
		 
		/*释放规则string，value*/
        list<IDENTIFIER_KV *>::iterator iter1;
        for(iter1=(rule->identiValueList).begin();iter1!=(rule->identiValueList).end();iter1++)
        {

              IDENTIFIER_KV * identifier_kv=*iter1;
			  
             if(identifier_kv->key)
              	free(identifier_kv->key);
             if(identifier_kv->value)
              	free(identifier_kv->value);
			 if(identifier_kv)
				free(identifier_kv);

         }

		rule->identiValueList.clear();
		
		/*释放结果*/
		list<RESULTS *>::iterator iter2;
		for(iter2=(scanner->resultList).begin();iter2!=(scanner->resultList).end();iter2++)
		{
			free(*iter2);

		}
	
		
		
		
		tmp=rule;
	    rule=rule->chained_to;	
	    delete tmp;
	}

	
	WEBLOG * log=scanner->GetLogs()->weblog;
	LogFree(log);
	
	free(this->rules);
	free(this->logs);




}
void Scanner::SetLinkTail(int *addr)//保存rules链表的尾节点
{
	
	this->tailAddr=addr;
	
}

int * Scanner::GetLinkTail()//获得rules链表的尾节点
{
	
	return this->tailAddr;
	
}	

int  Scanner::Match(int mode,char *str,char * match_value)/*匹配:字符串包含匹配、正则匹配*/
{
	int ret=0;
	regex_t oRegex;
	/*
	const char * pRegexStr ="union";  
    const char * pText = "POST /1.php?id=1 union select from ttt HTTP/1.1";  
   
    regex_t oRegex1;  
    int nErrCode = 0;  
    char szErrMsg[1024] = {0};  
    size_t unErrMsgLen = 0;  
    cout<<pRegexStr<<"   "<<pText<<endl; 
    if ((nErrCode = regcomp(&oRegex1, pRegexStr, REG_EXTENDED|REG_NEWLINE)) == 0)  
    {  
        if ((nErrCode = regexec(&oRegex1, pText, 0, NULL, 0)) == 0)  
        {  
            printf("%s matches %s\n", pText, pRegexStr);  
            regfree(&oRegex1);  
            return 0;  
        }  
    }  
	*/

	if(mode==TYPE_MATCH_NORMAL&&strstr(str,match_value))
	{		
			ret=1;
	}else if(mode==TYPE_MATCH_REGEX &&regcomp(&oRegex, match_value,REG_EXTENDED|REG_NEWLINE|REG_ICASE|REG_NOSUB) == 0 )
	{		
		if(regexec(&oRegex,str, 0, NULL, 0) == 0)
			ret=1;
			
		regfree(&oRegex);	
	}


	return ret;
}
int Scanner::Eval(struct ast *a, list<IDENTIFIER_KV *> kv,WEBLOG * log )
{
  int ret=-1;
  if(!a) 
  {
    yyerror("internal error, null eval");
	exit(0);
  }

  switch(a->nodetype) 
  {
	case 'K': 
	{			
		string identifier=((struct IdentifierVal *)a)->str;
		char *value=NULL;
		char * url=NULL;
		string url_str;
		int mode=-1;
		int64_t type=0;//匹配 url 或者 agent 或者 refer
		int64_t oprate_type=0;//0 默认,1 url解码
		list<IDENTIFIER_KV *>::iterator it;//=kv.find(identifier); 
		for (it = kv.begin(); it != kv.end(); ++it)   
		{
			if((*it)->key==identifier)
			{
				value=(*it)->value;
				mode=(*it)->mode;
				type=(*it)->type;
				oprate_type=(*it)->oprate_type;
			}
		}	
		
		if(value==NULL)
		{
			yyerror(("Invalid identifier:"+identifier).c_str());
			exit(0);
		}
		if(oprate_type==1) 	/*url解码*/
		{	
			url_str=urldecode(log->request);
			url=const_cast<char*>(url_str.c_str());
	
		}
		else 				/*不需要url解码*/
			url=log->request;
		
		if(type & TYPE_GFLAGS_REQUEST || type==0 ) //type=0,代表默认匹配 url
		{	
			ret=this->Match(mode,url,value);
			
		}else if(type & TYPE_GFLAGS_REFER)
		{
			ret=this->Match(mode,log->refer,value)	;	
			
		}else if(type & TYPE_GFLAGS_AGENT)
		{
			ret=this->Match(mode,log->user_agent,value)	;	
			
		}else 
		{
			yyerror("error match type");
			exit(0);	
		}
		

	}
	break;
	case 'M':
	{
		int request_method=((struct RequestMethodVal *)a)->type;	
		string request=log->request;
		if(request_method==1 && request.find("GET")==0) //GET method
		{			
			return 1;	
		}else if(request_method==2 && request.find("POST")==0) //POST method
		{
			return 1;	
		}
		return 0;
	}
	
	break;
	case 'S':
	{//&&
		int status=((struct RequestMethodVal *)a)->type;
		if(log->http_status==status)
		{
			
			ret=1;
		}else
		{
			ret=0;
			
		}
		
	}
	break;
	case '7':
	{//&&
		ret=(Eval(a->l,kv,log) && Eval(a->r,kv,log))?1:0;	
	}
	break;
	case '8':
	{//||
		ret=(Eval(a->l,kv,log) || Eval(a->r,kv,log))?1:0;	
	}
	break;
	case '9':
	{//not
		ret=(!Eval(a->l,kv,log))?1:0;	
	}
	break;
	default: printf("internal error: bad node %c\n", a->nodetype);
  }
  return ret;
	
	
}



int Scanner::Run()
{


	
	_RULE * rule=scanner->GetRules()->rule;
	
	while(rule!=NULL)		
	{ 
	  /*cout<<rule->identifier<<endl;	
		map<string,string>::iterator it;	
		for(it=(rule->identiValueMap).begin();it!=(rule->identiValueMap).end();it++)
		{	
				cout<<"identifier: "<<it->first<<" ="<<it->second<<endl;
		}
		
        for(it=(rule->metaMap).begin();it!=(rule->metaMap).end();it++)
		{	
				cout<<"meta: "<<it->first<<" ="<<it->second<<endl;
		}*/
		list<RESULTS *> resultList;
	
		WEBLOG * log=scanner->GetLogs()->weblog;
		while(log!=NULL)
		{	tm logtime=MonthParse(log->time_local);
			/*cout<<"logtime = "<<logtime.tm_year+1900<<" "<<logtime.tm_mon+1<<" "<<logtime.tm_mday<<" "<<logtime.tm_hour<<" "<<logtime.tm_min<<" "<<logtime.tm_sec<<endl;
			cout<<"tm_starttime = "<<this->tm_starttime.tm_year+1900<<" "<<this->tm_starttime.tm_mon+1<<" "<<this->tm_starttime.tm_mday<<" "<<this->tm_starttime.tm_hour<<" "<<this->tm_starttime.tm_min<<" "<<this->tm_starttime.tm_sec<<endl;
			cout<<"tm_endtime = "<<this->tm_endtime.tm_year+1900<<" "<<this->tm_endtime.tm_mon+1<<" "<<this->tm_endtime.tm_mday<<" "<<this->tm_endtime.tm_hour<<" "<<this->tm_endtime.tm_min<<" "<<this->tm_endtime.tm_sec<<endl;
			
			cout<<compare(this->tm_starttime,this->tm_endtime)<<endl;
			cout<<compare(this->tm_starttime,logtime)<<endl;			
			cout<<compare(logtime,this->tm_endtime)<<endl;		*/	
			
			if(compare(this->tm_starttime,this->tm_endtime)<=0&&
			   compare(this->tm_starttime,logtime)<=0&&
			   compare(logtime,this->tm_endtime)<=0)
			if(Eval(rule->conditions,rule->identiValueList,log))/*判断是否命中*/
			{
				
				AddResult(log,rule); /*保存命中的规则和日志*/
				
			}
			
			log=log->chained_to;	
		}
	
		rule=rule->chained_to;		
	
	}	
}


void Scanner::AddResult( WEBLOG * weblog,RULE * rule)
{

	RESULTS * results =( RESULTS *) malloc(sizeof( RESULTS));
  
	if(!results) {
		yyerror("results out of space");
		exit(0);
	}
	results->weblog=weblog;
	results->rule=rule;
	this->resultList.push_back(results);
}

RULES* Scanner::GetRules()
{
		return this->rules;
}


WEBLOGS * Scanner::GetLogs()
{
		return this->logs;
}
void Scanner::PrintResult()
{

    list<RESULTS *>::iterator iter1;
    for(iter1=(scanner->resultList).begin();iter1!=(scanner->resultList).end();iter1++)
    {
		WEBLOG * weblog=(*iter1)->weblog;
		RULE * rule=(*iter1)->rule;
		
		
		if(!(FLAGS_ip||FLAGS_time||FLAGS_request||FLAGS_status||FLAGS_size||FLAGS_refer||FLAGS_useragent))
		{	cout<<"\n-----------------------------------"<<endl;
			cout<<"Rule:       "<<rule->identifier<<endl;
			cout<<"IP:         "<<weblog->remote_addr<<endl;
			cout<<"Time:       "<<weblog->time_local<<endl;
			cout<<"Request:    "<<weblog->request<<endl;
			cout<<"Status:     "<<weblog->http_status<<endl;
			cout<<"size:       "<<weblog->responsize<<endl;
			cout<<"Refer:      "<<weblog->refer<<endl;
			cout<<"User_Agent: "<<weblog->user_agent<<endl;
			continue;
		}
		
		cout<<"\n-----------------------------------"<<endl;
		cout<<"Rule:       "<<rule->identifier<<endl;
		if(FLAGS_ip)		cout<<"IP:         "<<weblog->remote_addr<<endl;
		if(FLAGS_time)		
		{	
			cout<<"Time:       "<<weblog->time_local<<endl;
			}
	
		if(FLAGS_request)	cout<<"Request:    "<<weblog->request<<endl;
		if(FLAGS_status)	cout<<"Status:     "<<weblog->http_status<<endl;
		if(FLAGS_size)		cout<<"size:       "<<weblog->responsize<<endl;
		if(FLAGS_refer)		cout<<"Refer:      "<<weblog->refer<<endl;
		if(FLAGS_useragent)	cout<<"User_Agent: "<<weblog->user_agent<<endl;
		

    }
	
	
	
	
}



int Scanner::compare(struct tm tm_time1,struct tm tm_time2)
{
	
    int tm1 = tm_time1.tm_year*10000+tm_time1.tm_mon*100+tm_time1.tm_mday;
    int tm2 = tm_time2.tm_year*10000+tm_time2.tm_mon*100+tm_time2.tm_mday;

    if(tm1!=tm2) return tm1-tm2;/*比较年月日*/
    tm1 = tm_time1.tm_hour*3600+tm_time1.tm_min*60+tm_time1.tm_sec;
    tm2 = tm_time2.tm_hour*3600+tm_time2.tm_min*60+tm_time2.tm_sec;/*将时分秒转换为秒数*/

    if(tm1!=tm2) return tm1-tm2;/*比较时分秒*/
	
	return 0;/*时间相等*/
	
}

tm Scanner::MonthParse(string time_str)
{
	string  month_array[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	int i;
	for(i=0;i<12;i++)
	{	//cout<<month_array[i]<<endl;
		int index=time_str.find(month_array[i]);
		if(index!=-1)
		{	
			time_str = time_str.replace(index, 3, to_string(i+1)); //用str替换从指定迭代器位置的字符串 
			break;
		}
	}
	if(i==12)
	{	
		cout<<"bad month = "<<time_str<<endl;
	
	}
	
	struct tm tm_time;
	strptime(time_str.c_str(), "%d/%m/%Y:%H:%M:%S", &tm_time);  
	//cout<<tm_time.tm_year+1900<<" "<<tm_time.tm_mon+1<<" "<<tm_time.tm_mday<<" "<<tm_time.tm_hour<<" "<<tm_time.tm_min<<" "<<tm_time.tm_sec<<endl;
	
	
	
	return tm_time;
}
void Scanner::SetLogTime(string start_time,string end_time)
{
	if(start_time!="1900/01/01:00:00:00")
		cout<<"Start log time = "<<start_time<<endl;
	strptime(start_time.c_str(), "%Y/%m/%d:%H:%M:%S", &(this->tm_starttime));
	if(end_time!="2999/01/01:00:00:00")
		cout<<"End   log time = "<<end_time<<endl;
	strptime(end_time.c_str(), "%Y/%m/%d:%H:%M:%S", &(this->tm_endtime));
	//cout<<this->tm_starttime.tm_year+1900<<" "<<this->tm_starttime.tm_mon+1<<" "<<this->tm_starttime.tm_mday<<" "<<this->tm_starttime.tm_hour<<" "<<this->tm_starttime.tm_min<<" "<<this->tm_starttime.tm_sec<<endl;
	//cout<<this->tm_endtime.tm_year+1900<<" "<<this->tm_endtime.tm_mon+1<<" "<<this->tm_endtime.tm_mday<<" "<<this->tm_endtime.tm_hour<<" "<<this->tm_endtime.tm_min<<" "<<this->tm_endtime.tm_sec<<endl;
}

int main(int argc,char** argv)
{	

	google::SetUsageMessage("Uasge: ./scanner ");
	google::ParseCommandLineFlags(&argc, &argv, true);

	int ret;
	scanner=new Scanner();	
	if(argc<2)
	{
		printf("> "); 
		int ret= yyparse();
	}
	else
	{	
		FILE *f = fopen(argv[1], "r");
		yyin=f;
		ret=  yyparse();
		yyrestart(f);

		yylineno=0;/*清空行号,以备下一个文件记录行号*/
		f = fopen(argv[2], "r");
		yyin=f;
		ret=  yyparse();
        fclose(f);
	}
	scanner->SetLogTime(FLAGS_st,FLAGS_et);//设置扫描日志的时间段
	scanner->Run();
	
	scanner->PrintResult();
	delete scanner;
	google::ShutDownCommandLineFlags();
	return ret;
}

