#include <semaphore.h>
#include <signal.h>

typedef struct daqboardconfig {
  // board assembly configuration
  int   version;
  int   inchans;
  int   outchans;
  float clock;
  unsigned int id;
} DaqBoardConfig;

typedef struct daqaioctl {
  float vfs;
  float clockrate;
  int   ndiv;
  int   clkmaster;
  int   nmode;
  int   nrange;
  int   nchans;
  int   nburst;
  int   nthreshold;
  int   ndata;
} DaqAioCtl;

typedef struct daq_ctl {

  DaqBoardConfig cfg;
  DaqAioCtl      ictl;
  DaqAioCtl      octl;
  int            trigmaster;
} DAQDRV_Ctl;

typedef struct daqdrv_status {
  unsigned int status;
  unsigned int intstatus;
  unsigned int dma0counter;
  unsigned int dma1counter;
  unsigned int dma0overflow;
  unsigned int dma1overflow;
  unsigned long userdata;
  unsigned long useraddr0;
  unsigned long useraddr1;
  unsigned long usersize0;
  unsigned long usersize1;
} DAQDRV_Status;

typedef struct daqbuff {
  unsigned int *p;
  int  n;
  void *next;
} DaqBuff;

typedef struct daqboard {

  DAQDRV_Status  status;

  DAQDRV_Ctl     ctl;

  sem_t          isem;
  int            iwait;
  unsigned int   istatus;
  unsigned int   icounter;
  unsigned int   ioverflow;

  DaqBuff        *ibufferlist;
  DaqBuff        *ibuffertail;
  int            ibuffers;

  sem_t          osem;
  int            owait;
  unsigned int   ostatus;
  unsigned int   ocounter;
  unsigned int   ooverflow;

  DaqBuff        *obufferlist;
  DaqBuff        *obuffertail;
  int            obuffers;

  // for ring buffering
  DaqBuff        *iring0;
  DaqBuff        *iring1;

  DaqBuff        *oring0;
  DaqBuff        *oring1;

  int fd;

  int           (*ifunc)( void * );
  void           *ifuncarg;

  int           (*ofunc)( void * );
  void           *ofuncarg;

} DaqBoard;
