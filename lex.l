%{
/*本lex的生成文件是lex.yy.c
lex文件由3段组成，用2个%%行把这3段隔开。
第1段是声明段，包括：
1-C代码部分：include头文件、函数、类型等声明，这些声明会原样拷到生成的.c文件中。
2-状态声明，如%x COMMENT。
3-正则式定义，如digit ([0-9])。
第2段是规则段，是lex文件的主体，包括每个规则(如identifier)是如何匹配的，以及匹配后要执行的C代码动作。
第3段是C函数定义段，如yywrap()的定义，这些C代码会原样拷到生成的.c文件中。该段内容可以为空*/

//第1段：声明段
#include <string.h>
#include "main.h"		//lex和yacc要共用的头文件，里面包含了一些头文件，重定义了YYSTYPE
#include "parser_internal.h"
#include "yacc.tab.h"	//用yacc编译yacc.y后生成的C头文件，内含%token、YYSTYPE、yylval等定义(都是C宏)，供lex.yy.c和yacc.tab.c使用

extern "C"				//为了能够在C++程序里面调用C函数，必须把每一个需要使用的C函数，其声明都包括在extern "C"{}块里面，这样C++链接时才能成功链接它们。extern "C"用来在C++环境下设置C链接类型。
{						//yacc.y中也有类似的这段extern "C"，可以把它们合并成一段，放到共同的头文件main.h中
	int yywrap(void);
	int yylex(void);	//这个是lex生成的词法分析函数，yacc的yyparse()里会调用它，如果这里不声明，生成的yacc.tab.c在编译时会找不到该函数
	int get_id(char *s);
	char *get_qstring(char *qstring, int len);
}
%}

/*lex的每个正则式前面可以带有"<状态>"，例如下面的"<COMMENT>\n"。每个状态要先用%x声明才能使用。
当lex开始运行时，默认状态是INITIAL，以后可在C代码里用"BEGIN 状态名;"切换到其它状态(BEGIN是lex/yacc内置的宏)。
这时，只有当lex状态切换到COMMENT后，才会去匹配以<COMMENT>开头的正则式，而不匹配其它状态开头的。
也就是说，lex当前处在什么状态，就考虑以该状态开头的正则式，而忽略其它的正则式。
其应用例如，在一段C代码里，同样是串"abc"，如果它写在代码段里，会被识别为标识符，如果写在注释里则就不会。所以对串"abc"的识别结果，应根据不同的状态加以区分。
本例子需要忽略掉文本中的行末注释，行末注释的定义是：从某个"//"开始，直到行尾的内容都是注释。其实现方法是：
1-lex启动时默认是INITIAL状态，在这个状态下，串"abc"会识别为标识符，串"123"会识别为整数等。
2-一旦识别到"//"，则用BEGIN宏切换到COMMENT状态，在该状态下，abc这样的串、以及其它字符会被忽略。只有识别到换行符\n时，再用BEGIN宏切换到初始态，继续识别其它记号。*/
%x comment

/*非数字由大小写字母、下划线组成*/
letter	([A-Za-z])

/*一位数字，可以是0到9*/
digit		([0-9])

/*整数由1至多位数字组成*/
num		({digit}+)

s_num		[+\-]?{num}

/*标识符，以非数字开头，后跟0至多个数字或非数字*/
identifier	({letter}({letter}|{digit}|_)*)

/*下面%%后开始第2段：规则段*/
%%

"/*"			{BEGIN(comment);}
<comment>[^*]		{}
<comment>"*/"	{BEGIN(INITIAL);}
<comment>\*		{}
[ \n\t]			{}

{s_num}			{
				sscanf(yytext,"%d",&yylval.ival);
				return T_INT;
			}

{s_num}\.{num}		{
				sscanf(yytext,"%f",&yylval.rval);
				return T_REAL;
			}

"'"([^\'\n])*"'"       	{
				yylval.sval = get_qstring(yytext, yyleng);
                             			return T_QSTRING;
                             		}

{s_num}\.{num}[Ee]{s_num}	{
					sscanf(yytext,"%f", &yylval.rval);
					return T_REAL;
				}



{identifier}			{	
					return get_id(yylval.sval = yytext);
				}

"<"                  		{return T_LT;}
"<="                 		{return T_LE;}
">"                  		{return T_GT;}
">="                 		{return T_GE;}
"="                 		{return T_EQ;}
"!="                 		{return T_NE;}
"<>"                 		{return T_NE;}

[*/+\-=<>':;,.|&()]  	{
				return yytext[0];
			}

.				{printf("illegal character [%s]\n", yytext);}

%%

//第3段：C函数定义段
int yywrap(void)
{
	puts("-----the file is end");
	return 1;								//返回1表示读取全部结束。如果要接着读其它文件，可以这里fopen该文件，文件指针赋给yyin，并返回0
}

#include "lexhelper.cpp"
