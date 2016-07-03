/******************************************************************************\
Copyright(C):	Huazhong University of Science & Technology.
	FileName:	File.h
	  Author:	Zhulinfeng
	 Version:	1.0
	    Date:	2014-3-9
 Description:	Interface for file operations.
	  Others:	
	----------------------------------------------------------------------------		
	Modification History		
	Date			Author			Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD
\******************************************************************************/

#ifndef FILE_H
#define FILE_H

#define DATABUF_LENGTH	(1024 * 4)
#define PACK_LENGTH		16

PUBLIC struct File_S
{
	PUBLIC unsigned int uibase;
	PUBLIC unsigned long ulfileOffset;
	PUBLIC unsigned int  uibyteCount;
	PUBLIC unsigned int  uicompletion;
	PUBLIC FILE *pfinFd;
	PUBLIC FILE *pfoutFd;
	PUBLIC string sinFilePath;
	PUBLIC string soutFilePath;
	PUBLIC char DATABUF[DATABUF_LENGTH + PACK_LENGTH];
};

PUBLIC void File_Init(INOUT struct File_S *psFile);
PUBLIC int File_Finit(INOUT struct File_S *psFile);
PUBLIC int File_OpenInFile(IN string sfilePath, OUT struct File_S *psFile);
PUBLIC int File_OpenOutFile(IN string sfilePath, OUT struct File_S *psFile);
PUBLIC int File_Read(INOUT struct File_S *psFile);
PUBLIC int File_Write(INOUT struct File_S *psFile);

#endif
