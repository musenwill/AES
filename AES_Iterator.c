/******************************************************************************\
Copyright(C):	Huazhong University of Science & Technology.
	FileName:	AES_Iterator.c
	  Author:	Zhulinfeng
	 Version:	1.0
	    Date:	2014-3-16
 Description:	Some functions to encrypt and decrypt data.
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
#include "Length.h"
#include "Table.h"
#include "AES_Key.h"
#include "AES_Iterator.h"

/**function statements**/
PRIVATE void AES_Iter_SubBytes(INOUT byte *pbstate, IN unsigned int uilength);
PRIVATE void AES_Iter_ShiftRows(INOUT byte *pbstate, IN unsigned int uilength);
PRIVATE void AES_Iter_MixColumns(INOUT byte *pbstate, IN unsigned int uilength);
PRIVATE void AES_Iter_SubBytesInv(INOUT byte *pbstate, IN unsigned int uilength);
PRIVATE void AES_Iter_ShiftRowsInv(INOUT byte *pbstate, IN unsigned int uilength);
PRIVATE void AES_Iter_MixColumnsInv(INOUT byte *pbstate, IN unsigned int uilength);
PRIVATE void AES_Iter_AddRoundKey(INOUT byte *pbstate, IN byte *pbroundKey, IN unsigned int uilength);
PRIVATE int AES_Iter_ByteToInt(IN byte bbyte);
PRIVATE void AES_Iter_Transpot(INOUT byte *pbstate, IN unsigned int uilength);

/*******************************************************************************
	Func Name:	AES_Iter_Encrypt
 Date Created:	2014-3-17
 	   Author:	Zhulinfeng
  Description:	Encrypt 16 bytes data.
  		Input:	byte *pbcode			16 bytes data to be encrypted.
  				unsigned int uilength	Length of data to be encrypted, is 16.
  				AES_Key_S *psKey		Instance of key.
  	   Output:	byte *pbcode			encrypted data, 16 bytes.	
  	   Return:	NONE
  	  Caution:	pbcode and psKey should not be null, uilength must be 16.
  	  			All these guaranteed by caller.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC void AES_Iter_Encrypt(INOUT byte *pbcode, IN unsigned int uilength, IN struct AES_Key_S *psKey)
{
	assert(NULL != pbcode);
	assert(NULL != psKey);
	assert(AES_PAGE_SIZE == uilength);
	
	AES_Iter_Transpot(pbcode, AES_PAGE_SIZE);
	
	AES_Iter_AddRoundKey(pbcode, AES_Key_GetTransKey(psKey, 0), AES_PAGE_SIZE);
	
	int icount = 1;
	for(icount; icount < AES_NR; icount++)
	{
		AES_Iter_SubBytes(pbcode, AES_PAGE_SIZE);
		AES_Iter_ShiftRows(pbcode, AES_PAGE_SIZE);
		AES_Iter_MixColumns(pbcode, AES_PAGE_SIZE);
		AES_Iter_AddRoundKey(pbcode, AES_Key_GetTransKey(psKey, icount), AES_PAGE_SIZE);
	}
	
	AES_Iter_SubBytes(pbcode, AES_PAGE_SIZE);
	AES_Iter_ShiftRows(pbcode, AES_PAGE_SIZE);
	AES_Iter_AddRoundKey(pbcode, AES_Key_GetTransKey(psKey, AES_NR), AES_PAGE_SIZE);
	
	AES_Iter_Transpot(pbcode, AES_PAGE_SIZE);	
	
	return;
}

/*******************************************************************************
	Func Name:	AES_Iter_Decrypt
 Date Created:	2014-3-17
 	   Author:	Zhulinfeng
  Description:	Decrypt 16 bytes data.
  		Input:	byte *pbcode			16 bytes data to be decrypted.
  				unsigned int uilength	Length of data to be decrypted, is 16.
  				AES_Key_S *psKey		Instance of key.
  	   Output:	byte *pbcode			decrypted data, 16 bytes.	
  	   Return:	NONE
  	  Caution:	pbcode and psKey should not be null, uilength must be 16.
  	  			All these guaranteed by caller.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC void AES_Iter_Decrypt(INOUT byte *pbcode, IN unsigned int uilength, IN struct AES_Key_S *psKey)
{
	assert(NULL != pbcode);
	assert(NULL != psKey);
	assert(AES_PAGE_SIZE == uilength);
	
	AES_Iter_Transpot(pbcode, AES_PAGE_SIZE);
	
	AES_Iter_AddRoundKey(pbcode, AES_Key_GetTransKey(psKey, AES_NR), AES_PAGE_SIZE);
	
	int icount = 1;
	for(icount; icount < AES_NR; icount++)
	{
		AES_Iter_ShiftRowsInv(pbcode, AES_PAGE_SIZE);
		AES_Iter_SubBytesInv(pbcode, AES_PAGE_SIZE);
		AES_Iter_AddRoundKey(pbcode, AES_Key_GetTransKey(psKey, AES_NR - icount), AES_PAGE_SIZE);
		AES_Iter_MixColumnsInv(pbcode, AES_PAGE_SIZE);
	}
	
	AES_Iter_ShiftRowsInv(pbcode, AES_PAGE_SIZE);
	AES_Iter_SubBytesInv(pbcode, AES_PAGE_SIZE);
	AES_Iter_AddRoundKey(pbcode, AES_Key_GetTransKey(psKey, 0), AES_PAGE_SIZE);
	
	AES_Iter_Transpot(pbcode, AES_PAGE_SIZE);	
	
	return;	
}

/*******************************************************************************
	Func Name:	AES_Iter_SubBytes
 Date Created:	2014-3-17
 	   Author:	Zhulinfeng
  Description:	subbytes.
  		Input:	byte *pbstate			16 bytes data to subbytes
  				unsigned int uilength 	Length of pbstate, 16 bytes.
  	   Output:	byte *pbstate			End subbytes, 16 bytes.	
  	   Return:	NONE
  	  Caution:	pbstate should not be null, uilength must be 16.
  	  			All these guaranteed by caller.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE void AES_Iter_SubBytes(INOUT byte *pbstate, IN unsigned int uilength)
{	
	assert(NULL != pbstate);
	assert(AES_PAGE_SIZE == uilength);
	
	int icount = 0;
	for(icount; icount < AES_PAGE_SIZE; icount++)
	{
		byte tmp = *(pbstate + icount);
		int irow = (tmp >> 4) & 0x0f;
		int icol = tmp & 0x0f;
		*(pbstate + icount) = S_BOX[irow][icol];
	}
	
	return;	
}

/*******************************************************************************
	Func Name:	AES_Iter_ShiftRows
 Date Created:	2014-3-17
 	   Author:	Zhulinfeng
  Description:	shiftRows.
  		Input:	byte *pbstate			16 bytes data to shiftRows
  				unsigned int uilength 	Length of pbstate, 16 bytes.
  	   Output:	byte *pbstate			End shiftRows, 16 bytes.	
  	   Return:	NONE
  	  Caution:	pbstate should not be null, uilength must be 16.
  	  			All these guaranteed by caller.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE void AES_Iter_ShiftRows(INOUT byte *pbstate, IN unsigned int uilength)
{
	assert(NULL != pbstate);
	assert(AES_PAGE_SIZE == uilength);
	
	int irow = 0;
	for(irow; irow < AES_NB; irow++)
	{
		int j = 0;
		for(j; j < irow; j++)
		{
			byte tmp = *(pbstate + irow*AES_NB);
			int i = 0;
			for(i; i < 3; i++)
			{
				*(pbstate + irow*AES_NB + i) = *(pbstate + irow*AES_NB + i + 1);
			}
			*(pbstate + irow*AES_NB + 3) = tmp;
		}
	}
}

/*******************************************************************************
	Func Name:	AES_Iter_MixColumns
 Date Created:	2014-3-17
 	   Author:	Zhulinfeng
  Description:	Mixcolumns.
  		Input:	byte *pbstate			16 bytes data to mixcolumns
  				unsigned int uilength 	Length of pbstate, 16 bytes.
  	   Output:	byte *pbstate			End mixcolumns, 16 bytes.	
  	   Return:	NONE
  	  Caution:	pbstate should not be null, uilength must be 16.
  	  			All these guaranteed by caller.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE void AES_Iter_MixColumns(INOUT byte *pbstate, IN unsigned int uilength)
{
	assert(NULL != pbstate);
	assert(AES_PAGE_SIZE == uilength);
	
	/**get a copy first**/
	byte btmp[AES_PAGE_SIZE];
	int icount = 0;
	for(icount; icount < AES_PAGE_SIZE; icount++)
	{
		btmp[icount] = *(pbstate + icount);
	}
	
	int icol = 0;
	for(icol; icol < AES_NB; icol++)
	{
		*(pbstate + icol 	)  = MUL_02[AES_Iter_ByteToInt(btmp[0 + icol])] ^ 
								MUL_03[AES_Iter_ByteToInt(btmp[4 + icol])] 	^ 
								btmp[8 + icol]        						^ 
								btmp[12 + icol];
		*(pbstate + icol + 4)  = btmp[0 + icol]         					^ 
								MUL_02[AES_Iter_ByteToInt(btmp[4 + icol])] 	^ 
								MUL_03[AES_Iter_ByteToInt(btmp[8 + icol])] 	^ 
								btmp[12 + icol];
		*(pbstate + icol + 8)  = btmp[0 + icol]        		 				^ 
								btmp[4 + icol]         						^ 
								MUL_02[AES_Iter_ByteToInt(btmp[8 + icol])] 	^ 
								MUL_03[AES_Iter_ByteToInt(btmp[12 + icol])];
		*(pbstate + icol + 12) = MUL_03[AES_Iter_ByteToInt(btmp[0 + icol])] ^ 
								btmp[4 + icol]         						^ 
								btmp[8 + icol]         						^ 
								MUL_02[AES_Iter_ByteToInt(btmp[12 + icol])];
	}

	return;	
}

/*******************************************************************************
	Func Name:	AES_Iter_SubBytesInv
 Date Created:	2014-3-17
 	   Author:	Zhulinfeng
  Description:	subbytes inverse.
  		Input:	byte *pbstate			16 bytes data to subbytes inverse
  				unsigned int uilength 	Length of pbstate, 16 bytes.
  	   Output:	byte *pbstate			End subbytes inverse, 16 bytes.	
  	   Return:	NONE
  	  Caution:	pbstate should not be null, uilength must be 16.
  	  			All these guaranteed by caller.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE void AES_Iter_SubBytesInv(INOUT byte *pbstate, IN unsigned int uilength)
{	
	assert(NULL != pbstate);
	assert(AES_PAGE_SIZE == uilength);
	
	int icount = 0;
	for(icount; icount < AES_PAGE_SIZE; icount++)
	{
		byte tmp = *(pbstate + icount);
		int irow = (tmp >> 4) & 0x0f;
		int icol = tmp & 0x0f;
		*(pbstate + icount) = S_BOX_[irow][icol];
	}
	
	return;	
}

/*******************************************************************************
	Func Name:	AES_Iter_ShiftRowsInv
 Date Created:	2014-3-17
 	   Author:	Zhulinfeng
  Description:	shiftRows inverse.
  		Input:	byte *pbstate			16 bytes data to shiftRows inverse
  				unsigned int uilength 	Length of pbstate, 16 bytes.
  	   Output:	byte *pbstate			End shiftRows inverse, 16 bytes.	
  	   Return:	NONE
  	  Caution:	pbstate should not be null, uilength must be 16.
  	  			All these guaranteed by caller.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE void AES_Iter_ShiftRowsInv(INOUT byte *pbstate, IN unsigned int uilength)
{
	assert(NULL != pbstate);
	assert(AES_PAGE_SIZE == uilength);
	
	int irow = 0;
	for(irow; irow < AES_NB; irow++)
	{
		int j = 0;
		for(j; j < irow; j++)
		{
			byte tmp = *(pbstate + irow*AES_NB + 3);
			int i = 3;
			for(i; i > 0; i--)
			{
				*(pbstate + irow*AES_NB + i) = *(pbstate + irow*AES_NB + i - 1);
			}
			*(pbstate + irow*AES_NB ) = tmp;
		}
	}
}

/*******************************************************************************
	Func Name:	AES_Iter_MixColumnsinv
 Date Created:	2014-3-17
 	   Author:	Zhulinfeng
  Description:	Mixcolumns inverse.
  		Input:	byte *pbstate			16 bytes data to mixcolumns inverse
  				unsigned int uilength 	Length of pbstate, 16 bytes.
  	   Output:	byte *pbstate			End mixcolumns inverse, 16 bytes.	
  	   Return:	NONE
  	  Caution:	pbstate should not be null, uilength must be 16.
  	  			All these guaranteed by caller.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE void AES_Iter_MixColumnsInv(INOUT byte *pbstate, IN unsigned int uilength)
{
	assert(NULL != pbstate);
	assert(AES_PAGE_SIZE == uilength);
	
	/**get a copy first**/
	byte btmp[AES_PAGE_SIZE];
	int icount = 0;
	for(icount; icount < AES_PAGE_SIZE; icount++)
	{
		btmp[icount] = *(pbstate + icount);
	}
	
	int icol = 0;
	for(icol; icol < AES_NB; icol++)
	{
		*(pbstate + icol 	)  = MUL_0e[AES_Iter_ByteToInt(btmp[0 + icol])] ^ 
								 MUL_0b[AES_Iter_ByteToInt(btmp[4 + icol])] ^ 
								 MUL_0d[AES_Iter_ByteToInt(btmp[8 + icol])] ^ 
								 MUL_09[AES_Iter_ByteToInt(btmp[12 + icol])] ;
								 
		*(pbstate + icol + 4)  = MUL_09[AES_Iter_ByteToInt(btmp[0 + icol])] ^ 
								 MUL_0e[AES_Iter_ByteToInt(btmp[4 + icol])] ^ 
								 MUL_0b[AES_Iter_ByteToInt(btmp[8 + icol])] ^ 
								 MUL_0d[AES_Iter_ByteToInt(btmp[12 + icol])] ;
								 
		*(pbstate + icol + 8)  = MUL_0d[AES_Iter_ByteToInt(btmp[0 + icol])] ^ 
								 MUL_09[AES_Iter_ByteToInt(btmp[4 + icol])] ^ 
								 MUL_0e[AES_Iter_ByteToInt(btmp[8 + icol])] ^ 
								 MUL_0b[AES_Iter_ByteToInt(btmp[12 + icol])] ;
								 
		*(pbstate + icol + 12) = MUL_0b[AES_Iter_ByteToInt(btmp[0 + icol])] ^ 
								 MUL_0d[AES_Iter_ByteToInt(btmp[4 + icol])] ^ 
								 MUL_09[AES_Iter_ByteToInt(btmp[8 + icol])] ^ 
								 MUL_0e[AES_Iter_ByteToInt(btmp[12 + icol])] ;
	}

	return;	
}

/*******************************************************************************
	Func Name:	AES_Iter_AddRoundKey
 Date Created:	2014-3-17
 	   Author:	Zhulinfeng
  Description:	subbytes.
  		Input:	byte *pbstate			16 bytes data to addRoundKey
  				unsigned int uilength 	Length of pbstate, 16 bytes.
  	   Output:	byte *pbstate			End addRoundKey, 16 bytes.	
  	   Return:	NONE
  	  Caution:	pbstate should not be null, uilength must be 16.
  	  			All these guaranteed by caller.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE void AES_Iter_AddRoundKey(INOUT byte *pbstate, IN byte *pbroundKey, IN unsigned int uilength)
{
	assert(NULL != pbstate);
	assert(NULL != pbroundKey);
	assert(AES_PAGE_SIZE == uilength);
	
	int icount = 0;
	for(icount; icount < AES_PAGE_SIZE; icount++)
	{
		*(pbstate + icount) ^= *(pbroundKey + icount);
	}	
}

/*******************************************************************************
	Func Name:	AES_Iter_ByteToInt
 Date Created:	2014-3-17
 	   Author:	Zhulinfeng
  Description:	Change one byte to an int number.
  		Input:	byte bbyte				The byte to be changed.
  	   Output:	NONE
  	   Return:	int						Changed int number.
  	  Caution:	NONE
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE int AES_Iter_ByteToInt(IN byte bbyte)
{
	int irow = (bbyte >> 4) & 0x0f;
	int icol = bbyte & 0x0f;
	
	return 16 * irow + icol;
}

/*******************************************************************************
	Func Name:	AES_Iter_Transpot
 Date Created:	2014-3-17
 	   Author:	Zhulinfeng
  Description:	Inverse a 4x4 byte matrix.
  		Input:	byte *pbstate			matrix to be inversed.
  				unsigned int uilength	Length of matrix.
  	   Output:	byte *pbstate			Inversed matrix.
  	   Return:	NONE
  	  Caution:	pbstate should not be null and must be 16 bytes length.
  	  			These guaranteed by caller.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE void AES_Iter_Transpot(INOUT byte *pbstate, IN unsigned int uilength)
{
	assert(NULL != pbstate);
	assert(AES_PAGE_SIZE == uilength);
	
	int irow = 0;
	for(irow; irow < 4; irow++)
	{
		int icol = 0;
		for(icol; icol < irow; icol++)
		{
			byte tmp = *(pbstate + irow*4 + icol);
			*(pbstate + irow*4 + icol) = *(pbstate + icol*4 + irow);
			*(pbstate + icol*4 + irow) = tmp;
		}
	}
	return;	
}
