# ICS2019 Programming Assignment

This project is the programming assignment of the class ICS(Introduction to Computer System) in Department of Computer Science and Technology, Nanjing University.

For the guide of this programming assignment,
refer to http://nju-ics.gitbooks.io/ics2019-programming-assignment/content/

To initialize, run
```bash
bash init.sh
```

The following subprojects/components are included. Some of them are not fully implemented.
* [NEMU](https://github.com/NJU-ProjectN/nemu)
* [Nexus-am](https://github.com/NJU-ProjectN/nexus-am)
* [Nanos-lite](https://github.com/NJU-ProjectN/nanos-lite)
* [Navy-apps](https://github.com/NJU-ProjectN/navy-apps)

# memo(一周目)
1. ISA=x86
2. pa3 和 pa4 的选做题都没有做, pa5 没做.
3. pa4 中最难的选做题: *支持开机菜单程序的运行* 解题思路应该是的 `sys_execve()`, 需要创建合适的pcb和上下文并进行调度和终止.
4. 部分回答题依然思考不出答案
5. pa4 分支的管理有问题, 当时创建并切换后又切回到 master 分支, 所以 pa4 的实际提交都是从 master 分支合并过去的.
6. 总用时约三周, 15天左右, 平均一天6小时, 基本和讲义所写的平均时间一致(但讲义所写的平均时间是算上 homework 的, 所以应该比平均时间要长).
    + 难度其实是逐步叠加, 但不意味着新增代码量也越来越多, pa2 的代码量最多, pa3 和 pa4 重在理解, 理解了剩下的 coding 工作相对简单.
    + 一方面是的对代码框架的理解, 一方面是的对讲义的理解
7. 几个难理解的点:
    + DiffTest 中和 qemu 的通信和同步问题, 这也是 pa3 和 pa4 选做题的难度之一, pa3 后几乎没用过DiffTest调试了
    + AM 层的理解, 这个属于框架的问题, 即几个项目之间的关系, 不理解的话 pa4 的部分选做题几乎做不了
    + irq 的处理过程还需要多加思考, 只理解了基本的
    + 进程调度中的上下文切换, 这其实和 irq 的处理有关的