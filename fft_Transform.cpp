// fft_Transform.cpp: implementation of the fft_Transform class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "fft_Transform.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

fft_Transform::fft_Transform()
{
	memcpy(&FFT_result[0],&FFT_input[0],sizeof(COMPLEX)*FFT_N);
}

fft_Transform::~fft_Transform()
{

}

void fft_Transform::Init_forward()
{
	u8 I,J,LH,N1,K ;                        
	COMPLEX T;                             //替换结构体
	LH = FFT_N_cal/2;                          //N/2
	J = LH;                                 
	N1 = FFT_N_cal - 2;                         
	
	for(I=1;I<N1;I++)                       //从1到N-2开始倒位序
    {
        if(I<J)                            //此处的意思是当I不等于J时交换位置                             
        {                                  //然而I>J时不交换因为之前已经交换了
			T = FFT_result[I];                
			FFT_result[I] = FFT_result[J];
			FFT_result[J] = T;
		}
		K = LH;                          //将给K赋值N/2
		while(J>=K)                        //循环，，判断所需判断的位是否为1
		{
			J = J-K;
			K = K/2;  
		}
		J = J+K;
      }
}

COMPLEX fft_Transform::MUL_EE(COMPLEX X1, COMPLEX X2)
{
	COMPLEX c;
    c.real  = X1.real*X2.real - X1.image*X2.image;
    c.image = X1.real*X2.image + X1.image*X2.real;
    return  c;
}

COMPLEX fft_Transform::ADD_EE(COMPLEX X1, COMPLEX X2)
{
	COMPLEX c;
    c.real  = X1.real + X2.real;  
    c.image = X1.image + X2.image;   
    return  c;
}

COMPLEX fft_Transform::SUB_EE(COMPLEX X1, COMPLEX X2)
{
	COMPLEX c;
    c.real  = X1.real - X2.real;  
    c.image = X1.image - X2.image;   
    return  c;
}

void fft_Transform::FFT_Run()
{
	u8 B,P,K,i;
	u8 L = 0;                                             //蝶形变换级数
	u8 M = 0;                                             //N = 2^M  
	u8 J;
	u8 FFT_N1 = FFT_N_cal;//FFT_N;

	COMPLEX  Result_Wn,Result_MUL,Result_ADD,Result_SUB;

	memcpy(&FFT_result[0],&FFT_input[0],sizeof(COMPLEX)*FFT_N_cal);
	Init_forward();                                       //进行倒位序运算
	
	for(M=1; (FFT_N1=FFT_N1/2)!=1;M++);                      //计算蝶形级数
	for(L=1;L<=M;L++)
	{  
		B = pow(2,L-1);                                    // 旋转因子个数
		for(J=0;J<=B-1;J++)                          
		{   
			P = pow(2,M-L)*J;                              //旋转因子系数
			for(K=J;K<FFT_N_cal;K=K+pow(2,L))                
			{ 
				Result_Wn.real  =  cos((2*PI/FFT_N_cal)*P);
				Result_Wn.image = -sin((2*PI/FFT_N_cal)*P);
				
				Result_MUL =  MUL_EE(Result_Wn,FFT_result[K+B]);  //复数乘法Result_MUL = MUL_EE(Result_Wn,FFT_input[K+B]);//
				Result_ADD =  ADD_EE(FFT_result[K],Result_MUL);   //复数加法Result_ADD = ADD_EE(FFT_input[K],Result_MUL); //
				Result_SUB =  SUB_EE(FFT_result[K],Result_MUL);   //复数减法Result_SUB = SUB_EE(FFT_input[K],Result_MUL); //
				
				FFT_result[K]   =  Result_ADD;          //把加法后的结果放到 FFT_result[K]
				FFT_result[K+B] =  Result_SUB;          //把减法之后的结果放到FFT_result[K+B]			
			}  
		}
	}
	for (i=0;i<FFT_N_cal;i++)
	{
		FFT_result[i].real=FFT_result[i].real/FFT_N_cal;
		FFT_result[i].image=FFT_result[i].image/FFT_N_cal;
	}
}
// 
// void main(void)
// {
// 	u8 a;
// 	u8 M;
// 	u8 FFT_N1 = FFT_N;
// 	
//     FFT_Run(); 
// 	for(a=0;a<FFT_N;a++)
// 	{
// 		printf("%f",FFT_result[a].real/100);
// 		printf("    ");
// 		printf("%f",FFT_result[a].image/100);
// 		printf("\n");
// 	}
// 	a= pow(2,3); 
// 	printf("%d",a);
//   printf("\n");
// }

void fft_Transform::init()
{
	memset(maxfre,0,4*sizeof(FREQUENCY));//最大频率置0
	memset(FFT_input,0,sizeof(COMPLEX)*FFT_N);
	memset(FFT_result,0,sizeof(COMPLEX)*FFT_N);
	//baseFre=0;
	//int second=1;
	//int T0=1*second;
}


void fft_Transform::findMaxFre()
{
	int i,j,ni;
	///////////////////////求傅氏变换的绝对值////////////////再找出4个最大值

	memset(FFT_result_Abs,0,sizeof(float)*FFT_N/2);
	for (i=1;i<FFT_N_cal/2;i++)    // 0 阶不算，是直流量
	{
		FFT_result_Abs[i]=sqrt(FFT_result[i].real*FFT_result[i].real+FFT_result[i].image*FFT_result[i].image);
	}
	for (j=0;j<4;j++)
	{
		maxfre[j].amplitude=0;	
		ni=0;

		for (i=1;i<FFT_N_cal/2;i++)    //找到ABS最大值，不算直流量[0]
		{
			if (FFT_result_Abs[i]>maxfre[j].amplitude)
			{
				maxfre[j].amplitude=FFT_result_Abs[i];
				ni=i;
			}			
		}		
		FFT_result_Abs[ni]=0;
		maxfre[j].f=ni*baseFre;
	}

}
