#include "stdafx.h"
#include "AnimaNumber.h"
#include <assert.h>

int CAnimaNumber::CaculateDigit(int number)
{
	int n = 1;
	while(number/10 != 0)
	{
		n ++;
		number /= 10;
	}

	return n;
}

void CAnimaNumber::FillZeroNumber(VECTORINT& vectNumber,int size)
{
	if((int)vectNumber.size() >= size)
		return ;

	VECTORINT tmp = vectNumber;
	vectNumber.clear();
	vectNumber.resize(size,0);

	VECTORINT::reverse_iterator itTmp = tmp.rbegin();
	VECTORINT::reverse_iterator itNumber = vectNumber.rbegin();
	for (;itTmp!=tmp.rend()&&itNumber!=vectNumber.rend();itTmp++,itNumber++)
	{
		*itNumber = *itTmp;
	}
}
void CAnimaNumber::DealWithNumber(int nNumber,VECTORINT &vecNumberBuf)
{
	vecNumberBuf.clear();
	if (nNumber/10 != 0)
		DealWithNumber(nNumber/10,vecNumberBuf);
	nNumber = nNumber%10;
	vecNumberBuf.push_back(nNumber);
}

void CAnimaNumber::DealWithCarryAndNumber(const VECTORINT &src_number,const VECTORINT &dest_number
										  ,VECTORINT &carry,VECTORINT &cur_number)
{
	carry.resize(dest_number.size(),0);
	cur_number.clear();

	if (src_number.size() < dest_number.size())
	{
		int i = (int)(dest_number.size() - src_number.size());
		cur_number.resize(dest_number.size(),0);

		if (dest_number.size()>1)//如果有两位以上
		{
			for(int j=0;j<i;j++)
			{
				carry[j+1] = dest_number[j];
			}
			for(int k=0;i<(int)dest_number.size();i++,k++)
			{
				cur_number[i] = src_number[k];
				if (i+1 < (int)dest_number.size())
				{
					carry[i+1] = abs(dest_number[i]-cur_number[i]);
				}
			}
		}	
	}
	else
	{
		cur_number = src_number;
		for (int i=1;i<(int)dest_number.size();i++)
		{
			carry[i] = abs(dest_number[i-1]-src_number[i-1]);
		}
	}
}

inline void DealWithStep(int index,VECTORINT& vectCur,VECTORINT& vectControl,int step)
{
	switch(step)
	{
	case 1:
		{
			if (vectCur[index] == 9)
			{
				vectCur[index] = 0;
				vectControl[index] --;
			}
			else
			{
				vectCur[index] ++;
			}
			break;
		}
	case 3:
		{
			if (vectCur[index] == 9)
			{
				vectCur[index] = 2;
				vectControl[index] --;
			}
			else if (vectCur[index] == 8)
			{
				vectCur[index] = 1;
				vectControl[index] --;
			}
			else if (vectCur[index] ==7)
			{
				vectCur[index] = 0;
				vectControl[index] --;
			}
			else
			{
				vectCur[index] +=3;
			}
			break;
		}
	case 7:
		{
			if (vectCur[index] == 9)
			{
				vectCur[index] = 6;
				vectControl[index] --;
			}
			else if (vectCur[index] == 8)
			{
				vectCur[index] = 5;
				vectControl[index] --;
			}
			else if (vectCur[index] ==7)
			{
				vectCur[index] = 4;
				vectControl[index] --;
			}
			else if(vectCur[index] == 6)
			{
				vectCur[index] = 3;
				vectControl[index] --;
			}
			else if(vectCur[index] == 5)
			{
				vectCur[index] = 2;
				vectControl[index] --;
			}
			else if(vectCur[index] == 4)
			{
				vectCur[index] = 1;
				vectControl[index] --;
			}
			else if(vectCur[index] == 3)
			{
				vectCur[index] = 0;
				vectControl[index] --;
			}
			else
			{
				vectCur[index] +=7;
			}
			break;
		}

	default:
		assert(false);
	}
}

void CAnimaNumber::ChangeNumber(int index,int &curNumber,VECTORINT &vectCur,int nDestNum,const VECTORINT &vectDest
								,VECTORINT &vectControl,ulong &nnumber_per_last_ms,ulong nroll_pernumber_ms,ulong delta_tm,int bCarry,int nStep)
{
	bool bControl = false;
	//判断是否有需要进位的存在
	for(int i=0;i<=index;i++)
	{
		if (vectControl[i])
		{
			bControl = true;
			break;
		}
	}
	//如果指定时间，并且 当前数字与目标数字不同，或者需要进位，则需要变化
	if (nroll_pernumber_ms&&((vectCur[index] != vectDest[index]) || bControl))
	{
		if (bCarry)
		{
			if (vectCur[index] == 9)
			{
				vectCur[index] = 0;
				vectControl[index] --;//当前的进位减1
			}
			else
			{
				vectCur[index] ++;
			}
		}
		else
		{
			nnumber_per_last_ms += delta_tm;
			if (nnumber_per_last_ms>nroll_pernumber_ms)
			{
				curNumber += 3;//因为间隔的时间其实是不够每次加一的，所以这里采取每次加3的方法，奇数偶数都能看到而不是加2
				if (curNumber >= nDestNum)
				{//变化终止，已经达到指定的数字
					vectCur = vectDest;
					curNumber = nDestNum;
					return ;
				}
				nnumber_per_last_ms -= nroll_pernumber_ms;
				//因为每次都是加3，所以需要判断3种情况
				
				DealWithStep(index,vectCur,vectControl,nStep);
			}
		}
	}
}

void CAnimaNumber::RollNumber(int index,VECTORINT &vectCur,const VECTORINT &vectDest,VECTORINT &vectControl
							  ,ulong &nnumber_per_last_ms,ulong nroll_pernumber_ms,ulong delta_tm,float& percent,int bCarry,int nStep)
{
	percent = 0.0f;
	bool bControl = false;
	for(int i=0;i<=index;i++)
	{
		if (vectControl[i])
		{
			bControl = true;
			break;
		}
	}
	if (nroll_pernumber_ms&&((vectCur[index] != vectDest[index]) || bControl))
	{
		percent = nnumber_per_last_ms*1.0f/nroll_pernumber_ms;
		percent = percent<0.0f?0.0f:(percent>1.0f?1.0f:percent);
		if (bCarry)
		{
			if (vectCur[index] == 9)
			{
				vectCur[index] = 0;
				vectControl[index] --;
			}
			else
			{
				vectCur[index] ++;
			}
		}
		else
		{
			nnumber_per_last_ms += delta_tm;
			if (nnumber_per_last_ms>nroll_pernumber_ms)
			{
				nnumber_per_last_ms -= nroll_pernumber_ms;
				DealWithStep(index,vectCur,vectControl,nStep);
			}
		}
	}
}

void CAnimaNumber::OverlyingNumber(int index,VECTORINT &vectCur,const VECTORINT &vectDest,VECTORINT &vectControl
								   ,ulong &nnumber_per_last_ms,ulong nroll_pernumber_ms,ulong delta_tm,float &percent,int bCarry,int nStep)
{
	percent = 0.0f;
	bool bControl = false;
	for(int i=0;i<=index;i++)
	{
		if (vectControl[i])
		{
			bControl = true;
			break;
		}
	}
	if (nroll_pernumber_ms&&((vectCur[index] != vectDest[index]) || bControl))
	{
		percent = nnumber_per_last_ms*1.0f/nroll_pernumber_ms;
		percent = percent<0.0f?0.0f:(percent>1.0f?1.0f:percent);

		if (bCarry)
		{
			if (vectCur[index] == 9)
			{
				vectCur[index] = 0;
				vectControl[index] --;
			}
			else
			{
				vectCur[index] ++;
			}
		}
		else
		{
			nnumber_per_last_ms += delta_tm;
			if (nnumber_per_last_ms>nroll_pernumber_ms)
			{
				nnumber_per_last_ms -= nroll_pernumber_ms;
				DealWithStep(index,vectCur,vectControl,nStep);
			}
		}
	}
}

void CAnimaNumber::SmoothNumber(const int nDest,int& nCur,int& nNext,ulong &nnumber_per_last_ms
								,ulong nroll_pernumber_ms,ulong delta_tm,float& percent,int nStep)
{
	if((nCur >= nDest))
	{
		nNext = nDest;
		nCur = nNext;
		nnumber_per_last_ms = 0;
		percent = 0.0f;
		return ;
	}

	if(nroll_pernumber_ms == 0)
	{
		assert(false);
		return ;
	}

	percent = nnumber_per_last_ms*1.0f/nroll_pernumber_ms;
	percent = percent<0.0f?0.0f:(percent>1.0f?1.0f:percent);

	nNext = nCur + nStep;
	if(nNext>= nDest)
		nNext = nDest;

	if(nnumber_per_last_ms >= nroll_pernumber_ms)
	{
		nnumber_per_last_ms -= nroll_pernumber_ms;//做减法，或者取余，可能减法更快一点
		nCur = nNext;
	}

	nnumber_per_last_ms += delta_tm;
}

