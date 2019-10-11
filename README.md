# Tetris.c-from-0-to-colored-interface

大一c语言课设写的俄罗斯方块，看了好几遍原著的源码，从0开始重写并对原著进行了注释、改进与优化。原著能力很强，感谢原著自写的初版参考，原著链接：[https://blog.csdn.net/lanse_l/article/details/74959248](https://blog.csdn.net/lanse_l/article/details/74959248)

## 阅读指引（重要）

1. 文字说明比较长，建议先略过前半部分文字说明，直接按2.中所说的顺序看源码，看到某个自定义函数需要参考再回来看文字说明。

2. 想要从0开始写的朋友，给一个思路：别的不看，先写最底部的windowsAPI。然后从main()开始入手，遇到新的自定义函数或变量，去翻源码，一点点扣源码。

3.所有用到的自定义函数、全局变量、结构体均已在main()前声明，按第一次被调用的先后顺序排列，并标明了功能注释。

## 一、摘要

目的：做一个俄罗斯方块，具有旋转功能，满一行清除功能，记录得分、最高成绩。

### 方法：

1. 采用了模块化的开发方式，把程序分为好几个功能模块来实现，主函数mian()只用来调用这些功能模块。此外，各个功能模块间也有互相调用的情况。

2. 预设了宏定义DOWN LEFT RRIGHT BOX WALL KONG等，预设给其ASCII码，方便后续调用与辨识。

3. 通过blocks[base][space_z]，（一维base表示7个基础方块，二维space_z表示旋转状态），叠加space[4][4]，得到一个四维数组，用来预设7+7*3=28种图形。

4. 预设了一个结构体face，其中存储着face.data[][]记录BOX WALL KONG，face.color[][]记录堆叠的图形每一块的颜色。

5. 使用了c语言文件操作知识来保存/读取最高得分。

6. 为了使界面更美观，使用自定义函数hidden_cursor()隐藏了光标，使用自定义函数color控制光标输出字符的颜色。

7. 为了渲染界面，使用了自定函数gotoxy()控制光标在cmd窗口中的坐标位置。读取键入信息，实现操作功能，利用了控制键的ASCII码，使用switch()结构选择键入控制键对应的功能模块。

## 二、详细说明

1. 首先对cmd运行窗口做了处理，Hide_Cursor();中调用光标相关API隐藏光标。color();中对光标键入颜色做了预设，共计6种预设颜色。编写了gotoxy(int x,int y);函数，通过COORD这一windows API控制光标在cmd窗口中的坐标位置。

2. 在开发之初先使用Inter_Face();构设了界面（地图边界WALL，操作提示信息），之后通过blocks[base][space_z]，（一维base表示7个基础方块，二维space_z表示旋转状态），叠加space[4][4]，得到一个四维数组，用来预设7+7*3=28种图形。

3. 在main（）;主函数中，完成了初始化界面Inter_Face();和预设28种图形Inter_Blocks();的工作，并使用srand(time(NULL));预设了随机种子，供后续调用。使用Read_File();读取最高得分记录。而游戏部分，单独交给了套在while(1)永循环中的Start_Game();来执行。

4. Start_Game()分为两部分，开头是渲染右上角下一个提示图形，第二部分是while(1)，也就是左侧游戏区图形下落和堆叠

5. Start_Game();中，对于每帧刷新的实现通过如下方法实现：
  + 1. int t=15000;while(--t){ if (kbhit() != 0);break};停顿的时间即—t跑一万五千次+调用一万五千次kbhit()函数的时间（kbhit()是一个C和C++函数，用于非阻塞地响应键盘输入事件）。
  
  + 2. Draw_Kong是把原图形画成空白，Draw_Blocks是在x,y位置画出图形，gotoxy(x,y)会把光标移动到cmd窗口(x,y)坐标位置, 函数Bottom()判断是否到达底部这样一直gotoxy(x,y)，Draw_Blocks，Bottom判断，Draw_Kong，y++
  
  + 3. gotoxy(x,y)，Draw_Blocks，Bottom判断，Draw_Kong，y++
  
  + ……
  
6. 堆叠部分，使用face,data[][]和face,color[][]来保存堆叠的BOX数据。
 
7. 判断是否触碰底部函数Bottom()，也是在测算当前光标坐标（x,y）渲染出的blocks[base][space_z].space[4][4],如果再次y++下移，是否会与face.data[][]中的BOX或WALL重叠，如果重叠，那么把当前光标坐标（x,y）位置渲染出的blocks[base][space_z].space[4][4]中1（即图形部分），存在对应face.data[][]位置，置为BOX。

## 三、开发过程中遇到的问题及解决办法

1. 旋转算法，让7种基础形状变换出剩余的21种

+ 解决办法：blocks[base][space_z + 1].space[i][j] = tem[4 - j - 1][i];    //控制一边坐标不变，另一边为4 - i - 1，然后再行列互换，这样可以保证四次旋转不同，如果仅仅行列互换，将会导致只有两种旋转状态

2. Windows cmd控制台光标信息COORD中，x是横坐标，y是纵坐标，在开发过程中，这两者总是搞混，多数BUG出自于此。

+ 解决办法：这种横纵坐标搞混的错误现象还是很明显的，根据运行情况调试，设置断点，定位BUG位置
  
3. 清除函数Eliminate()约占100行，比较复杂，开发时耗时较多，这里面出现了x，y横纵坐标搞混，只能清除单行、清除后上部堆叠方块无法下移等问题。

+ 解决方法：x，y横纵坐标搞混问题根据调试情况，设置断点，定位BUG位置，肉眼Debug。清除单行改进办法，追加了一次判断上一行是否堆满，如果堆满那么执行下一次Eliminate()。清除后上部堆叠方块无法下移问题，利用face.data[m][n] = face.data[m - 1][n];和face.color[m][n] = face.color[m - 1][n];  ，使清除后的该行，继承上一行的face,data和face,color数据，上一行再继承上上行，以此类推。




  
  
  
