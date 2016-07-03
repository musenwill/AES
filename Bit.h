/******************************************************************************\
Copyright(C):	Huazhong University of Science & Technology.
	FileName:	Bit.h
	  Author:	Zhulinfeng
	 Version:	1.0
	    Date:	2014-3-1
 Description:	Interface of some operation on bits.
	  Others:	
	----------------------------------------------------------------------------		
	Modification History		
	Date			Author			Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD
\******************************************************************************/

#ifndef BIT_H
#define BIT_H

#define BIT1	(0x80)
#define BIT2	(0x40)
#define BIT3	(0x20)
#define BIT4	(0x10)
#define BIT5	(0x08)
#define BIT6	(0x04)
#define BIT7	(0x02)
#define BIT8	(0x01)

#define BIT1_	(0x7F)
#define BIT2_	(0xBF)
#define BIT3_	(0xDF)
#define BIT4_	(0xEF)
#define BIT5_	(0xF7)
#define BIT6_	(0xFB)
#define BIT7_	(0xFD)
#define BIT8_	(0xFE)


PUBLIC bool DS_GetBit(IN byte *pbbytes, IN unsigned int iposition);
PUBLIC void DS_SetBit(INOUT byte *pbbytes, IN unsigned int iposition, IN bool bvalue);
PUBLIC byte DS_16to2(IN char chex);
PUBLIC void DS_LeftShiftN(INOUT byte *pbbyte, IN unsigned int ilength, IN unsigned int ibits);

#endif
