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
//.$BDj?tDj5A(B
//..$B%G!<%?7?Dj5A(B
typedef		signed char	B;							// $BId9fIU$-(B 8$B%S%C%H@0?t(B
typedef		signed short	H;							// $BId9fIU$-(B16$B%S%C%H@0?t(B
typedef		signed long		W;							// $BId9fIU$-(B32$B%S%C%H@0?t(B
typedef		unsigned char	UB;							// $BId9fL5$7(B 8$B%S%C%H@0?t(B
typedef		unsigned short	UH;							// $BId9fL5$7(B16$B%S%C%H@0?t(B
typedef		unsigned long	UW;							// $BId9fL5$7(B32$B%S%C%H@0?t(B
typedef		unsigned char	BOOL;

typedef		float			FLOAT;						// $BC1@:EY(B(32$B%S%C%H(B)$BIbF0>.?tE@(B
typedef		double			DOUBLE;						// $BG\@:EY(B(64$B%S%C%H(B)$BIbF0>.?tE@(B

//------------------------------------------------------------------------------
//..$B%-!<%o!<%IDj5A(B
#define 	FALSE	0
#define 	TRUE	1

#ifndef __cplsuplus
	#define NULL_PTR ((void*)0)
#else
	#define NULL_PTR __null
#endif

#define	NULL_FAR_PTR ((void __far *)0)
//------------------------------------------------------------------------------
//..$B%U%i%0>uBVDj5A(B
#define 	NA			0								// $BHsBP1~(B
#define 	OFF			0								// OFF
#define 	ON			1								// ON

#define		LOW			0
#define		HIGH		1
//------------------------------------------------------------------------------
//..$B:GBgCMDj5A(B
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
//..bit$BC10L%"%/%;%9MQ9=B$BN(B
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

