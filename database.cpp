#include "database.h"


/***********************************************
* 功能：通过用户名和密码获取用户信息
* 输入：user
* 返回值：bool
************************************************/
bool fetch_user_info(userinfo& user)
{
	MYSQL mysql;		//数据库句柄
	MYSQL_RES* res;		//查询结果集
	MYSQL_ROW row;		//记录结构体
	char sql[256];
	bool ret = false;

	//1.连接数据库
	if (connect_db(mysql) == false)
		return false;

	//2.根据用户名和密码获取用户信息(id,level_id)
	snprintf(sql, 256, "select id,level_id from users where username='%s' and password='%s';", user.username.c_str(), user.password.c_str());
	//md5时一种加密存储
	//snprintf(sql, 256, "select id,level_id from users where username='%s' and password=md5('%s');", user.username.c_str(), user.password.c_str());
	ret = mysql_query(&mysql, sql);	//成功返回0

	if (ret)
	{
		printf("数据库查询出错，%s 错误原因： %s\n", sql, mysql_error(&mysql));
		mysql_close(&mysql);		//关闭数据库
		return false;
	}

	//3.获取结果
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);		//字符串

	if (row == NULL)
	{
		mysql_free_result(res);
		mysql_close(&mysql);
		return false;
	}

	user.id = atoi(row[0]);
	user.level_id = atoi(row[1]);

	if(debug==1)
		printf("userid: %d level_id: %d\n", user.id, user.level_id);	//打印ID

	//4.返回结果

	//释放结果集
	mysql_free_result(res);

	//关闭数据库
	mysql_close(&mysql);

	return true;
}

/*更新用户信息*/
bool update_user_level(userinfo& user, int next_level_id)
{
	MYSQL mysql;		//数据库句柄
	MYSQL_RES* res;		//查询结果集
	MYSQL_ROW row;		//记录结构体
	char sql[256];
	bool ret = false;

	//1.连接数据库
	if (connect_db(mysql) == false)
		return false;

	//2.根据用户id更新下一关的level_id
	snprintf(sql, 256, "update users set level_id=%d where id=%d", next_level_id, user.id);

	ret = mysql_query(&mysql, sql);	//成功返回0

	if (ret)
	{
		printf("数据库查询出错，%s 错误原因： %s\n", sql, mysql_error(&mysql));
		mysql_close(&mysql);		//关闭数据库
		return false;
	}

	return true;
}

bool connect_db(MYSQL& mysql)
{
	//初始化数据库
	mysql_init(&mysql);

	//设置字符编码
	mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");

	if (mysql_real_connect(&mysql, DB_HOST, DB_USER, DB_USER_PASSWORD, DB_NAME, DB_PORT, NULL, 0) == NULL)
	{
		printf("数据库连接失败，错误原因： %s\n", mysql_error(&mysql));
		return false;
	}

	return true;
}

/***********************************************
* 功能：通过用户关卡获取地图信息
* 输入：根据level_id，level保存关卡信息
* 返回值：bool
************************************************/
bool fetch_level_info(levelinfo& level, int level_id)
{
	MYSQL mysql;		//数据库句柄
	MYSQL_RES* res;		//查询结果集
	MYSQL_ROW row;		//记录结构体
	char sql[256];
	bool ret = false;

	//1.连接数据库
	if (connect_db(mysql) == false)
		return false;

	//2.查询
	snprintf(sql, 256, "select name,map_row,map_column,map_data,next_level_id from levels where id=%d;", level_id);

	ret = mysql_query(&mysql, sql);	//成功返回0

	if (ret)
	{
		printf("数据库查询出错，%s 错误原因： %s\n", sql, mysql_error(&mysql));
		cout << "数据库查询出错，%s 错误原因： %s\n";
		mysql_close(&mysql);
		return false;
	}

	//3.获取结果
	res = mysql_store_result(&mysql);
	row = mysql_fetch_row(res);		//字符串

	if (row == NULL)
	{
		cout << "row为空" << endl;
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
		cout << "关卡信息：" << level.name << "行数：" << level.map_row << "列数：" << level.map_column << "下一关：" << level.next_level << endl;
		cout << "地图信息：" << level.map_data << endl;
	}
	

	//4.返回结果

	//释放结果集
	mysql_free_result(res);

	//关闭数据库
	mysql_close(&mysql);

	return true;

}

bool transform__map_db2array(levelinfo level, int map[ROW][COLUMN])
{
	if (level.map_row > ROW || level.map_column > COLUMN)
	{
		printf("地图太大，请重新设置\n");
		return false;
	}

	if (level.map_data.length() < 1)
	{
		printf("地图数据有误，请重新设置！\n");
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

		//对行地图数据进行解析
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
			printf("地图数据解析错误！\n");
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
		printf("地图行数少于设定，%d<%d,终止！\n", row, level.map_row);
	}

	return true;
}

