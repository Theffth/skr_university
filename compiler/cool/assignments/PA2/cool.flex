/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */

static int comment_layer=0;
static std::string input;
static int flag=0;
%}

/*
 * Define names for regular expressions here.
 */

DARROW          =>
DIGIT           [0-9]
one_char_oper   [:+\-*/=)(}{~@.,;<]
%x COMMENT_IN_LINE
%x COMMENT
%x STRING

%%

<INITIAL>"--" {BEGIN COMMENT_IN_LINE;}
<COMMENT_IN_LINE>\n   {
  curr_lineno++;
  BEGIN INITIAL;
}
<COMMENT_IN_LINE><<EOF>> {
  BEGIN INITIAL;
}
<COMMENT_IN_LINE>.  {}

 /*
  *  Nested comments
  */
<INITIAL,COMMENT>"(*" {
  comment_layer++;
  BEGIN COMMENT;
}
<COMMENT>"*)" {
  comment_layer--;
  if(comment_layer==0){
    BEGIN INITIAL;
  }
}
<COMMENT>\n {
  curr_lineno++;
}
<COMMENT><<EOF>> {
  BEGIN INITIAL;
  yylval.error_msg="EOF in comment";
  return ERROR;
}
<COMMENT>. { }
"*)" {
  yylval.error_msg="unmatched )*";
  return ERROR;
}

 /*
  *  The multiple-character operators.
  */
"<-"  {return ASSIGN;}
"<="  {return LE;}
  /*"=>"  {return DARROW};*/
{DARROW}		{ return (DARROW); }

 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */
(?i:class)  {return CLASS;}
(?i:else)  {return ELSE;}
(?i:fi)  {return FI;}
(?i:if)  {return IF;}
(?i:in)  {return IN;}
(?i:inherits)  {return INHERITS;}
(?i:isvoid)  {return ISVOID;}
(?i:let)  {return LET;}
(?i:loop)  {return LOOP;}
(?i:pool)  {return POOL;}
(?i:then)  {return THEN;}
(?i:while)  {return WHILE;}
(?i:case)  {return CASE;}
(?i:esac)  {return ESAC;}
(?i:new)  {return NEW;}
(?i:of)  {return OF;}
(?i:not)  {return NOT;}
t(?i:rue)  {
  yylval.boolean = 1;
  return BOOL_CONST;
}
f(?i:alse)  {
  yylval.boolean = 0;
  return BOOL_CONST;
}
{DIGIT}+     {
  yylval.symbol = inttable.add_string(yytext);
  return INT_CONST; 
}
{one_char_oper} {
  return int(yytext[0]);
}
[A-Z][A-Za-z0-9_]* {
  yylval.symbol = idtable.add_string(yytext);
  return TYPEID;
}
[a-z][A-Za-z0-9_]*  {
  yylval.symbol = idtable.add_string(yytext);
  return OBJECTID;
}
\n   {curr_lineno++;}
[ \t\f\r\v]  { }

 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */
<INITIAL>\" {
  BEGIN STRING;
  input="";
}

<STRING>\"  {
  BEGIN INITIAL;
  if(input.size()>=MAX_STR_CONST) {
    yylval.error_msg="String constant too long";
    return ERROR;
  }
  if(flag){
    yylval.error_msg="String contains escaped null character.";
    return ERROR;
  }
  yylval.symbol = stringtable.add_string((char*)input.c_str());
  return STR_CONST;
}
<STRING>\\\n {
  curr_lineno++;
  input+='\n';
}
<STRING>\n {
  BEGIN INITIAL;
  curr_lineno++;
  yylval.error_msg="\n not allowed in string";
  return ERROR;  
}


<STRING>\0 {
  flag=1;
}

<STRING>\\. {
  switch(yytext[1]){
  case 'b':
  {input+='\b';break;}
  case 't':
  {input+='\t';break;}
  case 'f':
  {input+='\f';break;}
  case 'n':
  {input+='\n';break;}
  case '\0':
  {flag=1; break;}
  default:
  {input+=yytext[1];}
}
}
<STRING><<EOF>> {
  BEGIN INITIAL;
  yylval.error_msg="Wrong EOF in string";
  return ERROR;
}
<STRING>. {
  input+=yytext;
}

. {
  yylval.error_msg=yytext;
  return ERROR;
}
%%
