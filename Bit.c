/******************************************************************************\
Copyright(C):	Huazhong University of Science & Technology.
	FileName:	Bit.c
	  Author:	Zhulinfeng
	 Version:	1.0
	    Date:	2014-3-1
 Description:	Some functions on bits.
	  Others:	
	----------------------------------------------------------------------------		
	Modification History		
	Date			Author			Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD
\******************************************************************************/

/**include public head files**/
#include "Type.h"
#include "Sys.h"
#include "Err.h"
/**include module head files**/
#include "Bit.h"

/**function statements**/
PRIVATE bool DS_ByteLeftShift(INOUT byte *pbbyte, IN bool brightBit);
PRIVATE void DS_LeftShift1(INOUT byte *pbbyte, IN unsigned int ilength);

/*******************************************************************************
	Func Name:	DS_GetBit
 Date Created:	2014-3-1
 	   Author:	Zhulinfeng
  Description:	Get value of an bit.
  		Input:	byte *pbbytes			The first address of a memery block.
  				unsigned int iposition	Where the value of bit you want to get
  										begin with 0.
  	   Output:	NONE
  	   Return:	false					The value of that bit is 0.
  	   			true					The value of that bit is 1.
  	  Caution:	The iposition should not bigger than the total bits of input
  	  			memery block.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC bool DS_GetBit(IN byte *pbbytes, IN unsigned int iposition)
{
	assert(NULL != pbbytes);	
	
	int irow = iposition / 8;
	int icol = iposition % 8;
	icol++;
	byte bmask;
	
	switch(icol)
	{
	case 1 : bmask = BIT1;	break;
	case 2 : bmask = BIT2;	break;
	case 3 : bmask = BIT3;	break;
	case 4 : bmask = BIT4;	break;
	case 5 : bmask = BIT5;	break;
	case 6 : bmask = BIT6;	break;
	case 7 : bmask = BIT7;	break;
	case 8 : bmask = BIT8;	break;
	}
	
	if( *(pbbytes + irow) & bmask )
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

/*******************************************************************************
	Func Name:	DS_SetBit
 Date Created:	2014-3-1
 	   Author:	Zhulinfeng
  Description:	Set value of an bit.
  		Input:	byte *pbbytes			The first address of a memery block.
  				unsigned int iposition	Where the value of bit you want to get
  										begin with 0.
  				bool bvalue				The value you want to set in
  										true	1
  										false	0
  	   Output:	byte *pbbytes			The changed memery block.
  	   Return:	NONE
  	  Caution:	The iposition should not bigger than the total bits of input
  	  			memery block.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC void DS_SetBit(INOUT byte *pbbytes, IN unsigned int iposition, IN bool bvalue)
{
	assert(NULL != pbbytes);

	int irow = iposition / 8;
	int icol = iposition % 8;
	icol++;
	byte bmask;
	
	if(false == bvalue)
	{
		switch(icol)
		{
			case 1 : {	bmask = BIT1_;	break;}
			case 2 : {	bmask = BIT2_;	break;}
			case 3 : {	bmask = BIT3_;	break;}
			case 4 : {	bmask = BIT4_;	break;}
			case 5 : {	bmask = BIT5_;	break;}
			case 6 : {	bmask = BIT6_;	break;}
			case 7 : {	bmask = BIT7_;	break;}

			case 8 : {	bmask = BIT8_;	break;}
		}	
			
		*(pbbytes + irow) = *(pbbytes + irow) & bmask;
	}
	else
	{
		switch(icol)
		{
			case 1 : {	bmask = BIT1;	break;}
			case 2 : {	bmask = BIT2;	break;}
			case 3 : {	bmask = BIT3;	break;}
			case 4 : {	bmask = BIT4;	break;}
			case 5 : {	bmask = BIT5;	break;}
			case 6 : {	bmask = BIT6;	break;}
			case 7 : {	bmask = BIT7;	break;}
			case 8 : {	bmask = BIT8;	break;}
		}	
		
		*(pbbytes + irow) = *(pbbytes + irow) | bmask;	
	}
	
	return;
}

/*******************************************************************************
	Func Name:	DS_16to2
 Date Created:	2014-3-14
 	   Author:	Zhulinfeng
  Description:	Change Hexadecimal to binary system.
  		Input:	char chex					Input Hexadecimal number, between
  											0-f, guaranteed by caller.
  	   Output:	NONE
  	   Return:	byte						4 bits binary number, put in lower
  	   										4 bits, higher 4 bits are 0
  	  Caution:	Input Hexadecimal number, between 0-f, guaranteed by caller.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC byte DS_16to2(IN char chex)
{	
	bool bflag = (chex >= '0' && chex <= '9') ||
				 (chex >= 'a' && chex <= 'f') ||
				 (chex >= 'A' && chex <= 'F')  ;
	assert(bflag);
	
	byte bret = 0x00;
	
	if( chex >= '0' && chex <= '9' )
	{
		bret |= chex - '0';
	}
	else if(chex >= 'a' && chex <= 'f')
	{
		bret |= chex - 'a' + 10;
	}
	else
	{
		bret |= chex - 'A' + 10;
	}
	
	return bret;
}

/*******************************************************************************
	Func Name:	DS_LeftShiftN
 Date Created:	2014-3-2
 	   Author:	Zhulinfeng
  Description:	Left shift a given memery block by N bits.
  		Input:	byte *pbbyte				The first address of a memery block.
  				unsigned int ilength	The total bytes of the memery block.
  				unsigned int ibits		The number of bits you want to shift
  	   Output:	byte *pbbyte				The changed memery block
  	   Return:	NONE
  	  Caution:	The ilength should not bigger than the total bytes of input
  	  			memery block.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC void DS_LeftShiftN(INOUT byte *pbbyte, IN unsigned int ilength, IN unsigned int ibits)
{
	assert(NULL != pbbyte);	
	
	int icount = 0;
	
	for(icount; icount < ibits; icount++)
	{
		DS_LeftShift1(pbbyte, ilength);
	}	
	
	return;
}

/*******************************************************************************
	Func Name:	DS_ByteLeftShift
 Date Created:	2014-3-2
 	   Author:	Zhulinfeng
  Description:	Left shift a given byte by 1 bit.
  		Input:	byte *pbbyte				The address of the byte.
  				bool brightBit			The bit filled at the last of the byte.
  	   Output:	byte *pbbyte				The changed byte.
  	   Return:	bool					The bit shifted out.
  	  Caution:	bbyte should not be null.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE bool DS_ByteLeftShift(INOUT byte *pbbyte, IN bool brightBit)
{
	assert(NULL != pbbyte);

	bool bleftBit;
	byte bmask;
	
	bleftBit = DS_GetBit(pbbyte, 0);
	*pbbyte = *pbbyte << 1;
	
	if(false == brightBit)
	{
		bmask = BIT8_;
		*pbbyte = *pbbyte & bmask;
	}
	else
	{
		bmask = BIT8;
		*pbbyte = *pbbyte | bmask;
	}
	
	return bleftBit;
}

/*******************************************************************************
	Func Name:	DS_LeftShift1
 Date Created:	2014-3-2
 	   Author:	Zhulinfeng
  Description:	Left shift a given memery block by 1 bit.
  		Input:	byte *pbbyte				The first address of a memery block.
  				unsigned int ilength	The total bytes of the memery block.
  	   Output:	byte *pbbyte				The changed memery block.
  	   Return:	NONE
  	  Caution:	bbyte should not be null.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE void DS_LeftShift1(INOUT byte *pbbyte, IN unsigned int ilength)
{
	assert(NULL != pbbyte);
	
	int icount = ilength - 1;
	bool boutBit;
	
	for(icount; icount >= 0; icount--)
	{
		if(ilength - 1 == icount)
		{
			boutBit = DS_GetBit(pbbyte, 0);
		}
		
		boutBit = DS_ByteLeftShift((pbbyte + icount), boutBit);
	}
	
	return;
}





