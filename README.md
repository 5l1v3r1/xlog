# xlog</br>
web日志扫描工具</br>
</br>
</br>
一、语法规则</br>
//例1: 检测url</br>
//含义:请求url种含有$str,不含有$str1 且 请求为post 或者get方法,且返回请求为404的</br>
//关键字url:用于修饰str变量为匹配请求的url</br>
//关键字GET、POST:表示请求为GET\POST</br>
//关键字status:表示http status</br>
//关键字not:表示不满足 </br>
</br>
rule yara_attackAdmin {</br>
　　meta:</br>
　　　　description = "attack admin" </br>
　　strings:</br>
　　　　$str="admin.php" url</br>
　　　　$str1="feed"  url</br>
		</br>
　　condition:</br>
　　　　$str and (GET or POST) and status=404 and not $str1</br>
}</br>
</br>
//例2:检测agent</br>
//含义:请求agent含有DNSPod-Monitor的</br>
//关键字agent:用于修饰str变量为匹配请求的agent参数</br>
rule yara_agent {</br>
　　meta:</br>
　　　　description = "Detect agent" </br>
　　strings:</br>
　　　　$str="DNSPod-Monitor" agent		</br>
　　condition:</br>
　　　　$str and (GET or POST)</br>
}</br>
</br>
</br>
//例3:检测refer</br>
//含义:请求refer为某个链接</br>
//关键字refer:用于修饰str变量为匹配请求的refer参数</br>
rule yara_refer {</br>
　　meta:</br>
		description = "Detect refer" </br>
　　strings:</br>
		$str="http://blog.xxxx.me/wp-admin/post.php?post=2445&action=edit&message=10" refer		</br>
　　condition:</br>
		$str</br>
}</br>
</br>
</br>
//例4:正则检测url</br>
//含义:请求url为满足正则表达式shell[1-9].php</br>
//关键字双斜杠:双斜杠里面的内容为正则表达式</br>
//关键字url:用于修饰str变量为匹配请求的url</br>
rule yara_reg {</br>
　　meta:</br>
　　　　description = "Detect refer" </br>
　　strings:</br>
　　　　$str=/shell[1-9].php/ url		</br>
　　condition:</br>
　　　　$str  </br>
}</br>
二、使用</br>
1、下载源码后，可直接make可生产scanner程序</br>
2、执行：./scanner   规则文件路径  web访问日志</br>
3、输出命中的结果：</br>
　　　　可根据命令行定制化输出</br>
　　　　-a                  输出全部结果(默认)</br>
　　　　-ip                 输出ip</br>
　　　　-refer              输出ip</br>
　　　　-request            输出url</br>
　　　　....</br>
　　　　-st 参数time       输出访问时间大于time的结果</br>
　　　　-et 参数time       输出访问时间小雨time的结果      </br>
