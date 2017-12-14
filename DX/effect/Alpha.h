#ifndef ALPHA__H__
#define ALPHA__H__

typedef enum _AlphaType{
	eLINE,//线性增加减少
}AlphaType;

class CAlpha
{
public:
	/*
	@param cycle_ms : 指定一个周期的时间
	@param life：指定总时间 -1一直持续
	@param type : 指定变化的方式
	*/
	CAlpha(unsigned int cycle_ms,int life_ms=-1,AlphaType type=eLINE)
		:m_cur_ms(0),m_life_ms(life_ms),m_cycle_ms(cycle_ms),m_type(type),m_fAlpha(0.0f){}

	/*
	@param delta_ms : 逝去的实际
	*/
	void update(unsigned int delta_ms);

	/*
	@return 当前的Alpha值
	*/
	float getAlpha();
	/*
	重置当前的时间为0
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