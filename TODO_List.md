* 减少防御性编程，不涉及重要部分的函数不做过多的参数检查，减少代码量，增加可读性
* 代码简洁
1. 参考E:\rm\Code\27_3SE_hero\hero_down\PrivateApplications\Music，在bsp_pwm加入更改pwm频率的函数并且在Driver层的Buzzer加入十二平均律，根据音高计算分频数，定义v2版本MusicNote结构体
2. 参考E:\rm\Code\27_3SE_hero实现板载bmi088的姿态角结算，按照bsp->Driver->module架构,使用madgwick算法，那个名字错了你改一下，在Module层不实现任务的定义,module层只提供接口，任务的创建在Task层完成，你可以封装一个叫INS_Task的函数
3. 也是参考E:\rm\Code\27_3SE_hero实现Referee的移植
4. UART的BSP也封装一下
5. 尝试BSP层的CAN定义can实例结构体，包含can句柄，can监视器，can队列句柄,但是不用具体的can、queue实例，我要放在上层定义,大体函数结构不要做改动，就按照原来的来