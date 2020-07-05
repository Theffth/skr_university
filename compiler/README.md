## Compiler

根据cs143学习，cool文件夹是该课程的lab作业。

### PA2

已通过打分测试，待总结

### PA3

已通过打分测试，待总结

### PA4

待继续完善

PA4语义分析主要需要解决的问题：  

1. 对象（函数/变量）的作用域  -> SymbolTable  
2. 类的正确继承关系    ->ClassTable
3. 类型检查:确保操作是和正确的类型一起使用  ->Environment

主要借助三个相互引用的结构体实现：Environment、ClassTable和InheritanceNode

Environment类把需要检查的信息集中在一起，其中符号表就是一个双重链表：SymbolTable->主要解决了作用域问题

主要集中了：

- method符号表
- attr/var符号表
- class的符号表
- 表达式出现的类

InheritanceNode类： 是类结点作为继承图中结点的性质补充

ClassTable： InheritanceNode的符号表，实现继承图

(1)安装基础类
(2)安装用户定义类
(3)简单检查继承关系
(4)建立父子节点关系
(5)检查继承关系是否有环   
(6)建立feature表（同时进行attr和method类型检查）   
(7)检查main函数

### PA5

待完成
