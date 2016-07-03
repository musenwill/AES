/******************************************************************************\
Copyright(C):	Huazhong University of Science & Technology.
	FileName:	AES_Main.c
	  Author:	Zhulinfeng
	 Version:	1.0
	    Date:	2014-3-16
 Description:	The entrance of the program.
	  Others:	
	----------------------------------------------------------------------------		
	Modification History		
	Date			Author			Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD
\******************************************************************************/

/**include public head files**/
#include "Sys.h"
#include "Type.h"
#include "Err.h"
/**include module head files**/
#include "AES_Driver.h"


PUBLIC int main(int argc, char *argv[])
{
	if(1 == argc)
	{
	    AES_Driver_Main();
	}
	else
	{
	    AES_Driver_MainLine(argc, argv);
	}
	return 0;
}

