#ifndef ALPHA__H__
#define ALPHA__H__

typedef enum _AlphaType{
	eLINE,//�������Ӽ���
}AlphaType;

class CAlpha
{
public:
	/*
	@param cycle_ms : ָ��һ�����ڵ�ʱ��
	@param life��ָ����ʱ�� -1һֱ����
	@param type : ָ���仯�ķ�ʽ
	*/
	CAlpha(unsigned int cycle_ms,int life_ms=-1,AlphaType type=eLINE)
		:m_cur_ms(0),m_life_ms(life_ms),m_cycle_ms(cycle_ms),m_type(type),m_fAlpha(0.0f){}

	/*
	@param delta_ms : ��ȥ��ʵ��
	*/
	void update(unsigned int delta_ms);

	/*
	@return ��ǰ��Alphaֵ
	*/
	float getAlpha();
	/*
	���õ�ǰ��ʱ��Ϊ0
	*/
	void reset();
private:
	bool m_bReset;
	int m_life_ms;
	unsigned int m_cycle_ms;
	unsigned int m_cur_ms;
	AlphaType	m_type;
	float m_fAlpha;
};

#endif//ALPHA__H__