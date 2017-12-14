#ifndef OBJECTFIR__H__
#define OBJECTFIR__H__

#include "ISceneObject.h"

class CGLBlackCurtain;
class CTopNotifyRender;
class CFirObject : public ISceneObject
{
public:
	CFirObject(bool& isInit);
	virtual ~CFirObject();

	//���𣬳�ʼ������
	virtual bool awake(void* info);
	//���õ�ǰʱ��ֵ��
	virtual void setCurMs(unsigned int ms);
	//���²�����������
	//@param ms:��ȥ��ʱ�䣬��λ�����룩
	virtual bool update(unsigned int ms);
	//��Ⱦ����
	virtual bool render();

	CTopNotifyRender* getTopNotify(){return m_pTopNotify;}
private:
	CGLBlackCurtain*			m_pGLBlackCurtain;//��Ļ
	CTopNotifyRender*		m_pTopNotify;//������Ļ��ʾ
};

#endif//OBJECTFIR__H__
