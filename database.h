#include <mysql.h>
#include <iostream>

using namespace std;

#define DB_NAME "box_man"
#define DB_HOST "127.0.0.1"
#define DB_PORT 3306
#define DB_USER "root"
#define DB_USER_PASSWORD "mysql123"

#define RATIO 50	//图标大小
#define ROW 48		//地图行
#define COLUMN 48	//地图列

static int debug = 1;



//用户信息
typedef struct _userinfo {
	int id;		//用户id
	string username;	//用户名
	string password;	//密码
	int level_id;		//当前关卡
}userinfo;

//关卡信息
typedef struct _levelinfo {
	int level_id;
	string name;
	int map_row;
	int map_column;
	string map_data;
	int next_level;
}levelinfo;


bool fetch_user_info(userinfo& user);
bool update_user_level(userinfo& user, int next_level_id);

static bool connect_db(MYSQL& mysql);

bool fetch_level_info(levelinfo& level, int level_id);

bool transform__map_db2array(levelinfo level, int map[ROW][COLUMN]);

