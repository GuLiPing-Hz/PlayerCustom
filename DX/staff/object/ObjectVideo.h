#ifndef OBJECTVIDEO__H__
#define OBJECTVIDEO__H__

#include "ISceneObject.h"

class CVSingerRender;
class CVideoObject : public ISceneObject
{
public:
	CVideoObject(bool& bInit);
	virtual ~CVideoObject();

	//���𣬳�ʼ������
	virtual bool awake(void* info);
	//���õ�ǰʱ��ֵ��
	virtual void setCurMs(unsigned int ms);
	//���²�����������
	//@param ms:��ȥ��ʱ�䣬��λ�����룩
	virtual bool update(unsigned int ms);
	//��Ⱦ����
	virtual bool render();

private:
	CVSingerRender*			vsinger_render;
};


#endif//OBJECTVIDEO__H__
