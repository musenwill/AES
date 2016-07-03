/******************************************************************************\
Copyright(C):	Huazhong University of Science & Technology.
	FileName:	AES_Key.h
	  Author:	Zhulinfeng
	 Version:	1.0
	    Date:	2014-3-2
 Description:	Interface of some operation on keys.
	  Others:	
	----------------------------------------------------------------------------		
	Modification History		
	Date			Author			Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD
\******************************************************************************/

#ifndef AES_KEY_H
#define AES_KEY_H

PUBLIC struct AES_Key_S
{
	PUBLIC byte bkey[AES_KEY_SIZE];
	PUBLIC byte bsubKeys[AES_NR + 1][AES_KEY_SIZE];
	PUBLIC byte btransKeys[AES_NR + 1][AES_KEY_SIZE];
};

PUBLIC void  AES_Key_Set(INOUT struct AES_Key_S *psKey, IN byte *pbkey, IN unsigned int ilength);
PUBLIC byte* AES_Key_GetRoundKey(IN struct AES_Key_S *psKey, IN unsigned int iindex);
PUBLIC byte* AES_Key_GetTransKey(IN struct AES_Key_S *psKey, IN unsigned int iindex);
PUBLIC void  AES_Key_Make(INOUT struct AES_Key_S *psKey);

#endif
