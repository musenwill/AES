/******************************************************************************\
Copyright(C):	Huazhong University of Science & Technology.
	FileName:	AES_Key.c
	  Author:	Zhulinfeng
	 Version:	1.0
	    Date:	2014-3-1
 Description:	Some functions on keys.
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

/**function statements**/
PRIVATE void AES_Key_RotByte(INOUT byte *pbword);
PRIVATE void AES_Key_SubByte(INOUT byte *pbword);
PRIVATE void AES_Key_Rcon(INOUT byte *pbword, IN unsigned int iindex);
PRIVATE void AES_Key_Transpot(INOUT byte *pbroundKey, IN unsigned int ilength);
/**global values**/
PRIVATE const byte RCON[10][4] = 
{
	0x01,0x00,0x00,0x00,
	0x02,0x00,0x00,0x00,
	0x04,0x00,0x00,0x00,
	0x08,0x00,0x00,0x00,
	0x10,0x00,0x00,0x00,
	0x20,0x00,0x00,0x00,
	0x40,0x00,0x00,0x00,
	0x80,0x00,0x00,0x00,
	0x1b,0x00,0x00,0x00,
	0x36,0x00,0x00,0x00
};

/*******************************************************************************
	Func Name:	AES_Key_Set
 Date Created:	2014-3-2
 	   Author:	Zhulinfeng
  Description:	Get 16 byte key, set it in struct AES_Key_S.
  		Input:	AES_Key_S *psKey		Address of the instance of AES_Key_S.
  				byte *pbkey				16 bytes key.
  				unsigned int ilength	The length of key, must be 16.
  	   Output:	AES_Key_S *psKey		
  	   Return:	NONE
  	  Caution:	psKey should not be null, bkey must be 16 bytes,ilength must be 
  	  			16, all these guaranteed by caller.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC void AES_Key_Set(INOUT struct AES_Key_S *psKey, IN byte *pbkey, IN unsigned int ilength)
{
	assert(NULL != psKey);
	assert(NULL != pbkey);
	assert(AES_KEY_SIZE == ilength);
	
	int icount = 0;
	for(icount; icount < ilength; icount++)
	{
		psKey -> bkey[icount] = *(pbkey + icount);
	}
}

/*******************************************************************************
	Func Name:	AES_Key_GetRoundKey
 Date Created:	2014-3-16
 	   Author:	Zhulinfeng
  Description:	Get 16 byte round key.
  		Input:	AES_Key_S *psKey		Address of the instance of AES_Key_S.
  				unsigned int iindex		Which round key, less than 11.
  	   Output:	NONE	
  	   Return:	*byte					The first address of the round key.	
  	  Caution:	psKey should not be null, iindex smaller than 11, these 
  	  			guaranteed by caller.
  	  			This function must be used after AES_Key_Make
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC byte* AES_Key_GetRoundKey(IN struct AES_Key_S *psKey, IN unsigned int iindex)
{
	assert(NULL != psKey);
	assert(iindex >=0 && iindex < AES_NR + 1);
	
	return psKey -> bsubKeys[iindex];
}

/*******************************************************************************
	Func Name:	AES_Key_GetTransKey
 Date Created:	2014-3-16
 	   Author:	Zhulinfeng
  Description:	Get 16 byte round key.
  		Input:	AES_Key_S *psKey		Address of the instance of AES_Key_S.
  				unsigned int iindex		Which round key, less than 11.
  	   Output:	NONE	
  	   Return:	*byte					The first address of the round key.	
  	  Caution:	psKey should not be null, iindex smaller than 11, these 
  	  			guaranteed by caller.
  	  			This function must be used after AES_Key_Make
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC byte* AES_Key_GetTransKey(IN struct AES_Key_S *psKey, IN unsigned int iindex)
{
	assert(NULL != psKey);
	assert(iindex >=0 && iindex < AES_NR + 1);
	
	return psKey -> btransKeys[iindex];
}

/*******************************************************************************
	Func Name:	AES_Key_Make
 Date Created:	2014-3-3
 	   Author:	Zhulinfeng
  Description:	Make all the sub keys.
  		Input:	AES_Key_S *psKey		Address of the instance of AES_Key_S.
  	   Output:	AES_Key_S *psKey		
  	   Return:	NONE
  	  Caution:	This function should be used after function AES_Key_Set.
  	  			This function should be used after AES_Key_Set.
  	  			All these guaranteed by caller.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC void AES_Key_Make(INOUT struct AES_Key_S *psKey)
{
	assert(NULL != psKey);
	
	byte *pbroundKey = psKey -> bsubKeys[0];
	
	int icopyCount = 0;
	for(icopyCount; icopyCount < AES_PAGE_SIZE; icopyCount++)
	{
		*(pbroundKey + icopyCount) = psKey -> bkey[icopyCount];
	}
	
	byte *pbtmpWord = pbroundKey + AES_PAGE_SIZE;
	int iroundCount = AES_NK;
	for(iroundCount; iroundCount < AES_NB*(AES_NR+1); iroundCount++)
	{
		int j = 0;
		for(j; j < 4; j++)
		{
			*(pbtmpWord + j) = *(pbroundKey + 4*iroundCount + j - 4);
		}
		
		if(0 == iroundCount % AES_NK)
		{
			AES_Key_RotByte(pbtmpWord);
			AES_Key_SubByte(pbtmpWord);
			AES_Key_Rcon(pbtmpWord, iroundCount/AES_NK - 1);
		}
		
		int k = 0;
		for(k; k < 4; k++)
		{
			*(pbtmpWord + k) ^= *(pbroundKey + (iroundCount-AES_NK)*4 + k);
		}
		
		pbtmpWord += 4;
	}
	
	byte *btmpdest = psKey -> btransKeys[0];
	byte *btmpsour = psKey -> bsubKeys[0];
	int jcopyCount = 0;
	for(jcopyCount; jcopyCount < (AES_NR + 1) *AES_PAGE_SIZE; jcopyCount++)
	{
		*(btmpdest + jcopyCount) = *(btmpsour + jcopyCount);
	}
	
	int itransCount = 0;
	for(itransCount; itransCount < AES_NR + 1; itransCount++)
	{
		AES_Key_Transpot(psKey-> btransKeys[itransCount], AES_KEY_SIZE);
	}
	
	
	return;
}

/*******************************************************************************
	Func Name:	AES_Key_RotByte
 Date Created:	2014-3-16
 	   Author:	Zhulinfeng
  Description:	Rot byte.
  		Input:	byte *pbword			word to be roted
  	   Output:	byte *pbword		
  	   Return:	NONE
  	  Caution:	pbword should not be null, these guaranteed by
  	  			caller.
  	  			Called by AES_Key_SubByte.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE void AES_Key_RotByte(INOUT byte *pbword)
{
	assert(NULL != pbword);
	
	byte tmp = *pbword;
	*(pbword) = *(pbword + 1);
	*(pbword + 1) = *(pbword + 2);
	*(pbword + 2) = *(pbword + 3);
	*(pbword + 3) = tmp;
	
	return;
}

/*******************************************************************************
	Func Name:	AES_Key_SubByte
 Date Created:	2014-3-16
 	   Author:	Zhulinfeng
  Description:	subbyte.
  		Input:	byte *pbword			word to be sub byte
  	   Output:	byte *pbword		
  	   Return:	NONE
  	  Caution:	pbword should no be null, guaranteed by caller.
  	  			Called by AES_Key_Make.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE void AES_Key_SubByte(INOUT byte *pbword)
{
	assert(NULL != pbword);
	
	int icount = 0;
	for(icount; icount < 4; icount++)
	{
		byte tmp = *(pbword + icount);
		int irow = (tmp >> 4) & 0x0f;
		int icol = tmp & 0x0f;
		*(pbword + icount) = S_BOX[irow][icol];
	}
	
	return;
}

/*******************************************************************************
	Func Name:	AES_Key_Rcon
 Date Created:	2014-3-16
 	   Author:	Zhulinfeng
  Description:	rcon.
  		Input:	byte *pbword			word to rcon
  				unsigned int iindex		choose which rcon
  	   Output:	byte *pbword		
  	   Return:	NONE
  	  Caution:	pbword should no be null, guaranteed by caller.
  	  			Called by AES_Key_Make.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  				
*******************************************************************************/
PRIVATE void AES_Key_Rcon(INOUT byte *pbword, IN unsigned int iindex)
{
	assert(NULL != pbword);
	assert(iindex >= 0 && iindex < 10);
	
	int icount = 0;
	for(icount; icount < 4; icount++)
	{
		*(pbword + icount) ^= RCON[iindex][icount];
	}
	
	return;
}

PRIVATE void AES_Key_Transpot(INOUT byte *pbroundKey, IN unsigned int uilength)
{
	assert(NULL != pbroundKey);
	assert(AES_KEY_SIZE == uilength);
	
	int irow = 0;
	for(irow; irow < 4; irow++)
	{
		int icol = 0;
		for(icol; icol < irow; icol++)
		{
			byte tmp = *(pbroundKey + irow*4 + icol);
			*(pbroundKey + irow*4 + icol) = *(pbroundKey + icol*4 + irow);
			*(pbroundKey + icol*4 + irow) = tmp;
		}
	}
	return;		
}



