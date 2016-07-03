/******************************************************************************\
Copyright(C):	Huazhong University of Science & Technology.
	FileName:	AES_Driver.h
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

#ifndef AES_DRIVER_H
#define AES_DRIVER_H

#define AES_DRIVER_PATHLENGTH	256
#define AES_DRIVER_KEYLENGTH	32
#define AES_DRIVER_SUCCESS		0
#define AES_DRIVER_CMD			1
#define	AES_DRIVER_FILE 		2
#define AES_DRIVER_KEY			3
#define AES_DRIVER_ERR			4
#define AES_DRIVER_KEYERR		5
#define AES_DRIVER_FILEERR		6
#define AES_DRIVER_SYSERR		7

/**view**/
PUBLIC enum VIEW{START, INKEY, INFILE};
/***command*/
PUBLIC enum COMD{cnone, cencrypt, cdecrypt, cfile, ckey, chelp, cquit};
/**AES direction, encrypt or decrypt or none**/
PUBLIC enum AESD{dnone, dencrypt, ddecrypt};

PUBLIC void AES_Driver_Main();
PUBLIC void AES_Driver_MainLine(IN int iargc, IN char *pcArgv[]);

#endif
