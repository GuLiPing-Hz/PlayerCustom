/*
	2014-8-7
	copyright @glp 
*/

#ifndef ANIMANUMBER__H__
#define ANIMANUMBER__H__
#include "../Opengl.h"
#include "../GLListDef.h"

class CAnimaNumber
{
public:
	CAnimaNumber();
	virtual ~CAnimaNumber();

	static int CaculateDigit(int number);
	/*
	根据给定的数字vector，填充size个0，如果vector本身的size已经大于size则直接返回
	@param vectNumber[in out]:数字vector
	@param size[in]:指定需要填充多少位。不足以0补足
	*/
	static void FillZeroNumber(VECTORINT& vectNumber,int size);
	/*
	根据给定的数字产生一个数字vector
	@param nScore[in]：输入的数字（要求是正数）
	@param vecNumberBuf[out]：返回的数字vector
	*/
	static void DealWithNumber(int nNumber,VECTORINT &vecNumberBuf);
	/*
	根据提供的源数字到目的数字的vector，来生成对应的进制关系，以及初始化当前的数字vector
	@param src_number[in]：源数字vector
	@param dest_number[in]：目的数字vector
	@param carry[out]：进制关系
	@param cur_number[out]：初始化当前数字vector

	@例：
		12->200 进制vector是 0,2,9 当前vector是0,1,2
		12->22 进制vector是0,1 当前vector是1,2
	*/
	static void DealWithCarryAndNumber(const VECTORINT &src_number,const VECTORINT &dest_number
		,VECTORINT &carry,VECTORINT &cur_number);

	//无特殊效果，只是根据当前的时间描绘当前的数字，造成一种动画的假象
	/*
	@param index[in]：操作数字vector的第几位，从0开始，从左边开始
	@param curNumber[in out]：当前数字
	@param vectCur[in out]：当前的数字vector curNumber的vector形式
	@param nDestNum[in]：目标数字
	@param vectDest[in]：目标数字 vector nDestNum的vector形式
	@param vecControl[in out]：当前的进制关系
	@param nnumber_per_last_ms[in out]：单个数字的变化的累计已近逝去的时间（毫秒）
	@param nroll_pernumber_ms[in]：单个数字的变化时间（毫秒）
	@param delta_tm[in]：逝去的时间（毫秒）
	@param bCarry[in]：是否进位
	@param nStep[in]：采用的步长 1 3 7
	*/
	static void ChangeNumber(int index,int &curNumber,VECTORINT &vectCur,int nDestNum,const VECTORINT &vectDest
		,VECTORINT &vectControl,ulong &nnumber_per_last_ms,ulong nroll_pernumber_ms,ulong delta_tm,int bCarry=false,int nStep=3);

	/*
	翻滚数字
	@param index[in]：vector索引
	@param vectCur[in out]：当前数字的vector
	@param vectDest[in]：目标数字的vector
	@param vectControl[in out]：当前进制关系
	@param nnumber_per_last_ms[in out]：单个数字的变化的累计已近逝去的时间（毫秒）
	@param nroll_pernumber_ms[in]：单个数字的变化时间（毫秒）
	@param delta_tm[in]：逝去的时间（毫秒）
	@param percent[out]：变化百分比，指变化的那个数字的百分比，如果是0，则还没变化，如果是1，则是变化完全
	@param bCarry[in]：是否是进位
	@param nStep[in]：采用的步长 1 3 7
	*/
	static void RollNumber(int index,VECTORINT &vectCur,const VECTORINT &vectDest,VECTORINT &vectControl
		,ulong &nnumber_per_last_ms,ulong nroll_pernumber_ms,ulong delta_tm,float& percent,int bCarry=false,int nStep=1);
	/*
	叠放数字
	@param index[in]：vector索引
	@param vectCur[in out]：当前数字的vector
	@param vectDest[in]：目标数字的vector
	@param vectControl[in out]：当前进制关系
	@param nnumber_per_last_ms[in out]：单个数字的变化的累计已近逝去的时间（毫秒）
	@param nroll_pernumber_ms[in]：单个数字的变化时间（毫秒）
	@param delta_tm[in]：逝去的时间（毫秒）
	@param percent[out]：变化百分比，指变化的那个数字的百分比，如果是0，则还没变化，如果是1，则是变化完全
	@param bCarry[in]：是否是进位
	@param nStep[in]：采用的步长 1 3 7
	*/
	static void OverlyingNumber(int index,VECTORINT &vectCur,const VECTORINT &vectDest,VECTORINT &vectControl
		,ulong &nnumber_per_last_ms,ulong nroll_pernumber_ms,ulong delta_tm,float &percent,int bCarry=false,int nStep=1);

	/*
	
	数字变化 适于所有平滑的变化
	@param [in] nDest：目标数字
	@param [in out] nCur：当前数字
	@param [out] nNext：变化的下个数字
	@param [in out] nnumber_per_last_ms：持续时间(毫秒)
	@param [in] nroll_pernumber_ms：每个数字的变化时间(毫秒)
	@param [in] delta_tm：逝去的时间(毫秒)
	@param [out] percent：变化的百分比
	@param [in] nStep：步长

	*/
	static void SmoothNumber(const int nDest,int& nCur,int& nNext,ulong &nnumber_per_last_ms
		,ulong nroll_pernumber_ms,ulong delta_tm,float& percent,int nStep=1);
};

#endif//ANIMANUMBER__H__

