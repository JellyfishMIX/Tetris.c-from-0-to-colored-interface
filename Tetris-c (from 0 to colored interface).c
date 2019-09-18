#include<stdio.h>
#include<windows.h>
#include<time.h>
#include<stdlib.h>

#define FACE_Y 29	//游戏区行数
#define FACE_X 20	//游戏区列数
#define WALL  2
#define KONG 0
#define BOX 1	//已经堆积完毕的方块
#define LEFT  75
#define RIGHT 77
#define DOWN 80
#define SPACE 32
#define ESC 27

struct Face
{
	int data[FACE_Y][FACE_X + 10];	//结构体定义时数组均已被置零
	int color[FACE_Y][FACE_X + 10];
}face;
struct Blocks
{
	int space[4][4];
}blocks[7][4];	//blocks[base][space_z]，一维base表示7个基础方块，二维space_z表示旋转状态

void Read_File();	//读取最高得分
void Write_File();	//写入最高得分
void Inter_Face();	//初始化界面
void Inter_Blocks();	//初始化方块信息
void Start_Game();	//开始游戏
void Draw_Kong(int base, int space_z, int x, int y);	//覆盖前一个blocks的方块，取而代之画出空格
void Draw_Blocks(int base, int space_z, int x, int y);	//画出当前blocks的方块
int Bottom(int base, int space_z, int x, int y);	//WALL与BOX称为底部,判断是否触碰到底部，触碰到底部返回1，未触碰到底部返回0
int Eliminate();	//一行堆积满后清除该行，并记录分值，并询问玩家是否继续
void Hide_Cursor();	//隐藏光标
void color(int c);	//改变输出字符的颜色
void gotoxy(int x,int y);	//坐标跳转

int nn, max=0, grade=0;	//nn,全局变量，用来取blocks[base][space_z]中的base，表示7种基础图形之一
int main()
{
	system("title Tetris(DIY)");    //设置cmd窗口标题
	system("mode con lines=29 cols=60");    //设置cmd窗口高度、宽度
	color(7);    //改变输出字符的颜色
	Hide_Cursor();    //隐藏光标
	Read_File();    //读取最高得分
	Inter_Face();    //初始化界面
	Inter_Blocks();    //初始化方块信息
	srand(time(NULL));	//main函数中设置srand((unsigned)time(NULL));时，影响的将是所有的rand
	nn = rand() % 7;    //nn,全局变量，用来取blocks[base][space_z]中的base，表示7种基础图形之一。这里使nn随机取得0～6中的一个值，即随机取得7种基础图形之一
	while (1)
	{
		Start_Game();
	}
}

void Read_File()    //读取最高得分
{
#pragma warning(disable:4996)    //对 Visual Studio 2019 进行的警告消除，IDE不是Visual Studio可以删去
	FILE *fp;
	fp = fopen("俄罗斯方块最高得分记录.txt", "r+");	//r+ 以可读写方式打开文件，该文件必须存在
	if (fp == NULL)
	{
		fp = fopen("俄罗斯方块最高得分记录.txt", "w+");
		fwrite(&max, sizeof(int), 1, fp);
	}
	fseek(fp, 0, 0);	//函数设置文件指针stream的位置。如果执行成功，stream将指向以fromwhere（偏移起始位置：文件头0(SEEK_SET)，当前位置1(SEEK_CUR)，文件尾2(SEEK_END)）为基准，偏移offset（指针偏移量）个字节的位置。如果执行失败(比如offset超过文件自身大小)，则不改变stream指向的位置。
	fread(&max, sizeof(int), 1, fp);
	fclose(fp);
}

void Write_File()    //写入最高得分
{
#pragma warning(disable:4996)    //对 Visual Studio 2019 进行的警告消除，IDE不是Visual Studio可以删去
	FILE* fp;
	fp = fopen("俄罗斯方块最高得分记录.txt", "w+");
	fwrite(&grade, sizeof(int), 1, fp);
	fclose(fp);
}

void Inter_Face()    //初始化界面
{
	int i, j;
	for (i = 0; i < FACE_Y; i++)
	{
		for (j = 0; j < FACE_X + 10; j++)
		{
			if (j == 0 || j == FACE_X - 1 || j == FACE_X + 9)
			{
				face.data[i][j] = WALL;
				gotoxy(2 * j, i);
				printf("■");
			}
			else if (i == FACE_Y - 1)
			{
				face.data[i][j] = WALL;
				printf("■");
			}
			else
				face.data[i][j] = KONG;
		}
	}
	gotoxy(2 * FACE_X + 4, FACE_Y - 18);
	printf("左移：←");

	gotoxy(2 * FACE_X + 4, FACE_Y - 16);
	printf("右移：→");

	gotoxy(2 * FACE_X + 4, FACE_Y - 14);
	printf("旋转：space");

	gotoxy(2 * FACE_X + 4, FACE_Y - 12);
	printf("暂停: S");

	gotoxy(2 * FACE_X + 4, FACE_Y - 10);
	printf("退出: ESC");

	gotoxy(2 * FACE_X + 4, FACE_Y - 8);
	printf("重新开始:R");

	gotoxy(2 * FACE_X + 4, FACE_Y - 6);
	printf("最高纪录:%d", max);

	gotoxy(2 * FACE_X + 4, FACE_Y - 4);
	printf("分数：%d", grade);
}

void Inter_Blocks()    //初始化方块信息
{
	int i;

	///////////////////////////////////基础七个形状
	//倒置土字形
	for (i = 0; i < 3; i++)
		blocks[0][0].space[1][i] = 1;
	blocks[0][0].space[2][1] = 1;

	//L形-1
	for(i=1;i<4;i++)
		blocks[1][0].space[i][1] = 1;
	blocks[1][0].space[1][2] = 1;

	//L形-2
	for(i=1;i<4;i++)
		blocks[2][0].space[i][2] = 1;
	blocks[2][0].space[1][1] = 1;

	for (i = 0; i < 2; i++)
	{
		//Z形-1
		blocks[3][0].space[1][i] = 1;
		blocks[3][0].space[2][i + 1] = 1;

		//Z形状-2
		blocks[4][0].space[1][i + 1] = 1;
		blocks[4][0].space[2][i] = 1;

		//田字形
		blocks[5][0].space[1][i + 1] = 1;
		blocks[5][0].space[2][i + 1] = 1;
	}

	//1字形
	for (i = 0; i < 4; i++)
	{
		blocks[6][0].space[i][2] = 1;
	}

	///////////////////////////////////7个基础形状的旋转状态space_z，旋转状态共计7*3+7=21+7种
	int base, space_z, j, tem[4][4];
	for (base = 0; base < 7; base++)
	{
		for (space_z = 0; space_z < 3; space_z++)
		{
			for (i = 0; i < 4; i++)
			{
				for (j = 0; j < 4; j++)
				{
					tem[i][j] = blocks[base][space_z].space[i][j];
				}
			}
			for (i = 0; i < 4; i++)
			{
				for (j = 0; j < 4; j++)
				{
					blocks[base][space_z + 1].space[i][j] = tem[4 - j - 1][i];	//控制一边坐标不变，另一边为4 - i - 1，然后再行列互换，这样可以保证四次旋转不同，如果仅仅行列互换，将会导致只有两种旋转状态
				}
			}
		}
	}
}

void Start_Game()    //开始游戏。分为两部分，开头是渲染右上角下一个提示图形，第二部分是while(1)，也就是左侧游戏区图形下落和堆叠
{
	int space_z = 0, n, x = FACE_X / 2 - 2, y = 0, t = 0, i, j, ch;	//x,y初始值即屏幕顶端掉落blocks处
	Draw_Kong(nn, space_z, FACE_X + 3, 4);
	n = nn;	//因为右上角要显示下一块blocks，因此这里先记录下当前blocks的base
	nn = rand() % 7;	//随机生成下一块blocks的base
	color(nn);
	Draw_Blocks(nn, space_z, FACE_X + 3, 4);
	while (1)
	{
		color(n);	//把光标颜色调回当前blocks的base
		Draw_Blocks(n, space_z, x, y);
		if (t == 0)
			t = 15000;
		while (--t)
		{
			if (kbhit() != 0)	//kbhit()是一个C和C++函数，用于非阻塞地响应键盘输入事件。其中文可译为“键盘敲击”(keyboard hit)
				break;
		}
		if (t == 0)
		{
			if (Bottom(n, space_z, x, y+1) != 1)
			{
				Draw_Kong(n, space_z, x, y);
				y++;
			}
			else
			{
				for (i = 0; i < 4; i++)
				{
					for (j = 0; j < 4; j++)
					{
						if (blocks[n][space_z].space[i][j] == 1)
						{
							face.data[y + i][x + j] = BOX;
							face.color[y + i][x + j] = n;
							while (Eliminate());
						}
					}
				}
				return; 
			}
		}
		else
		{
			ch = getch();
			switch (ch)
			{
			case LEFT:
				if (Bottom(n, space_z, x - 1, y) != 1)
				{
					Draw_Kong(n, space_z, x, y);
					x--;
				}
				break;
			case RIGHT:
				if (Bottom(n, space_z, x + 1, y) != 1)
				{
					Draw_Kong(n, space_z, x, y);
					x++;
				}
				break;
			case DOWN:
				if (Bottom(n, space_z, x , y+1) != 1)
				{
					Draw_Kong(n, space_z, x, y);
					y++;
				}
				break;
			case SPACE:
				if (Bottom(n, (space_z + 1) % 4, x, y + 1) != 1)
				{
					Draw_Kong(n, space_z, x, y);
					y++;
					space_z = (space_z + 1) % 4;
				}
				break;
			case ESC:
				system("cls");
				gotoxy(FACE_X, FACE_Y / 2);
				printf("   ---游戏结束---");
				gotoxy(FACE_X, FACE_Y / 2 + 1);
				printf("---请按任意键退出---");
				system("pause>nul");
				exit(0);
			case 'S':
			case 's':
				while (1)
				{
					system("pause>nul");
					break;
				}
				break;
			case 'R':
			case 'r':
				main();
			}
		}
	}
}

void Draw_Kong(int base, int space_z, int x, int y)    //覆盖前一个blocks的方块，取而代之画出空格
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			gotoxy(2 * (x + j), y + i);
			if (blocks[base][space_z].space[i][j] == 1)
				printf("  ");
		}
	}
}

void Draw_Blocks(int base, int space_z, int x, int y)    //画出当前blocks的方块
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			gotoxy(2 * (x + j), y + i);
			if (blocks[base][space_z].space[i][j] == 1)
				printf("■");
		}
	}
}

int Bottom(int base, int space_z, int x, int y)    //WALL与BOX称为底部,判断是否触碰到底部，触碰到底部返回1，未触碰到底部返回0
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (blocks[base][space_z].space[i][j] == 0)
				continue;
			else if (face.data[y + i][x + j] == WALL || face.data[y + i][x + j] == BOX)
				return 1;
		}
	}
	return 0;
}

int Eliminate()    //一行堆积满后清除该行，并记录分值，并询问玩家是否继续
{
#pragma warning(disable:4996)    //对 Visual Studio 2019 进行的警告消除，IDE不是Visual Studio可以删去
	int i, j, sum, m, n;
	for (i = FACE_Y - 2; i > 4; i--)
	{
		sum = 0;
		for (j = 1; j < FACE_X - 1; j++)
		{
			sum += face.data[i][j];
		}
		if (sum == 0)
			break;
		if (sum == FACE_X - 2)
		{
			grade += 100;
			color(7);
			gotoxy(2 * FACE_X + 4, FACE_Y - 4);
			printf("分数：%d", grade);
			for (j = 1; j < FACE_X - 1; j++)
			{
				face.data[i][j] = KONG;
				gotoxy(2 * j, i);
				printf("  ");
			}
			for (m = i; m > 1; m--)
			{
				sum = 0;
				for (n = 1; n < FACE_X - 1; n++)
				{
					sum += face.data[m - 1][n];
					face.data[m][n] = face.data[m - 1][n];
					face.color[m][n] = face.color[m - 1][n];
					if (face.data[m][n] == KONG)
					{
						gotoxy(2 * n, m);
						printf("  ");
					}
					else
					{
						gotoxy(2 * n, m);
						color(face.color[m][n]);
						printf("■");
					}
				}
				if (sum == 0)
					return 1;
			}
		}
	}
	for (j = 1; j < FACE_X - 1; j++)
	{
		if (face.data[1][j] == BOX)
		{
			Sleep(1000);	//延时，给玩家反应时间
			system("cls");
			color(7);
			gotoxy(2 * (FACE_X / 3), FACE_Y / 2 - 2);
			if (grade > max)
			{
				printf("恭喜您打破记录，目前最高记录为：%d", grade);
				Write_File();
			}
			else if (grade == max)
				printf("与记录持平，请突破你的极限！");
			else
				printf("请继续努力，你与最高纪录之差：%d", max - grade);
			gotoxy(2 * (FACE_X / 3), FACE_Y / 2);
			printf("GAME OVER!");
			char ch;
			while (1)
			{
				gotoxy(2 * (FACE_X / 3), FACE_Y / 2 + 2);
				printf("请问是否继续游戏？(y/n)： ");
				scanf("%c", &ch);
				if (ch == 'Y' || ch == 'y')
					main();
				else if (ch == 'N' || ch == 'n')
					exit(0);
				else
				{
					gotoxy(2 * (FACE_X / 3), FACE_Y / 2 + 4);
					printf("输入错误，请重新输入！");
				}
			}
		}
	}
	return 0;
}

void Hide_Cursor()    //隐藏光标
{
	//做一个门把手，打开冰箱门，拿出大象，把大象修改一下，再把大象塞回去
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);	//HANDLE句柄指的是一个核心对象在某一个进程中的唯一索引，而不是指针。GetStdHandle是一个Windows API函数。它用于从一个特定的标准设备（标准输入、标准输出或标准错误）中取得一个句柄（用来标识不同设备的数值）。
	CONSOLE_CURSOR_INFO cci;	//CONSOLE_CURSOR_INFO包含的控制台光标的信息
	GetConsoleCursorInfo(hOut, &cci);	//检索有关指定的控制台屏幕缓冲区的光标的可见性和大小信息。(hConsoleOutput,控制台屏幕缓冲区的句柄。该句柄必须具有的 GENERIC_READ 的访问权限。; lpConsoleCursorInfo, 指向接收有关该控制台的光标的信息的CONSOLE_CURSOR_INFO结构的指针。)
	cci.bVisible = 0;	//赋值0为隐藏，赋值1为可见
	SetConsoleCursorInfo(hOut, &cci);
}

void color(int c)    //改变输出字符的颜色
{
	switch (c)
	{
	case 0: c = 9; break;
	case 1:
	case 2: c = 12; break;
	case 3:
	case 4: c = 14; break;
	case 5: c = 10; break;
	case 6: c = 13; break;
	default: c = 7; break;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);	//SetConsoleTextAttribute是一个API（应用程序编程接口），可以设置控制台窗口字体颜色和背景色的计算机函数
}

void gotoxy(int x,int y)    //坐标跳转
{
	COORD coord;	//COORD是Windows API中定义的一种结构，表示一个字符在控制台屏幕上的坐标。
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);    //SetConsoleCursorPosition是一个API（应用程序编程接口），可以设置设置控制台窗口中光标的位置。
}