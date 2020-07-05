#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>  
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"

#define TRUE 1
#define FALSE 0

enum Inheritable {CanInherit, CantInherit};
enum Basicness {Basic,NotBasic};
enum Reachability {Reachable,UnReachable};

class Environment;
typedef Environment* EnvironmentP;
class InheritanceNode;
typedef InheritanceNode* InheritanceNodeP;
class ClassTable;
typedef ClassTable *ClassTableP;

class Environment {
  private:
    SymbolTable<Symbol,method_class> method_table;
    SymbolTable<Symbol,Entry> var_table;
    ClassTableP class_table;
    Class_ self_class;

  public:
  Environment(ClassTableP ct,InheritanceNodeP sc);
  Environment(SymbolTable<Symbol,method_class> mt,
              SymbolTable<Symbol,Entry> vt,
              ClassTableP ct,
              InheritanceNodeP sc);
  EnvironmentP clone_Environment(InheritanceNodeP n);

  ostream& semant_error();
  ostream& semant_error(tree_node* t);

  InheritanceNodeP lookup_class(Symbol s);

  void method_add(Symbol s,method_class* m);
  method_class* method_lookup(Symbol s);
  method_class* method_probe(Symbol s);
  void method_enterscope();
  void method_exitscope();

  void var_add(Symbol s, Symbol typ);
  Symbol var_lookup(Symbol s);
  Symbol var_probe(Symbol s);
  void var_enterscope();
  void var_exitscope();

  Symbol get_self_type();
  int type_leq(Symbol subtype,Symbol supertype);
  Symbol type_lub(Symbol t1,Symbol t2);
};

class InheritanceNode : public class__class{
  private:
    InheritanceNodeP parentnd;
    List<InheritanceNode>* children;
    Inheritable inherit_status;
    Basicness basic_status;
    Reachability reach_status;
    EnvironmentP env;
  
  public:
    InheritanceNode(Class_ c,Inheritable istatus, Basicness bstatus);
    int basic() {return (basic_status==Basic);}
    int inherit()  {return (inherit_status==CanInherit);}
    void mark_reachable();
    int reachable() {return (reach_status==Reachable);}
    void add_child(InheritanceNodeP child);
    List<InheritanceNode>* get_children() {return children;}
    void set_parentnd(InheritanceNodeP p);
    InheritanceNodeP get_parentnd();

    void copy_env(EnvironmentP e) {env=e->clone_Environment(this);}
    void build_feature_tables();

    void init_env(ClassTableP ct);
    void type_check_features();
    void check_main_method();
    method_class* method_lookup(Symbol s) {return env->method_lookup(s);}
};


// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.
class ClassTable : public SymbolTable<Symbol , InheritanceNode> {
private:
  List<InheritanceNode>* nds;
  int semant_errors;
  void install_basic_classes();
  void install_class(InheritanceNodeP nd);
  void install_classes(Classes cs);
  void check_improper_inheritance();
  void build_inheritance_tree();
  void set_relations(InheritanceNodeP nd);
  void check_for_cycles();
  void build_feature_tables();
  void check_main();
  ostream& error_stream;

public:
  ClassTable(Classes);
  int errors() { return semant_errors; }
  InheritanceNodeP root();
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node* t);
};


#endif

