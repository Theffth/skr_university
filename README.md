# 二进制学习记录

## 这是什么

此文档主要记录自己在二进制方向的学习，包括基础知识的学习、CTF题目的练习和部分开发基础的积累。

## 学习进度

<details>
    <summary>Week 1-2</summary>
    <p></p>
<p>
    	实现简单的SGI STL，完成了分配器、迭代器、萃取机制以及vector容器
</p>
<p>
    	代码实现：https://github.com/Theffth/skr_university/tree/master/Simple_SGI_STL/T_SGI_STL
</p>
<p>
    	结题测试：https://github.com/Theffth/skr_university/tree/master/Simple_SGI_STL/finial_test
</p>
</details>
<details>
    <summary>Week 3-9</summary>
    <p></p>
	<p>
   		学习编译原理，主要参考Stanford的课程cs143和哈工大的教学视频，完成相应lab。
	</p>
	<p>
    	cool compiler lab代码：https://github.com/Theffth/skr_university/tree/master/compiler
	</p>
	<p>
    	总结笔记：https://theffth.github.io/2020/07/29/Compiler-CS143/
	</p>
</details>

<details>
    <summary>Week 10-11</summary>
    <p></p>
	<p>
    	参考CSAPP书和南京大学视频，完成CSAPP相关课程的lab
	</p>
	<p>
    	目前进度：完成Data Lab,Bomb Lab,Attack Lab,Cache Lab的Part A和Shell Lab
	</p>
	<p>
    	项目实现：https://github.com/Theffth/skr_university/tree/master/csapp%20lab
	</p>
	<p>
    	总结笔记：https://github.com/Theffth/skr_university/blob/master/csapp%20lab/CSAPP%20LAB.md
	</p>
</details>
<details>
    <summary>Week 12-20
    </summary>
    <p></p>
<p>
    这段时间主要根据清华大学操作系统课程，完成uCore实验。
    链接地址为: https://www.xuetangx.com/course/THU08091000267/4231154
</p>
<p>
    目前进度: 完成Lab0-Lab5的理论和实验部分。
</p>
<p>
    总结笔记: https://github.com/Theffth/skr_university/tree/master/uCore
</p>
</details>

> 从这里开始，主要以日报的形式记录自己每日的学习内容。

<details>
    <summary>2020.10.23</summary>

- [x] 配置完成[新的博客](https://theffth.github.io/)

</details>

<details>
    <summary>2020.10.24</summary>


- [x] pwnable.tw : start + orw

- [x] leetcode : 977 1365

  > [writeup](https://theffth.github.io/2020/11/01/leetcode-977%201365/) 

</details>

<details>
    <summary>2020.10.25</summary>


- [x] 体测：立定跳近(跪倒)、仰卧躺倒、坐位体后屈、50米走、800米爬、引体上吊(×)

</details>

<details>
    <summary>2020.10.26</summary>
    <p></p>
    <p>
        计算机安全导论：深度实践 (课程资料，简单二进制攻击实践)
    </p>

- [x] Set-Uid特权程序原理

- [x] 利用环境变量实现攻击
  - 通过动态链接器的攻击: LD_PRELOAD 和 LD_LIBRARY_PATH
  - 利用外部程序进行攻击: PATH 环境变量

- [x] 竞态条件漏洞: 原理及实践 
  - 脏牛攻击: MAP_SHARED & MAP_PRIVATE 和写时拷贝

</details>

<details>
    <summary>2020.10.27 
    </summary>

- [ ] 复现[ByteCTF](https://github.com/hebtuerror404/CTF_competition_warehouse_2020_Second/tree/master/2020_ByteCTF/PWN)

- easyheap
  - 找漏洞点利用 Off By Null 
  - 构造 Double Free

- gun
  - 找漏洞点利用 Use After Free 
  - 构造 Chunk Overlapping
  - 绕过高版本glibc Tcache Check & ORW getshell

- leak & ohmyjson & pwnandroid
  - Go PWN & Android PWN

> 复现笔记：待补

</details>

<details>
    <summary>2020.10.28 : 普林斯顿算法课</summary>

- [ ] [第一周中英对照链接](https://www.bilibili.com/video/BV1Fx411C73u?from=search&seid=17597613833573062644)

</details>

<details>
    <summary>2020.10.29</summary>

- [x] 买新车

</details>

<details>
    <summary>2020.10.30</summary>

- [x] 班级任务（先班评比），赶了两天PPT

</details>

<details>
    <summary>2020.10.31</summary>

- [x] 看了一下 X-NUCA ，太难了，啥都不会
  - ParseC PWN + VM PWN + Cpp PWN + V8 PWN 

</details>

<details>
    <summary>2020.11.01 - 2020.11.06</summary>

- [x] 修电脑 
  - 反复重启 -> 重装系统 -> 等配件 -> 维修 -> 6号才取到机

</details>

<details>
    <summary>2020.11.07</summary>

- [x] 重装软件 & 配置环境
  - 一堆验证 + github 和 blog 环境各种出错，心态要炸
- [x] 完成 4800 + 3000 字的思想汇报和自传，还差手抄
- [x] 其他杂七杂八的ddl

</details>

<details>
    <summary>2020.11.08</summary>

- [x] 学校任务
  - [学习并复现 shellshock 漏洞分析和竞态条件漏洞](https://theffth.github.io/2020/11/08/shellshock-race-condition-%E7%90%86%E8%A7%A3%E4%B8%8E%E5%A4%8D%E7%8E%B0/)
  - 先班评比会
- [ ] 学习搭建codeql环境

</details>