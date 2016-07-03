/******************************************************************************\
Copyright(C):	Huazhong University of Science & Technology.
	FileName:	AES_Driver.c
	  Author:	Zhulinfeng
	 Version:	1.0
	    Date:	2014-3-12
 Description:	Use AES encrypt or decrypt files.
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
#include "Bit.h"
#include "AES_Key.h"
#include "AES_Iterator.h"
#include "File.h"
#include "AES_Driver.h"

/**function statements**/
PRIVATE void AES_Driver_Fsm();
PRIVATE void AES_Driver_GetCmd();
PRIVATE void AES_Driver_UnknownCmd();
PRIVATE void AES_Driver_PrintView();
PRIVATE void AES_Driver_Help();
PRIVATE void AES_Driver_LineHelp();
PRIVATE int  AES_Driver_GetLine();
PRIVATE int  AES_Driver_Encrypt();
PRIVATE int  AES_Driver_Decrypt();
PRIVATE int  AES_Driver_CheckKey();

/**global values**/
PRIVATE char cpathBuf[AES_DRIVER_PATHLENGTH];
PRIVATE char ckeyBuf[AES_DRIVER_KEYLENGTH];
PRIVATE enum VIEW geview = START;
PRIVATE enum COMD gecomd = cnone;
PRIVATE enum AESD geaesd = dnone;		
PRIVATE bool 	  gbflag = 1;			//exit program when gbflag = 0

/*******************************************************************************
	Func Name:	AES_Driver_Main
 Date Created:	2014-3-13
 	   Author:	Zhulinfeng
  Description:	Entrance of AES.
  		Input:	NONE
  	   Output:	NONE	
  	   Return:	NONE
  	  Caution:	NONE
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC void AES_Driver_Main()
{
	geview = START;
	gecomd = cnone;
	geaesd = dnone;
	gbflag = 1;
	printf("AES--Enter h to get help\n");
	AES_Driver_PrintView();
	
	while(false != gbflag)
	{
		AES_Driver_GetCmd();
		AES_Driver_Fsm();
	}
}

/*******************************************************************************
	Func Name:	AES_Driver_MainLine
 Date Created:	2014-4-9
 	   Author:	Zhulinfeng
  Description:	Command line entrance of AES.
  		Input:	NONE
  	   Output:	NONE	
  	   Return:	NONE
  	  Caution:	NONE
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC void AES_Driver_MainLine(IN int iargc, IN char *pcArgv[])
{
    assert(NULL != pcArgv);
    
    enum AESD eaesd = dnone;
    
    if(1 == iargc)
    {
        AES_Driver_Main();
    }
    else if(2 == iargc)
    {
        if(0 == strcmp(pcArgv[1], "--help"))
        {
            AES_Driver_Help();
        }
        else
        {
            AES_Driver_LineHelp();
        }
    }
    else if(4 == iargc)
    {
        /** encrypt or decrypt **/
        if(0 == strcmp(pcArgv[1], "-e"))
        {
            eaesd = dencrypt;
        }
        else if(0 == strcmp(pcArgv[1], "-d"))
        {
            eaesd = ddecrypt;
        }
        else
        {
            AES_Driver_LineHelp();
            return;
        }
        
        /**get key and check**/
        unsigned int ikeyLength = strlen(pcArgv[2]);
        if(ikeyLength > AES_DRIVER_KEYLENGTH)
        {
            printf("The input key is too long.\n");
            return;
        }
        unsigned int ikeyCount;
        for(ikeyCount = 0; ikeyCount < ikeyLength; ikeyCount++)
        {
            ckeyBuf[ikeyCount] = *( pcArgv[2] + ikeyCount );
        }
        for(ikeyCount; ikeyCount < AES_DRIVER_KEYLENGTH; ikeyCount++)
        {
            ckeyBuf[ikeyCount] = '0';
        }       
        if(AES_DRIVER_SUCCESS != AES_Driver_CheckKey())
        {
            return;
        }
        
        /**start encrypt or decrypt**/
        unsigned int ipathLength = strlen(pcArgv[3]);
        if(ipathLength > AES_DRIVER_PATHLENGTH - 1)
        {
            printf("The input pathname is too long.\n");
            return;
        }
        unsigned int ipathCount;
        for(ipathCount = 0; ipathCount < ipathLength; ipathCount++)
        {
            cpathBuf[ipathCount] = *(pcArgv[3] + ipathCount);
        }
        cpathBuf[ipathCount] = '\0';
        if(dencrypt == eaesd)
	    {
			(void)AES_Driver_Encrypt();
		}
		else if(ddecrypt == eaesd)
		{
			(void)AES_Driver_Decrypt();
		}
		else
		{
			assert(false);
		}
    }
    else
    {
        AES_Driver_LineHelp();
    }
    
    return;
}

/*******************************************************************************
	Func Name:	AES_Driver_Fsm
 Date Created:	2014-3-13
 	   Author:	Zhulinfeng
  Description:	Change the status of AES prgram and do actions according to 
  				command line.
  		Input:	NONE
  	   Output:	NONE	
  	   Return:	NONE
  	  Caution:	Called by AES_Driver_Main
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE void AES_Driver_Fsm()
{
	switch(geview)
	{
	case START:
	{
		switch(gecomd)
		{
		case cnone:
		{
			AES_Driver_PrintView();
			break;
		}
		case cencrypt:
		{
			geaesd = dencrypt;
			geview = INKEY;
			AES_Driver_PrintView();
			break;
		}
		case cdecrypt:
		{
			geaesd = ddecrypt;
			geview = INKEY;
			AES_Driver_PrintView();
			break;
		}
		case cfile:
		{
			AES_Driver_UnknownCmd();
			AES_Driver_PrintView();
			break;
		}
		case ckey:
		{
			AES_Driver_UnknownCmd();
			AES_Driver_PrintView();
			break;
		}
		case chelp:
		{
			AES_Driver_Help();
			AES_Driver_PrintView();
			break;			
		}
		case cquit:
		{
			gbflag = false;
			break;
		}
		default:
		{
			assert(false);
		}
		}//end of switch
		break;
	}
	case INFILE:
	{
		switch(gecomd)
		{
		case cnone:
		{
			AES_Driver_PrintView();
			break;
		}
		case cencrypt:
		{
			AES_Driver_UnknownCmd();
			AES_Driver_PrintView();
			break;
		}
		case cdecrypt:
		{
			AES_Driver_UnknownCmd();
			AES_Driver_PrintView();
			break;
		}
		case cfile:
		{
			int iret;
			if(dencrypt == geaesd)
			{
				iret = AES_Driver_Encrypt();
			}
			else if(ddecrypt == geaesd)
			{
				iret = AES_Driver_Decrypt();
			}
			else
			{
				assert(false);
			}
			
			if(AES_DRIVER_SYSERR == iret)
			{
				gbflag = false;
			}
			
			geview = INFILE;
			AES_Driver_PrintView();
			break;
		}
		case ckey:
		{
			AES_Driver_UnknownCmd();
			AES_Driver_PrintView();
			break;
		}
		case chelp:
		{
			AES_Driver_Help();
			AES_Driver_PrintView();
			break;			
		}
		case cquit:
		{
			geview = INKEY;
			AES_Driver_PrintView();
			break;
		}
		default:
		{
			assert(false);
		}
		}//end of switch
		break;
	}
	case INKEY:
	{
		switch(gecomd)
		{
		case cnone:
		{
			AES_Driver_PrintView();
			break;
		}
		case cencrypt:
		{
			AES_Driver_UnknownCmd();
			AES_Driver_PrintView();
			break;
		}
		case cdecrypt:
		{
			AES_Driver_UnknownCmd();
			AES_Driver_PrintView();
			break;
		}
		case cfile:
		{
			AES_Driver_UnknownCmd();
			AES_Driver_PrintView();
			break;
		}
		case ckey:
		{
			int iret = AES_Driver_CheckKey();
			if(AES_DRIVER_SUCCESS != iret)
			{
				geview = INKEY;
			}
			else
			{
				geview = INFILE;
			}
			AES_Driver_PrintView();
			break;
		}
		case chelp:
		{
			AES_Driver_Help();
			AES_Driver_PrintView();
			break;			
		}
		case cquit:
		{
			geview = START;
			geaesd = dnone;
			AES_Driver_PrintView();
			break;
		}
		default:
		{
			assert(false);
		}
		}//end of switch
		break;
	}
	default:
	{
		assert(false);
	}
	}//end of switch
	
	return;
}

/*******************************************************************************
	Func Name:	AES_Driver_GetCmd
 Date Created:	2014-3-13
 	   Author:	Zhulinfeng
  Description:	Get command line from console and explain it.
  		Input:	NONE
  	   Output:	NONE	
  	   Return:	NONE
  	  Caution:	Called by AES_Driver_Main
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PUBLIC void AES_Driver_GetCmd()
{	
	char cgetc = getchar();
	while(' ' == cgetc || '\t' == cgetc)
	{
		cgetc = getchar();
	}
	
	switch(cgetc)
	{
		case 'h':
		{
			gecomd = chelp;
			while('\n' != (cgetc = getchar()));
			break;
		}
		case 'q':
		{
			gecomd = cquit;
			while('\n' != (cgetc = getchar()));
			break;
		}
		case 'e':
		{
			gecomd = cencrypt;
			while('\n' != (cgetc = getchar()));
			break;
		}
		case 'd':
		{
			gecomd = cdecrypt;
			while('\n' != (cgetc = getchar()));
			break;
		}
		case '-':
		{
			int iret = AES_Driver_GetLine();
			if(AES_DRIVER_FILE == iret)
			{
				gecomd = cfile;
			}
			else if(AES_DRIVER_KEY == iret)
			{
				gecomd = ckey;
			}
			else
			{
				gecomd = cnone;
				AES_Driver_UnknownCmd();
			}
			break;
		}
		case '\n':
		{
			gecomd = cnone;
			break;
		}
		default:
		{
			gecomd = cnone;
			while('\n' != (cgetc = getchar()));
			AES_Driver_UnknownCmd();
		}
	}
	
	return;
}

/*******************************************************************************
	Func Name:	AES_Driver_UnknownCmd
 Date Created:	2014-3-13
 	   Author:	Zhulinfeng
  Description:	Print message when receive a invalid command.
  		Input:	NONE
  	   Output:	NONE	
  	   Return:	NONE
  	  Caution:	Called by AES_Driver_Fsm
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE void AES_Driver_UnknownCmd()
{
	printf("Unknwon command!\n");
	return;
}

PRIVATE void AES_Driver_PrintView()
{
	switch(geview)
	{
		case START:
		{
			printf("AES:$");
			break;
		}
		case INFILE:
		{
			if(dencrypt == geaesd)
			{
				printf("AES-ENCRYPT-FILE:$");
			}
			else if(ddecrypt == geaesd)
			{
				printf("AES-DECRYPT-FILE:$");
			}
			else
			{
				assert(false);
			}
			break;
		}
		case INKEY:
		{
			if(dencrypt == geaesd)
			{
				printf("AES-ENCRYPT-KEY:$");
			}
			else if(ddecrypt == geaesd)
			{
				printf("AES-DECRYPT-KEY:$");
			}
			else
			{
				assert(false);
			}
			break;			
		}
		default:
		{
			assert(0);
		}
	}//end of switch
	
	return;
}

/*******************************************************************************
	Func Name:	AES_Driver_Help
 Date Created:	2014-3-13
 	   Author:	Zhulinfeng
  Description:	Print help information.
  		Input:	NONE
  	   Output:	NONE	
  	   Return:	NONE
  	  Caution:	Called by AES_Driver_Fsm
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE void AES_Driver_Help()
{
	printf("+----------------------------------------------------------+\n");
	printf("|                   Welcome use AES                        |\n");
	printf("+----------------------------------------------------------+\n");
	printf("|   h    look for help information                         |\n");
	printf("|   q    step back or exit program                         |\n");
	printf("|   e    choose encrypt file                               |\n");
	printf("|   d    choose decrypt file                               |\n");
	printf("|  -k    1-32 length key.    eg: -k 0123456789abcdef       |\n");
	printf("|  -f    file path           eg: -f /home/username/aa.txt  |\n");
	printf("+----------------------------------------------------------+\n");
	printf("|@zhulinfeng, Huazhong University of Science & Technology. |\n");
	printf("+----------------------------------------------------------+\n");
	return;
}

/*******************************************************************************
	Func Name:	AES_Driver_LineHelp
 Date Created:	2014-4-9
 	   Author:	Zhulinfeng
  Description:	Print help information for command line.
  		Input:	NONE
  	   Output:	NONE	
  	   Return:	NONE
  	  Caution:	Called by AES_Driver_MainLine
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE void AES_Driver_LineHelp()
{
    printf("AES Usage :\n");
    printf("AES : Enter AES.\n");
    printf("AES --help : Look for help information.\n");
    printf("AES -e key filepath : Use AES to encrypt but not enter it.\n");
    printf("AES -d key filepath : Use AES to decrypt but not enter it.\n");
    return;
}

/*******************************************************************************
	Func Name:	AES_Driver_GetLine
 Date Created:	2014-3-13
 	   Author:	Zhulinfeng
  Description:	Get key or infile path from console.
  		Input:	NONE
  	   Output:	NONE	
  	   Return:	int
  	   			AES_DRIVER_ERR			invalid command
  	   			AES_DRIVER_FILE			command is input file path
  	   			AES_DRIVER_KEY			command is input key
  	  Caution:	Called by AES_Driver_Fsm
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE int AES_Driver_GetLine()
{
	int iresultCode = AES_DRIVER_ERR;
	
	char cnext = getchar();
	if('f' == cnext)
	{
		cnext = getchar();
		while(' ' == cnext || '\t' == cnext)
		{
			cnext = getchar();
		}

		int ibyteCount = 0;
		while('\n' != cnext)
		{
			cpathBuf[ibyteCount] = cnext;
			cnext = getchar();
			ibyteCount ++;
			if(ibyteCount >= AES_DRIVER_PATHLENGTH)
			{
				break;
			}
		}	
		if(0 == ibyteCount || ibyteCount >= AES_DRIVER_PATHLENGTH)
		{
			iresultCode = AES_DRIVER_ERR;
		}
		else
		{
			iresultCode = AES_DRIVER_FILE;
			cpathBuf[ibyteCount] = '\0';
		}
	}
	else if('k' == cnext)
	{
		iresultCode = AES_DRIVER_KEY;
		cnext = getchar();
		while(' ' == cnext || '\t' == cnext)
		{
			cnext= getchar();
		}
		
		int ibyteCount = 0;
		while('\n' != cnext)
		{
			ckeyBuf[ibyteCount] = cnext;
			cnext = getchar();
			ibyteCount ++;
			if(ibyteCount >= AES_DRIVER_KEYLENGTH)
			{
				break;
			}
		}	
		if(AES_DRIVER_KEYLENGTH == ibyteCount)
		{
			if('\n' != cnext)
			{
				iresultCode = AES_DRIVER_ERR;
			}
			else
			{
				iresultCode = AES_DRIVER_KEY;
			}
		} 
		else if(0 == ibyteCount)
		{
			iresultCode = AES_DRIVER_ERR;
		}
		else
		{
			iresultCode = AES_DRIVER_KEY;
			while(ibyteCount < AES_DRIVER_KEYLENGTH)
			{
				ckeyBuf[ibyteCount] = '0';
				ibyteCount ++;
			}
		}
	}
	else
	{
		iresultCode = AES_DRIVER_ERR;
	}//end if
	
	while('\n' != cnext)
	{
		cnext = getchar();
	}
	
	return iresultCode;
}

/*******************************************************************************
	Func Name:	AES_Driver_Encrypt
 Date Created:	2014-3-13
 	   Author:	Zhulinfeng
  Description:	Encrypt file.
  		Input:	NONE
  	   Output:	NONE	
  	   Return:	int
  	   			AES_DRIVER_SUCCESS				
  	   			AES_DRIVER_ERR	
  	   			AES_DRIVER_SYSERR				//system error, exit program
  	  Caution:	Called by AES_Driver_Fsm
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE int AES_Driver_Encrypt()
{
	int iret = 	AES_DRIVER_SUCCESS;
	
	struct AES_Key_S 	smyKey;
	struct File_S		smyFile;
	
	/**Get out file path**/
	int ipathCount = 0;
	int ifileNameIndex = 0;
	while( '\0' != cpathBuf[ifileNameIndex])
	{
		ifileNameIndex++;
	}
	while('/' != cpathBuf[ifileNameIndex] && ifileNameIndex > 0)
	{
		ifileNameIndex--;
	}
	char coutFile[AES_DRIVER_PATHLENGTH + 1];
	for(ipathCount; ipathCount < AES_DRIVER_PATHLENGTH + 1; ipathCount++)
	{
		if(ipathCount <= ifileNameIndex)
		{
			coutFile[ipathCount] = cpathBuf[ipathCount];
		}
		else if(1 == ipathCount - ifileNameIndex)
		{
			coutFile[ipathCount] = '_';
		}
		else
		{
			coutFile[ipathCount] = cpathBuf[ipathCount - 1];
		}
		
		if('\0' == coutFile[ipathCount])
		{
			break;
		}
	}
	
	/**open in file and out file**/
	printf("Opening files...\n");
	File_Init(&smyFile);
	if(DS_SUCCESS != File_OpenInFile(cpathBuf, &smyFile) )
	{
		iret = AES_DRIVER_ERR;
		if(DS_SUCCESS != File_Finit(&smyFile))
		{
			iret = AES_DRIVER_SYSERR;
		}
		printf("Invalid file path!\n");
		return iret;
	}
	if(DS_SUCCESS != File_OpenOutFile(coutFile, &smyFile))
	{
		iret = AES_DRIVER_ERR;
		if(DS_SUCCESS != File_Finit(&smyFile))
		{
			iret = AES_DRIVER_SYSERR;
		}
		printf("Invalid file path!\n");
		return iret;		
	}
	printf("Opening files done.\n");
	
	/**make key**/
	printf("Making subkeys...\n");
	byte bkey[AES_KEY_SIZE];
	int ikeyCount = 0;
	for(ikeyCount; ikeyCount < AES_KEY_SIZE; ikeyCount++)
	{
		byte btmp = 0;
		btmp |= DS_16to2(ckeyBuf[ikeyCount * 2]);
		btmp <<= 4;
		btmp |= DS_16to2(ckeyBuf[ikeyCount * 2 + 1]);
		bkey[ikeyCount] = btmp;
	}
	AES_Key_Set(&smyKey, bkey, AES_KEY_SIZE);
	AES_Key_Make(&smyKey);
	printf("Making subkeys done.\n");
	
	/**ecnrypting**/
	printf("Encrypting...\n");
	while(1)
	{
		int ifileFlag = File_Read(&smyFile);
		int ibufIndex = 0;
		while(ibufIndex < smyFile.uibyteCount)
		{
			AES_Iter_Encrypt(smyFile.DATABUF + ibufIndex, AES_PAGE_SIZE, &smyKey);
			ibufIndex += AES_PAGE_SIZE;
		}
		File_Write(&smyFile);
		
		/**end encrypting**/
		if(DS_SUCCESS != ifileFlag)
		{
			break;
		}
	}
	printf("Encrypting done.\n");

	/**set off resources**/		
	if(DS_SUCCESS != File_Finit(&smyFile))
	{
		iret = AES_DRIVER_SYSERR;
	}
	
	return iret;
}

/*******************************************************************************
	Func Name:	AES_Driver_Decrypt
 Date Created:	2014-3-13
 	   Author:	Zhulinfeng
  Description:	Decrypt file.
  		Input:	NONE
  	   Output:	NONE	
  	   Return:	int
  	   			AES_DRIVER_SUCCESS				
  	   			AES_DRIVER_ERR	
  	   			AES_DRIVER_SYSERR				//system error, exit program
  	  Caution:	Called by AES_Driver_Fsm
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE int AES_Driver_Decrypt()
{
	int iret = 	AES_DRIVER_SUCCESS;
	
	struct AES_Key_S 	smyKey;
	struct File_S		smyFile;
	
	/**Get out file path**/
	int ipathCount = 0;
	int ifileNameIndex = 0;
	while( '\0' != cpathBuf[ifileNameIndex])
	{
		ifileNameIndex++;
	}
	while('/' != cpathBuf[ifileNameIndex] && ifileNameIndex > 0)
	{
		ifileNameIndex--;
	}
	char coutFile[AES_DRIVER_PATHLENGTH + 1];
	for(ipathCount; ipathCount < AES_DRIVER_PATHLENGTH + 1; ipathCount++)
	{
		if(ipathCount <= ifileNameIndex)
		{
			coutFile[ipathCount] = cpathBuf[ipathCount];
		}
		else if(1 == ipathCount - ifileNameIndex)
		{
			coutFile[ipathCount] = '_';
		}
		else
		{
			coutFile[ipathCount] = cpathBuf[ipathCount - 1];
		}
		
		if('\0' == coutFile[ipathCount])
		{
			break;
		}
	}
	
	/**open in file and out file**/
	printf("Opening files...\n");
	File_Init(&smyFile);
	if(DS_SUCCESS != File_OpenInFile(cpathBuf, &smyFile) )
	{
		iret = AES_DRIVER_ERR;
		if(DS_SUCCESS != File_Finit(&smyFile))
		{
			iret = AES_DRIVER_SYSERR;
		}
		printf("Invalid file path!\n");
		return iret;
	}
	if(DS_SUCCESS != File_OpenOutFile(coutFile, &smyFile))
	{
		iret = AES_DRIVER_ERR;
		if(DS_SUCCESS != File_Finit(&smyFile))
		{
			iret = AES_DRIVER_SYSERR;
		}
		printf("Invalid file path!\n");
		return iret;		
	}
	printf("Opening files done.\n");
	
	/**make key**/
	printf("Making subkeys...\n");
	byte bkey[AES_KEY_SIZE];
	int ikeyCount = 0;
	for(ikeyCount; ikeyCount < AES_KEY_SIZE; ikeyCount++)
	{
		byte btmp = 0;
		btmp |= DS_16to2(ckeyBuf[ikeyCount * 2]);
		btmp <<= 4;
		btmp |= DS_16to2(ckeyBuf[ikeyCount * 2 + 1]);
		bkey[ikeyCount] = btmp;
	}
	AES_Key_Set(&smyKey, bkey, AES_KEY_SIZE);
	AES_Key_Make(&smyKey);
	printf("Making subkeys done.\n");
	
	/**ecnrypting**/
	printf("Decrypting...\n");
	while(1)
	{
		int ifileFlag = File_Read(&smyFile);
		int ibufIndex = 0;
		while(ibufIndex < smyFile.uibyteCount)
		{
			AES_Iter_Decrypt(smyFile.DATABUF + ibufIndex, AES_PAGE_SIZE, &smyKey);
			ibufIndex += AES_PAGE_SIZE;
		}
		if(DS_SUCCESS != ifileFlag)
		{
			smyFile.uibyteCount -= AES_PAGE_SIZE;
			smyFile.uibyteCount -= (unsigned int)smyFile.DATABUF[smyFile.uibyteCount - 1];
		}
		File_Write(&smyFile);
		
		/**end encrypting**/
		if(DS_SUCCESS != ifileFlag)
		{
			break;
		}
	}
	printf("Decrypting done.\n");

	/**set off resources**/		
	if(DS_SUCCESS != File_Finit(&smyFile))
	{
		iret = AES_DRIVER_SYSERR;
	}
	
	return iret;
}

/*******************************************************************************
	Func Name:	AES_Driver_CheckKey
 Date Created:	2014-3-13
 	   Author:	Zhulinfeng
  Description:	Check input key whether it's invalid.
  		Input:	NONE
  	   Output:	NONE
  	   Return:	int     AES_DRIVER_SUCCESS
  	   			        AES_DRIVER_ERR
  	  Caution:	Called by AES_Driver_Fsm
	----------------------------------------------------------------------------
	Modification History
	Date		Author		Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD  		
*******************************************************************************/
PRIVATE int AES_Driver_CheckKey()
{
	printf("checking...\n");
	
	int iret = AES_DRIVER_SUCCESS;
	int icount = 0;
	for(icount; icount < AES_DRIVER_KEYLENGTH; icount++)
	{
		bool bflag = (ckeyBuf[icount] >= '0' && ckeyBuf[icount] <= '9') ||
					 (ckeyBuf[icount] >= 'a' && ckeyBuf[icount] <= 'f') ||
					 (ckeyBuf[icount] >= 'A' && ckeyBuf[icount] <= 'F')  ;
		if(false == bflag)
		{
			iret = AES_DRIVER_ERR;
			break;
		}
	}
	
	if(AES_DRIVER_SUCCESS != iret)
	{
		printf("Invalid key!\n");
	}
	else
	{
		printf("Correct key!\n");
	}
	
	return iret;
}



