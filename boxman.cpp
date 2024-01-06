#include "boxman.h"

int main()
{
	//�û������֤
	userinfo user;
	levelinfo level;

	IMAGE bg_img;

	if (!login(user))
	{
		cout << "��¼ʧ��" << endl;
		system("pause");
		exit(-1);
	}
	else
	{
		cout << "��¼�ɹ�,�û�id��" << user.id << ",�����ڵĹؿ��ǣ�" << user.level_id << endl;
		::system("pause");
	}

	//��ʼ������
	init_game_graphic(bg_img);

	//��Ϸ����
	bool quit = false;

	do {


		//����level_id��ȡ�ؿ�����
		if (!fetch_level_info(level, user.level_id))
		{
			cout << "��ȡ�ؿ���Ϣʧ�ܣ������ԣ�" << endl;
			::system("pause");
			exit(-1);
		}

		//�����ݿ��еĵ�ͼ����ת����map��
		if (!transform__map_db2array(level, map))
		{
			printf("��ͼ����ת��ʧ�ܣ�\n");
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
			if (_kbhit())  //��׽��������
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

					//������һ�ؿ���Ϣ
					if (update_user_level(user, level.next_level))
						user.level_id = level.next_level;

					break;
				}
			}
			//�����ӳ٣�����һֱѭ����ռ�ô���CPU��Դ
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
	
	//ǰ���ǵذ壬��ǰ��һ��
	if (isValid(next_pos) && map[next_pos.x][next_pos.y] == FLOOR)
	{
		changeMap(&next_pos, MAN);
		changeMap(&man_pos, FLOOR);
		man_pos = next_pos;
	}
	if (isValid(next_pos) && map[next_pos.x][next_pos.y] == BOX)
	{
		//����ǰ�ǵذ�
		if (isValid(next_next_pos) && map[next_next_pos.x][next_next_pos.y] == FLOOR)
		{
			changeMap(&next_next_pos, BOX);
			changeMap(&next_pos, MAN);
			changeMap(&man_pos, FLOOR);
			man_pos = next_pos;
			
		}
		//����Ŀ��λ��
		else if (isValid(next_next_pos) && map[next_next_pos.x][next_next_pos.y] == BOX_DES)
		{
			changeMap(&next_next_pos, HIT);
			changeMap(&next_pos, MAN);
			changeMap(&man_pos, FLOOR);
			man_pos = next_pos;
		}
		//����ǰ��ǽ
		
	}

}

//�ı���Ϸ��ͼ
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
	settextstyle(20, 0, _T("����"));
	drawtext(_T("��ϲ���أ��������ת��һ�أ�"), &rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	::system("pause");
	cleardevice();
}

void gameOverScene(IMAGE *bg)
{
	putimage(0, 0, bg);
	settextcolor(WHITE);
	RECT rec = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	settextstyle(20, 0, _T("����"));
	drawtext(_T("��ϲȫ��ͨ�أ�"), &rec, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

bool login(userinfo& user)
{
	int times = 0;
	bool ret = false;

	do {
		cout << "�������û�����" << endl;
		cin >> user.username;
		cout << "���������룺" << endl;
		cin >> user.password;

		ret = fetch_user_info(user);
		++times;

		if (times >= 5)
			break;

		if (ret == false)
			cout << "��¼ʧ�ܣ����������룡" << endl;

	} while (!ret);

	return ret;
}

void init_game_graphic(IMAGE& bg_img)
{
	//��ʼ������
	initgraph(SCREEN_WIDTH, SCREEN_HEIGHT);
	loadimage(&bg_img, _T("blackground.bmp"), SCREEN_WIDTH, SCREEN_HEIGHT, true);	//�ڶ���������Ϊ���ֽ��ַ���
	putimage(0, 0, &bg_img);	//��ʾͼƬ����1��2������Ϊ����ͼƬ�ĳ�ʼ����

	loadimage(&images[WALL], _T("wall_right.bmp"), RATIO, RATIO, true);
	loadimage(&images[FLOOR], _T("floor.bmp"), RATIO, RATIO, true);
	loadimage(&images[BOX_DES], _T("des.bmp"), RATIO, RATIO, true);
	loadimage(&images[MAN], _T("man.bmp"), RATIO, RATIO, true);
	loadimage(&images[BOX], _T("box.bmp"), RATIO, RATIO, true);
	loadimage(&images[HIT], _T("box.bmp"), RATIO, RATIO, true);

}