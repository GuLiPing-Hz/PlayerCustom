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
	���ݸ���������vector�����size��0�����vector�����size�Ѿ�����size��ֱ�ӷ���
	@param vectNumber[in out]:����vector
	@param size[in]:ָ����Ҫ������λ��������0����
	*/
	static void FillZeroNumber(VECTORINT& vectNumber,int size);
	/*
	���ݸ��������ֲ���һ������vector
	@param nScore[in]����������֣�Ҫ����������
	@param vecNumberBuf[out]�����ص�����vector
	*/
	static void DealWithNumber(int nNumber,VECTORINT &vecNumberBuf);
	/*
	�����ṩ��Դ���ֵ�Ŀ�����ֵ�vector�������ɶ�Ӧ�Ľ��ƹ�ϵ���Լ���ʼ����ǰ������vector
	@param src_number[in]��Դ����vector
	@param dest_number[in]��Ŀ������vector
	@param carry[out]�����ƹ�ϵ
	@param cur_number[out]����ʼ����ǰ����vector

	@����
		12->200 ����vector�� 0,2,9 ��ǰvector��0,1,2
		12->22 ����vector��0,1 ��ǰvector��1,2
	*/
	static void DealWithCarryAndNumber(const VECTORINT &src_number,const VECTORINT &dest_number
		,VECTORINT &carry,VECTORINT &cur_number);

	//������Ч����ֻ�Ǹ��ݵ�ǰ��ʱ����浱ǰ�����֣����һ�ֶ����ļ���
	/*
	@param index[in]����������vector�ĵڼ�λ����0��ʼ������߿�ʼ
	@param curNumber[in out]����ǰ����
	@param vectCur[in out]����ǰ������vector curNumber��vector��ʽ
	@param nDestNum[in]��Ŀ������
	@param vectDest[in]��Ŀ������ vector nDestNum��vector��ʽ
	@param vecControl[in out]����ǰ�Ľ��ƹ�ϵ
	@param nnumber_per_last_ms[in out]���������ֵı仯���ۼ��ѽ���ȥ��ʱ�䣨���룩
	@param nroll_pernumber_ms[in]���������ֵı仯ʱ�䣨���룩
	@param delta_tm[in]����ȥ��ʱ�䣨���룩
	@param bCarry[in]���Ƿ��λ
	@param nStep[in]�����õĲ��� 1 3 7
	*/
	static void ChangeNumber(int index,int &curNumber,VECTORINT &vectCur,int nDestNum,const VECTORINT &vectDest
		,VECTORINT &vectControl,ulong &nnumber_per_last_ms,ulong nroll_pernumber_ms,ulong delta_tm,int bCarry=false,int nStep=3);

	/*
	��������
	@param index[in]��vector����
	@param vectCur[in out]����ǰ���ֵ�vector
	@param vectDest[in]��Ŀ�����ֵ�vector
	@param vectControl[in out]����ǰ���ƹ�ϵ
	@param nnumber_per_last_ms[in out]���������ֵı仯���ۼ��ѽ���ȥ��ʱ�䣨���룩
	@param nroll_pernumber_ms[in]���������ֵı仯ʱ�䣨���룩
	@param delta_tm[in]����ȥ��ʱ�䣨���룩
	@param percent[out]���仯�ٷֱȣ�ָ�仯���Ǹ����ֵİٷֱȣ������0����û�仯�������1�����Ǳ仯��ȫ
	@param bCarry[in]���Ƿ��ǽ�λ
	@param nStep[in]�����õĲ��� 1 3 7
	*/
	static void RollNumber(int index,VECTORINT &vectCur,const VECTORINT &vectDest,VECTORINT &vectControl
		,ulong &nnumber_per_last_ms,ulong nroll_pernumber_ms,ulong delta_tm,float& percent,int bCarry=false,int nStep=1);
	/*
	��������
	@param index[in]��vector����
	@param vectCur[in out]����ǰ���ֵ�vector
	@param vectDest[in]��Ŀ�����ֵ�vector
	@param vectControl[in out]����ǰ���ƹ�ϵ
	@param nnumber_per_last_ms[in out]���������ֵı仯���ۼ��ѽ���ȥ��ʱ�䣨���룩
	@param nroll_pernumber_ms[in]���������ֵı仯ʱ�䣨���룩
	@param delta_tm[in]����ȥ��ʱ�䣨���룩
	@param percent[out]���仯�ٷֱȣ�ָ�仯���Ǹ����ֵİٷֱȣ������0����û�仯�������1�����Ǳ仯��ȫ
	@param bCarry[in]���Ƿ��ǽ�λ
	@param nStep[in]�����õĲ��� 1 3 7
	*/
	static void OverlyingNumber(int index,VECTORINT &vectCur,const VECTORINT &vectDest,VECTORINT &vectControl
		,ulong &nnumber_per_last_ms,ulong nroll_pernumber_ms,ulong delta_tm,float &percent,int bCarry=false,int nStep=1);

	/*
	
	���ֱ仯 ��������ƽ���ı仯
	@param [in] nDest��Ŀ������
	@param [in out] nCur����ǰ����
	@param [out] nNext���仯���¸�����
	@param [in out] nnumber_per_last_ms������ʱ��(����)
	@param [in] nroll_pernumber_ms��ÿ�����ֵı仯ʱ��(����)
	@param [in] delta_tm����ȥ��ʱ��(����)
	@param [out] percent���仯�İٷֱ�
	@param [in] nStep������

	*/
	static void SmoothNumber(const int nDest,int& nCur,int& nNext,ulong &nnumber_per_last_ms
		,ulong nroll_pernumber_ms,ulong delta_tm,float& percent,int nStep=1);
};

#endif//ANIMANUMBER__H__

