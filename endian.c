/***************************************************************************
 *  Description:
 *      Avoiding endian-dependency
 *
 *  History: 
 *  Date        Name        Modification
 *  2021-08-21  Jason Bacon Begin
 ***************************************************************************/

#include <stdio.h>
#include <sysexits.h>

typedef union
{
    short   big;
    struct
    {
	char    little1;
	char    little2;
    };
}   split_t;

int     main(int argc,char *argv[])

{
    split_t num;
    
    num.big = 0x0102;
    
    // Prints 2 1 on x86 and 1 2 on a Mac G5 (power7)
    printf("%d %d\n", num.little1, num.little2);
    
    // Prints 2 1 on any platform
    printf("%d %d\n", num.big & 0x00ff, num.big >> 8);
    return EX_OK;
}
