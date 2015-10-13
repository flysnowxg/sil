# sil语言  
作者：flysnowxg  
emal:flysnowxg@qq.com  
sil语言（simple interpretative lanuage）是一个简单的脚本语言，只是一个玩具，目的是演示用简短的代码去创建一个可用的脚本语言  
这样一个玩具会是怎么样的呢？  
sil的设计目标：  
1 . 非常容易将sil解释器嵌入到c++代码中  
2 . 非常容易用c++代码来扩展sil的函数调用，使得c++和sil脚本非常容易交互  
3 . 成为一个有简洁语法的动态语言，有容易使用的语法  
4 . 拥有一个语言一般都应当拥有的语法  
  
sil语法的完整定义可参考《sil语法说明》。  
sil语言是动态类型的，编译时不会检查函数的参数个数，参数类型是否合适，甚至不会检查函数定义是否存在，只有到了运行时才会查找函数，检查参数个数是否匹配。  
对于内置函数还会检查参数类型是否匹配，如果不匹配会试着进行参数类型转换  
对于用户定义函数，不会进行参数类型匹配的检查  
第一节 语法：  
1. 类型：  
	sil暂时支持整形，浮点型，字符串三种数据类型，暂时不支持数组，也不支持自定义类型（这两点是比较严重的缺点）  
	sil是弱类型的，变量不会和类型绑定  
	变量定义像是这样的：  
	var vi=1; //定义一个值为整数1的变量vi  
	var vf=1.0;//定义一个值为浮点数1.0的变量vf  
	var vs="1.0";//定义一个值为字符串"1.0"的字符串vs  
	  
2. 函数  
	sil是弱类型的，所以定义函数时不需要声明形参的类型，支持return语句  
	函数像这样定义:   
	function myfun(str)  
	{  
		print(str);  
		return 0;  
		print("after return\n");  
	}  
	函数像这样调用：myfun("hello sil");  
	
3. 分支  
	sil支持if else 语句,  
	像这样：  
	var i=read();   
	if(i==1) print("a");  
	else if(i==2)   
	{  
		print("b");  
	}  
	else print("c");  
	  
4. 循环  
	sil支持while和for循环，支持continue，break语句  
	while像这样：  
	var i=1;  
	while(true)  
	{  
		print("hello sil\n");  
		i=i+1;  
		if(i>5) break;  
		if(i<3) continue;  
		print("after continue\n");  
	}  
	for循环像这样  
	for(var i=0;i<5;i=i+1) print(i+"\n");  
  
5. 基本运算符  
	比较运算支持 == != > <  
	算术运算支持 + - * / % ，还支持一元 -  
	逻辑运算支持 !  && ||  
	支持括号 ( ) 改变求值顺序  
	算符优先级和c中一样  
	  
5. 内置函数  
	sil类内置函数是非常少的，详见函数说明，以下列出两个比较重要的。  
	eval 可对一个字符串形式的sil代码求值  
	例如 eval("for(var i=0;i<5;i=i+1)print(i);");  
	load 可以加载一个sil代码文件，代码文件中亦可递归调用load函数  
	  
6. c++嵌入和扩展  
	寥寥数行代码即可将sil嵌入到c++中  
	一个c函数只要形参和返回值类型是int float string，简单调用一个register_function即可将函数注册到sil中，脚本即可方便的调用这些扩展函数  
	extern工程中的代码示例了如何对sil提供文件读写函数的支持  
	  
第二节 源代码  
	这是一个用vs2008创建的工程，由于使用了shared_ptr，如果给vs2005引入shared_ptr的支持亦可在vs2005下编译通过  
	src目录下是sil语言用c++实现的内核  
	consle目录下是sil语言的命令行解释器  
	extern目录下是一个例子，演示了如何扩展sil的内置函数  
	lib目录下是用sil写的库代码和一些测试代码，但是现在只有简单的测试代码（里面有一个开平方根和求圆周率的有意思的例子）  
	doc目录下是文档  
	bin目录下是可执行文件  
	  
第三节 函数说明  
	to_int 将一个值转换为int类型，例如：to_int("123");  
	to_float 将一个值转换为float类型，例如：to_float(2);  
	to_string 将一个值转换为string类型，例如：to_string(254);  
	strlen 求字符串的长度，例如：strlen("hello");  
	substr 截取字符串的的一部分，例如：substr("hello",1,3);  
	eval 可以求值一个字符串形式的表达式，例如:  var code="1+2*3";eval(code);  
	load 可以加载并执行一个sil代码的文件，例如： load("../lib/math_test.sil");  
	exit 终止脚本的执行，例如：print("hello");exit();print("world");  
	print 打印一个值，例如：print("hello world"+3);  
	read 可以从控制台读取一个字符串，例如：var tmp=read();print(tmp);  
	bat 可以执行一个windows命令，例如：bat("dir");  
	list_function 打印已经定义的内置函数和脚本函数，例如：list_funciton();  
	list_asm_code 打印编译出的代码，例如：list_asm_code();  
	set_sil 可以设置解释器的一些开关，例如：set_sil("",0);  
	help 显示帮助信息，例如：help();  
  
  