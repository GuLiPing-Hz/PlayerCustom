#ifndef WMCOPYDATA__H__
#define WMCOPYDATA__H__
#define CLS_WINDOW_FLASH "FlashPlayer_TG"

//增加一个flash到播放队列，如果空闲则直接播放
#define WMDATA_ADD_FILE 1

//暂停播放flash
#define WMDATA_PAUSE 2

//通知可以继续播放flash，如果有未完成的播放队列
#define WMDATA_RESUME 3

//停止播放flash,等待命令
#define WMDATA_STOP 4

#endif//WMCOPYDATA__H__
