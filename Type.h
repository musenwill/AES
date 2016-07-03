/******************************************************************************\
Copyright(C):	Huazhong University of Science & Technology.
	FileName:	Type.h
	  Author:	Zhulinfeng
	 Version:	1.0
	    Date:	2014-2-29
 Description:	Define some most used value types, easy for coding, 
				decreasing errors.
	  Others:	
	----------------------------------------------------------------------------		
	Modification History		
	Date			Author			Modification
	----------------------------------------------------------------------------
	YYYY-MM-DD
\******************************************************************************/

#ifndef TYPE_H
#define TYPE_H

/**PUBLIC**/
#ifndef PUBLIC
#define PUBLIC
#endif

/**PRIVATE**/
#ifndef PRIVATE
#define PRIVATE static
#endif

/**IN**/
#ifndef IN
#define IN
#endif

/**OUT**/
#ifndef OUT
#define OUT
#endif

/**INOUT**/
#ifndef INOUT
#define INOUT
#endif

/**NULL**/
#ifndef NULL
#define NULL 0
#endif

/**false**/
#ifndef false
#define false 0
#endif

/**bool**/
#ifndef bool
#define bool int
#endif

/**byte**/
#ifndef byte
#define byte char
#endif

/**string**/
#ifndef string
#define string char*
#endif

#endif

