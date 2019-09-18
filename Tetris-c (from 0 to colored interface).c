#include<stdio.h>
#include<windows.h>
#include<time.h>
#include<stdlib.h>

#define FACE_Y 29	//��Ϸ������
#define FACE_X 20	//��Ϸ������
#define WALL  2
#define KONG 0
#define BOX 1	//�Ѿ��ѻ���ϵķ���
#define LEFT  75
#define RIGHT 77
#define DOWN 80
#define SPACE 32
#define ESC 27

struct Face
{
	int data[FACE_Y][FACE_X + 10];	//�ṹ�嶨��ʱ������ѱ�����
	int color[FACE_Y][FACE_X + 10];
}face;
struct Blocks
{
	int space[4][4];
}blocks[7][4];	//blocks[base][space_z]��һάbase��ʾ7���������飬��άspace_z��ʾ��ת״̬

void Read_File();	//��ȡ��ߵ÷�
void Write_File();	//д����ߵ÷�
void Inter_Face();	//��ʼ������
void Inter_Blocks();	//��ʼ��������Ϣ
void Start_Game();	//��ʼ��Ϸ
void Draw_Kong(int base, int space_z, int x, int y);	//����ǰһ��blocks�ķ��飬ȡ����֮�����ո�
void Draw_Blocks(int base, int space_z, int x, int y);	//������ǰblocks�ķ���
int Bottom(int base, int space_z, int x, int y);	//WALL��BOX��Ϊ�ײ�,�ж��Ƿ������ײ����������ײ�����1��δ�������ײ�����0
int Eliminate();	//һ�жѻ�����������У�����¼��ֵ����ѯ������Ƿ����
void Hide_Cursor();	//���ع��
void color(int c);	//�ı�����ַ�����ɫ
void gotoxy(int x,int y);	//������ת

int nn, max=0, grade=0;	//nn,ȫ�ֱ���������ȡblocks[base][space_z]�е�base����ʾ7�ֻ���ͼ��֮һ
int main()
{
	system("title Tetris(DIY)");    //����cmd���ڱ���
	system("mode con lines=29 cols=60");    //����cmd���ڸ߶ȡ����
	color(7);    //�ı�����ַ�����ɫ
	Hide_Cursor();    //���ع��
	Read_File();    //��ȡ��ߵ÷�
	Inter_Face();    //��ʼ������
	Inter_Blocks();    //��ʼ��������Ϣ
	srand(time(NULL));	//main����������srand((unsigned)time(NULL));ʱ��Ӱ��Ľ������е�rand
	nn = rand() % 7;    //nn,ȫ�ֱ���������ȡblocks[base][space_z]�е�base����ʾ7�ֻ���ͼ��֮һ������ʹnn���ȡ��0��6�е�һ��ֵ�������ȡ��7�ֻ���ͼ��֮һ
	while (1)
	{
		Start_Game();
	}
}

void Read_File()    //��ȡ��ߵ÷�
{
#pragma warning(disable:4996)    //�� Visual Studio 2019 ���еľ���������IDE����Visual Studio����ɾȥ
	FILE *fp;
	fp = fopen("����˹������ߵ÷ּ�¼.txt", "r+");	//r+ �Կɶ�д��ʽ���ļ������ļ��������
	if (fp == NULL)
	{
		fp = fopen("����˹������ߵ÷ּ�¼.txt", "w+");
		fwrite(&max, sizeof(int), 1, fp);
	}
	fseek(fp, 0, 0);	//���������ļ�ָ��stream��λ�á����ִ�гɹ���stream��ָ����fromwhere��ƫ����ʼλ�ã��ļ�ͷ0(SEEK_SET)����ǰλ��1(SEEK_CUR)���ļ�β2(SEEK_END)��Ϊ��׼��ƫ��offset��ָ��ƫ���������ֽڵ�λ�á����ִ��ʧ��(����offset�����ļ������С)���򲻸ı�streamָ���λ�á�
	fread(&max, sizeof(int), 1, fp);
	fclose(fp);
}

void Write_File()    //д����ߵ÷�
{
#pragma warning(disable:4996)    //�� Visual Studio 2019 ���еľ���������IDE����Visual Studio����ɾȥ
	FILE* fp;
	fp = fopen("����˹������ߵ÷ּ�¼.txt", "w+");
	fwrite(&grade, sizeof(int), 1, fp);
	fclose(fp);
}

void Inter_Face()    //��ʼ������
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
				printf("��");
			}
			else if (i == FACE_Y - 1)
			{
				face.data[i][j] = WALL;
				printf("��");
			}
			else
				face.data[i][j] = KONG;
		}
	}
	gotoxy(2 * FACE_X + 4, FACE_Y - 18);
	printf("���ƣ���");

	gotoxy(2 * FACE_X + 4, FACE_Y - 16);
	printf("���ƣ���");

	gotoxy(2 * FACE_X + 4, FACE_Y - 14);
	printf("��ת��space");

	gotoxy(2 * FACE_X + 4, FACE_Y - 12);
	printf("��ͣ: S");

	gotoxy(2 * FACE_X + 4, FACE_Y - 10);
	printf("�˳�: ESC");

	gotoxy(2 * FACE_X + 4, FACE_Y - 8);
	printf("���¿�ʼ:R");

	gotoxy(2 * FACE_X + 4, FACE_Y - 6);
	printf("��߼�¼:%d", max);

	gotoxy(2 * FACE_X + 4, FACE_Y - 4);
	printf("������%d", grade);
}

void Inter_Blocks()    //��ʼ��������Ϣ
{
	int i;

	///////////////////////////////////�����߸���״
	//����������
	for (i = 0; i < 3; i++)
		blocks[0][0].space[1][i] = 1;
	blocks[0][0].space[2][1] = 1;

	//L��-1
	for(i=1;i<4;i++)
		blocks[1][0].space[i][1] = 1;
	blocks[1][0].space[1][2] = 1;

	//L��-2
	for(i=1;i<4;i++)
		blocks[2][0].space[i][2] = 1;
	blocks[2][0].space[1][1] = 1;

	for (i = 0; i < 2; i++)
	{
		//Z��-1
		blocks[3][0].space[1][i] = 1;
		blocks[3][0].space[2][i + 1] = 1;

		//Z��״-2
		blocks[4][0].space[1][i + 1] = 1;
		blocks[4][0].space[2][i] = 1;

		//������
		blocks[5][0].space[1][i + 1] = 1;
		blocks[5][0].space[2][i + 1] = 1;
	}

	//1����
	for (i = 0; i < 4; i++)
	{
		blocks[6][0].space[i][2] = 1;
	}

	///////////////////////////////////7��������״����ת״̬space_z����ת״̬����7*3+7=21+7��
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
					blocks[base][space_z + 1].space[i][j] = tem[4 - j - 1][i];	//����һ�����겻�䣬��һ��Ϊ4 - i - 1��Ȼ�������л������������Ա�֤�Ĵ���ת��ͬ������������л��������ᵼ��ֻ��������ת״̬
				}
			}
		}
	}
}

void Start_Game()    //��ʼ��Ϸ����Ϊ�����֣���ͷ����Ⱦ���Ͻ���һ����ʾͼ�Σ��ڶ�������while(1)��Ҳ���������Ϸ��ͼ������Ͷѵ�
{
	int space_z = 0, n, x = FACE_X / 2 - 2, y = 0, t = 0, i, j, ch;	//x,y��ʼֵ����Ļ���˵���blocks��
	Draw_Kong(nn, space_z, FACE_X + 3, 4);
	n = nn;	//��Ϊ���Ͻ�Ҫ��ʾ��һ��blocks����������ȼ�¼�µ�ǰblocks��base
	nn = rand() % 7;	//���������һ��blocks��base
	color(nn);
	Draw_Blocks(nn, space_z, FACE_X + 3, 4);
	while (1)
	{
		color(n);	//�ѹ����ɫ���ص�ǰblocks��base
		Draw_Blocks(n, space_z, x, y);
		if (t == 0)
			t = 15000;
		while (--t)
		{
			if (kbhit() != 0)	//kbhit()��һ��C��C++���������ڷ���������Ӧ���������¼��������Ŀ���Ϊ�������û���(keyboard hit)
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
				printf("   ---��Ϸ����---");
				gotoxy(FACE_X, FACE_Y / 2 + 1);
				printf("---�밴������˳�---");
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

void Draw_Kong(int base, int space_z, int x, int y)    //����ǰһ��blocks�ķ��飬ȡ����֮�����ո�
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

void Draw_Blocks(int base, int space_z, int x, int y)    //������ǰblocks�ķ���
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			gotoxy(2 * (x + j), y + i);
			if (blocks[base][space_z].space[i][j] == 1)
				printf("��");
		}
	}
}

int Bottom(int base, int space_z, int x, int y)    //WALL��BOX��Ϊ�ײ�,�ж��Ƿ������ײ����������ײ�����1��δ�������ײ�����0
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

int Eliminate()    //һ�жѻ�����������У�����¼��ֵ����ѯ������Ƿ����
{
#pragma warning(disable:4996)    //�� Visual Studio 2019 ���еľ���������IDE����Visual Studio����ɾȥ
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
			printf("������%d", grade);
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
						printf("��");
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
			Sleep(1000);	//��ʱ������ҷ�Ӧʱ��
			system("cls");
			color(7);
			gotoxy(2 * (FACE_X / 3), FACE_Y / 2 - 2);
			if (grade > max)
			{
				printf("��ϲ�����Ƽ�¼��Ŀǰ��߼�¼Ϊ��%d", grade);
				Write_File();
			}
			else if (grade == max)
				printf("���¼��ƽ����ͻ����ļ��ޣ�");
			else
				printf("�����Ŭ����������߼�¼֮�%d", max - grade);
			gotoxy(2 * (FACE_X / 3), FACE_Y / 2);
			printf("GAME OVER!");
			char ch;
			while (1)
			{
				gotoxy(2 * (FACE_X / 3), FACE_Y / 2 + 2);
				printf("�����Ƿ������Ϸ��(y/n)�� ");
				scanf("%c", &ch);
				if (ch == 'Y' || ch == 'y')
					main();
				else if (ch == 'N' || ch == 'n')
					exit(0);
				else
				{
					gotoxy(2 * (FACE_X / 3), FACE_Y / 2 + 4);
					printf("����������������룡");
				}
			}
		}
	}
	return 0;
}

void Hide_Cursor()    //���ع��
{
	//��һ���Ű��֣��򿪱����ţ��ó����󣬰Ѵ����޸�һ�£��ٰѴ�������ȥ
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);	//HANDLE���ָ����һ�����Ķ�����ĳһ�������е�Ψһ������������ָ�롣GetStdHandle��һ��Windows API�����������ڴ�һ���ض��ı�׼�豸����׼���롢��׼������׼������ȡ��һ�������������ʶ��ͬ�豸����ֵ����
	CONSOLE_CURSOR_INFO cci;	//CONSOLE_CURSOR_INFO�����Ŀ���̨������Ϣ
	GetConsoleCursorInfo(hOut, &cci);	//�����й�ָ���Ŀ���̨��Ļ�������Ĺ��Ŀɼ��Ժʹ�С��Ϣ��(hConsoleOutput,����̨��Ļ�������ľ�����þ��������е� GENERIC_READ �ķ���Ȩ�ޡ�; lpConsoleCursorInfo, ָ������йظÿ���̨�Ĺ�����Ϣ��CONSOLE_CURSOR_INFO�ṹ��ָ�롣)
	cci.bVisible = 0;	//��ֵ0Ϊ���أ���ֵ1Ϊ�ɼ�
	SetConsoleCursorInfo(hOut, &cci);
}

void color(int c)    //�ı�����ַ�����ɫ
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
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);	//SetConsoleTextAttribute��һ��API��Ӧ�ó����̽ӿڣ����������ÿ���̨����������ɫ�ͱ���ɫ�ļ��������
}

void gotoxy(int x,int y)    //������ת
{
	COORD coord;	//COORD��Windows API�ж����һ�ֽṹ����ʾһ���ַ��ڿ���̨��Ļ�ϵ����ꡣ
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);    //SetConsoleCursorPosition��һ��API��Ӧ�ó����̽ӿڣ��������������ÿ���̨�����й���λ�á�
}