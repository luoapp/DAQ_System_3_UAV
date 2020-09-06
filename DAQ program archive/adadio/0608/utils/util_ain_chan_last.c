// $URL: http://subversion:8080/svn/gsc/trunk/drivers/LINUX/ADADIO/ADADIO_Linux_3.x.x.x_GSC_DN/utils/util_ain_chan_last.c $
// $Rev: 1620 $
// $Date: 2009-04-10 11:41:41 -0500 (Fri, 10 Apr 2009) $

#include <stdio.h>
#include <string.h>

#include "adadio_dsl.h"
#include "adadio_utils.h"
#include "gsc_utils.h"



/******************************************************************************
*
*	Function:	adadio_ain_chan_last
*
*	Purpose:
*
*		Provide a visual wrapper for the ADADIO_IOCTL_AIN_CHAN_LAST service.
*
*	Arguments:
*
*		fd		The handle to use to access the driver.
*
*		index	The index of the board to access. Ignore if < 0.
*
*		set		The setting to apply.
*
*		get		The current setting is recorded here, if not NULL.
*
*	Returned:
*
*		>= 0	The number of errors encountered here.
*
******************************************************************************/

int adadio_ain_chan_last(int fd, int index, __s32 set, __s32* get)
{
	int	errs;

	gsc_label_index("Last Input Channel", index);
	errs	= adadio_dsl_ioctl(fd, ADADIO_IOCTL_AIN_CHAN_LAST, &set);
	printf("%s  (", errs ? "FAIL <---" : "PASS");
	gsc_label_long_comma(set);
	printf(")\n");

	if (get)
		get[0]	= set;

	return(errs);
}


