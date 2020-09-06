
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "udel_daq.h"
#include "udel_ipc.h"
#include "main.h"




// variables	***************************************************************

static	int		_continuous		= 0;
static	int		_ignore_errors	= 0;
static	int		_index			= 0;
static	__s32	_io_mode		= GSC_IO_MODE_DMA;
static	long	_minute_limit	= 0;
static	int		_test_limit		= -1;



//--------------------------------------------------------------------------
static int _channels(int fd, __s32* chans)
{
	char	buf[64];
	int		errs	= 0;
	__s32	last	= -1;

	gsc_label("Input Channels");

	errs	+= adadio_dsl_ioctl(fd, ADADIO_IOCTL_AIN_CHAN_LAST,	&last	);

	if (errs == 0)
	{
		chans[0]	= last + 1;

		if (last == 0)
			sprintf(buf, "# %ld", (long) last);
		else
			sprintf(buf, "#s 0-%ld", (long) last);

		printf(	"%ld Channel%s  (%s)\n",
				(long) chans[0],
				(chans[0] == 1) ? "" : "s",
				buf);
	}

	return(errs);
}


/******************************************************************************

******************************************************************************/

int main( void){

	struct my_msgbuf buf;
	int msqid;
	key_t key; 

	nice(-1);
	int qty, errs, fd = 0, ret = EXIT_FAILURE;
	__u32 *_buffer;
	__s32 chans = 32;

	gsc_label_init(28);
	gsc_id_host();
	qty = gsc_count_boards(ADADIO_BASE_NAME);
	errs = gsc_select_1_board(qty, &_index);

	gsc_label("Accessing Board Index");
	printf("%d\n", _index);
	fd = gsc_dev_open(_index, ADADIO_BASE_NAME);

	if (fd == -1)
	{
		errs	= 1;
		printf(	"ERROR: Unable to access device %d.", _index);
	}

	
	if (errs == 0)
	{
		ret = EXIT_SUCCESS;
		errs += gsc_id_driver(fd, ADADIO_BASE_NAME);
		errs += adadio_id_board(fd, -1, NULL);

		_buffer = malloc( BUFFERSIZE );
		if ( _buffer == NULL ) 
		{
			printf("malloc error\n");
			return(-1);
		}
	

		gsc_label("Capture & Save");
		printf("\n");
		gsc_label_level_inc();

		errs += adadio_config(fd, -1);
		errs += adadio_rx_io_mode(fd, -1, _io_mode, NULL);
		errs += adadio_ain_nrate(fd, -1, 2000, NULL);
		errs += adadio_rx_io_timeout(fd, -1, 600, NULL);
		errs += _channels(fd, &chans);
	}

	


//------------------------------------------------------------------
	printf("Recording begins!\n");

    key_t mem_key[MAXBUFFERNUM];
    int shmid[MAXBUFFERNUM];
    __u32 *data[MAXBUFFERNUM];
    int ix;
    //int mode;

    for (ix = 0; ix < MAXBUFFERNUM; ix++) {
	    if ((mem_key[ix] = ftok(IPC_shmem_path[ix], 'R')) == -1) {
        	perror("ftok");
        	exit(1);
	    }	

	    if ((shmid[ix] = shmget(mem_key[ix], SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        	perror("shmget");
			printf("ix = %d\n",ix);
        	exit(1);
	    }

	    data[ix] = shmat(shmid[ix], (void *)0, 0);
	    if (data[ix] == (char *)(-1)) {
        	perror("shmat");
        	exit(1);
	    }
    }

	if ((key = ftok(IPC_msg_path, 'B')) == -1) {
        	perror("ftok");
        	exit(1);
    	}

	if ((msqid = msgget(key, 0644 | IPC_CREAT)) == -1) {
        	perror("msgget");
        	exit(1);
	}
    
	buf.mtype = 1; /* we don't really care in this case */
	ix = 0;
	struct timeval tv;
	long int sec_start, usec_start;
	gettimeofday(&tv, NULL); 
	sec_start = tv.tv_sec;
	usec_start = tv.tv_usec;

	while(ix < 50)
	{
		ret = read(fd, data[ix % MAXBUFFERNUM], BUFFERSIZE);
		printf("req=%ld ret=%ld\n",BUFFERSIZE, ret);
		gettimeofday(&tv, NULL); 
		printf("1 +=%f \n",(tv.tv_sec - sec_start)+(tv.tv_usec-usec_start)/1000000.);
		sec_start = tv.tv_sec;
		usec_start = tv.tv_usec;
		
		buf.mtext[0] = ( ix % MAXBUFFERNUM ) + 48;
		buf.mtext[1] = '\0';
	        if (msgsnd(msqid, (struct msgbuf *)&buf, sizeof(buf), 0) == -1)
	            perror("msgsnd");
	        ix++;
	}

	if (msgctl(msqid, IPC_RMID, NULL) == -1) 
	{
        	perror("msgctl");
	        exit(1);
	}

	for (ix = 0; ix<MAXBUFFERNUM; ix++)
	{
		if (shmdt(data[ix]) == -1) 
		{
	        	perror("shmdt");
	        	exit(1);
    		}
		if (shmctl(shmid[ix], IPC_RMID, NULL) == -1) 
		{
			perror("shmctl");
			exit(1);
		}
    	}
	gsc_dev_close(_index, fd);
	return 0;
}


