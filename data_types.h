/* 
 * File:   data_types.h
 * Author: 110202117
 *
 * Created on 2020/08/27, 10:21
 */

#ifndef DATA_TYPES_H
#define	DATA_TYPES_H

#ifdef	__cplusplus
extern "C" {
#endif

    //------------------------------------------------------------------------------
//.定数定義
//..データ型定義
typedef		signed char	B;							// 符号付き 8ビット整数
typedef		signed short	H;							// 符号付き16ビット整数
typedef		signed long		W;							// 符号付き32ビット整数
typedef		unsigned char	UB;							// 符号無し 8ビット整数
typedef		unsigned short	UH;							// 符号無し16ビット整数
typedef		unsigned long	UW;							// 符号無し32ビット整数
typedef		unsigned char	BOOL;

typedef		float			FLOAT;						// 単精度(32ビット)浮動小数点
typedef		double			DOUBLE;						// 倍精度(64ビット)浮動小数点

//------------------------------------------------------------------------------
//..キーワード定義
#define 	FALSE	0
#define 	TRUE	1

#ifndef __cplsuplus
	#define NULL_PTR ((void*)0)
#else
	#define NULL_PTR __null
#endif

#define	NULL_FAR_PTR ((void __far *)0)
//------------------------------------------------------------------------------
//..フラグ状態定義
#define 	NA			0								// 非対応
#define 	OFF			0								// OFF
#define 	ON			1								// ON

#define		LOW			0
#define		HIGH		1
//------------------------------------------------------------------------------
//..最大値定義
#define 	MAX_UB  	0xFF
#define 	MAX_B	    0x7F
#define 	MIN_B	    0x80

#define 	MAX_UH		0xFFFF
#define 	MAX_H	    0x7FFF
#define 	MIN_H	    0x8000

#define 	MAX_UW      0xFFFFFFFF
#define 	MAX_W       0x7FFFFFFF
#define 	MIN_W       0x80000000
//------------------------------------------------------------------------------
//..bit単位アクセス用構造体
typedef union {
	UB V;
	struct {
		UB b0  : 1 ;
		UB b1  : 1 ;
		UB b2  : 1 ;
		UB b3  : 1 ;
		UB b4  : 1 ;
		UB b5  : 1 ;
		UB b6  : 1 ;
		UB b7  : 1 ;
	} B;
} UB_BIT;
//------------------------------------------------------------------------------



#ifdef	__cplusplus
}
#endif

#endif	/* DATA_TYPES_H */

