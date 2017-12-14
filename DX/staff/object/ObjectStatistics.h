#ifndef OBJECTSTATISTICS__H__
#define OBJECTSTATISTICS__H__

#include "ISceneObject.h"

class CScoreFrame;
class CStatisticsObject : public ISceneObject
{
public:
	CStatisticsObject(bool& bInit,bool bFirst);
	virtual ~CStatisticsObject();

	//���𣬳�ʼ������
	virtual bool awake(void* info);
	//���õ�ǰʱ��ֵ��
	virtual void setCurMs(unsigned int ms);
	//���²�����������
	//@param ms:��ȥ��ʱ�䣬��λ�����룩
	virtual bool update(unsigned int ms);
	//��Ⱦ����
	virtual bool render();

	CScoreFrame*	 getScoreFrameRender(){return scoreframe_render;};
private:
	CScoreFrame*		scoreframe_render;
};


#endif//OBJECTSTATISTICS__H__
