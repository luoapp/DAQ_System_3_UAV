//#include "daqdrv.h"
#include <X11/Xlib.h>
#include "udel_drv.h"
#include <stdio.h>
#include <linux/types.h>
#define UDELdaqblocksize (1024l * 1024l *2)
#define UDELdaqfilesize  (134217728)  //2147483648
#define xwin_bufferlen 1024

#define UDELsignature "UDELdaq"
#define	BUFFERSIZE  ( UDELdaqblocksize *4)  
//#define BUFFERNUM 8
#define MAXBUFFERNUM 5



//------------------------------
typedef enum
{
	gscADADIO,
	gsc16AO20
} GSCboardtype;
//------------------------------


typedef struct udeldaqheader {
	char				udel_sig[8];		//UDELdaq
	char				UTCtime[32];
	DaqBoard		   board;
	unsigned int   sample_per_block;   
	unsigned int   block_requested;
} UDELdaqheader;

typedef struct udelarg {
	char           arg[32];
	int *          ipv;
	float *        fpv;
} UDELarg;

typedef struct udelctl{
	float length_in_sec;
	float input_nominal_sr;
	float output_nominal_sr;
	int flag_hd;
	int flag_disp;
	float xwin_pd;
	float xwin_fs;
	float xwin_fps;
	float xwin_offset;
	float xwin_chan;
	char daqfile[64];
	float flag_verbose;
	float out_freq;
	float out_ampl;
} UDELctl;

typedef struct udelxwin{
	Display* display;	
	Window win;			
	GC gc;
	XColor red;
	XColor blue;
	XColor green;
	XColor white;
	XColor black;
	unsigned int win_width;
	unsigned int win_height;
} UDELxwin;
 
FILE * udeldaqfileinit( DaqBoard * pboard, unsigned int bx1);
void udelwritedatatofile( __u16 * datas, long int datasize);

int udeldaqfileclose( FILE * , DaqBoard *);
void udelfileclose( void );

void udelraw2float ( unsigned int *d, float *d2, long int ndata, float vfs);
void udelprintinfo ( DAQDRV_Ctl * );
void udelprintstatus ( DAQDRV_Status * );

Window create_simple_window(Display* display, int width, int height, int x, int y);
GC create_gc(Display* display, Window win, int reverse_video);

void udeltime( char * );
int genwaveforms( unsigned int *raw, float f, float a, float vfs, int nsamples, float outrate, int nchans );
void udelargparser(int argc, char **argv, DaqBoard * pb, UDELctl * pu );	
//void udelargparser(int argc, char **argv,  UDELctl * pu );	
void udelctlinit(UDELctl * puctl);
void udelxwininit( UDELxwin * puxwin);
void _udeldrawdata(UDELxwin *puxwin, int nchans, int xwin_chan, float *rdata, int NDATA);
void udeldrawdata(UDELxwin * puxwin, UDELctl * puctl, DaqBoard * pboard, float *rdata, int ndata);
//void udeldrawdata(UDELxwin * puxwin, UDELctl * puctl, float *rdata, int ndata);
void udelboardinit(DaqBoard *pb, GSCboardtype boardtype);

