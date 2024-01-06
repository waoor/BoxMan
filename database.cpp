#include "database.h"


/***********************************************
* ���ܣ�ͨ���û����������ȡ�û���Ϣ
* ���룺user
* ����ֵ��bool
************************************************/
bool fetch_user_info(userinfo& user)
{
	MYSQL mysql;		//���ݿ���
	MYSQL_RES* res;		//��ѯ�����
	MYSQL_ROW row;		//��¼�ṹ��
	char sql[256];
	bool ret = false;

	//1.�������ݿ�
	if (connect_db(mysql) == false)
		return false;

	//2.�����û����������ȡ�û���Ϣ(id,level_id)
	snprintf(sql, 256, "select id,level_id from users where username='%s' and password='%s';", user.username.c_str(), user.password.c_str());
	//md5ʱһ�ּ��ܴ洢
	//snprintf(sql, 256, "select id,level_id from users where username='%s' and password=md5('%s');", user.username.c_str(), user.password.c_str());
	ret = mysql_query(&mysql, sql);	//�ɹ�����0

	if (ret)
	{
		printf("���ݿ��ѯ����%s ����ԭ�� %s\n", sql, mysql_error(&mysql));
		mysql_close(&mysql);		//�ر����ݿ�
		return false;
	}

	//3.��ȡ���
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);		//�ַ���

	if (row == NULL)
	{
		mysql_free_result(res);
		mysql_close(&mysql);
		return false;
	}

	user.id = atoi(row[0]);
	user.level_id = atoi(row[1]);

	if(debug==1)
		printf("userid: %d level_id: %d\n", user.id, user.level_id);	//��ӡID

	//4.���ؽ��

	//�ͷŽ����
	mysql_free_result(res);

	//�ر����ݿ�
	mysql_close(&mysql);

	return true;
}

/*�����û���Ϣ*/
bool update_user_level(userinfo& user, int next_level_id)
{
	MYSQL mysql;		//���ݿ���
	MYSQL_RES* res;		//��ѯ�����
	MYSQL_ROW row;		//��¼�ṹ��
	char sql[256];
	bool ret = false;

	//1.�������ݿ�
	if (connect_db(mysql) == false)
		return false;

	//2.�����û�id������һ�ص�level_id
	snprintf(sql, 256, "update users set level_id=%d where id=%d", next_level_id, user.id);

	ret = mysql_query(&mysql, sql);	//�ɹ�����0

	if (ret)
	{
		printf("���ݿ��ѯ����%s ����ԭ�� %s\n", sql, mysql_error(&mysql));
		mysql_close(&mysql);		//�ر����ݿ�
		return false;
	}

	return true;
}

bool connect_db(MYSQL& mysql)
{
	//��ʼ�����ݿ�
	mysql_init(&mysql);

	//�����ַ�����
	mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");

	if (mysql_real_connect(&mysql, DB_HOST, DB_USER, DB_USER_PASSWORD, DB_NAME, DB_PORT, NULL, 0) == NULL)
	{
		printf("���ݿ�����ʧ�ܣ�����ԭ�� %s\n", mysql_error(&mysql));
		return false;
	}

	return true;
}

/***********************************************
* ���ܣ�ͨ���û��ؿ���ȡ��ͼ��Ϣ
* ���룺����level_id��level����ؿ���Ϣ
* ����ֵ��bool
************************************************/
bool fetch_level_info(levelinfo& level, int level_id)
{
	MYSQL mysql;		//���ݿ���
	MYSQL_RES* res;		//��ѯ�����
	MYSQL_ROW row;		//��¼�ṹ��
	char sql[256];
	bool ret = false;

	//1.�������ݿ�
	if (connect_db(mysql) == false)
		return false;

	//2.��ѯ
	snprintf(sql, 256, "select name,map_row,map_column,map_data,next_level_id from levels where id=%d;", level_id);

	ret = mysql_query(&mysql, sql);	//�ɹ�����0

	if (ret)
	{
		printf("���ݿ��ѯ����%s ����ԭ�� %s\n", sql, mysql_error(&mysql));
		cout << "���ݿ��ѯ����%s ����ԭ�� %s\n";
		mysql_close(&mysql);
		return false;
	}

	//3.��ȡ���
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);		//�ַ���

	if (row == NULL)
	{
		cout << "rowΪ��" << endl;
		mysql_free_result(res);
		mysql_close(&mysql);
		return false;
	}

	level.level_id = level_id;
	level.name = row[0];
	level.map_row = atoi(row[1]);
	level.map_column = atoi(row[2]);
	level.map_data = row[3];
	level.next_level = atoi(row[4]);

	if (debug == 1)
	{
		cout << "�ؿ���Ϣ��" << level.name << "������" << level.map_row << "������" << level.map_column << "��һ�أ�" << level.next_level << endl;
		cout << "��ͼ��Ϣ��" << level.map_data << endl;
	}
	

	//4.���ؽ��

	//�ͷŽ����
	mysql_free_result(res);

	//�ر����ݿ�
	mysql_close(&mysql);

	return true;

}

bool transform__map_db2array(levelinfo level, int map[ROW][COLUMN])
{
	if (level.map_row > ROW || level.map_column > COLUMN)
	{
		printf("��ͼ̫������������\n");
		return false;
	}

	if (level.map_data.length() < 1)
	{
		printf("��ͼ�����������������ã�\n");
		return false;
	}

	int start = 0, end = 0;
	int row = 0, column = 0;
	
	do {
		end = level.map_data.find('|', start);
		
		if (end < 0)
		{
			end = level.map_data.length();
		}

		if (start >= end)
			break;

		string line = level.map_data.substr(start, end - start);
		printf("get line: %s\n", line.c_str());

		//���е�ͼ���ݽ��н���
		char* next_token = NULL;
		char* item = strtok_s((char*)line.c_str(), ",", &next_token);


		column = 0;

		while (item && column < level.map_column)
		{
			printf("%s ", item);
			map[row][column] = atoi(item);
			++column;
			item = strtok_s(NULL, ",", &next_token);	
		}

		if (column < level.map_column)
		{
			printf("��ͼ���ݽ�������\n");
			return false;
		}

		printf("\n");

		++row;

		if (row >= level.map_row)
		{
			break;
		}

		start = end + 1;

	} while (1==1);

	if (row < level.map_row)
	{
		printf("��ͼ���������趨��%d<%d,��ֹ��\n", row, level.map_row);
	}

	return true;
}

