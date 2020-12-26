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

#define  PI       3.141592653589793238462643     //定义圆周率
#define  FFT_N    1024           //采样点数

typedef struct Complex         //复数结构体
{ 
	double real;
	double image; 
}COMPLEX;

typedef struct frequncy         //复数结构体
{ 	
	float f;  //频率
	float amplitude;//幅度 
}FREQUENCY;
/*
COMPLEX FFT_result[FFT_N]=
{{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},
{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},
{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},
{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},{1,0},{3,0},{2,0},{5,0},{8,0},{4,0},
{1,0},{3,0},{2,0},{5,0}};                            //输入输出数据存储数组
*/
/////////////////////////////////////////
class fft_Transform  
{
public:
	void findMaxFre();
    int FFT_N_cal; //实际参与运算的数据,可以比512少
	float baseFre;//基准频率
	void init();
	void FFT_Run(void);
	frequncy maxfre[4];    //四个最大的频率值
	COMPLEX FFT_input[FFT_N];
	COMPLEX FFT_result[FFT_N];
	float FFT_result_Abs[FFT_N/2];  //求出傅氏变化的绝对值。

	COMPLEX SUB_EE(COMPLEX X1,COMPLEX X2);//-
	COMPLEX ADD_EE(COMPLEX X1,COMPLEX X2);//+
	COMPLEX MUL_EE(COMPLEX X1,COMPLEX X2);//*
	void Init_forward(void);   //倒位序，采用所谓的雷德算法
	fft_Transform();
	virtual ~fft_Transform();
};

#endif // !defined(AFX_FFT_TRANSFORM_H__F120911F_D301_4ACA_9385_0C4A38587A8A__INCLUDED_)

/**************************雷德算法思想*************************
* 自然顺序         倒位序
* 000               000 
* 001               100 
* 010               010
* 011               110
* 100               001 
* 101               101 
* 110               011 
* 111               000

由此可见到位序实际上就是镜像运算，然而我们没有采用镜像算法，（据说可以用汇编来实现比较容易）

我们所要做的工作是：
1.如果我们已知自然顺序的一个数想要知道下一个数只需要将当前数加1即可
2.再观察倒位序之后数据的规律，，，
3.如果我们知道倒位序后的其中一个数，要想求出下一个数，同样也可以采用加法，
  然而，此处的加法跟我们从小学习的不同，我们要实现的是向低位进位的加法（这里看不懂脑袋砸两下）
  话不多说，此函数就是实现这个功能仔细分析，如果能看懂说明逻辑思维不错

dt=T/N0 ; w0=2Pi/T; f=w0/2Pi=1/T; T是取样的最长时间。然后，每个频率为其中的n*f;n是序号

*/
