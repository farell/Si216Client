// fft_Transform.h: interface for the fft_Transform class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FFT_TRANSFORM_H__F120911F_D301_4ACA_9385_0C4A38587A8A__INCLUDED_)
#define AFX_FFT_TRANSFORM_H__F120911F_D301_4ACA_9385_0C4A38587A8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////
#include "math.h"

typedef unsigned short u8;//typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

#define  PI       3.141592653589793238462643     //����Բ����
#define  FFT_N    1024           //��������

typedef struct Complex         //�����ṹ��
{ 
	double real;
	double image; 
}COMPLEX;

typedef struct frequncy         //�����ṹ��
{ 	
	float f;  //Ƶ��
	float amplitude;//���� 
}FREQUENCY;
/*
COMPLEX FFT_result[FFT_N]=
{{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},
{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},
{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},
{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},
{1,0},{3,0},{2,0},{5,0}};                            //����������ݴ洢����
*/
/////////////////////////////////////////
class fft_Transform  
{
public:
	void findMaxFre();
    int FFT_N_cal; //ʵ�ʲ������������,���Ա�512��
	float baseFre;//��׼Ƶ��
	void init();
	void FFT_Run(void);
	frequncy maxfre[4];    //�ĸ�����Ƶ��ֵ
	COMPLEX FFT_input[FFT_N];
	COMPLEX FFT_result[FFT_N];
	float FFT_result_Abs[FFT_N/2];  //������ϱ仯�ľ���ֵ��

	COMPLEX SUB_EE(COMPLEX X1,COMPLEX X2);//-
	COMPLEX ADD_EE(COMPLEX X1,COMPLEX X2);//+
	COMPLEX MUL_EE(COMPLEX X1,COMPLEX X2);//*
	void Init_forward(void);   //��λ�򣬲�����ν���׵��㷨
	fft_Transform();
	virtual ~fft_Transform();
};

#endif // !defined(AFX_FFT_TRANSFORM_H__F120911F_D301_4ACA_9385_0C4A38587A8A__INCLUDED_)

/**************************�׵��㷨˼��*************************
* ��Ȼ˳��         ��λ��
* 000               000 
* 001               100 
* 010               010
* 011               110
* 100               001 
* 101               101 
* 110               011 
* 111               000

�ɴ˿ɼ���λ��ʵ���Ͼ��Ǿ������㣬Ȼ������û�в��þ����㷨������˵�����û����ʵ�ֱȽ����ף�

������Ҫ���Ĺ����ǣ�
1.���������֪��Ȼ˳���һ������Ҫ֪����һ����ֻ��Ҫ����ǰ����1����
2.�ٹ۲쵹λ��֮�����ݵĹ��ɣ�����
3.�������֪����λ��������һ������Ҫ�������һ������ͬ��Ҳ���Բ��üӷ���
  Ȼ�����˴��ļӷ������Ǵ�Сѧϰ�Ĳ�ͬ������Ҫʵ�ֵ������λ��λ�ļӷ������￴�����Դ������£�
  ������˵���˺�������ʵ�����������ϸ����������ܿ���˵���߼�˼ά����

dt=T/N0 ; w0=2Pi/T; f=w0/2Pi=1/T; T��ȡ�����ʱ�䡣Ȼ��ÿ��Ƶ��Ϊ���е�n*f;n�����

*/
