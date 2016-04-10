myshell
======

这是一个超级简单的shell，写它的目的是为了了解一个shell工作的大概流程。

1. 	**lexer.c**:进行词法分析。
2. 	**parser.c**:进行语法分析。这个过程中将创建一个AST(抽象语法树)。
3. 	**execute.c**:遍历AST执行相关命令。
4. 	**lib.c**:一些工具函数。
5. 	**shell.c**:shell的主函数。

构建环境: Ubuntu 14.04 + gcc 4.8

接触Linux以来一直都在终端下使用着shell，但却对shell背后的一些工作方式
却并不是很了解。
直到一次无意中看到[这篇博文](http://blog.chinaunix.net/uid-20106293-id-142129.html)。
在解决了很多疑惑之后，在这篇博文的基础上模仿着写了这个简单的shell。

