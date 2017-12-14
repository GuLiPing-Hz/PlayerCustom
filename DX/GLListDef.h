/*
	注释时间:2014-4-25
	author: glp
	基于bass音频库的声音管理类，
*/
#pragma  once

#include <vector>
#include <list>
#include <map>
#include <wchar.h>
#include <set>


#include "staff/KKType.h"

typedef std::list<FBControl>							LISTFBCONTROL;
typedef std::list<wchar_t>								LISTWCHAR;
typedef std::list<GLRealtimeGrade>			LISTGLREALTIMEGRADE;
typedef std::list<GLSentenceGrade>			LISTGLSENTENCEGRADE;
typedef std::list<_tGuiLyricInfo>					LISTGUILYRICVECT;
typedef std::list<_tGuiWaveInfo>					LISTGUIPITCHVECT;
typedef std::list<float>									LISTSENTENCELINEVECT;
typedef std::list<int>										LISTINT;
typedef std::list<float>									LISTFLOAT;
typedef std::list<bool>									LISTBOOL;
typedef std::list<std::string>							LISTSTRING;
typedef std::list<BarrageInfo>						LISTBARRAGEINFO;
typedef std::list<SpecialInfo>						LISTSPECIALINFO;
typedef std::list<RealTimePoint>					LISTREALTIMEPOINT;

typedef std::vector<_tGuiParagraphInfo>	VECTPARAGRAPHVECT;
typedef std::vector<ulong>							VECTULONG;
typedef std::vector<float>							VECTFLOAT;
typedef std::vector<LINEVERTEX>				VECTLINEVERTEX;
typedef std::vector<_tGuiLyricInfo>				VECTGUILYRICVECT;
typedef std::vector<_tGuiWaveInfo>			VECTGUIPITCHVECT;
typedef std::vector<int>								VECTORINT;

class RenderEImage;
typedef std::vector<RenderEImage*>	VECTORIMAGE;

typedef std::map<int,std::string> MAPIDSTRING;
typedef std::map<eVideoType,int>	MAPVIDEOTYPESWITCH;
typedef std::map<eVideoType,std::wstring> MAPVIDEONAME;
typedef std::map<wchar_t,int> MAPWCHARINT;
typedef std::map<std::string,SongListInfo> MAPSONGLIST;
typedef std::map<std::string,MAPSONGLIST> MAPCATEGORY;

