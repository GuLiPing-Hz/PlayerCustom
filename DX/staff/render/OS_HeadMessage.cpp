#include "stdafx.h"
#include "OS_HeadMessage.h"

#define TIME_TO_KEEP 2000
#define CONFIG_PIXEL_WIDTH 300
#define LEFT_PIXEL_PERTIME 0.03f
#define HEADMESSAGE_Y 0.0f
//10000毫秒
#define BARRAGE_AGE 10000
#define BARRMSGTOP 100.0f
#define BARRMSGBOTTOM 80.0f

#define SPECIAL_FONT_SIZE 48
#define SPECIAL_CENTER_HEIGHT 138
#define SPECIAL_CENTER_FONTH (SPECIAL_CENTER_HEIGHT-10)
#define SPECIAL_FONT_STAR_SPACE 20
#define SPECIAL_BANNER_EXTRA 50
//special text
//目标从右边移入,左边移出
#define SPECIAL_MOVE_TOCENTER 500
//目标文字渐现
#define SPECIAL_CHANGE_APPEAR 500
//目标保持停留
#define SPECIAL_KEEP_STAY 5000
//目标文字渐隐
#define SPECIAL_CHANGE_DISAPPEAR 500
#define SPECIAL_MOVE_TOLEFT 200

//门店三甲信息展示
#define SHOPTOP_DST_Y 100.0f
#define SHOPTOP_IN 1000
#define SHOPTOP_KEEP 5000
#define SHOPTOP_OUT 1000

//包厢金币信息
#define ROOM_MONEY_X 50
#define ROOM_MONEY_Y 50
#define ROOM_MONEYNOTIFY_X 100
#define ROOM_MONEYNOTIFY_Y 100
#define MONEY_NOTIFY_CYCLEMS 2000

//气泡
#define BUBBLE_PADDING 20
#define BUBBLE_TXTID_SPACE 0.8f
#define BUBBLE_TXTID_SIZE 0.7f
#define BUBBLE_TXTNAME_SPACE 0.75f
#define BUBBLE_TXTLOCATION_SPACE 0.8f
#define BUBBLE_TXTLOCATION_SIZE 0.6f

//设定一屏幕最大数量的气泡
#define CONFIG_BARRAGE_BUBBLE_MAX 3

#define SCAN_WIDTH 320
#define SCAN_TOP 280

COS_HeadMessage::COS_HeadMessage(bool& bInit,ISceneObject* pObj,const std::wstring cur_song,const std::wstring next_song)
:IComRender(pObj)
,m_gMoneyAlpha(MONEY_NOTIFY_CYCLEMS)
,m_pFontR(NULL)
,m_pFontL(NULL)
,m_pFontBarrage(NULL)
,m_pFontSpecial(NULL)
,m_nleft_total_ms(0)
,m_nleft_last_ms(0)
,m_nright_last_ms(0)
,m_nright_total_ms(0)
,m_bNeedCreateAgain(false)
,m_nbarrage_last_ms(0)
,m_nbarrate_total_ms(0)
,m_nspecial_last_ms(0)
,m_ImgStar(NULL)
,m_ImgBanner(NULL)
,m_fSpeed(0.0f)
,m_fSpeedOut(0.0f)
,m_bVaild(false)
,m_ImgBubbleL()
,m_ImgBubbleM()
,m_ImgBubbleR()
,m_pFontBarrageBubble(NULL)
,m_ImgIconShopTop(NULL)
,m_ImgBubbleSTL(NULL)
,m_ImgBubbleSTM(NULL)
,m_ImgBubbleSTR(NULL)
,m_ImgBgST(NULL)
,m_ImgAppDownload(NULL)
,m_ImgEnterRoom(NULL)
{
	bInit = true;
	assert(cls_gl != NULL);
	m_cur_songName = L"App下载扫描进包厢! 当前播放:"+cur_song;
	m_pFontR = new CGLFont(30,L"zheadr");
	m_pFontL = new CGLFont(30,L"zheadl");
	m_pFontBarrage = new CGLFont(cls_gl->m_pSceneMgr->m_gWaveConfig.CONFIG_BARRAGEF_SIZE,L"zibarrage");
	m_pFontBarrageBubble = new CGLFont(cls_gl->m_pSceneMgr->m_gWaveConfig.CONFIG_BARRAGEF_SIZE,L"zbarragebubble");
	m_pFontSpecial = new CGLFont(SPECIAL_FONT_SIZE,L"zspecial");

	if (m_pFontR->addFont(m_cur_songName))
		m_pFontR->loadFont(cls_gl->m_winSize,true,FONT_BANDING);

	Imageset* imgset = cls_gl->m_pImgsetMgr-> getImageset(L"renderer");
	m_ImgStar = imgset->getImage(L"RankStar");
	m_ImgBanner = imgset->getImage(L"RankBanner");
	if (!m_ImgStar || !m_ImgBanner)
	{
		WLOGH_ERRO << L"rank info img miss";
		bInit = false;
	}

	m_ImgBubbleL = imgset->getImage(L"BubbleL");
	m_ImgBubbleM = imgset->getImage(L"BubbleM");
	m_ImgBubbleR = imgset->getImage(L"BubbleR");
	if(!m_ImgBubbleL || !m_ImgBubbleM || !m_ImgBubbleL)
	{
		WLOGH_ERRO << L"bubble image miss";
		bInit = false;
	}

	if (!m_pFontL || !m_pFontR || !m_pFontBarrage || !m_pFontBarrageBubble || !m_pFontSpecial)
	{
		WLOGH_ERRO << L"font new failed";
		bInit = false;
	}

	m_pFontSpecial->setFontExtra(true,true);
	m_pFontBarrage->setBufferSize(1000);
	m_pFontSpecial->setBufferSize(400);
	m_pFontBarrageBubble->setBufferSize(2000);

	m_ImgIconShopTop = imgset->getImage(L"DailyRankingPic");
	m_ImgBubbleSTL = imgset->getImage(L"DailyRankingBG1L");
	m_ImgBubbleSTM = imgset->getImage(L"DailyRankingBG1M");
	m_ImgBubbleSTR = imgset->getImage(L"DailyRankingBG1R");
	m_ImgBgST = imgset->getImage(L"DailyRankingBG2");

	if(!m_ImgIconShopTop || !m_ImgBubbleSTL || !m_ImgBubbleSTM || !m_ImgBubbleSTR || !m_ImgBgST)
	{
		WLOGH_ERRO << L"shop top image miss";
		bInit = false;
	}

	CFloridMgr* floridMgr = cls_gl->m_pFloridMgr;
	m_FloridStar = floridMgr->getFlorid("specialstar");
	if (!m_FloridStar)
	{
		WLOGH_ERRO << L"florid star miss";
		bInit = false;
	}

	m_ImgAppDownload = cls_gl->m_pImgsetMgr->getImage(L"DownloadApp",L"DownloadApp");
	m_ImgEnterRoom = cls_gl->m_pImgsetMgr->getImage(L"EnterRoom",L"EnterRoom");
}

COS_HeadMessage::~COS_HeadMessage()
{
	SAFE_DELETE(m_pFontSpecial);
	SAFE_DELETE(m_pFontBarrageBubble);
	SAFE_DELETE(m_pFontBarrage);
	SAFE_DELETE(m_pFontL);
	SAFE_DELETE(m_pFontR);
}

void COS_HeadMessage::updateRightStr(const std::string next_song,bool b)
{
	std::wstring wstr;
	Str2Wstr(next_song,wstr);
	updateRightStr(wstr,b);
}

void COS_HeadMessage::updateRightStr(const std::wstring next_song,bool b)
{
	if (next_song == L"ktv1")
	{
		m_cur_songName = L"当前未点歌曲";
		m_next_songName = L"";
	}
	else if (next_song == L"ktv2")
	{
		m_next_songName = L"当前歌曲为最后一首";
	}
	else
	{
		m_next_songName = L"下一首:"+next_song;
	}

	if (b)
		m_bNeedCreateAgain = true;
	m_rightfuture_wstr = m_cur_songName + L";  "+m_next_songName;
}

bool COS_HeadMessage::upateBarrageStr(const BarrageInfo& info)
{

	if(m_listBarrageMsg.size() >= CONFIG_BARRAGE_BUBBLE_MAX)
		return false;

	std::string fontStr;//统计弹幕中所包含的文字
	int txtWidth = 0;//统计文字宽度
	int picWidth = 0;//统计图片宽度
	//glp
	static int s_bubble_height = m_ImgBubbleL->getHeight()+30;//信息高度 默认气泡的高度

	//解析xml
	TiXmlDocument doc;
	doc.Parse(info.str,NULL,TIXML_ENCODING_LEGACY);
	TiXmlElement* pElement = doc.FirstChildElement();
	do 
	{
		if(!pElement)
			break;
		if(strcmp(pElement->Value(),"txt") == 0)
		{
			const char* pTxt = pElement->Attribute("txt");
			fontStr += (pTxt?pTxt:"");
		}
		else if(strcmp(pElement->Value(),"space") == 0)
		{
			const char* pSpace = pElement->Attribute("width");//解析空格
			int nSpace = atoi(pSpace?pSpace:"");
			txtWidth += nSpace;
		}
		else if(strcmp(pElement->Value(),"img") == 0)
		{
			const char* pSpace = pElement->Attribute("space");//解析图片两边的空隙
			int nSpace = atoi(pSpace?pSpace:"");
			const char* pSrc = pElement->Attribute("src");//图片所使用的纹理图集和纹理名字。
			std::string imgName = pSrc?pSrc:"";
			RenderEImage* pImg = NULL;
			//查找单个纹理
			Imageset* pImgset = cls_gl->m_pImgsetMgr->getImageset(imgName);
			if(pImgset)
				pImg = pImgset->getImage(imgName);
			else
			{
				//查找主纹理
				pImgset = cls_gl->m_pImgsetMgr->getImageset(L"renderer");
				pImg = pImgset?pImgset->getImage(imgName):NULL;
			}
			picWidth += (nSpace + (pImg?pImg->getWidth():0));
		}
	} while (pElement = pElement->NextSiblingElement());

	wchar_t sSayId[260] = {0};
	std::wstring wstr;
	switch(info.saytype)
	{
	case 0://普通显示用户名ID
		{
			swprintf(sSayId,L"(%d)",info.sayid);
			wstr =std::wstring(sSayId)+info.sayname+info.location;
			if(m_pFontBarrage->addFont(wstr))
				m_pFontBarrage->loadFont(cls_gl->m_winSize,true,FONT_BANDING);
			break;
		}
	case 1://天天抢榜
		break;
	}

	Str2Wstr(fontStr,wstr);
	if(m_pFontBarrageBubble->addFont(wstr))
		m_pFontBarrageBubble->loadFont(cls_gl->m_winSize,true);

	pElement = doc.FirstChildElement();
	do 
	{
		if(!pElement)
			break;
		if(strcmp(pElement->Value(),"txt") == 0)
		{
			const char* pSpace = pElement->Attribute("space");
			float fSpace = (float)atof(pSpace?pSpace:"1.0");
			const char* pScale = pElement->Attribute("size");
			float fScale = (float)atof(pScale?pScale:"1.0");
			const char* pStr = pElement->Attribute("txt");
			txtWidth += m_pFontBarrageBubble->getTextWidth(pStr?pStr:"",fSpace,fScale);
		}
	} while (pElement = pElement->NextSiblingElement());


	float tmp_y = BARRMSGTOP;
	LISTBARRAGEMSG::iterator i = m_listBarrageMsg.begin();
	for(i;i!=m_listBarrageMsg.end();i++)
	{
		if (!(i->isfull))
		{
			float i_y = i->cur_y+i->height;
			tmp_y = i_y;
		}
	}
	static float s_font_bottom = cls_gl->m_winSize.height - BARRMSGBOTTOM;
	tmp_y = tmp_y > s_font_bottom?BARRMSGTOP:tmp_y;

	BarrageMessage tmp;
	tmp.saytype = info.saytype;
	tmp.sayid = sSayId;
	tmp.sayname = info.sayname;
	int nIdLen = 0;
	int nNameLen = 0;

	switch(tmp.saytype)
	{
	case 0://普通
		{
			nNameLen = m_pFontBarrage->getTextWidth(tmp.sayname,BUBBLE_TXTNAME_SPACE);
			nIdLen = m_pFontBarrage->getTextWidth(tmp.sayid,BUBBLE_TXTID_SPACE,0.7f);
			break;
		}
	case 1://天天抢榜
		{
			nNameLen = m_ImgIconShopTop->getWidth();
			break;
		}
	}
	tmp.isnamelonger = nNameLen>nIdLen;
	tmp.diffwidth = (int)fabs((float)nNameLen-nIdLen );

	tmp.location = info.location;
	tmp.islocation = !tmp.location.empty();
	if(tmp.islocation)
		tmp.locationwidth = m_pFontBarrage->getTextWidth(tmp.location,BUBBLE_TXTLOCATION_SPACE,BUBBLE_TXTLOCATION_SIZE);

	tmp.mode = info.mode;
	tmp.message = info.str;
	tmp.height = (float)s_bubble_height;
	tmp.width = txtWidth+picWidth;
	tmp.display_width = tmp.width + (tmp.isnamelonger?nNameLen:nIdLen)+BUBBLE_PADDING*2;
	tmp.speed = tmp.mode ? 0 : (1.0f*(cls_gl->m_winSize.width + tmp.width) / BARRAGE_AGE);
	switch(tmp.mode)
	{
	case  0:
		{
			tmp.cur_x = (float)cls_gl->m_winSize.width;
			tmp.cur_y = tmp_y;
			break;
		}
	case 1:
		{
			tmp.cur_x = (cls_gl->m_winSize.width-tmp.width)/2.0f;
			tmp.cur_y = BARRMSGTOP;
			break;
		}
	case 2:
		{
			tmp.cur_x = (cls_gl->m_winSize.width-tmp.width)/2.0f;
			tmp.cur_y = s_font_bottom;
			break;
		}
	}

	tmp.isfull = false;
	tmp.isold = false;
	tmp.age = 0;

	m_listBarrageMsg.push_back(tmp);
	return true;
}

void COS_HeadMessage::updateCenterSpecial(SpecialMessage& sm)
{
	//统计一下需要用到的字符串资源
	std::string fontStr;
	//统计一下该显示内容的最大宽度
	int maxWidth = 0;
	//统计一下该显示内容的最大高度
	int maxHeight = 0;
	
	std::vector<int> vectLineWidth;

	TiXmlDocument doc;
	doc.Parse(sm.msg.c_str(),NULL,TIXML_ENCODING_LEGACY);
	TiXmlElement* pChild = NULL;
	TiXmlElement* pElement = NULL;

	{
		pElement = doc.FirstChildElement();//<line>
		
		int nLine = 0;
		do 
		{
			if(!pElement)
				return ;

			const char* pIndentation = pElement->Attribute("indentation");
			int indentation = atoi(pIndentation?pIndentation:"");
			vectLineWidth.push_back(indentation);//插入对于的行缩进

			pChild = pElement->FirstChildElement();//<txt>

			int cur_txt_size = 0;
			do 
			{
				if (!pChild)
					break;
				if(strcmp(pChild->Value(),"txt") == 0)
				{
					const char* pTxt = pChild->Attribute("txt");
					std::string str = pTxt?pTxt:"";
					fontStr += str;
				}
				else if(strcmp(pChild->Value(),"space") == 0)
				{
					const char* pSpace = pChild->Attribute("width");//解析空格
					int nSpace = atoi(pSpace?pSpace:"");
					vectLineWidth[nLine] += nSpace;
				}
				else if(strcmp(pChild->Value(),"img") == 0)
				{
					const char* pSpace = pChild->Attribute("space");//解析图片两边的空隙
					int nSpace = atoi(pSpace?pSpace:"");
					const char* pSrc = pChild->Attribute("src");//图片所使用的纹理图集和纹理名字。
					std::string imgName = pSrc?pSrc:"";
					RenderEImage* pImg = NULL;
					//查找单个纹理
					Imageset* pImgset = cls_gl->m_pImgsetMgr->getImageset(imgName);
					if(pImgset)
						pImg = pImgset->getImage(imgName);
					else
					{
						//查找主纹理
						pImgset = cls_gl->m_pImgsetMgr->getImageset(L"renderer");
						pImg = pImgset?pImgset->getImage(imgName):NULL;
					}
					vectLineWidth[nLine] += (nSpace + (pImg?pImg->getWidth():0));
				}
			} while (pChild = pChild->NextSiblingElement());

			nLine ++;
		} while (pElement = pElement->NextSiblingElement());//<line>
	}

	CGLFont* font = NULL;
	std::wstring wstr;
	Str2Wstr(fontStr,wstr);
	//加载字符串资源
	if(sm.type == 0)
	{
		font = m_pFontSpecial;
		if (font &&font->addFont(wstr))
			font->loadFont(cls_gl->m_winSize,true,FONT_BANDING);
	}
	else if(sm.type == 1)
	{
		font = m_pFontBarrageBubble;
		if (font &&font->addFont(wstr))
			font->loadFont(cls_gl->m_winSize,true);
	}

	{
		pElement = doc.FirstChildElement();//<line>

		int nLine = 0;
		int maxLineWidth = 0;

		do 
		{
			if(!pElement)
				return ;

			const char* pYOffset = pElement->Attribute("yoffset");
			int yoffset = atoi(pYOffset?pYOffset:"");
			//maxHeight += yoffset;

			pChild = pElement->FirstChildElement();//<txt>

			int single_line_height = 0;
			int cur_txt_size = 0;
			do 
			{
				if (!pChild)
					break;
				if(strcmp(pChild->Value(),"txt") == 0)
				{
					const char* pSpace = pChild->Attribute("space");
					float fSpace = (float)atof(pSpace?pSpace:"1.0");
					const char* pSize = pChild->Attribute("size");
					float fScale = (float)atof(pSize?pSize:"1.0");
					const char* pTxt = pChild->Attribute("txt");
					std::string strTxt = pTxt?pTxt:"";
					vectLineWidth[nLine] += font->getTextWidth(strTxt,fSpace,fScale);
					int height = font->getTextHeight(strTxt,fScale);
					if(single_line_height < height)
						single_line_height = height;
				}
				else if(strcmp(pChild->Value(),"img") == 0)
				{
					const char* pSrc = pChild->Attribute("src");//图片所使用的纹理图集和纹理名字。
					std::string imgName = pSrc?pSrc:"";
					RenderEImage* pImg = NULL;
					//查找单个纹理
					Imageset* pImgset = cls_gl->m_pImgsetMgr->getImageset(imgName);
					if(pImgset)
						pImg = pImgset->getImage(imgName);
					else
					{
						//查找主纹理
						pImgset = cls_gl->m_pImgsetMgr->getImageset(L"renderer");
						pImg = pImgset?pImgset->getImage(imgName):NULL;
					}
					int nImgHeight = pImg?pImg->getWidth()/2:0;
					if(single_line_height < nImgHeight)
						single_line_height = nImgHeight;
				}
			} while (pChild = pChild->NextSiblingElement());

			//解析一行结束
			if(maxWidth < vectLineWidth[nLine])
				maxWidth = vectLineWidth[nLine];
			maxHeight += single_line_height;
			nLine ++;
		} while (pElement = pElement->NextSiblingElement());//<line>
	}

	sm.txt_width = maxWidth;
	sm.txt_height = maxHeight;
	sm.life = 0;
	m_listSpecialMsg.push_back(sm);
}

void COS_HeadMessage::updateLeftStr(const std::wstring wstr)
{
	if (!m_pFontL)
		return ;

	MessageTime tmp;
	tmp.message = wstr;
	int nTextWidth = m_pFontL->getTextWidth(wstr,0.8f);
	tmp.duration_ms = ulong((CONFIG_PIXEL_WIDTH+nTextWidth)/LEFT_PIXEL_PERTIME);
	m_listMessage.push_back(tmp);
}

void COS_HeadMessage::drawBarrageMessage()
{
	RenderEImage* bubbleImgL = NULL;
	RenderEImage* bubbleImgM = NULL;
	RenderEImage* bubbleImgR = NULL;
	if (m_pFontBarrage&&m_pFontBarrageBubble)
	{
		LISTBARRAGEMSG::iterator i = m_listBarrageMsg.begin();
		for(i;i!=m_listBarrageMsg.end();i++)
		{
			BarrageMessage& bm = *i;

			float bubble_x = bm.cur_x;

			switch(bm.saytype)
			{
			case 0:
				{
					if(bm.isnamelonger)
					{
						bubble_x += m_pFontBarrage->drawText(bm.sayname,GPoint(bm.cur_x,bm.cur_y,GL3D_Z_BARRAGE)
							,NULL,WHITE_RECT,BUBBLE_TXTNAME_SPACE);
						m_pFontBarrage->drawText(bm.sayid,GPoint(bm.cur_x+bm.diffwidth,bm.cur_y+40,GL3D_Z_BARRAGE)
							,NULL,WHITE_RECT,BUBBLE_TXTID_SPACE,BUBBLE_TXTID_SIZE);
					}
					else
					{
						m_pFontBarrage->drawText(bm.sayname,GPoint(bm.cur_x+bm.diffwidth,bm.cur_y,GL3D_Z_BARRAGE)
							,NULL,WHITE_RECT,BUBBLE_TXTNAME_SPACE);
						bubble_x += m_pFontBarrage->drawText(bm.sayid,GPoint(bm.cur_x,bm.cur_y+40,GL3D_Z_BARRAGE)
							,NULL,WHITE_RECT,BUBBLE_TXTID_SPACE,BUBBLE_TXTID_SIZE);
					}

					bubbleImgL = m_ImgBubbleL;
					bubbleImgM = m_ImgBubbleM;
					bubbleImgR = m_ImgBubbleR;
					break;
				}
			case 1:
				{
					m_ImgIconShopTop->draw(CGPointMake(bm.cur_x+30,bm.cur_y-15,GL3D_Z_BARRAGE));
					bubble_x += m_ImgIconShopTop->getWidth();

					bubbleImgL = m_ImgBubbleSTL;
					bubbleImgM = m_ImgBubbleSTM;
					bubbleImgR = m_ImgBubbleSTR;
					break;
				}
			}

			int s_bubble_height = bubbleImgL->getHeight();
			int s_bubble_height_2 = s_bubble_height/2;
			int s_bubble_width_l = bubbleImgL->getWidth();
			int s_bubble_width_m = bubbleImgM->getWidth();
			int s_bubble_width_r = bubbleImgR->getWidth();
			int s_bubble_trim = s_bubble_width_l+s_bubble_width_r;
			int s_bubble_width = s_bubble_width_l+s_bubble_width_m+s_bubble_width_r;

			int bubblesize = bm.width + BUBBLE_PADDING*2;
			bubble_x+= BUBBLE_PADDING;
			float bubble_end_x = bubble_x;
			GPoint bubble_point(bubble_x,bm.cur_y,GL3D_Z_BARRAGE);
			if(bubblesize > s_bubble_width)
			{
				bubbleImgL->draw(bubble_point);
				bubble_point.x += s_bubble_width_l;
				int bubble_w = bubblesize - s_bubble_trim;
				bubbleImgM->draw(bubble_point,GSize(bubble_w,s_bubble_height));
				bubble_point.x += bubble_w;
				bubbleImgR->draw(bubble_point);
				bubble_x+= BUBBLE_PADDING;
				bubble_end_x += bubblesize;
			}
			else
			{
				bubbleImgL->draw(bubble_point);
				bubble_point.x += s_bubble_width_l;
				bubbleImgM->draw(bubble_point);
				bubble_point.x += s_bubble_width_m;
				bubbleImgR->draw(bubble_point);
				bubble_x+= (s_bubble_width-bm.width)/2.0f;
				bubble_end_x += s_bubble_width;
			}

			TiXmlDocument doc;
			doc.Parse(bm.message.c_str(),0,TIXML_ENCODING_LEGACY);
			TiXmlElement* pElement = doc.FirstChildElement();
			do 
			{
				if(!pElement)
					break;
				if(strcmp(pElement->Value(),"txt") == 0)
				{
					const char* pTxt = pElement->Attribute("txt");
					if(!pTxt)
						continue;

					GColor color;
					const char* pColor = NULL;
					if((pColor=pElement->Attribute("color")) != NULL)
						sscanf(pColor,"%x",&color.c);
					float fScale = 1.0;
					const char* pSize = NULL;
					if((pSize=pElement->Attribute("size")) != NULL)
						fScale = (float)atof(pSize);
					const char* pSpace = pElement->Attribute("space");
					float fSpace = (float)atof(pSpace?pSpace:"1.0");
					bubble_x += m_pFontBarrageBubble->drawText(pTxt,GPoint(bubble_x,bm.cur_y+s_bubble_height_2-2,GL3D_Z_BARRAGE),NULL,color,fSpace,fScale,true);
				}
				else if(strcmp(pElement->Value(),"space") == 0)
				{
					const char* pSpace = pElement->Attribute("width");//解析空格
					int nSpace = atoi(pSpace?pSpace:"");
					bubble_x += nSpace;
				}
				else if(strcmp(pElement->Value(),"img") == 0)
				{
					const char* pSpace = pElement->Attribute("space");//与前面文字的空隙，没有该属性则默认为0
					int nSpace = atoi(pSpace?pSpace:"");
					std::string imgName = pElement->Attribute("src");//图片所使用的纹理图集和纹理名字。
					RenderEImage* pImg = NULL;
					Imageset* pImgset = cls_gl->m_pImgsetMgr->getImageset(imgName);
					if(pImgset)
						pImg = pImgset->getImage(imgName);
					else
					{
						//查找主纹理
						pImgset = cls_gl->m_pImgsetMgr->getImageset(L"renderer");
						pImg = pImgset?pImgset->getImage(imgName):NULL;
					}
// #ifdef SPECIAL_VERSION
// 					if(pImgset)
// 						pImgset->lockBatchBuffer();
// #endif
					if(pImg)
						pImg->draw(GPoint(bubble_x+nSpace/2.0f,bm.cur_y+s_bubble_height_2-pImg->getHeight(),GL3D_Z_BARRAGE));
// #ifdef SPECIAL_VERSION
// 					if(pImgset)
// 						pImgset->unlockBatchBuffer();
// #endif
					bubble_x += (nSpace + (pImg?pImg->getWidth():0));
				}
			} while (pElement = pElement->NextSiblingElement());

			if(bm.islocation)
				m_pFontBarrage->drawText(bm.location,GPoint(bubble_end_x-bm.locationwidth,bm.cur_y+s_bubble_height+5,GL3D_Z_BARRAGE)
				,NULL,WHITE_RECT,BUBBLE_TXTLOCATION_SPACE,BUBBLE_TXTLOCATION_SIZE);
		}
	}
}

void COS_HeadMessage::drawSpecialText(const SpecialMessage& sm,float cur_x,float cur_y,float cur_alpha)
{
	CGLFont* font = NULL;
	if(sm.type == 0)
		font = m_pFontSpecial;
	else if(sm.type == 1)
		font =  m_pFontBarrageBubble;

	if(!font)
		return ;

	GColor common_color(1.0f,1.0f,1.0f,cur_alpha);

	TiXmlDocument doc;
	doc.Parse(sm.msg.c_str(),NULL,TIXML_ENCODING_LEGACY);
	TiXmlElement* pChild = NULL;
	TiXmlElement* pElement = doc.FirstChildElement();//<line>

	float tmp_y = cur_y;
	do 
	{
		if(!pElement)
			return ;

		const char* pIntendation = pElement->Attribute("indentation");
		int indentation = atoi(pIntendation?pIntendation:"");
		//取出Y偏移
		const char* pYOffset = pElement->Attribute("yoffset");
		int yoffset = atoi(pYOffset?pYOffset:"");
		tmp_y += yoffset;
		//增加缩进
		float tmp_x = cur_x+indentation;
		int max_single_height = 0;
		pChild = pElement->FirstChildElement();//<txt>
		do 
		{
			if (!pChild)
				break;
			if(strcmp(pChild->Value(),"txt") == 0)
			{
				//取出颜色
				unsigned int cur_color = 0xffffffff;
				const char* pColor = pChild->Attribute("color");
				if(pColor)
					sscanf(pColor,"%x",&cur_color);

				//取出字符串大小
				const char* pSize = pChild->Attribute("size");
				float fScale = (float)atof(pSize?pSize:"1.0");
				//取出字间距
				const char* pSpace = pChild->Attribute("space");
				float fSpace = 1.0f;
				if (pSpace)
					fSpace = (float)atof(pSpace);
				//取出字符串
				const char* pTxt = pChild->Attribute("txt");
				std::string str = pTxt?pTxt:"";
				//字符串高度
				int nHeight = font->getTextHeight(str,fScale);
				if(max_single_height < nHeight)
					max_single_height = nHeight;
				GPoint dest_point = CGPointMake( tmp_x,(float)tmp_y-nHeight/2.0f,GL3D_Z_NOTIFY );
				unsigned char alpha = (unsigned char)(cur_alpha*255);
				GColor mix_color = (cur_color&0xffffff) | (alpha<<24);
				tmp_x += font->drawText(str,dest_point,NULL,mix_color,fSpace,fScale);
			}
			else if(strcmp(pChild->Value(),"space") == 0)
			{
				const char* pSpace = pChild->Attribute("width");//解析空格
				int nSpace = atoi(pSpace?pSpace:"");
				tmp_x += nSpace;
			}
			else if(strcmp(pChild->Value(),"img") == 0)
			{
				const char* pSpace = pChild->Attribute("space");//解析图片两边的空隙
				int nSpace = atoi(pSpace?pSpace:"");
				std::string imgName = pChild->Attribute("src");//图片所使用的纹理图集和纹理名字。
				RenderEImage* pImg = NULL;
				//查找单个纹理
				Imageset* pImgset = cls_gl->m_pImgsetMgr->getImageset(imgName);
				if(pImgset)
					pImg = pImgset->getImage(imgName);
				else
				{
					//查找主纹理
					pImgset = cls_gl->m_pImgsetMgr->getImageset(L"renderer");
					pImg = pImgset?pImgset->getImage(imgName):NULL;
				}
				if(pImg)
				{
					int nImgHeight_2 = pImg->getHeight()/2;
					pImg->draw(GPoint(tmp_x+nSpace/2.0f,tmp_y-nImgHeight_2,GL3D_Z_NOTIFY),NULL,true,common_color);
					if(max_single_height < nImgHeight_2)
						max_single_height = nImgHeight_2;
				}
				tmp_x += (nSpace + (pImg?pImg->getWidth():0));
			}
		} while (pChild = pChild->NextSiblingElement());

		tmp_y += max_single_height;
	} while (pElement = pElement->NextSiblingElement());//<line>
}
void COS_HeadMessage::drawShopTop(const SpecialMessage& sm,float x,float y,float alpha)
{
	alpha = alpha>1.0f?1.0f:(alpha<0.0f?0.0f:alpha);
	GColor common_color(1.0f,1.0f,1.0f,alpha);
	m_ImgBgST->draw(GPoint(x,y,GL3D_Z_NOTIFY),GSize(sm.txt_width,sm.txt_height),NULL,true,common_color);

	drawSpecialText(sm,x,y,alpha);
}
void COS_HeadMessage::drawSpecialMessage()
{
	if (!m_pFontSpecial)
		return ;

	if (m_listSpecialMsg.empty())
		return ;
	
	static int s_star_width = m_ImgStar->getWidth();
	SpecialMessage& sm = m_listSpecialMsg.front();

	if (sm.type == 0)//特效通知 1
	{
		static int s_star_width_2 = s_star_width/2;
		static int s_star_height_2 = m_ImgStar->getHeight()/2;
		static float s_star_y = (float)SPECIAL_CENTER_HEIGHT - s_star_height_2;
		static int s_banner_height = m_ImgBanner->getHeight();
		static float s_banner_y = (float)SPECIAL_CENTER_HEIGHT - s_banner_height;

		static unsigned int nAppear = SPECIAL_MOVE_TOCENTER+SPECIAL_CHANGE_APPEAR;
		static unsigned int nDisappear = SPECIAL_MOVE_TOCENTER+SPECIAL_CHANGE_APPEAR+SPECIAL_KEEP_STAY+SPECIAL_CHANGE_DISAPPEAR;

		float star_dest_x = (cls_gl->m_winSize.width-s_star_width-sm.txt_width)/2.0f;
		if (m_nspecial_last_ms<SPECIAL_MOVE_TOCENTER)//目标向右移动阶段
		{
			float x = cls_gl->m_winSize.width - m_fSpeed*m_nspecial_last_ms;
			m_ImgBanner->draw(GPoint(x+s_star_width_2,s_banner_y,GL3D_Z_NOTIFY)
				,GSize(s_star_width_2+sm.txt_width+SPECIAL_BANNER_EXTRA,s_banner_height*2));
			m_ImgStar->draw(GPoint(x,s_star_y,GL3D_Z_NOTIFY));
		}
		else if (m_nspecial_last_ms<nAppear)//目标文字渐现
		{
			m_ImgBanner->draw(GPoint(star_dest_x+s_star_width_2,s_banner_y,GL3D_Z_NOTIFY)
				,GSize(s_star_width_2+sm.txt_width+SPECIAL_BANNER_EXTRA,s_banner_height*2));
			m_ImgStar->draw(GPoint(star_dest_x,s_star_y,GL3D_Z_NOTIFY));

			float fAlpha = 1.0f-1.0f*(nAppear-m_nspecial_last_ms)/SPECIAL_CHANGE_DISAPPEAR;
			drawSpecialText(sm,star_dest_x+s_star_width+SPECIAL_FONT_STAR_SPACE,SPECIAL_CENTER_FONTH,fAlpha);
		}
		else if (m_nspecial_last_ms<(nAppear+SPECIAL_KEEP_STAY))//目标保持
		{
			m_ImgBanner->draw(GPoint(star_dest_x+s_star_width_2,s_banner_y,GL3D_Z_NOTIFY)
				,GSize(s_star_width_2+sm.txt_width+SPECIAL_BANNER_EXTRA,s_banner_height*2));
			m_ImgStar->draw(GPoint(star_dest_x,s_star_y,GL3D_Z_NOTIFY));

			drawSpecialText(sm,star_dest_x+s_star_width+SPECIAL_FONT_STAR_SPACE,SPECIAL_CENTER_FONTH,1.0f);
		}
		else if (m_nspecial_last_ms<nDisappear)//目标文字渐隐
		{
			m_ImgBanner->draw(GPoint(star_dest_x+s_star_width_2,s_banner_y,GL3D_Z_NOTIFY)
				,GSize(s_star_width_2+sm.txt_width+SPECIAL_BANNER_EXTRA,s_banner_height*2));
			m_ImgStar->draw(GPoint(star_dest_x,s_star_y,GL3D_Z_NOTIFY));

			float fAlpha = 1.0f*(nDisappear-m_nspecial_last_ms)/SPECIAL_CHANGE_DISAPPEAR;
			drawSpecialText(sm,star_dest_x+s_star_width+SPECIAL_FONT_STAR_SPACE,SPECIAL_CENTER_FONTH,fAlpha);
		}
		else if (m_nspecial_last_ms<(nDisappear+SPECIAL_MOVE_TOLEFT))//目标向右移出
		{
			float x = star_dest_x - m_fSpeedOut*(m_nspecial_last_ms-nDisappear);
			m_ImgBanner->draw(GPoint(x+s_star_width_2,s_banner_y,GL3D_Z_NOTIFY)
				,GSize(s_star_width_2+sm.txt_width+SPECIAL_BANNER_EXTRA,s_banner_height*2));
			m_ImgStar->draw(GPoint(x,s_star_y,GL3D_Z_NOTIFY));
		}
	}
	else if(sm.type == 1)
	{
		sm.life += cls_gl->m_pDrawListener->m_cur_time_delta_ms;
		if(sm.life < SHOPTOP_IN)
		{
			float alpha = 1.0f*sm.life/SHOPTOP_IN;
			float y = -sm.txt_height + alpha*sm.txt_height+SHOPTOP_DST_Y;
			drawShopTop(sm,cls_gl->m_winSize.width-sm.txt_width-50.0f,y,alpha);
		}
		else if(sm.life < SHOPTOP_IN+SHOPTOP_KEEP)
		{
			drawShopTop(sm,cls_gl->m_winSize.width-sm.txt_width-50.0f,SHOPTOP_DST_Y,1.0f);
		}
		else if(sm.life < SHOPTOP_IN+SHOPTOP_KEEP+SHOPTOP_OUT)
		{
			float alpha = 1.0f-1.0f*(sm.life-(SHOPTOP_IN+SHOPTOP_KEEP))/SHOPTOP_OUT;
			drawShopTop(sm,cls_gl->m_winSize.width-sm.txt_width-50.0f,SHOPTOP_DST_Y,alpha);
		}
		else
		{
			m_listSpecialMsg.pop_front();
			if (m_listSpecialMsg.empty())
				return ;
		}
	}
}


void COS_HeadMessage::drawRoomMoney(const ulong delta_ms)
{
	unsigned int cur_money;
	{
		CAutoLock lock(&cls_gl->m_csMoney);
		cur_money = cls_gl->m_nMoney;
	}

	CGLFont* pFont = cls_gl->m_pSceneMgr->m_pGLFont;
	if(!pFont)
		return ;
	//pFont->lockBuffer();
	static GPoint dest_info = CGPointMake(ROOM_MONEY_X,ROOM_MONEY_Y,GL3D_Z_NOTIFY);
	float x = pFont->drawText(L"当前包厢金币",dest_info,NULL,WHITE_RECT,0.8f)+50;
	wchar_t buf[260] = {0};
	swprintf(buf,L"%d",cur_money);
	pFont->drawText(std::wstring(buf),GPoint(ROOM_MONEY_X+x,ROOM_MONEY_Y,10.0f)
		,NULL,GColor(0xffffdd10),0.8f);

	bool bNotify = cur_money > cls_gl->m_pSceneMgr->CONFIG_MONEYTOTWINKLE;
	if(!bNotify)
	{
		//pFont->unlockBuffer();
		return ;
	}

	float fAlpha = m_gMoneyAlpha.getAlpha();//m_gMoneyAlpha.update(delta_ms);
	static GPoint dest_money = CGPointMake(ROOM_MONEYNOTIFY_X,ROOM_MONEYNOTIFY_Y,GL3D_Z_NOTIFY);
	GColor cur_color(fAlpha,0x00ffdd10);
	pFont->drawText(WSTR_MONEYEXCHANGE_NOTIFY,dest_money,NULL,cur_color,0.9f);
	//pFont->unlockBuffer();
}

void COS_HeadMessage::drawMessageRight()
{
	if (m_pFontR)
	{
		//定义右边300像素显示
		static CGRect show_clip_rect = CGRectMake(float(cls_gl->m_winSize.width-200),HEADMESSAGE_Y,GL3D_Z_NOTIFY,200,40);
		static CGRect alpha_clip_rect = CGRectMake(float(cls_gl->m_winSize.width-300),HEADMESSAGE_Y,GL3D_Z_NOTIFY,101,40);
		static GColorRect color_rect_alpha = GColorRect(0x00FFFFFF,0x00FFFFFF,0xFFFFFFFF,0xFFFFFFFF);
		float current_x = cls_gl->m_winSize.width - m_nright_last_ms*LEFT_PIXEL_PERTIME;
		m_pFontR->drawTextAlpha(m_rightcurrent_wstr,GPoint(current_x,HEADMESSAGE_Y,GL3D_Z_NOTIFY),&alpha_clip_rect,true,color_rect_alpha/*,0.9f*/);
		m_pFontR->drawText(m_rightcurrent_wstr,GPoint(current_x,HEADMESSAGE_Y,GL3D_Z_NOTIFY),&show_clip_rect,WHITE_RECT/*,0.9f*/);
	}
}

void	COS_HeadMessage::drawMessageLeft()
{
	if(m_nleft_last_ms >= m_nleft_total_ms)
		return ;
	if (m_pFontL)
	{
		//定义左边300像素显示
		static CGRect show_clip_rect = CGRectMake(0.0f,HEADMESSAGE_Y,GL3D_Z_NOTIFY,201,40);
		static CGRect alpha_clip_rect = CGRectMake(200.0f,HEADMESSAGE_Y,GL3D_Z_NOTIFY,100,40);
		static GColorRect color_rect_alpha = GColorRect(0xFFFFFFFF,0xFFFFFFFF,0x00FFFFFF,0x00FFFFFF);
		float current_x = 300 - m_nleft_last_ms*LEFT_PIXEL_PERTIME;
		m_pFontL->drawText(m_leftcurrent_wstr,GPoint(current_x,HEADMESSAGE_Y,GL3D_Z_NOTIFY),&show_clip_rect,WHITE_RECT,0.8f);
		m_pFontL->drawTextAlpha(m_leftcurrent_wstr,GPoint(current_x,HEADMESSAGE_Y,GL3D_Z_NOTIFY),&alpha_clip_rect,false,color_rect_alpha,0.8f);
	}
}

void COS_HeadMessage::resetTime()
{
	if (m_leftcurrent_wstr == L"")
	{
		return ;
	}
	m_nleft_last_ms = 0;
	m_nright_last_ms = 0;
}

void COS_HeadMessage::updateMessageLeft(ulong delta_ms)
{
	//一次描绘结束 update message left
	if (m_nleft_last_ms >= m_nleft_total_ms)
	{
		std::string leftStr;
		{
			//更新
			CAutoLock lock(&cls_gl->m_csLeftMsg);
			if(cls_gl->m_listLeffMsg.empty())
				return ;

			leftStr = cls_gl->m_listLeffMsg.front();
			cls_gl->m_listLeffMsg.pop_front();
		}

		std::wstring wstr;
		Str2Wstr(leftStr,wstr);
		if (m_pFontL->addFont(wstr))
			m_pFontL->loadFont(cls_gl->m_winSize,true,FONT_BANDING);
		updateLeftStr(wstr);

		if (!m_listMessage.empty())
		{
			MessageTime tmp;
			tmp = m_listMessage.front();
			m_nleft_last_ms = 0;
			m_nleft_total_ms = tmp.duration_ms;
			m_leftcurrent_wstr = tmp.message;
			m_listMessage.pop_front();
		}
		return ;
	}
	m_nleft_last_ms += delta_ms;
}

void COS_HeadMessage::updateMessageRight(ulong delta_ms)
{
	//一次描绘的结束 update message right
	if (m_nright_last_ms >= m_nright_total_ms)
	{
		m_nright_last_ms = 0;

		{
			if(cls_gl->m_bChangedNextSN)
			{
				updateRightStr(cls_gl->m_next_songName);
				cls_gl->m_bChangedNextSN = false;
			}
		}

		if (m_bNeedCreateAgain)
		{
			if (m_pFontR->addFont(m_rightfuture_wstr))
				m_pFontR->loadFont(cls_gl->m_winSize,true,FONT_BANDING);
			int w1 = m_pFontR->getTextWidth(m_cur_songName);
			int w2 = m_pFontR->getTextWidth(m_next_songName);
			int w = w1+w2;
			w += CONFIG_PIXEL_WIDTH;
			m_nright_total_ms = ulong(w/LEFT_PIXEL_PERTIME);
			m_rightcurrent_wstr = m_rightfuture_wstr;
			m_bNeedCreateAgain = false;
		}
	}
	m_nright_last_ms += delta_ms;
}
void COS_HeadMessage::updateMessageBarrage(ulong delta_ms)
{
	//update barrage message
	{
		CAutoLock lock(&cls_gl->m_csBarrage);
		LISTBARRAGEINFO::iterator it = cls_gl->m_listBarrage.begin();
		for (;it!=cls_gl->m_listBarrage.end();it++)
		{
			BarrageInfo& msg = *it;
			if(!upateBarrageStr(msg))
				break;
		}
		if(it != cls_gl->m_listBarrage.begin())
			cls_gl->m_listBarrage.erase(cls_gl->m_listBarrage.begin(),it);
	}

	if (m_pFontBarrage)
	{
		int nErase = 0;
		LISTBARRAGEMSG::iterator iFirst = m_listBarrageMsg.begin();
		LISTBARRAGEMSG::iterator iLast = iFirst;
		LISTBARRAGEMSG::iterator i = iFirst;
		for(i;i!=m_listBarrageMsg.end();i++)
		{
			BarrageMessage& bm = *i;
			if(bm.mode == 0)//如果是普通弹幕
			{
				bm.cur_x -= delta_ms*bm.speed;
				if (bm.cur_x + bm.display_width <= cls_gl->m_winSize.width)
				{
					if (bm.cur_x + bm.display_width <= 0)
					{
						bm.isold |= true;
						nErase ++;
					}
					bm.isfull |= true;
				}
			}
			else
			{
				bm.age += delta_ms;
				if(bm.age >= BARRAGE_AGE)
				{
					bm.isold |= true;
					nErase ++;
				}
			}
		}
		while(nErase)
		{
			iLast ++;
			nErase --;
		}
		if (iFirst != iLast)
			m_listBarrageMsg.erase(iFirst,iLast);
	}
}
void COS_HeadMessage::updateMessageSpecial(ulong delta_ms)
{
	//update special message
	{
		CAutoLock lock(&cls_gl->m_csSpecial);
		if (!cls_gl->m_listSpecialMsg.empty())
		{
			SpecialMessage tmpSpecialMsg;
			tmpSpecialMsg.type = cls_gl->m_listSpecialMsg.front().type;
			tmpSpecialMsg.msg = cls_gl->m_listSpecialMsg.front().str;
			cls_gl->m_listSpecialMsg.pop_front();
			updateCenterSpecial(tmpSpecialMsg);
		}
	}

	if (m_listSpecialMsg.empty())
		return ;

	SpecialMessage sm = m_listSpecialMsg.front();

	static int s_star_width = m_ImgStar->getWidth();
	if (!m_bVaild)
	{
		float total_way = (s_star_width+sm.txt_width+cls_gl->m_winSize.width)/2.0f;
		m_fSpeed = total_way/SPECIAL_MOVE_TOCENTER;
		m_fSpeedOut = total_way/SPECIAL_MOVE_TOCENTER;
		m_bVaild = true;
	}

	m_nspecial_last_ms += delta_ms;
	if (sm.type == 0)//特效通知
	{
		static int nDisappear = SPECIAL_MOVE_TOCENTER+SPECIAL_CHANGE_APPEAR+SPECIAL_KEEP_STAY+SPECIAL_CHANGE_DISAPPEAR;
		float star_dest_x = (cls_gl->m_winSize.width-s_star_width-sm.txt_width)/2.0f;

		if (m_nspecial_last_ms<SPECIAL_MOVE_TOCENTER)//目标向右移动阶段
		{
			float x = cls_gl->m_winSize.width - m_fSpeed*m_nspecial_last_ms;
			if (!m_FloridStar->getRunning())
				m_FloridStar->startPS();
			m_FloridStar->moveTo(x,SPECIAL_CENTER_HEIGHT,false);
		}
		else if (m_nspecial_last_ms<(nDisappear+SPECIAL_MOVE_TOLEFT))//目标向右移出
		{
			float x = star_dest_x - m_fSpeedOut*(m_nspecial_last_ms-nDisappear);
			m_FloridStar->moveTo(x,SPECIAL_CENTER_HEIGHT,false);
		}
		else if(m_nspecial_last_ms >= nDisappear+SPECIAL_MOVE_TOLEFT)
		{
			if (m_listSpecialMsg.empty())
				return ;
			m_FloridStar->stopPS();
			m_listSpecialMsg.pop_front();
			m_nspecial_last_ms = 0;
			m_bVaild = false;
		}
	}
}


void COS_HeadMessage::update(const ulong delta_ms)
{
	updateMessageLeft(delta_ms);
	if(cls_gl->m_gCurPlayInfo.m_isMV)
		updateMessageRight(delta_ms);
	updateMessageBarrage(delta_ms);
	updateMessageSpecial(delta_ms);
}

void COS_HeadMessage::drawGraphBuffer()
{
	
	//drawRoomMoney(delta_ms);
	
	drawMessageLeft();

	//是MV的时候才需要显示当前演唱歌曲名称以及下一首名称
	if(cls_gl->m_gCurPlayInfo.m_isMV)
		drawMessageRight();

	drawBarrageMessage();

	drawSpecialMessage();

	//不是MV,并且不是广告，则展示二维码信息
	if(!cls_gl->m_gCurPlayInfo.m_isMV && !cls_gl->m_gCurPlayInfo.m_bAdvertisement)
	{
		static GSize s_scan_size = GSize(160,160);
		static float s_x = 960+(SCAN_WIDTH-160)/2.0f;
		if(m_ImgAppDownload)
		{
			static GPoint point = GPoint(s_x,SCAN_TOP,GL3D_Z_NOTIFY);
			m_ImgAppDownload->draw(point,s_scan_size);

			static int s_font_w = m_pFontR->getTextWidth(L"App下载",0.7,0.8);
			static float s_font_x = 960+(SCAN_WIDTH-s_font_w)/2.0f;
			static GPoint point_font = GPoint(s_font_x,SCAN_TOP+170,GL3D_Z_NOTIFY);
			m_pFontR->drawText(L"App下载",point_font,NULL,WHITE_RECT,0.7,0.8);
		}

		if(m_ImgEnterRoom)
		{
			//210 = 160+50
			static GPoint point = GPoint(s_x,SCAN_TOP+210,GL3D_Z_NOTIFY);
			m_ImgEnterRoom->draw(point,s_scan_size);

			static int s_font_w = m_pFontR->getTextWidth(L"扫描进包厢",0.8,0.8);
			static float s_font_x = 960+(SCAN_WIDTH-s_font_w)/2.0f;
			static GPoint point_font = GPoint(s_font_x,SCAN_TOP+380,GL3D_Z_NOTIFY);
			m_pFontR->drawText(L"扫描进包厢",point_font,NULL,WHITE_RECT,0.8,0.8);
		}
	}
}
