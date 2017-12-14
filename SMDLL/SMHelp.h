/*
	注释时间:2014-4-25
	author: glp
	封装与播放器的协议发送，以及回调
	提供给点歌台使用
*/
#ifndef SMHELP__H__
#define SMHELP__H__

#include "smdatadef.h"

#ifdef SMDLL_EXPORTS
#define DLL_CLASS __declspec(dllexport)
#else
#define DLL_CLASS __declspec(dllimport)
#endif

typedef void (*DEALMESSAGE) (int cmd,int wParam,int lParam,void* sParam);

class DLL_CLASS CSMHelp
{
public:
	//注册回调函数
	static void InitSMHelp(DEALMESSAGE pDealMessage=NULL);
	//(player.exe 所在的目录,"player.exe")
	CSMHelp(const char* exeDir,const char* exeName);
	~CSMHelp();
	//关闭播放器
	bool PostClosePlayer();
	//切换歌曲,第二个参数用于互联网用户的mdm文件，一般只要设为NULL,
	/*
	@param file: 指定mv文件名，包含全路径
	@param info:一些切换歌曲的信息结构的组装，具体参见CMD_DXPLAYER_SWITCHSONG 定义
	@param mdm:指定mdm文件名，用于播放互联网用户演唱的歌曲，如果是播放普通mv，则为NULL即可
	@return : true 成功；false 失败
	*/
	bool PostSwitchSong(const wchar_t* file,int info,const wchar_t* mdm=NULL);
	//歌手图片
	bool PostSingerPic(const wchar_t* file);
	//继续播放
	bool PostControlResume();
	//暂停播放
	bool PostControlPause();
	//切换评分模式
	bool PostPlayGrade(unsigned int eType);
	//原伴唱切换
	bool PostPlayAccompany();
	//歌曲信息
	bool PostSongInfo(const ThreeWString* pParam);
	//支持简版歌曲
	bool PostSongStartEnd(int start,int end);
	//设置麦克风音量
	bool PostMicValue(int info);
	//设置音量
	bool PostSetVolume(int info);
	//bool PostGetVolume();
	//增加音调
	bool PostAddTone();
	//减音调
	bool PostSubTone();
	//获取音调
	bool PostGetTone();
	//静音
	bool PostMuteOpen();
	//取消静音
	bool PostMuteClose();
	//发送公告,以及弹幕信息，具体参数参见processcommunicate.h PMessage结构
	bool PostMessage(PMessage* pMsg);
	//广播上榜通知
	/*
	@param shop:店名
	@param room:包厢名
	@param song:歌曲名
	@param score:演唱的分数，三位数，第三位为小数
	@pPlayerScore: PlayerScore结构体数据，具体释义参见结构说明

	@return: true,成功；false,失败
	*/
	bool PostSpecialMsg(const wchar_t* shop,const wchar_t* room,const wchar_t* song,int score,void* pPlayerScore);
	//火警
	bool PostFireWarning();
	//取消火警
	bool PostNoFireWarning();
	//下首歌的名字
	bool PostNextSong(const wchar_t* name);
	//音效提示 参见CMD_DXPLAYER_EFFECTNOTIFY
	bool PostEffectNotify(unsigned int eInt);
	//气氛 参见CMD_DXPLAYER_ATMOSPHERE
	bool PostAtomosphere(unsigned int eInt);
	//是否联网 1代表在线，0代表离线
	bool PostOnline(int eInt);
	//发送视频信息或者携带数据,具体参数参见processcommunicate.h VideoInfo结构
	bool PostVideoInfo(int operation,int type,unsigned int width,unsigned int height
		,unsigned int bytes_pixel,unsigned int size,bool first,const char* name);
	//发送演唱者排名信息，具体参数参见processcommunicate.h RankingInfo结构
	//pPlayerScore:PlayerScore* 指针;nPlayer: 含几个分数
	bool PostRankingList(void* pPlayerScore,int nPlayer);
	//@Deprecated 参数移交到PostSwitchSong();
	//通知播放器播放的是否是外网用户演唱的歌曲,是否是广告宣传片
	//bool PostFromOther(bool bOther,bool bAdvertisement);
	//来自星光的实时评测，包含句评测,具体参数参见processcommunicate.h OtherGrade结构
	bool PostOhterGrade(const OtherGrade* pOG);
	//调整播放进度，0-100
	bool PostProgress(int progress);
	//选择歌曲分类，播放器默认会在当前目录读取randomlist.csv文件，获取分类歌曲信息，请保证歌曲名不同
	bool PostCategory(const char* category);
	//正如你所见，这个的意思true是暂停滚动，如果为false可以继续重新滚动
	bool PostSelectState(bool bStop);
	//更新包厢金币信息
	bool PostUpdateMoney(int money);
	//礼物兑换通知
	bool PostGiftExchange(GiftExchange* pData);
	//送礼通知
	bool PostGiftSend(GiftSend* pData);

	//门店排名信息
	bool PostShopTop(ShopTopTip* pData);
	//门店排名更新信息
	bool PostSTUpdate(STUpdate* pData);

	//主播连接状态
	bool PostConnectStatus(int status);
	//扫码信息
	bool PostScanInfo(const wchar_t* downloadUrl,const wchar_t* enterRoomUrl);

	//以下配置信息，只需要发一次就够了，可以放在程序起来之后发送！
	//设置音乐音量范围
	bool PostMusicRang(int min,int max);
	//设置麦克风音量范围
	bool PostMicRang(int min,int max);
	//初始化开房
	bool PostInitOpenRoom();
private:
	bool KillProcessFromName(const char* pProcessName);
public:
	static DEALMESSAGE m_pDealMessage;
};

#endif//SMHELP__H__