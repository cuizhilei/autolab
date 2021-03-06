//*****************************************************************************
// 文件名 : SplineCurve.h
// 
// 概要   : 样条差值算法类
// 
// 更新日期     修订人       修改内容         
// ------------------------------------------
// 2013-4       崔志雷       新建
// ------------------------------------------
//*****************************************************************************
//
//		       三次样条插值算法原理
// ------------------------------------------
//  三次样条曲线函数可写成如下形式(详细推导过程请参考教材<数值分析(李庆扬)>):
//  S(x) =		M(j) * ( x(j+1)  - x ) ^ 3 / 6 h(j)					
//			+	M(j+1) * ( x - x(j) ) ^ 3 / 6 h(j)					 
//			+	( y(j) - M(j) * h(j)^2 / 6) * ( x(j+1) - x ) / h(j)  
//			+	( y(j+1) - M(j+1) * h(j)^2 / 6) * ( x - x(j)) / h(j) 
//
//	其中x(j)表示第j个插值节点的x坐标，
//		y(j)表示第j个插值节点的y坐标，
//		M(j)表示样条曲线在第j点处的二阶导数值,是待求的未知数
//		h(j) = x(j+1) - x(j)

//	以下是求解M(0) ~ M(n-1) 的矩阵
//  矩阵形式方程组
//	( 2			λ0							)	( M(0)	)			( d(0)	)
//	( μ1		2			λ1				)	( M(1)	)			( d(1)	)
//	(     ·		·		 ·				)	( ·	)			( ·	)
//	(		·		  ·		· 			)	( ·	)       =   ( ·	) 
//	(		  ·	     ·        ·		)	( ·	)			( ·	)
//	(			μn-2	  2      	λn-2   )	( M(n-2))			( d(n-2))	
//	(					 μn-1       2      )   ( M(n-1))			( d(n-1))
//
//	其中 μi =	h(j-1) / ( h(j-1) + h(j)) ,
//		 λi = 1 - μi = h(j) / ( h(j-1) + h(j))
//		 d(j) = 6*( f[x(j),x(j+1)] - f[x(j-1),x(j)] ) / ( h(j-1) + h(j))
//		      = 6*( (y(j+1) - y(j)) / h(j) - (y(j) - y(j-1)) / h(j-1) ) / ( h(j-1) + h(j))
//		 λ0 = 0
//		 μn-1 = 0
//		 d(0) = 0
//		 d(n-1) = 0
//			
//
//  用追赶法解此方程组(详细推导过程请参考教材<数值分析(李庆扬)>)
//  
//  （1）βi（中间变量） 递推：
//		 β0 = λ0 / 2 = 0
//		 βi = λi / ( 2 - μi * βi-1)
//	
//  （2）Yi（中间变量） 递推：
//		 Y0 = d(0) / 2 = 0
//		 Yi = (d(i) - μi * Yi-1) / (2 - μi * βi-1)
//
//  （2）M(i)(方程解) 递推：
//		 M(n-1) = μn-1 = 0
//		 M(i) = Yi - βi * M(i+1)
//
//*****************************************************************************

#pragma once
#include "stdafx.h"
#include <assert.h>
#include <vector>
#include "math.h"
#include "CoordPoint.h"

using namespace std;

#define  x(i) m_InterNodeArr[##i].x						// 为直观，把数组形式写成宏
#define  y(i) m_InterNodeArr[##i].y
#define  M(i) m_M[##i]


// ******************************************************
//  样条曲线类
// ******************************************************
class CSplineCurve
{
public:
	CSplineCurve();																
	~CSplineCurve(void);
	// --------------------------------------------------
	// 摘要：
	//     输入插值节点数组构造样条曲线
	// 参数：
	//     1.InterNodeArr - 插值节点数组
	// --------------------------------------------------
	CSplineCurve(const vector<CCoordPoint> &InterNodeArr);

	// --------------------------------------------------
	// 摘要：
	//     输入插值节点数组重置样条曲线
	// 参数：
	//     1.InterNodeArr - 插值节点数组
	// --------------------------------------------------
	bool Reset(const vector<CCoordPoint> &InterNodeArr);

	// --------------------------------------------------
	// 摘要：
	//     根据给定的x坐标值,取得该曲线上的y坐标值
	// 参数：
	//     1.x - 给定的x值
	// --------------------------------------------------
	double GetYByX(const double &x) const;

	// --------------------------------------------------
	// 摘要：
	//     根据给定的y坐标值,逆推x值,即求方程S(x) - y = 0根
	// 参数：
	//     1.y - 给定的y值
	// --------------------------------------------------
	double GetXByY(const double &y) const;

	// --------------------------------------------------
	// 摘要：
	//     取得拐点数
	// --------------------------------------------------
	int GetInflectionPointCount(void) const;

private:
	// --------------------------------------------------
	// 摘要：
	//     计算函数，用追赶法计算出函数参数Mj保存在M(j)中
	// --------------------------------------------------
	 void _Calculate();

	 // --------------------------------------------------
	 // 摘要：
	 //    查找区间，即查看x值位于哪两个标准点之间，
	 //    返回较小的索引值j
	 // --------------------------------------------------
	 int _FindIntervalByX(const double& x) const;										

	 // --------------------------------------------------
	 // 摘要：
	 //    查找区间，即查看y值位于哪两个标准点之间，
	 //    返回较小的索引值j
	 // --------------------------------------------------
	 int _FindIntervalByY(const double& y) const;										// 查找区间，即查看y值位于哪两个标准点之间，返回较大的索引值j

	 // --------------------------------------------------
	 // 摘要：
	 //    为方便书写，hi定义成h(i)的内联函数形式
	 // --------------------------------------------------
	 inline double h(const int& j)	 const
	 {
		if(j < 0 || j >= m_NodeCount - 1)
			assert(0);
		return x(j+1) - x(j);
	 }

	 // --------------------------------------------------
	 // 摘要：
	 //    为方便书写，μi定义成Mu(i)的内联函数形式
	 // --------------------------------------------------
	 inline double Mu(const int& j) const		
	 {
		 assert(j>=1);

		 if(j < m_NodeCount - 1)
			 return h(j-1) / ( h(j-1) + h(j));
		 else
			 return 0;					// μn-1 = λ0 = 0
	 }

	 // --------------------------------------------------
	 // 摘要：
	 //    为方便书写，λi定义成Lambda(i)的内联函数形式
	 // --------------------------------------------------
	 inline double Lambda(const int& j) const
	 {
		 assert(j > 0);
		 return 1 - Mu(j);
	 }

	 // --------------------------------------------------
	 // 摘要：
	 //    为方便书写，ｄi定义成d(i)的内联函数形式
	 // --------------------------------------------------
	 inline double d(const int& j)	 const
	 {
		 if(j < 1)
			 return 0;
		 if(j < m_NodeCount -1)
			return 6*( (y(j+1) - y(j)) / h(j) - (y(j) - m_InterNodeArr[j-1].y) / h(j-1) ) / ( h(j-1) + h(j));
		 else
			 return 0;					// d(n) = 0
	 }

	 // --------------------------------------------------
	 // 摘要：
	 //    求S(x)的值
	 // --------------------------------------------------
	 inline double S(const double& x) const
	 {
		 int j = _FindIntervalByX(x);										// 查找出x属于哪个段
		 return		M(j) * pow( x(j+1)  - x ,3) / (6*h(j))					// 计算S(x)值
				+	M(j+1) * pow( x - x(j) ,3) / (6*h(j))					 
				+	( y(j) - M(j) * pow(h(j),2) / 6) * ( x(j+1) - x ) / h(j)  
				+	( y(j+1) - M(j+1) * pow(h(j),2) / 6) * ( x - x(j)) / h(j) ;
	 }

	 // --------------------------------------------------
	 // 摘要：
	 //    S(x)的一阶导数S'(x)
	 // --------------------------------------------------
	 inline double dS(const double& x) const	
	 {
		 int j = _FindIntervalByX(x);										// 查找出x属于哪个段
		 return	  - M(j) * pow(x(j+1)-x ,2)/(2*h(j))
				  +	M(j+1) * pow(x-x(j) ,2)/(2*h(j))
				  +	( y(j+1) - y(j))/ h(j)  
				  -	( M(j+1) - M(j))*h(j)/6;  
	 }

private:
	vector<CCoordPoint> m_InterNodeArr;										// 曲线的插值节点数组
	vector<double> m_M;														// 曲线方程系数数组
	int m_NodeCount;														// 插值节点数量
};
