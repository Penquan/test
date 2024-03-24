#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <easyx.h>
#include <windows.h>
#include <graphics.h>		// 引用图形库头文件
#include<mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define ROW 10 // 定义规格
#define COL 10
#define MINENUM 10 //雷的数量
#define IMGSIZE 40
//定义图片资源
IMAGE imgs[12];
static bool isfirst = true;
void loadResource()
{
    for (int i = 0; i < 12; i++)
    {
        char imgPath[50] = { 0 };
        sprintf(imgPath, "./images/%d.jpg", i);
        loadimage(&imgs[i], imgPath, IMGSIZE, IMGSIZE);
    //    putimage(i*IMGSIZE,0,imgs+i);
    }
}

void init(int map[][COL]);
//初始化数据
void showMap(int map[][COL]);
//绘制
void draw(int map[][COL]);
//鼠标操作数据
void mouseMsg(ExMessage* msg, int map[][COL]);
//点击空白格子，连环爆开周围的所有空白格子还有数字，row col是当前点击的格子
void boomBlank(int map[][COL], int row, int col);
//游戏结束条件 输了返回-1 没结束返回0 赢了返回1
int judge(int map[][COL],int row, int col);

int main(void)
{
    //创建窗口
    initgraph(IMGSIZE* ROW, IMGSIZE * COL, EW_SHOWCONSOLE);
   
    setbkcolor(WHITE);
    cleardevice();
    int map[ROW][COL] = { 0 };

    init(map);
    //游戏主循环
    
    int flag = 0;

    while (1)
    {
        //处理消息
        ExMessage msg;
        while (peekmessage(&msg,EM_MOUSE))
        {
            switch (msg.message)
            {
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
                //接收鼠标消息
                mouseMsg(&msg, map);
                int ret = judge(map, msg.y / IMGSIZE, msg.x / IMGSIZE);//点击之后判断
                if (ret == -1)
                {
                    if (flag == 0) {
                        draw(map);
                        flag = 1;
                    }
                    PlaySound("./images/explode.wav", NULL, SND_ASYNC | SND_FILENAME);
                    int select = MessageBox(GetHWnd(), "抱歉，你输了,是否选择再来一把？", "继续努力！", MB_OKCANCEL);
                    if (select == IDOK)
                    {
                        flag = 0;
                        // 重新初始化
                        init(map);
                    }
                    else // 退出
                    {
                        exit(0);
                    }
                }
                else if (ret == 1)
                {
                    if (flag == 0) {
                        draw(map);
                        flag = 1;
                    }
                    PlaySound("./images/win.wav", NULL, SND_ASYNC | SND_FILENAME);

                    int select = MessageBox(GetHWnd(), "恭喜，你赢了，是否选择再来一把？", "太强了！", MB_OKCANCEL);
                    if (select == IDOK)
                    {
                        flag = 0;
                        // 重新初始化
                        init(map);
                    }
                    else // 退出
                    {
                        exit(0);
                    }
                }
                system("cls");
                printf("judge:%d\n",ret);
                showMap(map);
                break;
            }
        }
        if (flag == 0) {
            draw(map);
        }

    }
    
   // showMap(map);
   
    system("pause");

    return 0;

}

int judge(int map[][COL],int row,int col)
{
    //点到了雷，结束，输了
    if (map[row][col] == -1 || map[ROW][COL] == 19)
    {
        return -1;
    }

    //点完了格子，结束，赢了，点开了90个格子
    int cnt = 0;
    for (int i = 0;  i < ROW; i++)
    {
        for (int k = 0; k < COL; k++)
        {
            //统计打开的格子的数量
            if (map[i][k] >= 0 && map[i][k] <= 8)
            {
                ++cnt;
            }
        }
    }
    if (ROW * COL - MINENUM == cnt)
    {
        return 1;
    }
}

void boomBlank(int map[][COL], int row, int col)
{
    
    //判断row col位置是不是空白格子
    if (map[row][col] == 0)
    {
        for (int r = row - 1; r <= row + 1; r++) 
        {
            for (int c = col - 1; c <= col + 1; c++) 
            {
                if((r >= 0 && r < ROW && c >= 0 && c < COL) && map[r][c] >= 19 && map[r][c] <= 28) // 没越界
                {
                    //放的是search.wav，记得改
                    if (isfirst) {
                        PlaySound("./images/search.wav", NULL, SND_ASYNC | SND_FILENAME);
                        isfirst = false;
                    }
                    
                    map[r][c] -= 20;
                    boomBlank(map, r, c);
                }
            }
        }
    }
    return;
}

void mouseMsg(ExMessage *msg,int map[][COL])
{
    //鼠标：原点在左上角 先根据戍边点击坐标求出对应的数组下标
    int r = msg->y / IMGSIZE;
    int c = msg->x / IMGSIZE;
    //printf("%d %d\n", c, r);
    //左键打开，右键标记
    if (msg->message == WM_LBUTTONDOWN)
    {
        //已经点过了，不能一致重复减下去
        if (map[r][c] >= 19 && map[r][c] <= 28)
        {
            //此函数只能播放wav
            PlaySound("./images/click.wav",NULL,SND_ASYNC | SND_FILENAME);
            map[r][c] -= 20;
            boomBlank(map, r, c);    //检测一下是不是空白格子，是，炸开
            isfirst = true;
            system("cls");
            showMap(map); showMap(map);
        }
    }
    else if (msg->message == WM_RBUTTONDOWN)
    {
        PlaySound("./images/rightClick.wav", NULL, SND_ASYNC | SND_FILENAME);
        //是否能标记：没打开过的就能标记
        if (map[r][c] >= 19 && map[r][c] <= 28)
        {
            map[r][c] += 20;
            system("cls");
            showMap(map);
        }
        else if (map[r][c] >= 39)    //取消标记
        {
            map[r][c] -= 39;
        }
    }
    
}

void showMap(int map[][COL])
{
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            printf("%2d ", map[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void init(int map[][COL])
{
    PlaySound("./images/start.wav", NULL, SND_ASYNC | SND_FILENAME);
    loadResource();
    // 初始化数据
    // 随机设置10个雷

    srand((unsigned)time(NULL));

    //把map全部初始化为0
    memset(map,0,sizeof(int) * ROW * COL);
    for (int i = 0; i < MINENUM;)
    {
        int row = rand() % ROW;
        int col = rand() % COL;
        // 只有执行了if里面的内容，才成功设置了雷 -- 避免了设置了同一个雷
        if (map[row][col] == 0)
        {
            map[row][col] = -1;
            i++;
        }
    }

    // 以雷为中心，设置个九宫格,除去雷其他全部自加1
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            // 找到雷，并遍历雷所在的九宫格 -- 优化:分配的时候把雷的位置存了
            if (map[i][j] == -1)
            {
                // tips:i j 如果贴边，会越界
                for (int k = i - 1; k <= i + 1; k++)
                {
                    for (int l = j - 1; l <= j + 1; l++)
                    {
                        // 自增：把雷旁边的数计算出来 -- 数的九宫格有两个雷，num = 2
                        if (k >= 0 && l >= 0 && k <= ROW && l <= COL && map[k][l] != -1)
                            map[k][l]++;
                    }
                }
            }
        }
        
    }

    //加密格子 -- 把格子里的东西盖起来
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            map[i][j] += 20;
        }
    }
}

//绘制
void draw(int map[][COL])
{
    //贴图 -- 根据map的数据贴图片
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            if (map[i][j] >= 0 && map[i][j] <= 8) //数据范围：0-8 --九宫格
                putimage(j * IMGSIZE, i * IMGSIZE, &imgs[map[i][j]]);
            else if (map[i][j] == -1)    //贴雷的图
                putimage(j * IMGSIZE, i * IMGSIZE, &imgs[9]);
            else if (map[i][j] >= (-1 + 20) && map[i][j] <= (8 + 20))  //贴空格
                putimage(j * IMGSIZE, i * IMGSIZE, &imgs[10]);
            else if (map[i][j] >= 39)  //标记之后：-1+20+20 = 39  盖旗子
                putimage(j * IMGSIZE, i * IMGSIZE, &imgs[11]);
        }
    }
  
}