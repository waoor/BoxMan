#pragma once

#include <string>
#include <graphics.h>
#include <conio.h>	//键盘

#include "database.h"


#define SCREEN_WIDTH 800	//窗口宽
#define SCREEN_HEIGHT 650	//窗口高

#define START_X 100	//地图初始点
#define START_Y 100

//控制键
#define KEY_UP 'w'
#define KEY_LEFT 'a'
#define KEY_RIGHT 'd'
#define KEY_DOWN 's'
#define KEY_QUIT 'q'

typedef enum _DIRECTION DIRECTION;
typedef struct _POS POS;
typedef enum _PROPS PROPS;

#define isValid(pos) (pos.x >= 0 && pos.x <= ROW && pos.y >= 0 && pos.y <= COLUMN)

//0,0,0,0,0,0|0,4,0,1,2,0|0,1,0,4,1,0|0,2,3,0,1,0|0,0,0,0,0,0
//0,0,0,0,0|0,1,1,1,0|0,1,1,1,0|0,1,1,1,0|0,0,0,0,0
int map[ROW][COLUMN];

//墙-0；地板-1；目的地-2；人-3；箱子-4；箱子命中目标-5
//枚举，从0开始
enum _PROPS {
	WALL,
	FLOOR,
	BOX_DES,
	MAN,
	BOX,
	HIT,
	ALL
};
//ALL表示总个数

//存储不同图像
IMAGE images[ALL];

//二维坐标
struct _POS {
	int x;	//行
	int y;	//列
};


//游戏控制方向
enum _DIRECTION
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

//人物位置
POS man_pos;

//实现游戏上下左右的控制
void gameControl(DIRECTION direct);

void changeMap(POS* pos, PROPS prop);

bool isGameOver();

void gameNextScene(IMAGE* bg);
void gameOverScene(IMAGE *bg);

bool login(userinfo& user);

void init_game_graphic(IMAGE& bg_img);