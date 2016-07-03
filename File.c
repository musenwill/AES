/******************************************************************************\
Copyright(C):	Huazhong University of Science & Technology.
	FileName:	File.c
	  Author:	Zhulinfeng
	 Version:	1.0
	    Date:	2014-3-9
 Description:	Operation on files.
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
#include "File.h"

/*******************************************************************************
	Func Name:	File_Init
 Date Created:	2014-3-11
 	   Author:	Zhulinfeng
  Description:	Init struct File_S *psFile.
  		Input:	File_S *psFile			Storation of some temp data during read
  										or write file.
  	   Output:	File_S *psFile		
  	   Return:	NONE
  	  Caution:	psFile should not be null, this function should be used first.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC void File_Init(INOUT struct File_S *psFile)
{
	assert(NULL != psFile);
	
	psFile -> uibase = 0;
	psFile -> ulfileOffset = 0;
	psFile -> uibyteCount = 0;
	psFile -> uicompletion = 0;
	psFile -> pfinFd = NULL;
	psFile -> pfoutFd = NULL;
	psFile -> sinFilePath = NULL;
	psFile -> soutFilePath = NULL;
}

/*******************************************************************************
	Func Name:	File_Finit
 Date Created:	2014-3-11
 	   Author:	Zhulinfeng
  Description:	Init struct File_S *psFile.
  		Input:	File_S *psFile			Storation of some temp data during read
  										or write file.
  	   Output:	File_S *psFile		
  	   Return:	int
  	   			DS_SUCCESS
  	   			DS_SYSERR				Error occured when close file fd.
  	  Caution:	psFile should not be null, this function should be used at last
  	  			to set off all the resources when psFile will never be used.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC int File_Finit(INOUT struct File_S *psFile)
{
	assert(NULL != psFile);
	
	int iresultCode1 = 0;
	int iresultCode2 = 0;
	if(NULL != psFile -> pfinFd)
	{
		iresultCode1 = fclose(psFile -> pfinFd);
	}
	if(NULL != psFile -> pfoutFd)
	{
		iresultCode2 = fclose(psFile -> pfoutFd);
	}

	psFile -> uibase = 0;
	psFile -> ulfileOffset = 0;
	psFile -> uibyteCount = 0;
	psFile -> uicompletion = 0;
	psFile -> pfinFd = NULL;
	psFile -> pfoutFd = NULL;
	psFile -> sinFilePath = NULL;
	psFile -> soutFilePath = NULL;
	
	if(0 == iresultCode1 && 0 == iresultCode2)
	{
		return DS_SUCCESS;
	}
	else
	{
		return DS_SYSERR;
	}
}

/*******************************************************************************
	Func Name:	File_OpenInFile
 Date Created:	2014-3-11
 	   Author:	Zhulinfeng
  Description:	Open a file to read from, store file fd in psFile.
  		Input:	string sfilePath		File path, should not be null.
  	   Output:	File_S *psFile			Storation of some temp data during read
  										or write file.
  	   Return:	int
  	   			DS_SUCCESS				Open file successfully.
  	   			DS_FAILED				Error occured when open file.
  	  Caution:	sfilePath and psFile should not be null, this function should be
  	  			used after function File_Init.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC int File_OpenInFile(IN string sfilePath, OUT struct File_S *psFile)
{
	assert(NULL != sfilePath);
	assert(NULL != psFile);
	
	psFile -> sinFilePath = sfilePath;
	
	const char *pcfileName = psFile -> sinFilePath;
	const char *pcmode = "rb";
	
	psFile -> pfinFd = fopen(pcfileName, pcmode);
	
	if(NULL == psFile -> pfinFd)
	{
		return DS_FAILED;
	}
	else
	{
		return DS_SUCCESS;	
	}
}

/*******************************************************************************
	Func Name:	File_OpenOutFile
 Date Created:	2014-3-11
 	   Author:	Zhulinfeng
  Description:	Open a file to write on, store file fd in psFile.
  		Input:	string sfilePath		File path, should not be null.
  	   Output:	File_S *psFile			Storation of some temp data during read
  										or write file.
  	   Return:	int
  	   			DS_SUCCESS				Open file successfully.
  	   			DS_FAILED				Error occured when open file.
  	  Caution:	sfilePath and psFile should not be null, this function should be
  	  			used after function File_Init.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC int File_OpenOutFile(IN string sfilePath, OUT struct File_S *psFile)
{
	assert(NULL != sfilePath);
	assert(NULL != psFile);
	
	psFile -> soutFilePath = sfilePath;
	
	const char *pcfileName = psFile -> soutFilePath;
	const char *pcmode = "wb+";
	
	psFile -> pfoutFd = fopen(pcfileName, pcmode);
	
	if(NULL == psFile -> pfoutFd)
	{
		return DS_FAILED;
	}
	else
	{
		return DS_SUCCESS;	
	}
}

/*******************************************************************************
	Func Name:	File_Read
 Date Created:	2014-3-11
 	   Author:	Zhulinfeng
  Description:	Read some data from input file.
  		Input:	File_S *psFile			Storation of some temp data during read
  										or write file.
  	   Output:	NONE
  	   Return:	int
  	   			DS_SUCCESS				Read file successfully.
  	   			DS_FILE_END				End reading file.
  	  Caution:	sfilePath and psFile should not be null, this function should be
  	  			used after function File_OpenInFile.
  	  			The data read in must align at 8, so this function will complete
  	  			some bytes if neccessary, the number of completed bytes will be
  	  			stored in psFile.
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC int File_Read(INOUT struct File_S *psFile)
{
	assert(NULL != psFile);
	assert(NULL != psFile -> pfinFd);
	
	psFile -> uibyteCount = 0;
	
	if(DATABUF_LENGTH == fread(psFile -> DATABUF, 1, DATABUF_LENGTH, psFile -> pfinFd))
	{
		psFile -> uibyteCount = DATABUF_LENGTH;
		psFile -> ulfileOffset = ftell(psFile -> pfinFd);
		
		return DS_SUCCESS;
	}
	else
	{
		/**step back**/
		fseek(psFile -> pfinFd, psFile -> ulfileOffset, psFile -> uibase);		
		
		while(0 == feof(psFile -> pfinFd))
		{
			psFile -> DATABUF[psFile -> uibyteCount] = fgetc(psFile -> pfinFd);
			psFile -> uibyteCount ++;
		}
		psFile -> uibyteCount--;
		
		/**complet package**/
		psFile -> uicompletion = (PACK_LENGTH - (psFile -> uibyteCount%PACK_LENGTH)) % PACK_LENGTH + PACK_LENGTH;
		int icptCount = 0;		
		for(icptCount; icptCount < psFile -> uicompletion ; icptCount++)
		{
			psFile -> DATABUF[psFile -> uibyteCount] = 0x00;
			psFile -> uibyteCount++;
		}
		psFile -> DATABUF[psFile -> uibyteCount - 1] = (unsigned char)(psFile -> uicompletion);
		
		return DS_FILE_END;
	}
	
	return DS_SUCCESS;
}

PUBLIC int File_Write(INOUT struct File_S *psFile)
{
	assert(NULL != psFile);
	assert(NULL != psFile -> pfoutFd);
	
	if(psFile -> uibyteCount != fwrite(psFile -> DATABUF, 1, psFile -> uibyteCount, psFile -> pfoutFd))
	{
		return DS_FAILED;
	}
	
	return DS_SUCCESS;
}



