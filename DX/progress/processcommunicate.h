/*
	注释时间:2014-4-25
	author: glp
	播放器与点歌台交互的协议。以及相应的结构数据
	共用一个头文件
*/
#ifndef PROCESSCOMMUNICATE__H__
#define PROCESSCOMMUNICATE__H__

#include "smdatadef.h"

//*****************************Center to Player***********************//
enum eCMD_C2P
{
	//命令背后标注了该命令 需要的结构数据，数据中有相应的解释说明，或者已经在这说明了。
	CMD_CENTER_FIRST=0,
	//程序退出
	CMD_DXPLAYER_CLOSE,//NULL
	//音乐音量值的范围：最小wparam(int),最大lparam(int)
	CMD_DXPLAYER_MUSIC_RANGE,//DoubleInt
	//麦克风音量值的范围：最小wparam(int),最大lparam(int)
	CMD_DXPLAYER_MIC_RANGE,//DoubleInt
	//音调值的范围：最小wparam(int),最大lparam(int)
	CMD_DXPLAYER_TONE_RANGE,//DoubleInt
	//下一首歌曲名，只需要名字
	//wparam(char*),（注：如果歌曲列表为空则发送"ktv1",如果没有下一首则发送"ktv2"）
	CMD_DXPLAYER_NEXT_SONGNAME,//SingleWString
	//歌曲信息
	//str1歌手名(wchar_t*),str2歌名(wchar_t*),str3备用歌手名字，用于在组合对唱歌曲的情况(wchar_t*)
	CMD_DXPLAYER_SONGINFO,//ThreeWString
	//歌手图片 ！！！！！！（需要在CMD_DXPLAYER_SONGINFO之前发送）
	//wparam歌手图片(char*包含路径,后缀)
	CMD_DXPLAYER_SINGERPIC,//SingleWString
	//设置歌曲的起始秒跟结束秒
	//wparam起始(int)lparam结束(int)
	CMD_DXPLAYER_SONGSTARTEND,//DoubleInt
	//切换评分状态，
	//wparam(int)类型，0,关闭，1单人模式，2双人pk模式，3对唱模式
	CMD_DXPLAYER_GRADETYPE,//SingleInt
	//切换歌曲，第一次可以当成播放 
	//wparam歌曲名，包含完整路径和后缀名mp4/mpg/avi(char*)
	//lparam(int):曲谱高度1(第一个字节*10)；第9位:是否评分歌曲(1是评分，0是非评分)
	//10-17位:是当前音乐音量；
	/*新增CMD_DXPALYER_FROMOTHER过来的参数变量 去掉音调值的设置*/
	//18:是否是MV:1是MV;0不是MV
	//19:是否是广告:1是广告;0不是广告
	//20:是否需要跳过片头片尾:1跳过;0不跳过
	//21:暂时空出
	//22位:是否对调音轨的；23位:是否是对唱；24-31位:(数据*10，曲谱高度2)；32位:最后是否显示评测结果(1是显示，0是不显示)
	CMD_DXPLAYER_SWITCHSONG,//SwitchInfo
	//继续播放
	CMD_DXPLAYER_CONTROL_RESUME,//NULL
	//暂停
	CMD_DXPLAYER_CONTROL_PAUSE,//NULL
	//切换伴奏状态，开启/关闭
	CMD_DXPLAYER_ACCOMPANY,//NULL
	//获取当前播放器音量
	CMD_DXPLAYER_GETMUSICVOLUME,//NULL
	//打开静音
	CMD_DXPLAYER_MUTE_OPEN,//NULL
	//关闭静音
	CMD_DXPLAYER_MUTE_CLOSE,//NULL
	//设置播放器声音 (0-10]
	//wparam音量(int)
	CMD_DXPLAYER_SETMUSICVOLUME,//SingleInt
	//获取麦克风音量
	CMD_DXPLAYER_GETMICVOLUME,//NULL
	//设置麦克风音量wparam(int)
	CMD_DXPLAYER_SETMICVOLUME,//SingleInt
	//获取音调
	CMD_DXPLAYER_GETTONEVALUE,//NULL
	//加1音调
	CMD_DXPLAYER_ADDTONEVALUE,//NULL
	//减1音调
	CMD_DXPLAYER_SUBTONEVALUE,//NULL
	//打开幻影
	CMD_DXPLAYER_OPENHY,//NULL
	//关闭幻影
	CMD_DXPLAYER_CLOSEHY,//NULL
	//公告,火警,到时提醒等一切信息
	CMD_DXPLAYER_MESSAGE,//PMessage
	//火警需要额外通知
	CMD_DXPLAYER_FIREWARNING,//NULL
	//取消火警
	CMD_DXPLAYER_NOFIREWARNING,//NULL
	//气氛wparam(int) : 0 代表 鼓掌，1代表喝彩
	CMD_DXPLAYER_ATMOSPHERE,//SingleInt 
	//是否联网wparam(int),1 代表在线，0代表离线
	CMD_DXPLAYER_ONLINE,//SingleInt 
	//视频数据传输
	CMD_DXPLAYER_VIDEO_INFO,//VideoInfo 
	//实时评分句评分数据，来自星光
	CMD_DXPLAYER_OTHERGRADE,//OtherGrade 
	//@Deprecated 参数提交到CMD_DXPLAYER_SWITCHSONG里面
	//CMD_DXPALYER_FROMOTHER,//DoubleInt 
	//设置播放进度0-100
	CMD_DXPLAYER_PROGRESS,//SingleInt 
	//选歌分类
	CMD_DXPLAYER_CATEGORY,//SingleString 
	//控制滚动
	CMD_DXPLAYER_SELECTSTATE,//SingleInt 
	//音效提示 wparam(int)
	//0：魔音快歌，1：魔音顶尖，2：魔音唱将，3：魔音K歌
	//4：专业快歌，5：专业顶尖，6：专业唱将，7：专业K歌
	CMD_DXPLAYER_EFFECTNOTIFY,//SingleInt 
	//更新包厢金币信息，如果开启线上视频的时候金币信息不显示
	CMD_DXPLAYER_UPDATEMONEY,//SingleInt 
	//美女主播视屏互动提示
	//0-提示正在连接中,1-提示连接成功,2-提示连接超时,3-提示连接异常
	CMD_DXPLAYER_PERISTATUS,//SingleInt
	//二维码信息 str1是app下载二维码，str2是进包厢二维码
	CMD_DXPLAYER_SCAN,//DoubleWString
	//唱歌得分排名
	CMD_DXPLAYER_RANKINGLIST,//RankingInfo 
	//初始化开房
	CMD_DXPLAYER_INITOPEN,//NULL

	//////////////////////////下面的信息会分流到新的一个信息展示进程 notify.exe////////////////////////////////////////////////
	CMD_NOTIFY_BEGIN=200,
	//弹幕信息
	CMD_NOTIFY_MESSAGE,//PMessage
	//特殊消息提醒，用户演唱超过90分
	CMD_NOTIFY_OVERNINETY,//OverNinety
	//礼物兑换通知，属于特效通知，具体信息参见GiftSending结构说明
	CMD_NOTIFY_GIFTEXCHANGE,//GiftExchange
	//礼物送礼通知
	CMD_NOTIFY_GIFTSEND,//GiftSend
	//门店上榜前三
	CMD_NOTIFY_SHOPTOP,//ShopTop 
	//门店榜单更新通知
	CMD_NOTIFY_STUPDATE,//STUpdate
	//位置更改
	CMD_NOTIFY_POS,
};
//*****************************Player to Center***********************//
enum e_CMD_P2C
{	
	CDM_PLAYER_FIRST=500,
	//当前歌曲播放完毕
	CMD_CENTER_ENDSONG,
	//wparam第一个人的总分(int),lparam第二个人的总分(int),需要操作界面自己判断什么模式，如果是单人模式，第二个人的评分需要丢弃
	CMD_CENTER_SCORE_STARNUM,
	//wparam音乐音量(int) 
	CMD_CENTER_MUSICVOLUME_CURRENT,
	//wparam麦克风音量(int)
	CMD_CENTER_MICVOLUME_CURRENT,
	//wparam音调(int)
	CMD_CENTER_TONE_CURRENT,
	//启用button，通知操作界面
	CMD_CENTER_ENABLE_BUTTON,
	//禁掉一些按钮，直到切歌或重唱
	CMD_CENTER_DISABLE_BUTTON,
	//需要几条排名信息 
	//int
	CMD_CENTER_RANKING_NUM,
	//const wchar_t* sParam错误原因
	CMD_CENTER_ERROR_LOG,
	//通知notify.exe改变窗口位置
	CMD_NOTIFYPOS_FROM_PLAYER,
};
#endif// PROCESSCOMMUNICATE__H__

