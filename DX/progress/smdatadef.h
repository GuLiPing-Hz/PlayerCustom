#ifndef SMDATADEF__H__
#define SMDATADEF__H__

#define CONFIG_MAX_LEN 900

#define SM_CONFIG_NAME 50
#define SM_CONFIG_ROOM 100
#define SM_CONFIG_SONG 100
#define SM_CONFIG_GIFT 50
#define SM_CONFIG_FILE 260
#include <wchar.h> 

typedef struct _DoubleInt
{
	int cmd;//0
	int wparam;
	int lparam;
}DoubleInt;

typedef struct _SingleInt
{
	int cmd;//0
	int wparam;
}SingleInt;

typedef struct _DoubleString
{
	int cmd;//0
	char str1[SM_CONFIG_FILE];//包含'\0'
	char str2[SM_CONFIG_FILE];//包含'\0'
}DoubleString;

typedef struct _DoubleWString
{
	int cmd;//0
	wchar_t str1[SM_CONFIG_FILE];//包含'\0'
	wchar_t str2[SM_CONFIG_FILE];//包含'\0'
}DoubleWString;

typedef struct _ThreeWString
{
	int cmd;//0
	wchar_t str1[SM_CONFIG_FILE];//包含'\0'
	wchar_t str2[SM_CONFIG_FILE];//包含'\0'
	wchar_t str3[SM_CONFIG_FILE];//包含'\0'
}ThreeWString;

typedef struct _DoubleStringB
{
	int cmd;
	char str1[SM_CONFIG_FILE];//包含'\0'
	char str2[SM_CONFIG_FILE];//包含'\0'
	bool needstart;
}DoubleStringB;

typedef struct _SingleString
{
	int cmd;//0
	char str[SM_CONFIG_FILE];//包含'\0'
}SingleString;

typedef struct _SingleWString
{
	int cmd;//0
	wchar_t str[SM_CONFIG_FILE];//包含'\0'
}SingleWString;

typedef struct _SwitchInfo
{
	int cmd;//0
	wchar_t str[SM_CONFIG_FILE];//包含'\0'
	wchar_t mdm[SM_CONFIG_FILE];//包含'\0',mdm文件名，用于播放非MV视频文件的时候。一般不用
	int wparam;
	//@Deprecated;
	//int mv;//是否需要显示星光MV，必须在这之前发送MV info
}SwitchInfo;

typedef struct _VideoInfo
{
	int cmd;//0
	int operation;//0：关闭，1：开启，2：暂停排麦
	int type;//0：主持人，1：本地
	bool bname;//第一次发送名字的时候需要置为true，其他置为false
	char name[SM_CONFIG_NAME];
    unsigned int width;//视频宽度
	unsigned int height;//视频高度
	unsigned int bytes_pixel;//单位像素的字节数
	unsigned int size;//视频数据量，字节为单位,无数据设为0
	unsigned char* buffer;//视频 RGB 数据
}VideoInfo;

typedef struct _PMessage
{
	//如果为普通的时候可以对弹幕有效的内容设置为0
	int cmd;//0
	int type;//0： 普通,1：手机聊天弹幕,2：星光聊天弹幕
	unsigned int mode;//对聊天弹幕有效，字幕模式: 0:普通弹幕;1:顶上弹幕;2:底下弹幕
	unsigned int fsize;//对聊天弹幕有效，字体大小1-10
	unsigned int argb;//对聊天弹幕有效，字体颜色
	char str[1024];//包含'\0'   不要超过CONFIG_MAX_LEN个字符 如果是聊天弹幕的话则是说话者内容
	int sayid;//对弹幕有效 说话者ID
	wchar_t sayname[100];//对弹幕有效 说话者名字
}PMessage;

typedef struct _PlayerScore
{
	bool valid;//有效？
	int noInSong;//这个歌的历史排名
	int noInHistory;//不同歌,该店今日的排名(店排名)
	int percentHistory;//击败多少本店演唱者
	int noInCountry;//不同歌,全国今日的排名
    int percentCountry;//击败多少全国演唱者
}PlayerScore;

typedef struct _RankingInfo
{
	int cmd;//0
	PlayerScore ps[2];//0 是第一位演唱者的数据;1是第二位演唱者的数据,单人演唱的时候不需要
}RankingInfo;

typedef struct _OtherGrade
{
	int cmd;//0
	unsigned int		cur_tm;						// 当前时间
	float						cur_pitch;					// 当前演唱者唱的音高
	int						sentence_index;			// 句序号（第一句序号为0）
	float						realtime_score;			// 当前句的实时得分
	float						accumulate_score;		// 当前句的累加得分
	double					cur_db;

	int						sentence_level;			 //句评分等级
	float						sentence_pitch_score;// 句得分
	float						sentence_lyric_score; //歌词得分
	bool						sentence_switch;		 // 是否切换了句子
}OtherGrade;

typedef struct _OverNinety
{
	int cmd;//0
    wchar_t shop[SM_CONFIG_ROOM];//店名
    wchar_t room[SM_CONFIG_ROOM];//房间名
    wchar_t song[SM_CONFIG_SONG];//歌曲名字
	int score;//三位数，第三位为小数
	PlayerScore ps;//成绩
}OverNinety;

typedef struct _GiftExchange
{
	int cmd;//0
    wchar_t shop[SM_CONFIG_ROOM];//店名
    wchar_t room[SM_CONFIG_ROOM];//房间名
	int money;//金币
    wchar_t giftname[SM_CONFIG_GIFT];//礼物名字
	int count;//礼物数量
    wchar_t giftunit[10];//礼物单位
}GiftExchange;

typedef struct _GiftSend
{
	int cmd;//0
	int sendid;//发送者ID
	wchar_t sendname[SM_CONFIG_NAME];//发送者名字
	wchar_t sendlocation[SM_CONFIG_ROOM];//发送者位置 店名+包厢名  新浪好声音A01
	int recvid;//接受者ID
	wchar_t recvname[SM_CONFIG_NAME];//接受者名字
	wchar_t recvlocation[SM_CONFIG_ROOM];//接受者位置
	int giftId;//礼物ID
	wchar_t giftname[SM_CONFIG_GIFT];//礼物名称
	wchar_t giftfile[SM_CONFIG_FILE];//礼物的图片文件 含路径
	int count;//礼物数量
	wchar_t giftunit[10];//礼物单位
}GiftSend;

typedef struct _TopInfo
{
	bool valid;//是否有效
    wchar_t room[SM_CONFIG_ROOM];
	int    score;
}TopInfo;

typedef struct _ShopTopTip
{
	int cmd;//0
    wchar_t time[SM_CONFIG_NAME];//坚持到的时间点，请组织时分秒 例如：22点30分
    wchar_t gift[SM_CONFIG_NAME];//奖励的礼物信息
	TopInfo top[3];//前三名，务必按照0,1,2的顺序存储
}ShopTopTip;

typedef struct _STUpdate 
{
	int cmd;//0
    wchar_t room[SM_CONFIG_ROOM];//唱歌上门店榜的包厢
    wchar_t song[SM_CONFIG_SONG];//唱的歌名
	int score;//唱的分数
	bool overed;//是否是超过，如果是超过，则意味着本包厢在此之前是前三的。
	int rank;//排名，如果overed是true，则是本包厢下降的排名；如果为false，则是唱歌得到的排名。
}STUpdate;

typedef struct _NotifyPos
{
	int cmd;//0
	int left;
	int top;
	int right;
	int bottom;
}NotifyPos;

struct _tOLRoomInfo
{
	int cmd;
	int at_index;
	char room_logo[260];
	char room_name[260];
	int	room_idx;
	int	cur_player_num;
	int	maxplayernum;
	bool is_lock;
	bool is_pwd;
	bool is_close;

	_tOLRoomInfo() {memset(this, 0, sizeof(_tOLRoomInfo));}
};

struct _tConnectStatus
{
	int cmd;
	int nStatus;
	int nRoomID;
	char room_name[260];

	_tConnectStatus() {memset(this, 0, sizeof(_tConnectStatus));}
};

struct _tDicingResult
{
	int cmd;
	bool isMe;          //是否是自己，如果是自己，id和昵称可以不填
	int idx;            //用户id
	char userName[260]; //用户昵称
	int dicesCount;     //骰子组个数
	char dices[16];     //骰子组

	_tDicingResult() {memset(this, 0, sizeof(_tDicingResult));}
};

struct _tDicingTimes
{
	int cmd;
	bool isMe;          //是否是自己，如果是自己，id和昵称可以不填
	int idx;            //用户id
	int times;          //摇骰子次数

	_tDicingTimes() {memset(this, 0, sizeof(_tDicingTimes));}
};

//center 指控制端，player 指dx播放端 定义两个共享内存的名字
const char C2X[] = "CENTER_TO_XINGGUANG";//center 只写，XINGGUANG只读
const char X2C[] = "XINGGUANG_TO_CENTER";//center 只读，XINGGUANG只写
const char C2P[] = "CENTER_TO_PLAYER";//center 只写，PLAYER只读
const char P2C[] = "PLAYER_TO_CENTER";//center 只读，PLAYER只写
const char C2N[] = "CENTER_TO_NOTIFY";//center只写，notify只读
const char VIDEO[] = "XINGGUANG_TO_PLAYER";

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) \
    if ((x) != NULL)      \
{                   \
    delete (x);        \
    (x) = NULL;        \
}
#endif
#endif
