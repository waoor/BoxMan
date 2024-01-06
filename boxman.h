#pragma once

#include <string>
#include <graphics.h>
#include <conio.h>	//����

#include "database.h"


#define SCREEN_WIDTH 800	//���ڿ�
#define SCREEN_HEIGHT 650	//���ڸ�

#define START_X 100	//��ͼ��ʼ��
#define START_Y 100

//���Ƽ�
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

//ǽ-0���ذ�-1��Ŀ�ĵ�-2����-3������-4����������Ŀ��-5
//ö�٣���0��ʼ
enum _PROPS {
	WALL,
	FLOOR,
	BOX_DES,
	MAN,
	BOX,
	HIT,
	ALL
};
//ALL��ʾ�ܸ���

//�洢��ͬͼ��
IMAGE images[ALL];

//��ά����
struct _POS {
	int x;	//��
	int y;	//��
};


//��Ϸ���Ʒ���
enum _DIRECTION
{
	UP,
	DOWN,
	LEFT,
	RIGHT
};

//����λ��
POS man_pos;

//ʵ����Ϸ�������ҵĿ���
void gameControl(DIRECTION direct);

void changeMap(POS* pos, PROPS prop);

bool isGameOver();

void gameNextScene(IMAGE* bg);
void gameOverScene(IMAGE *bg);

bool login(userinfo& user);

void init_game_graphic(IMAGE& bg_img);