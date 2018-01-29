# xlog
web日志扫描工具


一、语法规则
//例1: 检测url
//含义:请求url种含有$str,不含有$str1 且 请求为post 或者get方法,且返回请求为404的
//关键字url:用于修饰str变量为匹配请求的url
//关键字GET、POST:表示请求为GET\POST
//关键字status:表示http status
//关键字not:表示不满足 

rule yara_attackAdmin {
	meta:
		description = "attack admin" 
	strings:
		$str="admin.php" url
		$str1="feed"  url
		
	condition:
		$str and (GET or POST) and status=404 and not $str1
}

//例2:检测agent
//含义:请求agent含有DNSPod-Monitor的
//关键字agent:用于修饰str变量为匹配请求的agent参数
rule yara_agent {
	meta:
		description = "Detect agent" 
	strings:
		$str="DNSPod-Monitor" agent		
	condition:
		$str and (GET or POST)
}


//例3:检测refer
//含义:请求refer为某个链接
//关键字refer:用于修饰str变量为匹配请求的refer参数
rule yara_refer {
	meta:
		description = "Detect refer" 
	strings:
		$str="http://blog.xxxx.me/wp-admin/post.php?post=2445&action=edit&message=10" refer		
	condition:
		$str 
}


//例4:正则检测url
//含义:请求url为满足正则表达式shell[1-9].php
//关键字双斜杠:双斜杠里面的内容为正则表达式
//关键字url:用于修饰str变量为匹配请求的url
rule yara_reg {
	meta:
		description = "Detect refer" 
	strings:
		$str=/shell[1-9].php/ url		
	condition:
		$str  
}
二、使用
1、下载源码后，可直接make可生产scanner程序
2、执行：./scanner   规则文件路径  web访问日志
3、输出命中的结果：
       可根据命令行定制化输出
       -a                       输出全部结果(默认)
       -ip                       输出ip
       -refer                  输出ip
       -request              输出url
        ....
        -st 参数time       输出访问时间大于time的结果
        -et 参数time       输出访问时间小雨time的结果      
