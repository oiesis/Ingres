/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cv.h>

/**
**
**  Name: cvlx.C - Convert long to hex.
**
**  Description:
**      This module implements the conversion routines for the compatibility
**	library.
**
**	    CVlx - Convert long to hex.
**
** History:
 * Revision 1.1  90/03/09  09:14:24  source
 * Initialized as new code.
 * 
 * Revision 1.2  90/02/12  09:40:12  source
 * sc
 * 
 * Revision 1.1  89/05/26  15:45:43  source
 * sc
 * 
 * Revision 1.1  89/05/11  01:05:27  source
 * sc
 * 
 * Revision 1.1  89/01/13  17:11:22  source
 * This is the new stuff.  We go dancing in.
 * 
 * Revision 1.2  88/10/25  12:14:36  mikem
 * remove vms lse cruft.
 * 
 * Revision 1.1  88/08/05  13:28:49  roger
 * UNIX 6.0 Compatibility Library as of 12:23 p.m. August 5, 1988.
 * 
 * Revision 1.2  87/11/17  16:55:05  mikem
 * changed to not use CH anymore
 * 
 * Revision 1.1  87/11/10  11:52:58  mikem
 * Initial revision
 * 
**      Revision 1.3  87/07/27  11:23:45  mikem
**      Updated to conform to jupiter coding standards.
**      
**	11-dec-1986 (Joe)
**		Brought over from VMS.
**      20-jul-87 (mmm)
**          Updated to meet jupiter coding standards.      
**	15-jul-93 (ed)
**	    adding <gl.h> after <compat.h>
**	17-jan-1996 (toumi01; from 1.1 axp_osf port) (schte01)
**	    added routine CVlx8 for 8 byte conversion.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	20-Jul-2004 (lakvi01)
**		SIR #112703, cleaned-up warnings.
**      26-Jul-2004 (lakvi01)
**          Backed-out the above change to keep the open-source stable.
**          Will be revisited and submitted at a later date. 
**/

/*{
** Name: CVlx	- Convert long to hex.
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History:
**	{@dd-mmm-yy (login)@}
{@history_line@}...
*/
VOID
CVlx(num, string)
u_i4               num;
char               *string;
{
    i4		    i;

    for (i = 8; --i >= 0; )
    {
	string[i] = (num & 0xf) + '0';
	if (string[i] > '9')
	    string[i] += 'A' - '9' - 1;
	num >>= 4;
    }
    string[8] = 0;
}
VOID
CVlx8(num, string)
long               num;
char               *string;
{
    i4		    i;

    for (i = 16 ; --i >= 0; )
    {
	string[i] = (num & 0xf) + '0';
	if (string[i] > '9')
	    string[i] += 'A' - '9' - 1;
	num >>= 4;
    }
    string[16] = 0;
}
