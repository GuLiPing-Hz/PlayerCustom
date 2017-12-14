#ifndef ISWFLISTENER__H__
#define ISWFLISTENER__H__

interface ISwfListener
{
	virtual void onSwfEnd()=0;
	virtual void onSwfFrameRender(int n)=0;
	virtual void onSwfStart()=0;
};

#endif//ISWFLISTENER__H__