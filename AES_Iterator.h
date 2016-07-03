/******************************************************************************\
Copyright(C):	Huazhong University of Science & Technology.
	FileName:	AES_Iterator.h
	  Author:	Zhulinfeng
	 Version:	1.0
	    Date:	2014-3-16
 Description:	Interface of AES encryption and decryption.
	  Others:	
	----------------------------------------------------------------------------		
	Modification History		
	Date			Author			Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD
\******************************************************************************/

#ifndef AES_ITERATOR_H
#define AES_ITERATOR_H

PUBLIC void AES_Iter_Encrypt(INOUT byte *pbcode, IN unsigned int uilength, IN struct AES_Key_S *psKey);
PUBLIC void AES_Iter_Decrypt(INOUT byte *pbcode, IN unsigned int uilength, IN struct AES_Key_S *psKey);

#endif
