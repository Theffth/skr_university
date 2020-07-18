#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"


extern int semant_debug;
extern char *curr_filename;
extern int node_lineno;
//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}

//functions for class Environment
//构造函数
Environment::Environment (ClassTableP ct,InheritanceNodeP sc):
    method_table(*(new SymbolTable<Symbol,method_class>())),
    var_table(*(new SymbolTable<Symbol,Entry>())),
    class_table(ct),
    self_class(sc)
{
    method_table.enterscope();
    var_table.enterscope();
    var_table.addid(self,SELF_TYPE);
}

Environment::Environment(SymbolTable<Symbol,method_class> mt,
              SymbolTable<Symbol,Entry> vt,
              ClassTableP ct,
              InheritanceNodeP sc):
              method_table(mt),
              var_table(vt),
              class_table(ct),
              self_class(sc){
    method_table.enterscope();
    var_table.enterscope();
}


EnvironmentP Environment::clone_Environment(InheritanceNodeP n)
{
    return new Environment(method_table,var_table,class_table,n);
}

//错误消息打印
ostream& Environment::semant_error(){
    return class_table->semant_error();
}

ostream& Environment::semant_error(tree_node* t){
    return class_table->semant_error(self_class->get_filename(),t);
}

InheritanceNodeP Environment::lookup_class(Symbol s){
    return class_table->probe(s);
}

void Environment::method_add(Symbol s,method_class* m){
    method_table.addid(s,m);
}

method_class* Environment::method_lookup(Symbol s){
    return method_table.lookup(s);
}

method_class* Environment::method_probe(Symbol s){
    return method_table.probe(s);
}

void Environment::method_enterscope(){
    method_table.enterscope();
}

void Environment::method_exitscope(){
    method_table.exitscope();
}

void Environment::var_add(Symbol s,Symbol v){
    var_table.addid(s,v);
}

Symbol Environment::var_lookup(Symbol s){
    return var_table.lookup(s);
}

Symbol Environment::var_probe(Symbol s){
    return var_table.probe(s);
}

void Environment::var_enterscope(){
    var_table.enterscope();
}

void Environment::var_exitscope(){
    var_table.exitscope();
}

Symbol Environment::get_self_type(){
    return self_class->get_name();
}


////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

//
ostream& ClassTable::semant_error(Class_ c){
    return semant_error(c->get_filename(),c);
}

ostream& ClassTable::semant_error(Symbol filename,tree_node* t){
    error_stream<<filename << ":" << t->get_line_number() <<":";
    return semant_error();
}

ostream& ClassTable::semant_error(){
    semant_errors++;
    return error_stream;
}


InheritanceNode::InheritanceNode(Class_ nd,
    Inheritable istatus,Basicness bstatus):
        class__class((const class__class&)*nd),
        parentnd(NULL),
        children(NULL),
        inherit_status(istatus),
        basic_status(bstatus),
        reach_status(UnReachable),
        env(NULL){ }

void InheritanceNode::set_parentnd(InheritanceNodeP p){
    assert(parentnd==NULL);
    assert(p!=NULL);
    parentnd=p;
}

InheritanceNodeP InheritanceNode::get_parentnd(){
    return parentnd;
}


ClassTable::ClassTable(Classes classes) : 
    nds(NULL),semant_errors(0),error_stream(cerr)
{
    enterscope();
    install_basic_classes();
    if(semant_debug) cerr<<"Installed basic classes."<<endl;
    install_classes(classes);
    if(semant_debug){
        cerr<<"Installed user-defined classes"<<endl;
        dump();
    }
    check_improper_inheritance();
    if(semant_debug){
        cerr<<"Checked for simple inheritance errors!"<<endl;
    }
    if(errors()) return;//

    build_inheritance_tree();
    if(semant_debug) {
        cerr<<"Built inheritance tree!"<<endl;
    }
    root()->mark_reachable();
    if(semant_debug){
        cerr<<"Marked reachable classes!"<<endl;
    }
    check_for_cycles();
    if(semant_debug){
        cerr<<"Checked for cycles!"<<endl;
    }
    if(errors()) return;

    build_feature_tables();
    if(semant_debug) {
        cerr<<"Built feature tables."<<endl;
    }
    check_main();
    if(semant_debug) {
        cerr<<"Checked Main class and method!"<<endl;
    }
    root()->type_check_features();
}


void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
    node_lineno=0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    addid(No_class,
        new InheritanceNode(class_(No_class,No_class,nil_Features(),filename),
                                        CanInherit,Basic));
    addid(SELF_TYPE,
        new InheritanceNode(class_(SELF_TYPE,No_class,nil_Features(),filename),
                                        CanInherit,Basic));
    addid(prim_slot,new InheritanceNode(class_(prim_slot,No_class,nil_Features(),filename),
                                        CanInherit,Basic));
    
    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.
    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);
    install_class(new InheritanceNode(Object_class,CanInherit,Basic));
    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);  
    install_class(new InheritanceNode(IO_class,CanInherit,Basic));

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);
    install_class(new InheritanceNode(Int_class,CanInherit,Basic));
    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);
    install_class(new InheritanceNode(Bool_class,CantInherit,Basic));
    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);
    install_class(new InheritanceNode(Str_class,CantInherit,Basic));
}

//会检查出以下错误：
//(1)重复定义基本类
//(2)重复定义用户定义类
//若不存在重复定义，则是合法类，加入到继承节点链表中
void ClassTable::install_class(InheritanceNodeP nd){
    Symbol name=nd->get_name();
    if(probe(name)){
        InheritanceNodeP old=probe(name);
        if(old->basic())
            semant_error(nd)<<"Redefinition of basic class"<<name<<"."<<endl;
        else
        {
            semant_error(nd)<<"Class"<<name<<"was previously defined."<<endl;
        }
        return ;
    }
    nds=new List<InheritanceNode>(nd,nds);
    addid(name,nd);
}

void ClassTable::install_classes(Classes cs)
{
    for(int i=cs->first();cs->more(i);i=cs->next(i)){
        install_class(new InheritanceNode(cs->nth(i),CanInherit,NotBasic));
    }   
}

//简单检查不正确的继承关系
//可以检查以下错误：
//(1)继承自未定义的父类
//(2)继承自不可被继承的父类
//
void ClassTable::check_improper_inheritance(){
    for(List<InheritanceNode>* l=nds;l;l=l->tl()){
        InheritanceNodeP c=l->hd();
        Symbol parent =c->get_parent();
        InheritanceNode* node=probe(parent);

        if(!node){
            semant_error(c)<<"Class "<<c->get_name()<<"inherits from an undefined class "
            <<parent<<"."<<endl;
            continue;
        }
        if(!node->inherit())
            semant_error(c)<<"Class "<<c->get_name()<<" cannot inherit class "
                <<parent<<"."<<endl;
    }
}

//建立继承树->建立父子继承关系
void ClassTable::build_inheritance_tree(){
    for(List<InheritanceNode>* l=nds;l;l=l->tl())   
        set_relations(l->hd());
}

void ClassTable::set_relations(InheritanceNodeP nd){
    InheritanceNode* parent_node=probe(nd->get_parent());
    nd->set_parentnd(parent_node);
    parent_node->add_child(nd);
}

//检查继承图中是否有环存在
//在mark_reachable()函数执行之后
void ClassTable::check_for_cycles(){
    for(List<InheritanceNode>* l=nds;l;l=l->tl()){
        if(!(l->hd()->reachable())){
            semant_error(l->hd())<<"Class "<<l->hd()->get_name()<<
            ", or an ancestor of "<<l->hd()->get_name()<<
            ", is involved in an inheritance cycle."<<endl;
        }
    }
}

void InheritanceNode::add_child(InheritanceNodeP n)
{
    children=new List<InheritanceNode>(n,children);
}

void InheritanceNode::mark_reachable()
{
    reach_status=Reachable;
    for(List<InheritanceNode>* kids=children;kids;kids=kids->tl())
        kids->hd()->mark_reachable();
}

//建立类继承节点中Environment的feature表
//添加该类的每一个attr和method
//遍历所有的子节点，并拷贝父节点的Environment，再进行操作
//这样子节点就继承了父节点全部的attr和method
void InheritanceNode::build_feature_tables()
{
    for(int i=features->first();features->more(i);i=features->next(i))
        features->nth(i)->add_to_table(env);
    for(List<InheritanceNode>* l=children;l;l=l->tl())
    {
        l->hd()->copy_env(env);
        l->hd()->build_feature_tables();       
    }
}

void InheritanceNode::type_check_features(){
    if(semant_debug){
        cerr<<"Type checking class "<<name<<endl;
    }
    for(int i=features->first();features->more(i);i=features->next(i))
        features->nth(i)->tc(env);   
    for(List<InheritanceNode>* l=children;l;l=l->tl())
        l->hd()->type_check_features();
}

//初始化Environment，只用于Object类，其他类均从父类上拷贝
void InheritanceNode::init_env(ClassTableP ct)
{
    env=new Environment(ct,this);
}

//从根节点Object开始递归建立符号表
void ClassTable::build_feature_tables()
{
    root()->init_env(this);
    root()->build_feature_tables();
}

//AST以Object类作为根节点
InheritanceNodeP ClassTable::root(){
    return probe(Object);
}


//将函数添加到method_table中
//会检查以下错误：
//(1)函数重定义
//(2)继承函数的返回类型和参数列表与被继承函数不符
void method_class::add_to_table(EnvironmentP env){
    if(env->method_probe(name)){
        env->semant_error(this)<<"Method "<<name<<" is multiply defined."<<endl;
        return ;
    }
    method_class* old=env->method_lookup(name);
    if(old){
        if(old->get_return_type()!=return_type){
            env->semant_error(this)<<"In redefined method "<<name
            << ", return type " <<return_type<<" is different from original return type "<<
            old->get_return_type()<<"."<<endl;
            return ;
        }
        if(old->num_formals()!=num_formals()){
            env->semant_error(this)<<"Incompatible number of formal parameters in redefined method "
            <<name <<"."<<endl;
            return ;
        }
        Formals old_formals = old->get_formals();
        for(int i=formals->first();formals->more(i);i=formals->next(i)){
            if(old_formals->nth(i)->get_type_decl()!=formals->nth(i)->get_type_decl())
            {
                env->semant_error(this)<<"In redefined method "<<name << ",parameter type "
                <<formals->nth(i)->get_type_decl()<<" is different from original type "<<
                old_formals->nth(i)->get_type_decl()<<endl;
                return ;
            }
        }
    }
    env->method_add(name,this);
}

//将变量添加到var_table
//会检查以下错误：
//(1)attribute的名字不能是self
//(2)在父类或自身类中重复定义
//
void attr_class::add_to_table(EnvironmentP env){
    if (name == self ){
        env->semant_error(this)<<"'self' cannot be the name of an attribute"<<endl;
        return ;
    }
    if(env->var_probe(name)){
        env->semant_error(this)<<"Attribute "<<name <<"is multiply defined in class."<<endl;
        return ;
    }
    if(env->var_lookup(name)){
        env->semant_error(this)<<"Attribute "<<name <<"is an attribute of an inherited class."<<endl;
        return ;
    }
    env->var_add(name,type_decl);
}

//Cool程序必须要有main类,main类中必须有main函数
void ClassTable::check_main(){
    InheritanceNodeP mainclass=probe(Main);
    if(!mainclass) semant_error()<< "Class Main is not defined."<<endl;
    else mainclass->check_main_method();
}

//main类中必须要有main函数，并且不应该有参数
void InheritanceNode::check_main_method()
{
    if(!env->method_probe(main_meth)){
        env->semant_error(this)<<"No 'main' method in class Main."<<endl;
        return ;
    }
    if(env->method_lookup(main_meth)->num_formals()!=0){
        env->semant_error(this)<<"'main' method in class Main should have no arguments."<<endl;
    }
}

//根据规则，判断subtype是否是supertype的子类
int Environment::type_leq(Symbol subtype,Symbol supertype){
    if(!(lookup_class(supertype)&&lookup_class(subtype))){
        return TRUE;
    }
    if(subtype==SELF_TYPE && supertype==SELF_TYPE){
        return TRUE;
    }
    if(supertype==SELF_TYPE) return FALSE;
    if(subtype==SELF_TYPE) subtype=get_self_type();
    if(supertype==SELF_TYPE) supertype=get_self_type();

    InheritanceNodeP y=lookup_class(supertype);
    for(InheritanceNodeP x=lookup_class(subtype);x;x=x->get_parentnd())
    {
        if(x==y) return TRUE;
    }
    return FALSE;
}

//找到两个type的公共父类
Symbol Environment::type_lub(Symbol type1 , Symbol type2){
    if(!lookup_class(type1)) return type2;
    if(!lookup_class(type2)) return type1;
    if(type1==type2)  return type1;
    if(type1==SELF_TYPE) type1=get_self_type();
    if(type2==SELF_TYPE) type2=get_self_type();

    InheritanceNodeP nd;
    for(nd=lookup_class(type1);!type_leq(type2,nd->get_name());nd=nd->get_parentnd());
    return nd->get_name();
}

//按照规则对不同类型进行类型检查
void attr_class::tc(EnvironmentP env){
    if(!env->lookup_class(type_decl))
        env->semant_error(this)<<"Class "<<type_decl<<" of attribute "<<name
        << "is undefined."<<endl;
    if(!env->type_leq(init->tc(env),type_decl)){
        env->semant_error(this)<<"Inferred type "<<init->get_type()<<" of initialization of attribute "
        <<name <<" does not conform to declared type "<<type_decl<<"."<<endl;
    }
}

void method_class::tc(EnvironmentP env)
{
    env->var_enterscope();
    for(int i=formals->first();formals->more(i);i=formals->next(i))
        formals->nth(i)->install_formal(env);
    if(!env->lookup_class(return_type)){
        env->semant_error(this)<<"undefined return type "<<return_type<<" in method "<<name<<"."<<endl;
    }
    if(!env->type_leq(expr->tc(env),return_type))
        env->semant_error(this)<<"Inferred return type "<<expr->get_type()<<" of method "<<name
        <<" does not comform to declared return type "<<return_type<<"."<<endl;
    env->var_exitscope();   
}

void formal_class::install_formal(EnvironmentP env){
    if(type_decl==SELF_TYPE)
        env->semant_error(this)<<"Formal parameter "<<name<<" cannot have type SELF_TYPE."<<endl;
    else {
        if(!env->lookup_class(type_decl))
            env->semant_error(this)<<"Class "<<type_decl<<" of formal parameter "
            <<name<<" is undefined."<<endl;
    }
    if(name == self){
        env->semant_error(this)<<"'self' cannot be the name of formal parameter."<<endl;
        return ;
    }
    if(env->var_probe(name)){
        env->semant_error(this)<<"Formal parameter "<<name<<" is multiply defined."<<endl;
        return ;
    }
    env->var_add(name,type_decl);
}

Symbol int_const_class::tc(EnvironmentP){
    type=Int;
    return Int;
}

Symbol bool_const_class::tc(EnvironmentP){
    type=Bool;
    return Bool;
}

Symbol string_const_class::tc(EnvironmentP){
    type=Str;
    return Str;
}

Symbol plus_class::tc(EnvironmentP env){
    e1->tc(env);
    e2->tc(env);
    if((e1->get_type()!=Int)||(e2->get_type()!=Int))
        env->semant_error(this)<<"non_Int arguments: "<<
            e1->get_type()<<"+"<<e2->get_type()<<endl;
    type=Int;
    return Int;
}

Symbol sub_class::tc(EnvironmentP env){
    e1->tc(env);
    e2->tc(env);
    if((e1->get_type()!=Int)||(e2->get_type()!=Int))
        env->semant_error(this)<<"non-Int arguments: "<<e1->get_type()<<
            "-"<<e2->get_type()<<endl;
    type=Int;
    return Int;
}

Symbol mul_class::tc(EnvironmentP env){
    e1->tc(env);
    e2->tc(env);
    if((e1->get_type()!=Int)||(e2->get_type()!=Int))
        env->semant_error(this)<<"non-Int arguments: "<<e1->get_type()<<
            "*"<<e2->get_type()<<endl;
    type=Int;
    return Int;
}

Symbol divide_class::tc(EnvironmentP env){
    e1->tc(env);
    e2->tc(env);
    if((e1->get_type()!=Int)||(e2->get_type()!=Int))
        env->semant_error(this)<<"non-Int arguments: "<<e1->get_type()<<
            "/"<<e2->get_type()<<endl;
    type=Int;
    return Int;
}

Symbol neg_class::tc(EnvironmentP env){
    e1->tc(env);
    if(e1->get_type()!=Int)
        env->semant_error(this)<<"Argument of '~' has type "<<e1->get_type()
            <<" instead of Int."<<endl;
    type=Int;
    return Int;
}

Symbol lt_class::tc(EnvironmentP env){
    e1->tc(env);
    e2->tc(env);
    if((e1->get_type()!=Int)||(e2->get_type()!=Int))
        env->semant_error(this)<<"non-Int arguments: "<<e1->get_type()
            <<" < "<<e2->get_type()<<endl;
    type=Bool;
    return Bool;
}

Symbol leq_class::tc(EnvironmentP env){
    e1->tc(env);
    e2->tc(env);

    if((e1->get_type()!=Int)||(e2->get_type()!=Int))
        env->semant_error(this)<<"non-Int arguments: "<<e1->get_type()<<" <= "
            <<e2->get_type()<<endl;
    type=Bool;
    return Bool;
}

Symbol comp_class::tc(EnvironmentP env){
    e1->tc(env);
    if(e1->get_type()!=Bool)
        env->semant_error(this)<<"Argument of 'not' has type "<<
            e1->get_type()<<" instead of Bool."<<endl;
    type = Bool;
    return Bool;
}

Symbol object_class::tc(EnvironmentP env){
    if(env->var_lookup(name))
        type = env->var_lookup(name);
    else {
        env->semant_error(this)<<"Undeclared identifier "<<name<<"."<<endl;
        type = Object;
    }
    return type;
}

Symbol no_expr_class::tc(EnvironmentP)
{
    type=No_type;
    return No_type;
}

Symbol new__class::tc(EnvironmentP env){
    if(env->lookup_class(type_name))
        type=type_name;
    else {
        env->semant_error(this)<<"'new' used with undefined class "<<
        type_name<<"."<<endl;
        type=Object;
    }
    return type;
}

Symbol isvoid_class::tc(EnvironmentP env){
    e1->tc(env);
    type = Bool;
    return Bool;
}

Symbol eq_class::tc(EnvironmentP env){
    Symbol t1=e1->tc(env);
    Symbol t2=e2->tc(env);
    if((t1!=t2)&&((t1==Int)||(t2==Int)||(t1==Bool)||(t2==Bool)||(t1==Str)||(t2==Str)))
        env->semant_error(this)<<"Illegal comparision with a basic type."<<endl;
    type=Bool;
    return Bool;
}

Symbol let_class::tc(EnvironmentP env){
    if(!env->lookup_class(type_decl))
        env->semant_error(this)<<"Class "<<type_decl<<" of let-bound identifier "
            <<identifier<<" is undefined."<<endl;
    if(!env->type_leq(init->tc(env),type_decl))
        env->semant_error(this)<<"Inferred type "<<init->get_type()<<" of initialization of "
            <<identifier<<" does not conform to identifier's declared type "<<type_decl<<"."
                <<endl;
    env->var_enterscope();
    if(identifier == self)
        env->semant_error(this) << "'self' cannot be bound in a 'let' expression."
            <<endl;
    else 
        env->var_add(identifier,type_decl);
    type = body->tc(env);
    env->var_exitscope();   
    return type;
}

Symbol block_class::tc(EnvironmentP env){
    for(int i=body->first();body->more(i);i=body->next(i)){
        type=body->nth(i)->tc(env);
    }
    return type;
}

Symbol assign_class::tc(EnvironmentP env){
    if(name==self)
        env->semant_error(this)<<"Cannot assign to 'self'."<<endl;
    if (!env->var_lookup(name))
        env->semant_error(this)<<"Assignment to undeclared variable "<<name 
            <<"."<<endl;
    type = expr->tc(env);
    if(!env->type_leq(type,env->var_lookup(name))){
        env->semant_error(this)<<"Type "<<type<<" of assigned expression does not conform to declared type "
            <<env->var_lookup(name)<<" of identifier "<<name<<"."<<endl;
    }
    return type;
}

Symbol dispatch_class::tc(EnvironmentP env){
    Symbol expr_type= expr->tc(env);
    if(expr_type == SELF_TYPE)  expr_type=env->get_self_type();
    for(int i=actual->first();actual->more(i);i=actual->next(i))
    {
        actual->nth(i)->tc(env);
    }
    InheritanceNode* nd=env->lookup_class(expr_type);
    if(!nd){
        env->semant_error(this)<<"Dispatch on undefined class "<<expr_type<<
            "."<<endl;
        type=Object;
        return Object;
    }
    method_class *meth=nd->method_lookup(name);

    if(!meth){
        env->semant_error(this)<<"Dispatch to undefined method "<<name<<"."<<endl;
        type=Object;
        return Object;
    }
    if(actual->len()!=meth->num_formals()){
        env->semant_error(this)<<"Method "<<name<<" called with wrong number of arguments."<<endl;
    }
    else
    {
        for(int i=actual->first();actual->more(i);i=actual->next(i)){
            if(!env->type_leq(actual->nth(i)->get_type(),meth->sel_formal(i)->get_type_decl()))
                env->semant_error(this)<<"In call of method "<<name<<",type "<<actual->nth(i)->get_type()
                    <<"of parameter "<<meth->sel_formal(i)->get_name()<<"does not conform to declared type "
                        <<meth->sel_formal(i)->get_type_decl()<<"."<<endl;
        }
    }
    type = (meth->get_return_type()==SELF_TYPE)?expr->get_type():meth->get_return_type();
    return type;
}

Symbol static_dispatch_class::tc(EnvironmentP env){
    Symbol expr_type=expr->tc(env);
    for(int i=actual->first();actual->more(i);i=actual->next(i))
        actual->nth(i)->tc(env);
    if(type_name == SELF_TYPE)
    {
        env ->semant_error(this) <<"Static dispatch to SELF_TYPE." <<endl;
        type=Object;
        return Object;
    }
    InheritanceNode* nd=env->lookup_class(type_name);
    if(!nd){
        env->semant_error(this)<<"Static dispatch to undefined class "<<type_name<<"."<<endl;
        type=Object;
        return Object;
    }
    if(!env->type_leq(expr_type,type_name)){
        env->semant_error(this)<<"Expression type "<<expr_type<<
            " does not conform to declare static dispatch type "<<type_name<<"."<<endl;
        type = Object;
        return Object;
    }

    method_class* meth=nd->method_lookup(name);
    if(!meth){
        env->semant_error(this)<<"Static dispatch to undefined method "<<name<<"."<<endl;
        type=Object;
        return Object;
    }

    if(actual->len()!=meth->num_formals())
        env->semant_error(this)<<"Method "<<name <<" invoked with wrong number of arguments."<<endl;
    else 
        for(int i=actual->first();actual->more(i);i=actual->next(i))
            if(!env->type_leq(actual->nth(i)->get_type(),meth->sel_formal(i)->get_type_decl()))
                env->semant_error(this)<<"In call of method "<<name <<" , type "<<actual->nth(i)->get_type()
                    <<"of parameter "<<meth->sel_formal(i)->get_name()<<
                        " does not conform to declared type "<<meth->sel_formal(i)->get_type_decl()
                            <<"."<<endl;
    type =(meth->get_return_type()==SELF_TYPE)?expr_type : meth->get_return_type();
    return type;
}

Symbol cond_class::tc(EnvironmentP env){
    if(pred->tc(env) != Bool)
        env->semant_error(this)<<"predicate of 'if' does not have type Bool.\n";
    Symbol then_type=then_exp->tc(env);
    Symbol else_type=else_exp->tc(env);
    type=env->type_lub(then_type,else_type);
    return type;
}

Symbol loop_class::tc(EnvironmentP env){
    if(pred->tc(env)!=Bool)
        env->semant_error(this)<<"Loop condition does not have type Bool."<<endl;
    body->tc(env);
    type=Object;
    return Object;
}

Symbol typcase_class::tc(EnvironmentP env)
{
    type = No_type;
    expr->tc(env);

    for (int i=cases->first();cases->more(i);i=cases->next(i))
    {
        Case c=cases->nth(i);
        for(int j=cases->first();j<i;j=cases->next(j))
        {
            if(cases->nth(j)->get_type_decl()==c->get_type_decl()){
                env->semant_error(c)<<"Duplicate branch "<<c->get_type_decl()
                    <<" in case statement."<<endl;
                break;
            }
        }
        env->var_enterscope();
        if(!env->lookup_class(c->get_type_decl()))
            env->semant_error(c)<<"Class "<<c->get_type_decl()<<
                " of case branch is undefined."<<endl;
        if(c->get_name()==self)
            env->semant_error(c)<<"'self' bound in 'case'."<<endl;
        if(c->get_type_decl()==SELF_TYPE)
            env->semant_error(c)<<"Indetifier "<<c->get_name()<<
                " declared with type SELF_TYPE in case branch."<<endl;
        env->var_add(c->get_name(),c->get_type_decl());
        type=env->type_lub(type,c->tc(env));
        env->var_exitscope();
    }
    return type;
}


/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
InheritanceNodeP program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);

    /* some semantic analysis code may go here */

    if (classtable->errors()) {
	cerr << "Compilation halted due to static semantic errors." << endl;
	exit(1);
    }
    return classtable->root();
}
