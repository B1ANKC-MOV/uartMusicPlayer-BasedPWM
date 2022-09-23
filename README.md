# uart-MusicPlayer-Based-on-PWM-
嵌入式系统与开发的课设|基于PWM的串口控制音乐播放器
# 项目内容
利用S5PV210的中断、串口以及定时器实现以下功能：   
（1）由串口接收PC端的字母“play”，在PC端显示“Playing music”，利用PWM定时器播放一段不少于10s的音乐  
（2）由开发板按下SW4键，利用串口在PC端显示 “Marquee”，同时，4颗流水灯自左向右一次，每颗LED亮灭时间各2s，最后全亮  
# 程序总体设计图
![image](https://user-images.githubusercontent.com/66285048/191890127-55533f01-251f-4ce4-99f8-50a69cd8c170.png)
# 使用步骤
## 1．编写键控制代码，将代码编译为二进制文件
 1）在 ubuntu 系统中，进入共享文件夹 forlinux，新建fsj文件夹;  
 2）进入fsj文件夹，新建启动文件 start.S，并添加启动代码;  
 3）新建按键控制代码文件main.c，自行编写 c 语言代码;  
 4）新建 Makefile 文件， 添加编译命令（参考附录）， 由以下命令编译生成二进制文件uart.bin。  
```
$ make clean
$ make
```
## 2．安装 USB 驱动
1.将实验箱中的拨码开关 2 拨到 on， 长按 Power 键直至电脑提示安装驱动。  
2.打开计算机设备管理器，选择下图所示硬件安装驱动。  
3.右键选择更新驱动程序，手动添加 USB 驱动程序路径“D:\新 509\04-常用工具\DNW”。  
4.在驱动安装过程中，关闭开发板的电源，将拨码开关 2重新置为 OFF 状态。
## 3. 使用 DNW 软件下载裸机程序至 SRAM 中运行  
（1）用 USB device 线连接电脑和开发板，设置开发板为 nandflash 启动(拨码开关全部拨至 OFF 状态);  
（2）在目录“D:\新 509\04-常用工具\DNW”中打开 DNW.exe。 设置串口： 波特率为 115200，USB Port 为 Download， Address 为 0xd0020010;  
（3）在菜单栏开启 DNW 串口连接（Serial Port ->Connect）。启动开发板后立即在 DNW 窗口迅速敲击空格键进入 Uboot 状态，可见图 11 启动界面;  
（4）在 DNW 窗口中输入“dnw 0xd0020010” 设置下载地址。如果 DNW 驱动安装失败或首次使用 DNW，会提示安装驱动，请正确安装驱动，等到提示硬件可使用从进行下一步;  
（5）在 DNW 菜单中，选择 usbport->Transmit->Transmit 发送生成的 uart.bin 文件， DNW自动下载 uart.bin 文件至开发板;  
（6）在 DNW 窗口中输入“go 0xd0020010”，即可开始运行 key.bin 程序。注意，将二进制文件下载到 SRAM 中不会破坏开发板中现有文件与程序，但是掉电后所下载的文件将丢失。  
