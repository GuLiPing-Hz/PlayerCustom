#include "stdafx.h"
#include "Alpha.h"

inline float onLine(unsigned int delta_ms,unsigned int& cur_ms,unsigned int total_ms)
{
	float alpha = 0.0f;
	float f_half_ms = total_ms/2.0f;
	int n_half_ms = total_ms/2;

	bool bMinus = (cur_ms/n_half_ms)%2;

	//线性变化
	if (bMinus)
		alpha = 1.0f - (cur_ms%n_half_ms)/f_half_ms;
	else
		alpha = (cur_ms%n_half_ms)/f_half_ms;
		
	cur_ms += delta_ms;
	return alpha;
}

void CAlpha::update(unsigned int delta_ms)
{
	m_fAlpha = 0.0f;

	if(m_bReset)
	{
		m_cur_ms = 0;
		m_bReset = false;
	}

	if(m_life_ms != -1 && m_cur_ms > m_life_ms)
		return ;

	switch(m_type)
	{
	case eLINE:
		{
			m_fAlpha = onLine(delta_ms,m_cur_ms,m_cycle_ms);
			break;
		}
	}
}

float CAlpha::getAlpha()
{
	return m_fAlpha;
}

void CAlpha::reset()
{
	m_bReset = true;
}

