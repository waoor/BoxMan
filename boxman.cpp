#include "boxman.h"

int main()
{
	//用户身份验证
	userinfo user;
	levelinfo level;

	IMAGE bg_img;

	if (!login(user))
	{
		cout << "登录失败" << endl;
		system("pause");
		exit(-1);
	}
	else
	{
		cout << "登录成功,用户id：" << user.id << ",您所在的关卡是：" << user.level_id << endl;
		::system("pause");
	}

	//初始化窗口
	init_game_graphic(bg_img);

	//游戏环节
	bool quit = false;

	do {


		//根据level_id获取关卡数据
		if (!fetch_level_info(level, user.level_id))
		{
			cout << "获取关卡信息失败，请重试！" << endl;
			::system("pause");
			exit(-1);
		}

		//把数据库中的地图数据转换到map中
		if (!transform__map_db2array(level, map))
		{
			printf("地图数据转换失败！\n");
			::system("pause");
			exit(-1);
		}

		for (int i = 0; i < level.map_row; ++i)
		{
			for (int j = 0; j < level.map_column; ++j)
			{
				if (map[i][j] == MAN)
				{
					man_pos.x = i;
					man_pos.y = j;
				}

				putimage(START_X + j * RATIO, START_Y + i * RATIO, &images[map[i][j]]);
			}
		}

		do {
			if (_kbhit())  //捕捉键盘输入
			{
				char ch = _getch();

				if (ch == KEY_UP)
				{
					gameControl(UP);
				}
				else if (ch == KEY_LEFT)
				{
					gameControl(LEFT);
				}
				else if (ch == KEY_RIGHT)
				{
					gameControl(RIGHT);
				}
				else if (ch == KEY_DOWN)
				{
					gameControl(DOWN);
				}
				else if (ch == KEY_QUIT)
				{
					quit = true;
				}

				if (isGameOver())
				{
				
					if (level.next_level < 1)
					{
						gameOverScene(&bg_img);
						quit = true;
						break;
					}

					gameNextScene(&bg_img);

					//更新下一关卡信息
					if (update_user_level(user, level.next_level))
						user.level_id = level.next_level;

					break;
				}
			}
			//定义延迟，避免一直循环，占用大量CPU资源
			Sleep(100); //ms
		} while (quit == false);

	} while (quit == false);

	::system("pause");
	closegraph();
	return 0;
}

void gameControl(DIRECTION direct)
{
	POS next_pos=man_pos, next_next_pos=man_pos;

	switch (direct)
	{
	case UP:
		next_pos.x = man_pos.x - 1;
		next_pos.y = man_pos.y;
		next_next_pos.x = man_pos.x - 2;
		next_next_pos.y = man_pos.y;
		break;
	case LEFT:
		next_pos.x = man_pos.x;
		next_pos.y = man_pos.y - 1;
		next_next_pos.x = man_pos.x;
		next_next_pos.y = man_pos.y - 2;
		break;
	case RIGHT:
		next_pos.x = man_pos.x;
		next_pos.y = man_pos.y + 1;
		next_next_pos.x = man_pos.x;
		next_next_pos.y = man_pos.y + 2;
		break;
	case DOWN:
		next_pos.x = man_pos.x + 1;
		next_pos.y = man_pos.y;
		next_next_pos.x = man_pos.x + 2;
		next_next_pos.y = man_pos.y;
		break;
	}
	
	//前面是地板，人前进一步
	if (isValid(next_pos) && map[next_pos.x][next_pos.y] == FLOOR)
	{
		changeMap(&next_pos, MAN);
		changeMap(&man_pos, FLOOR);
		man_pos = next_pos;
	}
	if (isValid(next_pos) && map[next_pos.x][next_pos.y] == BOX)
	{
		//箱子前是地板
		if (isValid(next_next_pos) && map[next_next_pos.x][next_next_pos.y] == FLOOR)
		{
			changeMap(&next_next_pos, BOX);
			changeMap(&next_pos, MAN);
			changeMap(&man_pos, FLOOR);
			man_pos = next_pos;
			
		}
		//到达目标位置
		else if (isValid(next_next_pos) && map[next_next_pos.x][next_next_pos.y] == BOX_DES)
		{
			changeMap(&next_next_pos, HIT);
			changeMap(&next_pos, MAN);
			changeMap(&man_pos, FLOOR);
			man_pos = next_pos;
		}
		//箱子前是墙
		
	}

}

//改变游戏视图
void changeMap(POS* pos, PROPS prop)
{
	map[pos->x][pos->y] = prop;
	putimage(START_X + pos->y * RATIO, START_Y + pos->x * RATIO, &images[prop]);
}

bool isGameOver()
{
	for (int i = 0; i < ROW; ++i)
	{
		for (int j = 0; j < COLUMN; ++j)
		{
			if (map[i][j] == BOX_DES)
				return false;
		}
	}
	return true;
}

void gameNextScene(IMAGE* bg)
{
	putimage(0, 0, bg);
	settextcolor(WHITE);
	RECT rec = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	settextstyle(20, 0, _T("宋体"));
	drawtext(_T("恭喜过关，任意键跳转下一关！"), &rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	::system("pause");
	cleardevice();
}

void gameOverScene(IMAGE *bg)
{
	putimage(0, 0, bg);
	settextcolor(WHITE);
	RECT rec = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	settextstyle(20, 0, _T("宋体"));
	drawtext(_T("恭喜全部通关！"), &rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

bool login(userinfo& user)
{
	int times = 0;
	bool ret = false;

	do {
		cout << "请输入用户名：" << endl;
		cin >> user.username;
		cout << "请输入密码：" << endl;
		cin >> user.password;

		ret = fetch_user_info(user);
		++times;

		if (times >= 5)
			break;

		if (ret == false)
			cout << "登录失败，请重新输入！" << endl;

	} while (!ret);

	return ret;
}

void init_game_graphic(IMAGE& bg_img)
{
	//初始化窗口
	initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
	loadimage(&bg_img, _T("blackground.bmp"), SCREEN_WIDTH, SCREEN_HEIGHT, true);	//第二个参数改为多字节字符集
	putimage(0, 0, &bg_img);	//显示图片，第1，2个参数为放置图片的初始坐标

	loadimage(&images[WALL], _T("wall_right.bmp"), RATIO, RATIO, true);
	loadimage(&images[FLOOR], _T("floor.bmp"), RATIO, RATIO, true);
	loadimage(&images[BOX_DES], _T("des.bmp"), RATIO, RATIO, true);
	loadimage(&images[MAN], _T("man.bmp"), RATIO, RATIO, true);
	loadimage(&images[BOX], _T("box.bmp"), RATIO, RATIO, true);
	loadimage(&images[HIT], _T("box.bmp"), RATIO, RATIO, true);

}