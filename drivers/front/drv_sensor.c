/* OV_OV5647_RAW */

 sfr P0     = 0x80;
 sfr P1     = 0x90;
 sfr P2     = 0xA0;
 sfr P3     = 0xB0;
 sfr PSW    = 0xD0;
 sfr ACC    = 0xE0;
 sfr B      = 0xF0;
 sfr SP     = 0x81;
 sfr DPL    = 0x82;
 sfr DPH    = 0x83;
 sfr PCON   = 0x87;
 sfr TCON   = 0x88;
 sfr TMOD   = 0x89;
 sfr TL0    = 0x8A;
 sfr TL1    = 0x8B;
 sfr TH0    = 0x8C;
 sfr TH1    = 0x8D;
 sfr IE     = 0xA8;
 sfr IP     = 0xB8;
 sfr SCON0  = 0x98;
 sfr SBUF0  = 0x99;

 sfr DPL1   = 0x84;
 sfr DPH1   = 0x85;
 sfr DPS    = 0x86;
 sfr CKCON  = 0x8E;
 sfr EXIF   = 0x91;
 sfr SADDR0 = 0xA9;
 sfr SADDR1 = 0xAA;
 sfr SADEN0 = 0xB9;
 sfr SADEN1 = 0xBA;
 sfr SCON1  = 0xC0;
 sfr SBUF1  = 0xC1;
 sfr STATUS = 0xC5;
 sfr TA     = 0xC7;
 sfr T2CON  = 0xC8;
 sfr T2MOD  = 0xC9;
 sfr RCAP2L = 0xCA;
 sfr RCAP2H = 0xCB;
 sfr TL2    = 0xCC;
 sfr TH2    = 0xCD;
 sfr WDCON  = 0xD8;
 sfr EIE    = 0xE8;
 sfr EIP    = 0xF8;

 sbit CY    = PSW^7;
 sbit AC    = PSW^6;
 sbit F0    = PSW^5;
 sbit RS1   = PSW^4;
 sbit RS0   = PSW^3;
 sbit OV    = PSW^2;
 sbit FL    = PSW^1;
 sbit P     = PSW^0;

 sbit TF1   = TCON^7;
 sbit TR1   = TCON^6;
 sbit TF0   = TCON^5;
 sbit TR0   = TCON^4;
 sbit IE1   = TCON^3;
 sbit IT1   = TCON^2;
 sbit IE0   = TCON^1;
 sbit IT0   = TCON^0;

 sbit EA    = IE^7;
 sbit ES1   = IE^6;
 sbit ET2   = IE^5;
 sbit ES0   = IE^4;
 sbit ET1   = IE^3;
 sbit EX1   = IE^2;
 sbit ET0   = IE^1;
 sbit EX0   = IE^0;

 sbit PS1   = IP^6;
 sbit PT2   = IP^5;
 sbit PS0   = IP^4;
 sbit PT1   = IP^3;
 sbit PX1   = IP^2;
 sbit PT0   = IP^1;
 sbit PX0   = IP^0;

 sbit INT5  = P1^7;
 sbit INT4  = P1^6;
 sbit INT3  = P1^5;
 sbit INT2  = P1^4;
 sbit TXD1  = P1^3;
 sbit RXD1  = P1^2;
 sbit T2EX  = P1^1;
 sbit T2    = P1^0;

 sbit RD    = P3^7;
 sbit WR    = P3^6;
 sbit T1    = P3^5;
 sbit T0    = P3^4;
 sbit INT1  = P3^3;
 sbit INT0  = P3^2;
 sbit TXD0  = P3^1;
 sbit RXD0  = P3^0;

 sbit SM0   = SCON0^7;  
 sbit SM1   = SCON0^6;  
 sbit SM2   = SCON0^5;  
 sbit REN   = SCON0^4;  
 sbit TB8   = SCON0^3;  
 sbit RB8   = SCON0^2;  
 sbit TI    = SCON0^1;  
 sbit RI    = SCON0^0;  

 sbit SM01  = SCON1^7;  
 sbit SM11  = SCON1^6;  
 sbit SM21  = SCON1^5;  
 sbit REN1  = SCON1^4;  
 sbit TB81  = SCON1^3;  
 sbit RB81  = SCON1^2;  
 sbit TI1   = SCON1^1;  
 sbit RI1   = SCON1^0;  

 sbit TF2    = T2CON^7;
 sbit EXF2   = T2CON^6;
 sbit RCLK   = T2CON^5;
 sbit TCLK   = T2CON^4;
 sbit EXEN2  = T2CON^3;
 sbit TR2    = T2CON^2;
 sbit C_T2   = T2CON^1;
 sbit CP_RL2 = T2CON^0;

 sbit SMOD_1 = WDCON^7;
 sbit POR    = WDCON^6;
 sbit EPFI   = WDCON^5;
 sbit PFI    = WDCON^4;
 sbit WDIF   = WDCON^3;
 sbit WTRF   = WDCON^2;
 sbit EWT    = WDCON^1;
 sbit RWT    = WDCON^0;

 sbit EWDI  = EIE^4;
 sbit EX5   = EIE^3;
 sbit EX4   = EIE^2;
 sbit EX3   = EIE^1;
 sbit EX2   = EIE^0;

 sbit PWDI  = EIP^4;
 sbit PX5   = EIP^3;
 sbit PX4   = EIP^2;
 sbit PX3   = EIP^1;
 sbit PX2   = EIP^0;

 typedef unsigned int size_t;

 #pragma SAVE
 #pragma REGPARMS
 extern char _getkey (void);
 extern char getchar (void);
 extern char ungetchar (char);
 extern char putchar (char);
 extern int printf   (const char *, ...);
 extern int sprintf  (char *, const char *, ...);
 extern int vprintf  (const char *, char *);
 extern int vsprintf (char *, const char *, char *);
 extern char *gets (char *, int n);
 extern int scanf (const char *, ...);
 extern int sscanf (char *, const char *, ...);
 extern int puts (const char *);

 #pragma RESTORE

 #pragma SAVE
 #pragma REGPARMS
 extern char *strcat (char *s1, char *s2);
 extern char *strncat (char *s1, char *s2, int n);

 extern char strcmp (char *s1, char *s2);
 extern char strncmp (char *s1, char *s2, int n);

 extern char *strcpy (char *s1, char *s2);
 extern char *strncpy (char *s1, char *s2, int n);

 extern int strlen (char *);

 extern char *strchr (const char *s, char c);
 extern int strpos (const char *s, char c);
 extern char *strrchr (const char *s, char c);
 extern int strrpos (const char *s, char c);

 extern int strspn (char *s, char *set);
 extern int strcspn (char *s, char *set);
 extern char *strpbrk (char *s, char *set);
 extern char *strrpbrk (char *s, char *set);
 extern char *strstr  (char *s, char *sub);
 extern char *strtok  (char *str, const char *set);

 extern char memcmp (void *s1, void *s2, int n);
 extern void *memcpy (void *s1, void *s2, int n);
 extern void *memchr (void *s, char val, int n);
 extern void *memccpy (void *s1, void *s2, char val, int n);
 extern void *memmove (void *s1, void *s2, int n);
 extern void *memset  (void *s, char val, int n);
 #pragma RESTORE

 typedef char wchar_t;

 #pragma SAVE
 #pragma REGPARMS

 extern int    abs  (int   val);
 extern long  labs  (long  val);

 extern float atof (char *s1);
 extern long  atol (char *s1);
 extern int   atoi (char *s1);
 extern int   rand ();
 extern void  srand (int);

 extern float         strtod  (char *, char **);
 extern long          strtol  (char *, char **, unsigned char);
 extern unsigned long strtoul (char *, char **, unsigned char);

 extern void init_mempool          (void xdata *p, unsigned int size);
 extern void xdata *malloc  (unsigned int size);
 extern void free                  (void xdata *p);
 extern void xdata *realloc (void xdata *p, unsigned int size);
 extern void xdata *calloc  (unsigned int size, unsigned int len);

 #pragma RESTORE

 typedef unsigned char BIT;

 typedef unsigned char UINT8;
 typedef unsigned short int UINT16;
 typedef unsigned long UINT32;

 typedef signed char SINT8;
 typedef signed short int SINT16;
 typedef signed long SINT32;

 typedef float FINT32; 

 extern switchbank (unsigned char bank_number);
 extern char INTR_putchar (char c);

 extern xdata UINT8 G_ShadowData[];
 extern const UINT8 xdata prjDbgMode;
 extern UINT8 xdata gErrLine;

 extern UINT8 xdata G_FPGAMemPool[0x3000];

 typedef struct {
 UINT8 Year;
 UINT8 Month;
 UINT8 Day;
 UINT8 Hour;
 UINT8 Minute;
 UINT8 Second;
 } dateStc_t, *pDateStc_t;

 typedef enum {
 CAM_MODE_IDLE  = 0x00,
 CAM_MODE_PV    = 0x01,
 CAM_MODE_STILL = 0x02,
 CAM_MODE_VIDEO = 0x03,
 CAM_MODE_PCCAM = 0x04,
 CAM_MODE_PB    = 0x05,
 CAM_MODE_UPDN  = 0x06,
 } sp1kCamMode_t;

 typedef void (code* pFuncGT2)(UINT8*);

 extern pFuncGT2 code GTmr2FuncCallList[];

 void HAL_GlobalTimerInit(void);
 void HAL_GlobalTimerRead10us(UINT32 xdata* G10ustimerPt); 
 void HAL_GlobalReadGTimer(UINT32* GtimerPt);
 UINT8 HAL_GlobalTimerWait10us(UINT8 WaitTime);
 UINT8 HAL_GlobalTimerWait(UINT32 WaitTime);
 UINT8 HAL_GlobalDateToBin(pDateStc_t DatePt, pDateStc_t BinPt);
 UINT8 HAL_GlobalWriteRTCData(UINT8 Addr, UINT8 Data);
 UINT8 HAL_GlobalWriteRTCDataIntr(UINT8 Addr, UINT8 Data);
 UINT8 HAL_GlobalWriteRTC(pDateStc_t DatePt);
 UINT8 HAL_GlobalRWRTCDataStsGet(void);
 UINT8 HAL_GlobalReadRTCData(UINT8 Addr, UINT8* DataPt);
 UINT8 HAL_GlobalBinToDate(pDateStc_t BinPt, pDateStc_t DatePt);
 UINT8 HAL_GlobalReadRTC(pDateStc_t DatePt);
 UINT8 HAL_GlobalWriteAlarm(pDateStc_t DatePt);
 UINT8 HAL_GlobalReadAlarm(pDateStc_t DatePt);
 UINT8 Hal_GlobalAlarmRTCIntrSet(UINT8 enable);
 void HAL_GlobalRTCSecondIntrSet(UINT8 enable);
 UINT8 HAL_GlobalConfigPG(UINT8 mode, UINT8 polarity,UINT8 idlstate,UINT16 lowDuty,UINT16 highDuty,UINT8 state);
 void HAL_GlobalStartPG(UINT8  mode);
 void HAL_GlobalStopPG(UINT8  mode);
 UINT8 HAL_GlobalReadRegister(UINT16 reg);
 void HAL_GlobalWriteRegister(UINT16 reg, UINT8 value);
 UINT8 HAL_GlobalCamModeSet(sp1kCamMode_t CamMode);
 void HAL_GlobalCamModeGet(UINT8* CamModePt);
 UINT16	HAL_GlobalReadIOTrap(void);
 void HAL_GlobalRTCTransferText(pDateStc_t DateTime,UINT8* TimeString);
 void HAL_GlobalSuspendResume(void);
 void HAL_GlobalPwrCtrl(UINT8 cpuclk, UINT8 dramclk, UINT8 reg2013, UINT8 reg2014, UINT8 reg2015);
 void HAL_GlobalDramClkSet(UINT8 DramClk);
 void HAL_GlobalBaudRateSet(UINT32 BaudRate);
 void HAL_GlobalTimer2Handle_intr(void);
 UINT8 HAL_GlobalTimer2Config(UINT32 loadtime, UINT8 countmode, UINT8 timebase, pFuncGT2 pFunc, UINT8* pFuncPara);
 void HAL_GlobalTimer2Start(void);
 UINT8 HAL_GlobalTimer2Stop(void);
 UINT8 HAL_GlobalTimer2IntEvtGet(void);
 UINT8 HAL_GlobalWDTCfg(UINT32 initval,UINT8 countmode,UINT8 enable);
 void HAL_GlobalWDTSet(UINT8 enable);
 void HAL_GlobalWDTTimeLoad(UINT32 loadtime);
 void HAL_GlobalPwrMosSet(UINT8 en);
 void HAL_GlobalUartSet(UINT8 en);
 void HAL_GlobalRTCDrivingSet(UINT8 val);
 void HAL_GlobalAlarmRTCIntrSet_Intr();

 enum{
 K_FRAME_RATE_30 = 300,
 K_FRAME_RATE_25 = 301,
 K_FRAME_RATE_15 = 150,
 K_FRAME_RATE_12P5 = 151
 };

 void appVideoView(UINT16 msg);
 void appVideoViewInit(UINT8 initMode);
 void appVideoResSet(UINT8 res);
 void appVideoPreSizeSet(UINT8 res) ;
 void appVideoPauseProcess(void) ;
 void appVideoSnap(void) ;
 UINT8 appVideoQualitySet(void);
 UINT8 appVideoFrmRateSet(void);

 void appVideoViewTele(void);
 void appVideoViewWide(void);

 void appVideoViewTeleReply(void);
 void appVideoViewWideReply(void);

 void appVideoBrcSet(void);
 UINT8 appVideoRecMemFullChk(void);
 void appMotionDetectTimeSet(void);
 UINT8 appVideoSmoothZoomMsgProcFlow (UINT16 sysMsg);

 void appVideoSeamlessTimeSet(void);

 typedef enum{
 RAW_NONE,
 RAW_SAVE,	
 RAW_LOAD,
 }	LSC_CAL_RAWMODE_e;

 typedef enum{
 SP1K_CALIB_LSC_LUMA_LEVEL,

 SP1K_CALIB_LSC_GR_AVG_HIGH,
 SP1K_CALIB_LSC_GR_AVG_LOW,
 SP1K_CALIB_LSC_STEP_X,
 SP1K_CALIB_LSC_STEP_Y,
 SP1K_CALIB_LSC_CHANNEL,
 SP1K_CALIB_LSC_RESOLUTION,

 } LSC_CAL_CONFIG_e;

 typedef enum{
 LSC_3CH,
 LSC_4CH,	
 }	LSC_CAL_GAINRES_e;

 void lscTblGen (UINT32 rawAddr,UINT16 hSize,UINT16 vSize,UINT16 posX, UINT16 posY,UINT8 cfa,
 UINT16 *gbGain,UINT16 *bGain,UINT16 *grGain,UINT16 *rGain,UINT8 FalloffPer,
 UINT8 maxGbVal,UINT8 maxBVal,UINT8 maxGrVal,UINT8 maxRVal);

 void lscMaxValGetbyFilter(UINT32 rawAddr,UINT16 hSize,UINT16 vSize,
 UINT8 cfa,UINT8 *maxR,UINT8 *maxGr,UINT8 *maxB,UINT8 *maxGb,UINT8 *warningM);

 UINT8 lscCalibration(UINT32 rawAddr,UINT16 hSize,UINT16 vSize,UINT8 cfa);
 UINT8 lsEq(float *xin, float *yin, UINT8 len, float C[3]);
 void lscCalInit(void);
 UINT8 sp1kLscCfgSet(UINT8 selector, UINT8 value);

 void lscRawSumGet(UINT32 rawAddr,UINT16 hSize,UINT16 xStart,UINT16 yStart,UINT16 xEnd,UINT16 yEnd,	UINT8 cfa,UINT16 *r,UINT16 *gr,UINT16 *b,UINT16 *gb);

 void lscTableShapeParCal(UINT16 hSize,UINT16 vSize,UINT8 per);
 void lscTblFilter(UINT32 addr,UINT16 hSize,UINT16 vSize,UINT8 maskSize);
 void lscTblFilterMaskDataGet(UINT32 addr,UINT8 xdata* pTbl,UINT16 hSize,UINT16 yStart,UINT16 yEnd);

 UINT8 sp1kLscCal(void);

 void appMILSCCalSet(void);
 void appSetMILSCCalSts(UINT8 enable);
 UINT8 appGetMILSCCalSts(void);

 typedef enum mdSensitivity_e {
 MD_SEN_LOW = 0,
 MD_SEN_MIDDLE,
 MD_SEN_HIGH,
 } mdSensitivity_t;

 typedef enum mdEn_e {
 MD_DISABLE = 0,
 MD_ENABLE,
 } mdEn_t;

 typedef enum mdLearningEn_e {
 MD_LEARNING_DISABLE = 0,
 MD_LEARNING_ENABLE,
 } mdLearningEn_t;

 typedef enum mdAdjustTdEn_e {
 MD_ADJUST_TD_DISABLE = 0,
 MD_ADJUST_TD_ENABLE,
 } mdAdjustTdEn_t;

 typedef struct _MD_BAR{
 UINT8 numeratorMdStatus; 
 UINT8 denominatorMdStatus; 
 }MD_BAR;

 typedef enum mdFuncStatus_e {
 MOTION_DETECTION = 0,
 MOTION_PANORAMA,	
 } mdFuncStatus_t;

 typedef enum mdActionStatus_e {
 MD_NOT_ACTION = 0,
 MD_ACTION,
 MD_LEARNING,
 } mdActionStatus_t;

 void mdInit(void);
 void mdEnSet(UINT8 en);
 UINT8 mdEnGet(void);

 void mdSenSet(UINT8 sen);
 UINT8  mdSenGet(void);

 void mdLearningStart(UINT8 start);

 UINT8 mdEdgeTblFill(UINT16 hSize,UINT16 vSize,UINT16 hOffset,UINT16 vOffset,UINT8 hNum,UINT8 vNum); 
 void mdSvcProc(void *cnt);
 void mdMotionVectorGet(UINT8 num, UINT8 blkNumThr, UINT8 *mvX, UINT8 *mvY,	UINT8 *mvXNum,UINT8 *mvYNum);
 void mdFindMaxMv(UINT8 *mvPt0, UINT8 blockNum, UINT8 thr, UINT8* maxMv, UINT8 *maxMvNum);
 void mdMeOutAddrGet(UINT8 mode,UINT32 *meAddr);
 void mdEdgeTblAddrGet(UINT8 mode,UINT32 *edgeWinAddrA,UINT32 *edgeWinAddrB);
 void mdAdjustTempoalDenoiseWeightEnSet(UINT8 en);
 void mdAdjustTempoalDenoiseWeightAlgo(UINT8* phistXArray,UINT8* phistYArray,UINT8 mdActionSts,UINT8 mvXMidCntNum,UINT8 mvYMidCntNum);
 MD_BAR* mdBarGet (void);

 typedef enum memCbIdx_e {

 MEM_IDX_COMMON_START = 0,  
 MEM_IDX_TEXT = MEM_IDX_COMMON_START, 
 MEM_IDX_DATA,
 MEM_IDX_BP,
 MEM_IDX_LSC,
 MEM_IDX_ZOOM,
 MEM_IDX_OSD,

 MEM_IDX_GRAF_FONT,
 MEM_IDX_DOS_WORK,
 MEM_IDX_DOS_FAT1,
 MEM_IDX_DCF_DIR,
 MEM_IDX_FDB,
 MEM_IDX_FILE_ORDER_TAB,
 MEM_IDX_KEY_SOUND,

 MEM_IDX_DOS_RAM,
 MEM_IDX_DPS,
 MEM_IDX_ALC_MOE_CMD,
 MEM_IDX_ALC_MOE_EXTRA,
 MEM_IDX_CODE_SENTRY_REDIR,
 MEM_IDX_ICON_LAYOUT,
 MEM_IDX_SOUND_CTRL,

 MEM_IDX_AWB_WIN ,	 
 MEM_IDX_MOE_TMP,

 MEM_IDX_USB,

 MEM_IDX_SMOOTH_ZOOM_FACTOR,
 MEM_IDX_SMOOTH_REG_TABLE,
 MEM_IDX_SMOOTH_REG_VALUE,

 MEM_IDX_COMMON_END,

 MEM_IDX_STILL_VIEW_START,
 MEM_IDX_STILL_VIEW_PV2_A = MEM_IDX_STILL_VIEW_START,
 MEM_IDX_STILL_VIEW_PV2_B,
 MEM_IDX_STILL_VIEW_PV2_C,
 MEM_IDX_STILL_VIEW_PV2_D, 

 MEM_IDX_STILL_VIEW_AE,
 MEM_IDX_STILL_VIEW_AE_WIN,	
 MEM_IDX_STILL_VIEW_FD,
 MEM_IDX_STILL_VIEW_EDGE_WINA,
 MEM_IDX_STILL_VIEW_EDGE_WINB,
 MEM_IDX_STILL_VIEW_DIST_ME,		

 MEM_IDX_STILL_VIEW_PV_A,
 MEM_IDX_STILL_VIEW_PV_B,
 MEM_IDX_STILL_VIEW_PV_C,
 MEM_IDX_STILL_VIEW_PV_D,

 MEM_IDX_STILL_VIEW_PF_YUV,
 MEM_IDX_STILL_VIEW_PF_JPG,
 MEM_IDX_STILL_VIEW_PANORAMA_DST_YUV,
 MEM_IDX_STILL_VIEW_PANORAMA_TMP_ME,
 MEM_IDX_STILL_VIEW_PANORAMA_TMP_GMV,
 MEM_IDX_STILL_VIEW_PANORAMA_TMP_YUV,
 MEM_IDX_STILL_VIEW_END,

 MEM_IDX_VIDEO_VIEW_START,
 MEM_IDX_VIDEO_VIEW_PV2_A = MEM_IDX_VIDEO_VIEW_START,
 MEM_IDX_VIDEO_VIEW_PV2_B,
 MEM_IDX_VIDEO_VIEW_PV2_C,
 MEM_IDX_VIDEO_VIEW_PV2_D, 

 MEM_IDX_VIDEO_VIEW_AE,
 MEM_IDX_VIDEO_VIEW_AE_WIN,
 MEM_IDX_VIDEO_VIEW_EDGE_WINA,
 MEM_IDX_VIDEO_VIEW_EDGE_WINB,
 MEM_IDX_VIDEO_VIEW_DIST_ME,

 MEM_IDX_VIDEO_VIEW_PV_A,
 MEM_IDX_VIDEO_VIEW_PV_B,
 MEM_IDX_VIDEO_VIEW_PV_C,
 MEM_IDX_VIDEO_VIEW_PV_D,  

 MEM_IDX_VIDEO_VIEW_END,

 MEM_IDX_STILL_PB_START,
 MEM_IDX_STILL_PB_DISP = MEM_IDX_STILL_PB_START,
 MEM_IDX_STILL_PB_GEN_A,
 MEM_IDX_STILL_PB_GEN_B,
 MEM_IDX_STILL_PB_END,

 MEM_IDX_VIDEO_PB_START,
 MEM_IDX_VIDEO_PB_GEN_A = MEM_IDX_VIDEO_PB_START,
 MEM_IDX_VIDEO_PB_GEN_B,
 MEM_IDX_VIDEO_PB_GEN_C,
 MEM_IDX_VIDEO_PB_FRAG,
 MEM_IDX_VIDEO_PB_VIDEO,
 MEM_IDX_VIDEO_PB_AUDIO,
 MEM_IDX_VIDEO_PB_AUDIO_CHUNK,
 MEM_IDX_VIDEO_PB_IDX,
 MEM_IDX_VIDEO_PB_END,

 MEM_IDX_GLOBAL,

 MEM_IDX_MAX,
 } memCbIdx_t;

 typedef struct memCb_s {
 UINT32 addr;
 UINT32 size;
 } memCb_t;

 typedef enum memFBSize_e {
 MEM_FB_QVGA,
 MEM_FB_1080p,
 MEM_FB_HD,
 MEM_FB_D1,	
 MEM_FB_VGA,
 MEM_FB_HD_960,
 } memFBSize_t;

 extern const UINT32 xdata devMemSize;

 void sysMemSizeInit(void);
 void sysMemAddrUpdate(void);
 void sysMemFrameBufAddrUpdate(UINT8 size);
 void sysMemStillPbDispAddrUpdate(UINT8 HDMI_On);

 void sysMemSizeSet(memCbIdx_t idx, UINT32 size);
 UINT32 sysMemAddrGet(memCbIdx_t idx);
 UINT32 sysMemAddrGet_Intr(memCbIdx_t idx);
 UINT32 sysMemSizeGet(memCbIdx_t idx);
 UINT32 sysMemSizeGet_Intr(memCbIdx_t idx);
 UINT32 sysMemTotSizeGet(void);
 UINT32 sysMemRemainSizeGet(UINT32);
 void sysMemStillPvPanoramaAddrUpdate(UINT8 size,UINT16 pvHsize,UINT16 pvVsize,UINT8 frameNum,UINT16 dstHsize,UINT16 dstVsize);

 void sysMemVideoPvAddrUpdate(UINT8 size,UINT16 pvHSize,UINT16 pvVsize);
 void secondPathPvMenSizeSet();

 typedef enum {
 HAL_FRONT_PARA_MCLK_CFG,
 HAL_FRONT_PARA_PCLK_CFG,
 HAL_FRONT_PARA_SYNC_INVERT_ENABLE,
 HAL_FRONT_PARA_BYPASS_ENABLE,
 HAL_FRONT_PARA_RESHAPE_ENABLE,
 HAL_FRONT_PARA_RESHAPE_REGION,
 HAL_FRONT_PARA_CROP_REGION,
 HAL_FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE,
 HAL_FRONT_PARA_TG_TOTAL_REGION,
 HAL_FRONT_PARA_TG_SYNC_GENERATOR_ENABLE,
 HAL_FRONT_PARA_YUV_OUTPUT_SEQUENCE_CFG,
 HAL_FRONT_PARA_YUV_CCIR_MODE,
 HAL_FRONT_PARA_YUV_SUB128_ENABLE,
 HAL_FRONT_PARA_YUV_INPUT_ENABLE,
 HAL_FRONT_PARA_MAX_ID,
 } HAL_FrontParaId_e;

 typedef enum {
 HAL_FRONT_YUV_OUTPUT_UYVY = (0 << 0),
 HAL_FRONT_YUV_OUTPUT_YVYU = (1 << 0),
 HAL_FRONT_YUV_OUTPUT_VYUY = (2 << 0),
 HAL_FRONT_YUV_OUTPUT_YUYV = (3 << 0),
 } HAL_FrontYuvOutputSeqCfg_e;

 typedef enum {
 HAL_FRONT_YUV_CCIR601 = (0 << 4),
 HAL_FRONT_YUV_CCIR656 = (1 << 4),
 } HAL_FrontYuvCcirMode_e;

 typedef enum {
 HAL_FRONT_MCLK_SRC_INT = (0 << 2),
 HAL_FRONT_MCLK_SRC_EXT = (1 << 2),
 } HAL_FrontMclkSrc_e;

 typedef enum {
 HAL_FRONT_PCLK_SRC_INT = (0 << 3),
 HAL_FRONT_PCLK_SRC_EXT = (1 << 3),
 } HAL_FrontPclkSrc_e;

 typedef enum {
 HAL_FRONT_WAIT_HSYNC  = (1 << 0),
 HAL_FRONT_WAIT_VSYNC  = (1 << 1),
 HAL_FRONT_WAIT_HVALID = (1 << 2),
 HAL_FRONT_WAIT_VVALID = (1 << 3),
 } HAL_FrontWaitSource_e;

 typedef enum {
 HAL_FRONT_WAIT_FALLING = 0,
 HAL_FRONT_WAIT_RISING  = 1,
 HAL_FRONT_WAIT_HIGH    = 2,
 HAL_FRONT_WAIT_LOW     = 3,
 } HAL_FrontWaitMode_e; 

 void HAL_FrontIntrEnable(UINT8 enable, UINT8 value);
 void HAL_FrontReset(void);
 UINT8 HAL_FrontSnapTrig(UINT8 snapNum,UINT8 trig);
 UINT8 HAL_FrontSignalWait(UINT8 src, UINT8 mode, UINT8 num);

 void HAL_FrontParaMClkCfgSet(SINT16 *paraAry, UINT8 upd);
 void HAL_IntrFrontParaMClkCfgSet(SINT16 *paraAry, UINT8 upd);

 void HAL_FrontParaPClkCfgSet(SINT16 *paraAry, UINT8 upd);
 void HAL_IntrFrontParaPClkCfgSet(SINT16 *paraAry, UINT8 upd);

 void HAL_FrontParaSyncInvertEnableSet(SINT16 *paraAry, UINT8 upd);
 void HAL_FrontParaByPassEnableSet(SINT16 *paraAry, UINT8 upd);
 void HAL_FrontParaReshapeEnableSet(SINT16 *paraAry, UINT8 upd);
 void HAL_FrontParaReshapeRegionSet(SINT16 *paraAry, UINT8 upd);
 void HAL_FrontParaCropRegionSet(SINT16 *paraAry, UINT8 upd);
 void HAL_FrontSizeGet(UINT16 *hSize,UINT16 *vSize);
 void HAL_FrontParaFrontToCdspGatingEnableSet(SINT16 *paraAry, UINT8 upd);
 void HAL_IntrFrontParaFrontToCdspGatingEnableSet(SINT16 *paraAry,UINT8 upd);
 void HAL_FrontParaTgTotalRegionSet(SINT16 *paraAry, UINT8 upd);
 void HAL_FrontParaTgSyncGeneratorEnableSet(SINT16 *paraAry, UINT8 upd);
 void HAL_FrontParaYuvOutputSequenceCfgSet(SINT16 *paraAry, UINT8 upd);
 void HAL_FrontParaYuvCcirModeSet(SINT16 *paraAry, UINT8 upd);
 void HAL_FrontParaYuvSub128EnableSet(SINT16 *paraAry, UINT8 upd);
 void HAL_FrontParaYuvInputEnableSet(SINT16 *paraAry, UINT8 upd);
 void HAL_FrontParaMClkCfgGet(UINT8 *pSrc, UINT16 *pDiv, UINT8 *pPhase, UINT8 *pDrv, UINT8 *pUpd);
 void HAL_FrontParaPClkCfgGet(UINT8 *psrc, UINT16 *pdiv, UINT8 *pphase);

 void sp1kHalFrontInputGate( UINT8 tggz );

 typedef enum {
 FRONT_YUV_OUTPUT_UYVY = (0 << 0),
 FRONT_YUV_OUTPUT_YVYU = (1 << 0),
 FRONT_YUV_OUTPUT_VYUY = (2 << 0),
 FRONT_YUV_OUTPUT_YUYV = (3 << 0),
 } frontYuvOutputSeqCfg_e;   

 typedef enum {
 FRONT_YUV_CCIR601 = (0 << 4),
 FRONT_YUV_CCIR656 = (1 << 4),
 } frontYuvCcirMode_e;   

 typedef enum {
 FRONT_MCLK_SRC_INT = (0 << 2),
 FRONT_MCLK_SRC_EXT = (1 << 2),
 } frontMclkSrc_e;   

 typedef enum {
 FRONT_BAYER_PTN_GRRBGB = 0,
 FRONT_BAYER_PTN_RGRGBB,
 FRONT_BAYER_PTN_BGBGRR,
 FRONT_BAYER_PTN_GBBRGR,
 } frontBayerPattern_e;

 typedef enum {
 FRONT_SRAM_MODE_DISABLE = 0,
 FRONT_SRAM_MODE_ENABLE,
 } frontSramMode_e;

 typedef enum {
 FRONT_WAIT_HSYNC  = (1 << 0),
 FRONT_WAIT_VSYNC  = (1 << 1),
 FRONT_WAIT_HVALID = (1 << 2),
 FRONT_WAIT_VVALID = (1 << 3),
 } frontWaitSource_e;   

 typedef enum {
 FRONT_WAIT_FALLING = 0,
 FRONT_WAIT_RISING  = 1,
 FRONT_WAIT_HIGH    = 2,
 FRONT_WAIT_LOW     = 3,
 } frontWaitMode_e;   

 typedef enum {
 FRONT_DEV_INFO_DATA_FMT,
 FRONT_DEV_INFO_BAYER_FMT,
 FRONT_DEV_INFO_PREVIEW_LINE_TOL	,	
 FRONT_DEV_INFO_PREVIEW_PCLK,
 FRONT_DEV_INFO_PREVIEW_SENSOR_ZOOM_FACTOR,
 FRONT_DEV_INFO_TOT,
 } frontDeviceInfo_e;

 typedef enum {
 FRONT_DATA_FMT_RAW,
 FRONT_DATA_FMT_YUV,
 FRONT_DATA_FMT_TOT,
 } frontDataFmt_e;

 typedef enum {
 FRONT_PV_DYNAMIC_TD_DISABLE = 0,
 FRONT_PV_DYNAMIC_TD_ENABLE,
 } frontDynamicTempDenoiseMode_e;

 typedef enum {
 FRONT_EXT_OP_AE_TARGET_LUMINANCE_SET,
 FRONT_EXT_OP_AE_EXPOSURE_COMPENSATION_SET,
 FRONT_EXT_OP_AE_FRAME_RATE_SET,
 FRONT_EXT_OP_AWB_MODE_SET,
 FRONT_EXT_OP_AFTER_SNAP_SHOT,
 FRONT_EXT_OP_TOT,
 } frontExtendOperatingFunc_e;

 typedef enum {
 SENSOR_SNAP_FLASH_CFG_MODE = 0,
 SENSOR_SNAP_FLASH_CFG_WIDTH = 1,
 SENSOR_SNAP_FLASH_CFG_H_POS = 2,
 SENSOR_SNAP_FLASH_CFG_DELAY_TYPE = 3,
 SENSOR_SNAP_FLASH_CFG_MAX_GAIN = 4,
 } sensor_snap_flash_cfg_t;

 typedef enum {
 SENSOR_SNAP_FLASH_MODE_OFF = 0,
 SENSOR_SNAP_FLASH_CURTAIN_FRONT_SYNC = 0x02,
 SENSOR_SNAP_FLASH_CURTAIN_REAR_SYNC = 0x82,
 } sensor_snap_flash_mode_t;

 typedef enum {
 SENSOR_FLASH_DELAY_TYPE_H_POS = 0,
 SENSOR_FLASH_DELAY_TYPE_IMMEDIATE_TIMER = 2,
 SENSOR_FLASH_DELAY_TYPE_TOTAL,
 } sensor_flash_delay_t;

 typedef enum {
 SENSOR_PREFLASH_CFG_WIDTH = 1,
 SENSOR_PREFLASH_CFG_H_POS = 2,
 SENSOR_PREFLASH_CFG_DELAY_TYPE = 3,
 } sensor_preflash_cfg_t;

 typedef enum {
 SENSOR_GAIN_1X,
 SENSOR_GAIN_2X,
 SENSOR_GAIN_4X,
 SENSOR_GAIN_8X,
 SENSOR_GAIN_AUTO = 0xff,
 } sensor_flash_gain_t;  

 typedef struct {
 UINT16 ev;
 UINT32 shutter;
 UINT16 gain;
 UINT16 iris;
 UINT8  captureCompensationGainIdx;
 } frontAe_t;

 typedef struct {
 UINT16 Shutter;
 UINT8 GainIdx;
 } frontShutter_t;

 typedef struct {
 UINT16 mclkDiv;
 UINT8  mclkPhase;
 UINT16 pclkDiv;
 UINT8  pclkPhase;
 UINT32 pclkFreq;
 UINT8  enTDGainIdx;
 UINT8  idxCvgZon;
 } frontPreviewTemporalDenoise_t;

 typedef struct {
 UINT8  sramModeEn;
 UINT8  pvBayerPtn;
 UINT16 width;
 UINT16 height;
 UINT16 xoff;
 UINT16 yoff;
 UINT16 lineTot;
 UINT16 frameTot;
 UINT16 mclkDiv;
 UINT8  mclkPhase;
 UINT16 pclkDiv;
 UINT8  pclkPhase;
 UINT32 pclkFreq;
 UINT8  bypassHref;
 UINT8  bypassVref;
 UINT8  reshapeHen;
 UINT16 reshapeHfall;
 UINT16 reshapeHrise;
 UINT8  reshapeVen;
 UINT16 reshapeVfall;
 UINT16 reshapeVrise;
 frontAe_t *ae60hzTbl;
 frontAe_t *ae50hzTbl;
 UINT8  *aeMaxIdxTbl;
 UINT8  *aeMinIdxTbl;
 UINT8  ev10ExpIdx;
 UINT16 *gainTbl;
 UINT8  gainMaxIdx;
 UINT8  gainMinIdx;
 frontPreviewTemporalDenoise_t *prevDynamicTDTbl;
 UINT16 sensorZoomFactor;
 } frontPrevCapabDesc_t;

 typedef struct {
 UINT8  snapBayerPtn;
 UINT16 width;
 UINT16 height;
 UINT16 xoff;
 UINT16 yoff;
 UINT16 lineTot;
 UINT16 frameTot;
 UINT16 mclkDiv;
 UINT16 mclkDivBurst;
 UINT16 mclkDivBurstUp;
 UINT8  mclkPhase;
 UINT16 pclkDiv;
 UINT8  pclkPhase;
 UINT32 pclkFreq;
 UINT32 pclkFreqBurst;
 UINT32 pclkFreqBurstUp;
 UINT8  bypassHref;
 UINT8  bypassVref;
 UINT8  reshapeHen;
 UINT16 reshapeHfall;
 UINT16 reshapeHrise;
 UINT8  reshapeVen;
 UINT16 reshapeVfall;
 UINT16 reshapeVrise;	
 } frontSnapCapabDesc_t;

 typedef struct {
 frontPrevCapabDesc_t **ppprevCap;
 frontSnapCapabDesc_t **ppsnapCap;
 } frontCapabilityArg_t;

 typedef struct {
 UINT32 val;
 UINT8 isr;
 UINT8 upd;
 } frontExposureTimeArg_t;

 typedef UINT8 *(code *pfOperatingFrontDevNameGet_t)(void);
 typedef void (code *pfOperatingFrontFrameRateSet_t)(UINT8 fps);
 typedef void (code *pfOperatingFrontCapabilityGet_t)(frontCapabilityArg_t *parg);
 typedef void (code *pfOperatingFrontIntrCapabilityGet_t)(frontCapabilityArg_t *parg);
 typedef void (code *pfOperatingFrontPowerOn_t)(void);
 typedef void (code *pfOperatingFrontPowerOff_t)(void);
 typedef void (code *pfOperatingFrontPreviewModeSet_t)(void);
 typedef void (code *pfOperatingFrontPreviewSensorZoomModeSet_t)(UINT16 zoomfactor);
 typedef void (code *pfOperatingFrontIntrPreviewSensorZoomModeSet_t)(UINT16 zoomfactor);
 typedef void (code *pfOperatingFrontSnapModeSet_t)(void);
 typedef void (code *pfOperatingFrontSnapShot_t)(UINT8 first, UINT8 mode, UINT8 scaleUp);
 typedef void (code *pfOperatingFrontGainSet_t)(UINT8 val, UINT8 isr, UINT8 upd);
 typedef void (code *pfOperatingFrontExposureTimeSet_t)(frontExposureTimeArg_t *parg);
 typedef void (code *pfOperatingFrontIntrGainSet_t)(UINT8 val, UINT8 isr, UINT8 upd);
 typedef void (code *pfOperatingFrontIntrExposureTimeSet_t)(frontExposureTimeArg_t *parg);
 typedef UINT8 (code *pfOperatingFrontOpticalBlackStatusGet_t)(void);
 typedef UINT8 (code *pfOperatingFrontOpticalBlackCalibrate_t)(void);
 typedef void (code *pfOperatingFrontShutterOpen_t)(void);
 typedef void (code *pfOperatingFrontShutterClose_t)(void);
 typedef void (code *pfOperatingFrontIntrPreviewDynamicTempoalDenoiseSet_t)(UINT8 en);

 typedef struct {
 UINT8 expGainSetPos;
 UINT8 gainAfterExp;
 pfOperatingFrontDevNameGet_t pfOpDevNameGet;
 pfOperatingFrontFrameRateSet_t pfOpFrameRateSet;
 pfOperatingFrontCapabilityGet_t pfOpCapabilityGet;
 pfOperatingFrontIntrCapabilityGet_t pfOpIntrCapabilityGet;
 pfOperatingFrontPowerOn_t pfOpPowerOn;
 pfOperatingFrontPowerOff_t pfOpPowerOff;
 pfOperatingFrontPreviewModeSet_t pfOpPreviewModeSet;
 pfOperatingFrontPreviewSensorZoomModeSet_t pfOpPreviewSensorZoomModeSet;
 pfOperatingFrontIntrPreviewSensorZoomModeSet_t pfOpIntrPreviewSensorZoomModeSet;
 pfOperatingFrontSnapModeSet_t pfOpSnapModeSet;
 pfOperatingFrontSnapShot_t pfOpSnapShot;
 pfOperatingFrontGainSet_t pfOpGainSet;
 pfOperatingFrontExposureTimeSet_t pfOpExposureTimeSet;
 pfOperatingFrontIntrGainSet_t pfOpIntrGainSet;
 pfOperatingFrontIntrExposureTimeSet_t pfOpIntrExposureTimeSet;
 pfOperatingFrontOpticalBlackStatusGet_t pfOpOpticalBlackStatusGet;
 pfOperatingFrontOpticalBlackCalibrate_t pfOpOpticalBlackCalibrate;
 pfOperatingFrontShutterOpen_t pfOpShutterOpen;
 pfOperatingFrontShutterClose_t pfOpShutterClose;
 pfOperatingFrontIntrPreviewDynamicTempoalDenoiseSet_t pfOpIntrPreviewDynamicTempoalDenoiseSet;
 } frontOperatingFunc_t;

 typedef void (code *pfFrontExtOperatingFunc_t)(void *parg);

 typedef struct frontDev_s {
 frontOperatingFunc_t opFunc;
 pfFrontExtOperatingFunc_t extOpFunc[FRONT_EXT_OP_TOT];
 } frontDev_t; 

 typedef enum {
 FRONT_PARA_MCLK_CFG,
 FRONT_PARA_PCLK_CFG,
 FRONT_PARA_SYNC_INVERT_ENABLE,
 FRONT_PARA_BYPASS_ENABLE,
 FRONT_PARA_RESHAPE_ENABLE,
 FRONT_PARA_RESHAPE_REGION,
 FRONT_PARA_CROP_REGION,
 FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE,
 FRONT_PARA_TG_TOTAL_REGION,
 FRONT_PARA_TG_SYNC_GENERATOR_ENABLE,
 FRONT_PARA_YUV_OUTPUT_SEQUENCE_CFG,
 FRONT_PARA_YUV_CCIR_MODE,
 FRONT_PARA_YUV_SUB128_ENABLE,
 FRONT_PARA_YUV_INPUT_ENABLE,
 FRONT_PARA_MAX_ID,
 } frontParaId_e; 

 typedef enum {
 MS_CLOSE,
 MS_OPEN,	
 MS_IDLE,
 } frontMSSts_e;

 typedef enum {
 SNAP_MS_NORMAL,
 SNAP_OBCAL,
 SNAP_MSCAL_FRAME,
 SNAP_MSCAL_ROLLING,
 SNAP_MS_DARK,
 } frontSnapMode_e;

 extern UINT8 frontPrevSensorZoomStatus;
 extern UINT8 frontPrevSensorZoom2xGainEn;
 extern UINT8 frontPrevSensorZoomSnapStatus;

 UINT8 frontDevOpen(frontOperatingFunc_t *popFunc, UINT16 opFuncSize);
 UINT8 frontExtendOpFuncSet(UINT8 id, UINT8 opId, pfFrontExtOperatingFunc_t pfExtOp);
 UINT8 frontDevActSet(UINT8 id);
 UINT8 frontDevActGet(void);
 UINT8 frontIntrDevActGet(void);
 UINT8 frontDevIdCheck(UINT8 *pid, frontDev_t **ppdev);
 UINT8 frontFrameRateSet(UINT8 id, UINT8 fps);
 UINT8 frontCapabilityGet(UINT8 id, frontPrevCapabDesc_t **ppPrevCap, frontSnapCapabDesc_t **ppSnapCap);
 UINT8 frontIntrCapabilityGet(UINT8 id, frontPrevCapabDesc_t **ppPrevCap, frontSnapCapabDesc_t **ppSnapCap);
 UINT8 frontDeviceInfoGet(UINT8 id, UINT8 infId, UINT16 *ppara1, UINT16 *ppara2);
 UINT8 frontPowerOn(UINT8 id);
 UINT8 frontPowerOff(UINT8 id);
 UINT8 frontPreviewModeSet(UINT8 id);
 UINT8 frontPreviewSensorZoomModeSet(UINT8 id, UINT16 factor);
 UINT8 frontIntrPreviewSensorZoomModeSet(UINT8 id, UINT16 factor);
 UINT8 frontSnapModeSet(UINT8 id);
 UINT8 frontSnapShot(UINT8 id, UINT8 burst, UINT8 first, UINT8 mode, UINT8 scaleUp);
 UINT8 frontGainSet(UINT8 id, UINT8 val, UINT8 isr, UINT8 upd);
 UINT8 frontExposureTimeSet(UINT8 id, UINT32 val, UINT8 isr, UINT8 upd);
 UINT8 frontIntrGainSet(UINT8 id, UINT8 val, UINT8 isr, UINT8 upd);
 UINT8 frontIntrExposureTimeSet(UINT8 id, UINT32 val, UINT8 isr, UINT8 upd);
 UINT8 frontOpticalBlackStatusGet(UINT8 id);
 UINT8 frontOpticalBlackCalibrate(UINT8 id);
 UINT8 frontShutterOpen(UINT8 id);
 UINT8 frontShutterClose(UINT8 id);
 UINT8 frontIntrPreviewDynamicTempoalDenoiseSet(UINT8 id, UINT8 en);

 UINT8 frontLensCompensationSet(UINT8 mode);
 UINT8 frontPreviewSizeGet(UINT8 id, UINT16 *pHSize, UINT16 *pVSize);
 UINT8 frontSnapSizeGet(UINT8 id, UINT16 *pHSize, UINT16 *pVSize);

 UINT8 frontAeTargetLuminanceSet(UINT8 id, UINT8 val);
 UINT8 frontAeExposureCompensationSet(UINT8 id, UINT8 val);
 UINT8 frontAeFrameRateSet(UINT8 id, UINT16 val);
 UINT8 frontAwbModeSet(UINT8 id, UINT8 mode);

 UINT8 frontProbeModeSet(UINT8 probsel, UINT8 probmode,	UINT8 proben);

 void frontSnapMethodSet(UINT8 method);
 UINT8 frontSnapMethodGet(void);
 void frontDarkSubCtrl(UINT8 en,UINT8 mode,UINT16 time);

 UINT8 frontHighQualityDigitalZoomCapabilityGet(UINT8 id);
 UINT8 frontHighQualityDigitalZoomModeSet(UINT8 id, UINT8 en);

 void frontPreflashAeCfg(UINT32 shutter,UINT8 gain,UINT8 wait);
 void frontSnapFlashAeCfg(UINT32 shutter,UINT8 gainIndex,UINT8 wait);

 void frontPreflashCfg(UINT8 cfgsel,UINT16 value);
 void frontSnapFlashCfg(UINT32 cfgsel,UINT32 value);

 void frontPreflashTrig(UINT8 *befPreLuma,UINT8 *aftPreLuma); 
 void frontSnapFlashTrig(UINT32 exptime);
 UINT8  frontIgbtFlashCalibTrig(UINT32 trigTime,UINT8 saveYUV);

 UINT32 frontSnapFlashTimeGet(UINT8 befPreFlashLuma,UINT8 preFlashLuma,UINT8 tagLuma,UINT8 pv2CapGainRatio);

 void frontStrobeInit(void);

 void frontLastShutterGainSet(UINT32 shutter, UINT8 gain);

 sensor_flash_gain_t sensorFlashGainIdxGet(void);

 UINT8 sensorGainIdxGet(sensor_flash_gain_t flashGainIdx);

 UINT16 sensorPvToSnapFrmTimeRatio(void);

 typedef enum {
 HAL_GPIO_BYTE_NULL,
 HAL_GPIO_BYTE_GEN0,    
 HAL_GPIO_BYTE_GEN1,    
 HAL_GPIO_BYTE_GEN2,    
 HAL_GPIO_BYTE_GEN3,    
 HAL_GPIO_BYTE_FM0,	 
 HAL_GPIO_BYTE_FM1,	 
 HAL_GPIO_BYTE_FM2,	 
 HAL_GPIO_BYTE_TG0,	   
 HAL_GPIO_BYTE_DISP0,   
 HAL_GPIO_BYTE_DISP1,   
 HAL_GPIO_BYTE_DISP2,   
 HAL_GPIO_BYTE_DISP3,   
 HAL_GPIO_BYTE_MA1,	   
 HAL_GPIO_BYTE_RGB0,    
 HAL_GPIO_BYTE_RGB1,    
 HAL_GPIO_BYTE_MAX,
 } HAL_GpioByte_e;

 void  HAL_GpioByteFuncSet(UINT8 byteId, UINT8 msk, UINT8 func);
 UINT8 HAL_GpioByteFuncGet(UINT8 byteId, UINT8 msk);
 void  HAL_GpioByteDirSet(UINT8 byteId, UINT8 msk, UINT8 dir);
 void  HAL_GpioByteDirSet_Intr(UINT8 byteId, UINT8 msk, UINT8 dir);
 UINT8 HAL_GpioByteDirGet(UINT8 byteId, UINT8 msk);
 void  HAL_GpioByteOutSet(UINT8 byteId, UINT8 msk, UINT8 val);
 void  HAL_GpioByteOutSet_Intr(UINT8 byteId, UINT8 msk, UINT8 val);
 UINT8 HAL_GpioByteOutGet(UINT8 byteId, UINT8 msk);
 void  HAL_GpioByteInGateSet(UINT8 byteId, UINT8 msk, UINT8 gate);
 UINT8 HAL_GpioByteInGet(UINT8 byteId, UINT8 msk, UINT8 polarity);
 void  HAL_GpioPullEnSet(UINT8 byteId, UINT8 msk, UINT8 en);
 UINT8 HAL_GpioPullEnGet(UINT8 byteId, UINT8 msk);
 void  HAL_GpioPullSelSet(UINT8 byteId, UINT8 msk, UINT8 sel);
 UINT8 HAL_GpioPullSelGet(UINT8 byteId, UINT8 msk);
 void  HAL_GpioDrivingSet(UINT8 byteId, UINT8 msk, UINT8 sel);
 UINT8 HAL_GpioDrivingGet(UINT8 byteId, UINT8 msk);
 UINT8 HAL_GpioSarAdcConfig(UINT8 tp_en,	UINT8 sarclk, UINT8 pwrctl);
 UINT8 HAL_GpioSarAdcValGet(UINT8 channel, UINT16 *adcval);

 typedef enum {
 GPIO_BYTE_NULL,
 GPIO_BYTE_GEN0,    
 GPIO_BYTE_GEN1,    
 GPIO_BYTE_GEN2,    
 GPIO_BYTE_GEN3,    
 GPIO_BYTE_FM0,     
 GPIO_BYTE_FM1,     
 GPIO_BYTE_FM2,     
 GPIO_BYTE_TG0,     
 GPIO_BYTE_DISP0,   
 GPIO_BYTE_DISP1,   
 GPIO_BYTE_DISP2,   
 GPIO_BYTE_DISP3,   
 GPIO_BYTE_MA1,     
 GPIO_BYTE_RGB0,    
 GPIO_BYTE_RGB1,    
 GPIO_BYTE_MAX,
 } gpioByte_e;   

 typedef enum {
 HAL_I2C_TRANS_MODE_NORMAL = (0 << 0),
 HAL_I2C_TRANS_MODE_BURST  = (1 << 0),
 } HAL_I2cTransMode_e; 

 void HAL_I2cDeviceAddrSet(UINT8 addr);
 void HAL_I2cDeviceAddrGet(UINT8* pAddr);
 void HAL_I2cRegClkCfgSet(UINT8 regClkDiv);
 void HAL_I2cRegClkCfgGet(UINT8* pRegClkDiv);
 void HAL_I2cClkCfgSet(UINT8 div);
 void HAL_I2cClkCfgGet(UINT8* pDiv);
 UINT8 HAL_I2cCmdTableSend_Intr(UINT8 *cmdTbl, UINT8 cmdLen, UINT8 cmdTot, UINT8 transMode, UINT8 isr, UINT8 upd);
 UINT8 HAL_I2cCmdTableSend(UINT8 *cmdTbl, UINT8 cmdLen, UINT8 cmdTot, UINT8 transMode, UINT8 isr, UINT8 upd);
 UINT8 HAL_I2cRegRead(UINT8 *addrTbl, UINT8 *dataTbl, UINT8 dataLen, UINT8 regTot, UINT8 subAddrEn, UINT8 restartMode, UINT8 upd);
 UINT8 HAL_I2cReg16Read(UINT16 *addrTbl, UINT8 *dataTbl, UINT8 dataLen, UINT8 regTot, UINT8 subAddrEn, UINT8 restartMode, UINT8 upd);
 UINT8 HAL_I2cReg16Read16(UINT16 *addrTbl, UINT8 *dataTbl, UINT8 dataLen, UINT8 regTot, UINT8 subAddrEn, UINT8 restartMode, UINT8 upd);

 typedef enum {
 I2C_TRANS_MODE_NORMAL = (0 << 0),
 I2C_TRANS_MODE_BURST  = (1 << 0),
 } i2cTransMode_e;   

 typedef enum {
 I2C_REG_CLK_DIV_2048 = 0,
 I2C_REG_CLK_DIV_1024  = 1,
 I2C_REG_CLK_DIV_512  = 2,
 I2C_REG_CLK_DIV_256  = 3,
 I2C_REG_CLK_DIV_128  = 4,
 I2C_REG_CLK_DIV_64  = 5,
 I2C_REG_CLK_DIV_32  = 6,
 } i2cRegClkDivMode_e;   

 typedef enum {
 I2C_SOURCE_DIV_1 = 0,
 I2C_SOURCE_DIV_2,
 I2C_SOURCE_DIV_3,
 I2C_SOURCE_DIV_4,
 I2C_SOURCE_DIV_5,
 I2C_SOURCE_DIV_6,
 I2C_SOURCE_DIV_7,
 I2C_SOURCE_DIV_8,
 I2C_SOURCE_DIV_9,
 I2C_SOURCE_DIV_10,
 I2C_SOURCE_DIV_11,
 I2C_SOURCE_DIV_12,
 I2C_SOURCE_DIV_13,
 I2C_SOURCE_DIV_14,
 I2C_SOURCE_DIV_15,
 I2C_SOURCE_DIV_16,
 I2C_SOURCE_DIV_17,
 I2C_SOURCE_DIV_18,
 I2C_SOURCE_DIV_19,
 I2C_SOURCE_DIV_20,
 I2C_SOURCE_DIV_21,
 I2C_SOURCE_DIV_22,
 I2C_SOURCE_DIV_23,
 I2C_SOURCE_DIV_24,
 I2C_SOURCE_DIV_25,
 I2C_SOURCE_DIV_26,
 I2C_SOURCE_DIV_27,
 I2C_SOURCE_DIV_28,
 I2C_SOURCE_DIV_29,
 I2C_SOURCE_DIV_30,
 I2C_SOURCE_DIV_31,
 I2C_SOURCE_DIV_32,	
 } i2cSourceDivMode_e;   

 enum {
 PRV_BLEND_PHOTOFRM = 1,
 PRV_BLEND_ROTATION,
 PRV_BLEND_DATESTAMP,
 PRV_BLEND_END,
 }; 

 UINT8 pvPhoFrmCdspEofGet(void);
 void pvPhoFrmCdsoEofSet(UINT8 eof);
 void pvPhoFrmCdspEofIntrSet(UINT8 eof);

 void pvPhofrmBlend(UINT32 srcAddr, UINT16 srcHSize, UINT16 srcVSize);
 void pvPhofrmBlendIntr(UINT32 srcAddr, UINT16 srcHSize, UINT16 srcVSize);

 void pvPhofrmInit(void);

 UINT8 pvPhofrmEnableGet(void);

 void sp1kPvDlyPhofrmBlendCntSet(UINT8 dlyCnt);

 typedef enum button_e {
 SP1K_BTN_UP,
 SP1K_BTN_DOWN,
 SP1K_BTN_LEFT,
 SP1K_BTN_RIGHT,
 SP1K_BTN_MENU,
 SP1K_BTN_OK,
 SP1K_BTN_DISP,
 SP1K_BTN_MODE,
 SP1K_BTN_TELE,
 SP1K_BTN_WIDE,
 SP1K_BTN_S1,
 SP1K_BTN_S2,
 SP1K_BTN_POWER,
 } button_t;

 typedef enum keyStatus_e {
 KEY_STATUS_PRESSED = (unsigned char)0x01,
 KEY_STATUS_RELEASED = (unsigned char)0x02,
 KEY_STATUS_REPEAT = (unsigned char)0x04,
 } keyStatus_t;

 typedef enum keyType_e {
 KEY_TYPE_ADC_A = (unsigned char)0x01,
 KEY_TYPE_ADC_B = (unsigned char)0x02,
 KEY_TYPE_GPIO = (unsigned char)0x03,
 } keyType_t;

 typedef struct keyAttrAdc_s {
 UINT8 keyNum;
 UINT8 keyType;
 UINT8* pKeyEn;
 } keyAttrAdc_t;

 typedef struct keyAttrGpio_s {
 UINT8 keyStsPrev;
 UINT8* pKeyEn;
 } keyAttrGpio_t;

 typedef struct keyMsg_s {
 UINT16 keyMsgPrs;
 UINT16 keyMsgRpt;
 UINT16 keyMsgRls;
 } keyMsg_t; 

 typedef struct keyCtrlAdc_s {
 UINT8 keyDec;
 UINT16 code* pKeyVal;
 UINT16 code* pKeyDev;
 UINT16 keyValPrev;
 UINT16 keyValRpt;
 } keyCtrlAdc_t;

 typedef struct keyCtrlGpio_s {
 UINT8 keyDec;
 UINT16 keyValRpt;
 } keyCtrlGpio_t;

 typedef struct keyUnitAdc_s {
 struct keyAttrAdc_s keyAttr;
 struct keyMsg_s code* pKeyMsg;
 struct keyCtrlAdc_s keyCtrl;
 UINT8 keyActiveIdx;
 } keyUnitAdc_t;

 typedef struct keyUnitGpio_s {
 struct keyAttrGpio_s keyAttr[4];
 struct keyMsg_s code* pKeyMsg;
 struct keyCtrlGpio_s keyCtrl;
 } keyUnitGpio_t;

 typedef struct battUnitAdc_s {
 UINT8 battLvl;
 UINT8 battDec;
 UINT8 battDetectEn;
 UINT8 battLvlTol;
 UINT16* pbattAdcVal;
 UINT16 battAdcDev;
 } battUnitAdc_t;

 typedef struct scanTag_s {
 struct keyUnitAdc_s keyAdcA;
 struct keyUnitAdc_s keyAdcB;
 struct keyUnitGpio_s keyGpio;
 struct battUnitAdc_s battAdc;
 } scanTag_t;

 void keyScanInit(void);
 void keyScan(void);
 UINT8 sp1kBtnCfg(keyType_t BtnType, UINT16 BitMask, UINT8 Enable);
 UINT8 sp1kBtnCfgIntr(keyType_t BtnType, UINT16 BitMask, UINT8 Enable);
 void sp1kAdcHdlGet(const scanTag_t** pAdcHdl);
 UINT8 battDetectInit(void);
 UINT8 battDetectCfg(UINT8 battLvlTol, UINT16* pBattAdcVal, UINT16 battAdvValDev);
 void sp1kBattEnable(UINT8 enable);
 UINT8 sp1kBtnChk(UINT8 button);

 typedef void (code* pFuncTmr0)(void);
 typedef void (code* pFuncTmr1)(UINT8*);

 void TIMER0_Routine(void);
 void TIMER0_Handler(void);
 void TIMER0_Start(void);
 void TIMER0_Stop(void);
 void TIMER0_PowerOffCfg(UINT8 enable, UINT32 delay);
 UINT8 TIMER0_PowerOffProcChk(void);

 void DELAY_10us(void);
 void DELAY_1ms(void);
 void DELAY_1s(void);

 void TIMER1_Config(UINT32 Period, pFuncTmr1 pFunc, UINT8* pFuncPara);
 void TIMER1_Routine(void);
 void TIMER1_Handler(void);
 void TIMER1_Start(void);
 void TIMER1_Stop(void);

 UINT32 videoRecStampAddrGet(void);
 void videoRecStampEnable(UINT8 enable, UINT8 init);
 UINT32 INTR_videoRecStampAddrGet(void);

 UINT8 fwUpdateDo(void);

 UINT8 dramFileSave(UINT8 *name,UINT32 startAddrWord, UINT32 size);
 UINT8 dramFileLoad(UINT8 *name,UINT32 startAddrWord, UINT32 *size);

 void csSvcProc(void);

 typedef enum {
 SP1K_SPI_CALLBACK_WP_SET = 0,
 SP1K_SPI_CALLBACK_WP_MAX
 } SfCallback_e;

 typedef enum{
 SPI_STS_UNLOCK		= (UINT8)0x00,
 SPI_STS_LOCK_0		= (UINT8)0x01,
 SPI_STS_LOCK_ALL	= (UINT8)0x02,
 SPI_STS_LOCK_MAX	= (UINT8)0xFF,
 }spiLockSts_e;

 void HAL_SPIWritePort(UINT8 PortData);
 UINT8 HAL_SPIReadPort(void);
 UINT8 HAL_SPIInit(UINT8 Freq,UINT8 Mode);
 void sfCallbackReg(UINT32 idx,void * pfunct);

 void sp1kPvPhotofrmModeSet(UINT8 mode);

 void sp1kRTCInit(void);

 UINT8 sp1kSuspendResume(void);

 void sp1kLoadDateStampData( void );

 void sp1kDateStampCustomStrSet(UINT8 *str);

 void sp1kDateStampParamSet(UINT8 paraId, UINT16 value);

 UINT16 sp1kDateStampParamGet(UINT8 paraId);

 void sp1kUsbPccamStart(void);

 void sp1kUsbIntrDataTx(UINT8 Event,UINT8 Value);

 void sp1kUsbPccamStop( void );

 UINT8 sp1kUsbStrSet(UINT32 resIdx );

 void sp1kSuspendUiIntrSet(UINT8 enable);

 void sp1kCodeSentrySet(UINT8 enable);

 void sp1kUsbMsdcMLunSet(UINT8 en);

 void sp1kFwUpdateAddrSet(UINT32 addr);

 UINT8 sp1kCardFirmwareUpdate(UINT8 *name, UINT32 addr);

 UINT8 sp1kCardFirmwareBoot(UINT8 *name,	UINT32 addr);

 void sp1kCardResourceUpdate(void);

 void sp1kRTCDateInit(pDateStc_t DatePt);

 UINT8 sp1kUIToolSet(UINT8 order,UINT8 id,UINT32 addr,UINT32 size,UINT16 resid,UINT16 msg);
 UINT8 sp1kUIToolGet(	UINT8 id,	UINT32 xdata* addr,	UINT32 xdata* size);

 typedef enum devId_e {
 DEV_ID_CARD = (unsigned char)0x00,
 DEV_ID_DISK = (unsigned char)0x01,
 DEV_ID_PANEL = (unsigned char)0x02,
 DEV_ID_TV = (unsigned char)0x03,
 DEV_ID_KEYBROAD = (unsigned char)0x04,
 DEV_ID_USB = (unsigned char)0x05,
 DEV_ID_FRONT = (unsigned char)0x06,
 DEV_ID_SPEAKER = (unsigned char)0x07,
 DEV_ID_PRINTER = (unsigned char)0x08,
 DEV_ID_MAX = (unsigned char)0x09,
 } devId_t;

 typedef enum devSts_e {
 DEV_STS_RDY = (unsigned char)0x00,
 DEV_STS_BUSY = (unsigned char)0x01,
 DEV_STS_ERR = (unsigned char)0x02,
 DEV_STS_CONNECTED = (unsigned char)0x0f,
 DEV_STS_DISCONNECTED = (unsigned char)0xf0,
 DEV_STS_UNKNOWN = (unsigned char)0xff,
 } devSts_t;

 typedef enum devType_e {
 DEV_TYPE_CARD_COM = 0x00,  
 DEV_TYPE_CARD_SD = 0x01,
 DEV_TYPE_CARD_MMC = 0x02,

 DEV_TYPE_DISK_COM = 0x10,
 DEV_TYPE_DISK_NAND = 0x11,
 DEV_TYPE_DISK_NOR = 0x12,
 DEV_TYPE_DISK_SPI = 0x13,
 DEV_TYPE_DISK_DRAM = 0x14,

 DEV_TYPE_PANEL_COM = 0x20,
 DEV_TYPE_PANEL_LCD = 0x21,
 DEV_TYPE_PANEL_LCM = 0x22,

 DEV_TYPE_TV_COM = 0x30,
 DEV_TYPE_TV_NTSC = 0x31,
 DEV_TYPE_TV_PAL = 0x32,

 DEV_TYPE_KEYBROAD_COM = 0x40,

 DEV_TYPE_USB_COM = 0x50,
 DEV_TYPE_USB_20 = 0x51,
 DEV_TYPE_USB_11 = 0x52,

 DEV_TYPE_FRONT_COM = 0x60,

 DEV_TYPE_SPEAK_COM = 0x70,

 DEV_TYPE_PRINTER_COM = 0x80,

 DEV_TYPE_UNKNOWN = 0xff,  
 } devType_t;

 typedef struct devAttr_s {
 UINT8 devId;

 UINT8 devEn;
 UINT8 devSts;
 UINT8 devType;

 } devAttr_t;

 UINT8 sp1kDevInit(void);
 devAttr_t* sp1kDevHdlGet(void);

 UINT8 dispPanelOpen(void);

 UINT8 dispNtscOpen(void);

 UINT8 dispPalOpen(void);

 UINT8 hdmiOpen(void);

 UINT8 frontSensorOpen(void);

 struct DATESTC
 {
 UINT8 Year;
 UINT8 Month;
 UINT8 Day;
 UINT8 Hour;
 UINT8 Minute;
 UINT8 Second;
 };

 typedef enum {
 HAL_DRAM_DISP_SRC_PB  = (0 << 0),
 HAL_DRAM_DISP_SRC_MPG = (2 << 0),
 } HAL_DramDispSrc_e;

 typedef enum {
 LOG_BANK_COM = 0,
 LOG_BANK_0,
 LOG_BANK_1,
 LOG_BANK_2,
 LOG_BANK_3,
 LOG_BANK_4,
 LOG_BANK_5,
 LOG_BANK_6,
 LOG_BANK_7,
 LOG_BANK_8,
 LOG_BANK_9,
 LOG_BANK_10,
 LOG_BANK_11,
 LOG_BANK_12,
 LOG_BANK_13,
 LOG_BANK_14,
 LOG_BANK_15,
 LOG_BANK_16,
 LOG_BANK_17,
 LOG_BANK_18,
 LOG_BANK_19,
 LOG_BANK_MAX,
 } logBank_t;

 UINT8  HAL_Suspend(UINT8);
 void   HAL_Resume(void);

 UINT8 HAL_CpuImemSdSet(logBank_t Bank, UINT16 Addr);
 void HAL_CpuImemSdGet(logBank_t *Bank, UINT16 *Addr);

 void awbDefaultProc(void);
 void awbDefaultProc_Video(void);
 void awbDefaultProc_PostWB( void );
 void aeDefaultProc(void);
 void fdDefaultProc(void);
 UINT8 HAL_AWBWindowRead1024(void);
 void audioAlcCal_Intr(void);

 typedef UINT8 raw_pixel_t;
 typedef UINT16 raw_gain_t;  
 typedef UINT8 pixel_cnt_t;  

 UINT8 awbWhitePointFilter(raw_pixel_t	R,raw_pixel_t	G,raw_pixel_t	B	);
 void awbRBGainDecider(void);
 void awbThresholdInit(UINT8 mode);
 void awbCalConvergeGain(void);

 static UINT16 Slop_Calculator(	UINT16	start_x,	UINT16	start_y,	UINT16	end_x,	UINT16	end_y	);
 static UINT8 Slop_filter(	UINT16	start_x,	UINT16	start_y,	UINT16	end_x,	UINT16	end_y,	UINT16	slop);

 void  HAL_DRAMsizetest(void);

 UINT8 HAL_CpuRomSdToDram(void);
 UINT8 HAL_CpuDramSdToImem(void);
 UINT8 HAL_DramSkipWrite2Dram(void);

 UINT8 HAL_InitLCDTV(UINT8 CamMode);
 UINT8 HAL_SetDisplaySrcImgSize(UINT16 hsize, UINT16 vsize, UINT8 type);
 UINT8 HAL_DefineColorKey(UINT8 attrib);

 UINT8 HAL_InitFront(UINT8 CamMode);
 UINT8 HAL_SSCConfig (UINT8 SlaveAddr, UINT8 Frequency);
 UINT8 HAL_WriteSSC (UINT8 *RegAddr, UINT8 *RegData, UINT8 Count, UINT8 Option);
 UINT8 HAL_WriteSSC_Intr (UINT8 *RegAddr, UINT8 *RegData, UINT8 Count, UINT8 Option);
 UINT8 HAL_ReadSSC(UINT8 RegAddr, UINT8 *RegData, UINT8 Count, UINT8 Option);
 UINT8 HAL_SetFrontTGEn(UINT16 LineTotal, UINT16 LineBlank, UINT16 FrameTotal, UINT16 FrameBlank, UINT8 SyncMode, UINT8 TGEn);
 UINT8 HAL_SetFrontYUVEn(UINT8 UVSel, UINT8 YSub128en, UINT8 USub128en, UINT8 VSub128en, UINT8 Bt656en, UINT8 YUVen);
 UINT8 HAL_SetFrontYUVEn(UINT8 UVSel,UINT8 YSub128en,UINT8 USub128en,
 UINT8 VSub128en,UINT8 Bt656en,UINT8 YUVen);
 UINT8 HAL_SetFrontInverse(UINT8 Hinverse, UINT8 Vinverse);
 UINT8 HAL_SetFrontImgCrop(UINT16 HOffset, UINT16 VOffset, UINT16 HSize, UINT16 VSize, UINT8 CropMode);
 UINT8 HAL_SetFrontReshape(UINT16 HFall, UINT16 HRise, UINT8 HReshen, UINT16 VFall, UINT16 VRise, UINT8 VReshen);
 UINT8 HAL_SetFrontIOConfig(UINT8 TGGPIOSel, UINT8 TGInputGate, UINT8 TGGPIOen, UINT8 TGGPIOVal);
 UINT8 HAL_SetFrontCLKSet(UINT8 MasterClkDiv, UINT8 PixelClkDiv, UINT8 SyncMode);
 UINT8 HAL_SetFrontCLKSource(UINT8 MasterClkSrc, UINT8 PixelClkSrc);
 UINT8 HAL_SetFrontInterrupten(UINT16 Enable);
 UINT8 HAL_ClrFrontInterrupt(UINT16 Clear);
 UINT8 HAL_FrontInterruptCfg(UINT8 VdRintNum, UINT8 VdFintNum, UINT16 IntHNum0, UINT16 IntHNum1, UINT16 IntHNum2, UINT16 IntHNum3);

 UINT8 HAL_CpuSramModeSet(UINT8);
 UINT8 HAL_CpuRamMapSet(UINT32 dramAddrWord, UINT16 mapSizeByte, void xdata* xdata* ppMem);
 UINT8 HAL_CpuRamMapRcv(void);

 UINT8 HAL_DownloadROM(UINT16, UINT32, UINT16);
 void  HAL_Int0(void);
 void  HAL_EnterInterrupt(void);
 void  HAL_LeaveInterrupt(void);
 void  HAL_DisableInterrupt(void);
 void  HAL_EnableInterrupt(void);
 void  HAL_CtlInOutEnable(void);

 UINT8 HAL_NandDMACheck(void);
 UINT8 HAL_DoNandDMA(UINT8 , UINT16 );
 UINT8 HAL_DmaDo(UINT8, UINT8, UINT32, UINT8, UINT8);
 void  HAL_DmaReset(void);
 UINT8 HAL_DmaSetDramAddr(UINT32);
 UINT8 HAL_DmaSetSramAddr(UINT16);

 UINT8 HAL_SetAudDMA(UINT8 );
 void  HAL_DmaReadDramAddr(UINT32* );

 UINT8 INTR_HAL_DmaSetDramAddr(UINT32 DRAMAddr);
 UINT8 INTR_HAL_DmaSetSramAddr(UINT16 SramRstIdx);
 UINT8 INTR_HAL_DmaDo(UINT8 Src,UINT8 Dst, UINT32 nByte, UINT8 Mode, UINT8 SramMode);

 UINT8 HAL_FlashMode(UINT8, UINT8, UINT8);
 UINT8 HAL_FlashMode_Intr(UINT8, UINT8, UINT8);
 UINT8 HAL_ReadECC(UINT8, UINT8*);
 void HAL_ClearECC(void);
 void HAL_ECCMode(UINT8);

 typedef enum CDSPrstIdx_e {
 CDSPRstCDSP = 0,
 CDSPRstAE,
 CDSPRstAWB,
 CDSPRstEDGE_LSC,
 CDSPRstDramV,
 CDSPRstDramV1,
 CDSPRstVDram,
 CDSPRstVDram1,
 }CDSPrstIdx_t;

 typedef enum cdspSkipWr_e {
 CDSP_SKIP_IMG 	   = 0xfe,
 CDSP_SKIP_AWB 	   = 0xfd,
 CDSP_SKIP_AE  	   = 0xfb,
 CDSP_SKIP_DISTEDGE = 0xf7,	
 CDSP_SKIP_ALL 	   = 0xf0,
 CDSP_SKIP_AWBAEEDGE= 0xf1,
 CDSP_SKIP_AEEDGE   = 0xf3,
 }cdspSkipWr_t;

 UINT8 HAL_CdspCropSet(UINT8 crop_henable ,UINT8 crop_venable ,UINT8 crop_sync_enable, UINT16 crop_hoffset,UINT16 crop_hsize,UINT16 crop_voffset,UINT16 crop_vsize);

 UINT8 HAL_CdspHScaleRgbSet(UINT8 hscale_enable, UINT8 hscale_mode,UINT8 hscale_sync_mode,UINT16 shsize,UINT16 dhsize);
 UINT8 HAL_CdspHscaleRGBEnable(UINT8 hscale_enable, UINT8 hscale_mode,UINT8 hscale_sync_mode);

 void HAL_CdspWriteBufIdxSet(UINT8  bufIdx);
 UINT8 HAL_CdspWriteBufIdxGet(void);
 void HAL_CdspFrmCtrlEnSet(UINT8 en);

 UINT8 HAL_CdspClockSet(UINT8 src,UINT8 clkdiv);

 UINT8 HAL_CdspRstBlock(UINT8 cdspRstIdx);

 UINT8 HAL_CdspDramVMirrorSet(UINT8 mirrorLine,UINT8 vsMirrorEn,UINT8 veMirrorEn);
 UINT8 HAL_CdspDramVSkippixelSet(UINT8 skipPixel);
 UINT8 HAL_CdspDramVSizeOffsetSet(UINT16 rHSize,UINT16 rHOffset,UINT16 rVSize,UINT16 rVOffset);
 UINT8 HAL_CdspDramVModeSet(UINT8 remode);

 void HAL_CdspVDramSkipWrSyncSet(UINT8 sync);
 void HAL_CdspVDramSkipWrSet(UINT8 mask,UINT8 val);
 void HAL_CdspVDramSkipWrGet(UINT8 *val);

 UINT8 HAL_CdspVDramDataType(UINT8 cdspType,UINT8 cdspSync,UINT8 cdspRange);
 UINT8 HAL_CdspVDramSizeClamp(UINT8 hSizeClpEn, UINT16 hSize, UINT8 vSizeClpEn, UINT16 vSize, UINT8 sizeSVd);
 UINT8 HAL_CdspRawImgSource(UINT8 srcSel);
 UINT8 HAL_CdspCoreImgSource(UINT8 src,UINT8 sync);

 UINT8 HAL_CdspEofWait( UINT8 Number );
 UINT8 HAL_CdspDramVInterval(UINT16 lineInterval);

 UINT8 HAL_CdspAEWinEnSet(UINT8 aeWinEn, UINT8 aeHoldSvdEn, UINT8 aeSizeZf);
 UINT8 HAL_CdspAEWinModeSet(UINT8 aeWinMode);
 UINT8 HAL_CdspAEWinModeGet(void);
 UINT8 HAL_CdspAEWinHoldSet(UINT8 aeWinHold);
 UINT8 HAL_IntrCdspAEWinHoldSet(UINT8 aeWinHold);
 UINT8 HAL_CdspAEWinTest(UINT8 aeWinTest);

 UINT8 HAL_CdspAEwinfactorset(UINT8 aeHAccFactor, UINT8 aeVAccFactor);
 UINT8 HAL_CdspAEwinoffset(UINT16 aeHOffset, UINT16 aeVOffset);
 UINT8 HAL_CdspAEwinsizeset(UINT8 aeHSize, UINT8  aeVSize);
 UINT8 HAL_CdspAEwinintvlset(UINT8 aeHIntvl,UINT8 AEVIntvl);
 UINT8 HAL_CdspAEwinAddrset(UINT32 aeWinAddr);
 UINT8 HAL_CdspAEwinCntset(UINT8 aeHCnt,UINT8 aeVCnt);
 UINT8 HAL_CdspAEWinRead(UINT8 aeWinNum,UINT8 *aeWin);

 UINT8 HAL_CdspAWBWinEn(UINT8 awbWinEn,UINT8 pAwbWinHold,UINT8 awbWinTest,UINT8 awbWinSvd,UINT8 awbSizeZfEn);
 UINT8 HAL_IntrCdspAWBWinEn(UINT8 awbWinEn,UINT8 pAwbWinHold,UINT8 awbWinTest,UINT8 awbWinSvd,UINT8 awbSizeZfEn);

 UINT8 HAL_CdspAWBWinScaleEn(UINT8 AWBhscaleen, UINT8 AWBvscaleen);
 UINT8 HAL_CdspAWBWincfg(UINT16 AWBhstart, UINT16 AWBhend,UINT16 AWBvstart,UINT16 AWBvend,UINT16 AWBhscalefactor,UINT16 AWBvscalefactor);
 UINT8 HAL_CdspAWBwinAddrset(UINT32 AWBWinaddr);
 UINT8 HAL_CdspAWBWindowSizeSet(UINT16 AWBhstart,UINT16 AWBhend,UINT16 AWBvstart,UINT16 AWBvend, UINT8 WinhSize, UINT8 WinvSize);
 UINT8 HAL_CdspAWBWndGainSet(UINT8 AWBrgain,UINT8 AWBbgain);
 UINT8 HAL_CdspAWBWinRead(UINT16 AWBWinNum,UINT8 *AWBWin);

 UINT8 HAL_CdspAFWinEn(UINT8 afWinEn,UINT8 pAFWinHold,UINT8 afWinTest,UINT8 afFlt2Type,UINT8 afWinHoldSvd);
 UINT8 HAL_CdspAFDataModeSet(UINT8 afRgbMode,UINT8 afPgType);
 UINT8 HAL_CdspAFFilterWtSet(UINT8 afFlt1Wt,UINT8 afFlt2Wt);
 UINT8 HAL_CdspAFCoringSet(UINT8 afCorThr,UINT8 afCorVal);
 UINT8 HAL_CdspAFWinSizeSet(UINT16 afHStart,UINT16 afHEnd,UINT16 afVStart,UINT16 afVEnd);
 UINT8 HAL_ReadAFWindow(UINT32* afWinValue);

 UINT8 HAL_CdspDistEdgeWinEn(UINT8 edgeWinEn,UINT8 edgeWinHold,UINT8 edgeWinTest,UINT8 edgePosition,	UINT8 edgeHoldSvdEn);
 UINT8 HAL_CdspDistEdgeWinOffSet(UINT8 edgeHOffset, UINT8 edgeVOffset);
 UINT8 HAL_CdspDistEdgeWinSizeSet(UINT8 edgeHSize, UINT8 edgeVSize);
 UINT8 HAL_CdspDistEdgeWinCntSet(UINT8 edgeHCnt,UINT8 edgeVCnt);
 UINT8 HAL_CdspDistEdgeWinThrSet(UINT8 edgeHThr,UINT8 edgeVThr);
 UINT8 HAL_CdspDistEdgeWinAddrSet(UINT32 edgeWinAddrA,UINT32 edgeWinAddrB);
 UINT8 HAL_CdspDistEdgeWinMap(UINT8 edgeHMap,UINT8 edgeVMap);
 UINT8 HAL_CdspDistEdgeWinRead(UINT16 edgeWinNum,UINT8 *edgeWin);

 UINT8 HAL_CdspIntplmir(UINT8 intplMirHLEn,UINT8 intplMirHREn,UINT8 intplMirVTEn,UINT8 intplMirVDEn);
 UINT8 HAL_CdspIntplLowThrSet(	UINT8 intplLowThr);

 void HAL_CdspRegBackup(void);
 void HAL_CdspRegBackupBufSet(UINT32 addr,UINT16 size);
 void HAL_CdspRegBackupCntSet(UINT16 cnt);

 void HAL_CdspRegGet(UINT16 addr,UINT8 *val);
 void HAL_CdspRegSet(UINT16 addr,UINT8 val);

 void HAL_CdspBayerPatternSet(UINT8 ptn);

 UINT8 HAL_CdspColorMtxSet( UINT16 a00, UINT16 a01, UINT16 a02, UINT16 a10, UINT16 a11, UINT16 a12, UINT16 a20, UINT16 a21, UINT16 a22 );

 UINT8 HAL_CdspBriContEnSet(UINT8 ybycen);
 UINT8 HAL_CdspYThrEnSet(UINT8 ythren);
 UINT8 HAL_CdspYRevrseEnSet(UINT8 yreven);
 UINT8 HAL_CdspYThrSet(UINT8 ythr);

 UINT8 HAL_CdspYBrightOffsetSet(SINT8 yb);
 UINT8 HAL_CdspYContrastOffsetSet(UINT8 yc);

 UINT8 HAL_CdspSatHueEnSet(UINT8 enable);
 UINT8 HAL_CdspSatHueParaSet(UINT8 uvsat,UINT8 huesin1data,UINT8 huecosdata,UINT8 huesin2data,UINT8 uOffset ,UINT8 vOffset);

 UINT8 HAL_CdspWBGainEnSet( UINT8 enable );
 UINT8 HAL_CdspWBGainEnGet( void );
 UINT8 HAL_CdspWBOffsetEnSet( UINT8 enable );
 UINT8 HAL_CdspWBOffsetEnGet( void );
 UINT8 HAL_CdspWBGainSet(UINT16 rGain, UINT16 grGain, UINT16 bGain, UINT16 gbGain);
 UINT8 HAL_CdspWBGainGet(UINT8 *rGain, UINT8 *grGain, UINT8 *bGain, UINT8 *gbGain);
 UINT8 HAL_CdspWBOffsetSet( UINT16 rOffset, UINT16 grOffset, UINT16 bOffset, UINT16 gbOffset);
 UINT8 HAL_CdspWBOffsetGet( UINT16 *rOffset, UINT16 *grOffset, UINT16 *bOffset, UINT16 *gbOffset);
 UINT8 HAL_CdspWBOffsetCfgSet(UINT8 gOsVdEn, UINT8 wbOValidOn);

 UINT8 HAL_CdspGammaEnSet( UINT8 enable );
 UINT8 HAL_CdspGammaEnGet( void );
 UINT8 HAL_CdspGammalTbInit(UINT8 *pGammaData);

 UINT8 HAL_CdspBPEnSet( UINT8 enable );
 UINT8 HAL_CdspBPEnGet( void );

 UINT8 HAL_CdspLscEnSetGet( void );
 UINT8 HAL_CdspLscWinSizeSet(UINT8 lscHSize);
 UINT8 HAL_CdspLscWinStepSet(UINT8 lscHStep,UINT8 lscVStep);

 UINT8 HAL_CdspLscWinIncStepSet(UINT8 lscHOIncStep,UINT8 lscHEIncStep,UINT8 lscVOIncStep,UINT8 lscVEIncStep);
 UINT8 HAL_CdspLscResolution(UINT8 lscRes);
 UINT8 HAL_CdspLscWinOffsetSet(UINT8 lscHOff,UINT8 lscVOff);
 UINT8 HAL_CdspLscEnSet(UINT8 lscEnable);
 UINT8 HAL_CdspLscEnGet(void);
 UINT8 HAL_CdspLscModeSet(UINT8 lscChMode);

 UINT8 HAL_CdspLscWinAddrset(UINT32 lscWinAddr);

 void HAL_CdspYuvFilterBufSet(UINT8 yBufEn);

 void HAL_CdspYEdgeEnSet(UINT8 yEdgeEn);
 void HAL_CdspLEdgeEnSet(UINT8 lEdgeEn);
 void HAL_CdspYCoringEnSet(UINT8 yCoringEn);
 void HAL_CdspUVMedianEnSet(UINT8 uvMedianEn);
 void HAL_CdspMedianModeSet(UINT8 medianMode);

 UINT8 HAL_CdspEdgeSuppressionMir (UINT8 edgeMirHLEn,UINT8 edgeMirHREn,UINT8 edgeMirVTEn,UINT8 edgeMirVDEn);
 UINT8 HAL_CdspYCoringCoefficientSet(UINT8 *par);
 UINT8 HAL_CdspEdgeYHDivSet(UINT8 yhDiv);
 UINT8 HAL_CdspEdgeYHClampSet(UINT8 yhtClamp);
 UINT8 HAL_CdspEdgeGainSet(UINT8 gain1, UINT8 gain2);
 UINT8 HAL_CdspEdgeThrSet(UINT8 yhtLowThr,UINT8 yhtHighThr);
 UINT8 HAL_CdspEdgeBwSet(UINT8 yhtBw);
 UINT8 HAL_CdspEdgeFilterSet(	UINT8 ftr0,	UINT8 ftr5,	UINT8 ftr4,	UINT8 ftr3,	UINT8 ftr2,	UINT8 ftr1);

 void HAL_CdspEffectYuvNormalSave( void );
 void HAL_CdspEffectYuvNormalResotre( void );

 UINT8 HAL_CdspDarkTDSizeSet(UINT16 hSize);
 UINT8 HAL_IntrCdspDarkTDSizeSet(UINT16 hSize);
 UINT8 HAL_CdspDarkCfg(UINT8 src,UINT8 mode);
 UINT8 HAL_CdspDarkEnSet(UINT8 en);

 UINT8 HAL_CdspStillBpCfg(UINT8 mode,UINT8 sync,	UINT8 rip,UINT16 hSize);
 UINT8 HAL_CdspStillBpEnSet(UINT8 en);
 UINT8 HAL_CdspStillBpTblAddrSet(UINT32 bpTblAddr);
 UINT8 HAL_CdspStillBpRAWAddrSet(UINT32 bpRawAddr);
 UINT8 HAL_CdspStillBpTrigger(UINT8 bpTrig);

 UINT8 HAL_CdspPvBpTblAddrSet(UINT32 pvBPTblAddr);
 UINT8 HAL_CdspPvBpEnSet(UINT8 pvBpEn);

 UINT8 HAL_CdspWDRTblnit(UINT8* pWdrData);
 UINT8 HAL_CdspWDREnSet(UINT8 en);

 UINT8 HAL_CdspTDCfg(UINT8 rst,UINT8 sync,UINT8 zf);
 UINT8 HAL_CdspTDEnSet(UINT8 en);
 UINT8 HAL_IntrCdspTDEnSet(UINT8 en);

 UINT8 HAL_CdspHueCorrectionEnSet(UINT8 enable  );
 UINT8 HAL_CdspHueCorrectionXSet(UINT16 angleX,  UINT8 n1x, UINT8 n2x ,UINT8 n3x, UINT8 n4x,UINT8 shx,UINT8 ssx );
 UINT8 HAL_CdspHueCorrectionYSet(UINT16 angleY,  UINT8 n1y, UINT8 n2y ,UINT8 n3y, UINT8 n4y,UINT8 shy,UINT8 ssy );
 UINT8 HAL_CdspHueCorrectionZSet(UINT16 angleZ,  UINT8 n1z, UINT8 n2z ,UINT8 n3z, UINT8 n4z,UINT8 shz,UINT8 ssz, UINT8 yThrz );

 void cdspBinRead( UINT8 *f, UINT32 startAddr );
 void hueBinRead( UINT8 *f, UINT8 channel, UINT32 startAddr );
 void lscBinRead( UINT8 *f, UINT32 startAddr );
 void gammaBinRead( UINT8 *f, UINT32 startAddr );
 void wdrBinRead( UINT8 *f, UINT32 startAddr );

 void pvCdspBinRead( UINT8 *f, UINT32 startAddr );
 void pvCdspDynamicBinRead(UINT8 *f, UINT8 intensity, UINT32 startAddr);
 void pvCdspDynamicBinReadtoPool(UINT8 *f,	UINT32 startAddr);
 void cdspPvDynamicEnhanSet(UINT8 fileId, UINT8 intensity);
 void cdspPvDynamicParaFullLoad(UINT8 fileId);

 void stillCdspBinRead( UINT8 *f, UINT32 startAddr );
 void stillCdspDynamicBinRead(UINT8 *f, UINT8 intensity, UINT32 startAddr);
 void cdspStillDynamicEnhanSet(UINT8 fileId, UINT8 intensity);

 void iqDataWrite( UINT8 id, UINT32 startAddr );
 void criteriaDataRead( UINT32 startAddr );
 void criteriaDataWrite( UINT32 startAddr );

 void cdspColorMatrixRestore(void);
 void cdspWdrInit(void);

 void cdspHueBinSet( UINT8 fileId, UINT8 channel);

 UINT8 HAL_FrontParaSet(UINT8 id, SINT16 para1, SINT16 para2, SINT16 para3, SINT16 para4, UINT8 upd);

 typedef struct _AEstr{
 unsigned short ev;
 unsigned long shutter;
 unsigned short gain;
 unsigned short iris;
 }AEstr;

 typedef struct _AECQ{
 unsigned long shutter;
 unsigned short gain;
 }AECQ;

 typedef struct _AEHeaderInformation{
 UINT8 aeProc;
 UINT8 stableCondition;
 UINT8 luma[2];            
 UINT16 gId[2];            
 UINT32 shutter[2];        
 UINT16 gain[2];           
 UINT8 Reserved[16];
 }AEHeaderInformation;

 typedef struct _CaptureHeaderInformation{
 UINT32 snapPclk;
 UINT16 lineTot;
 UINT16 frameTot;
 UINT16 gain;
 UINT32 espline;
 UINT32 compensationLine;	
 UINT8 Reserved[16];
 }CaptureHeaderInformation;

 typedef struct _FlashHeaderInformation{
 UINT32 preFlashShutter;
 UINT16 preFlashGain;
 UINT16 preFlashTime;
 UINT32 mainFlashShutter;
 UINT16 mainFlashGain;
 UINT16 mainFlashTime;
 UINT16 beforePreFlashLuma;
 UINT16 afterPreFlashLuma;
 UINT8 previewToCaptureRate;
 UINT8 Reserved[16];
 }FlashHeaderInformation;

 typedef struct _BShutterHeaderInformation{
 UINT8 Reserved[20];
 }BShutterHeaderInformation;

 typedef struct _MShutterHeaderInformation{
 UINT8 Reserved[20];
 }MShutterHeaderInformation;

 typedef struct _CaptureHeaderAeInformation{
 UINT8 id[4];
 AEHeaderInformation ae;
 FlashHeaderInformation flash;
 BShutterHeaderInformation bShutter;
 MShutterHeaderInformation mShutter;
 CaptureHeaderInformation capture;
 UINT8 Reserved[16];
 }CaptureHeaderAeInformation;

 typedef enum {
 HEADER_INFORMATION_AE_CONFIG = 0,
 HEADER_INFORMATION_FLASH_CONFIG,
 HEADER_INFORMATION_BSHUTTER_CONFIG,
 HEADER_INFORMATION_MSHUTTER_CONFIG,
 HEADER_INFORMATION_CAPTURE_CONFIG,
 }CAPTURE_HEADER_AE_INFORMATION_CONFIG;

 typedef enum {
 AE_DISABLE = 0,
 AE_ENABLE
 }AE_EN_MODE;

 typedef enum {
 AE_WIN_MODE_Y = 0,
 AE_WIN_MODE_RGB
 }AE_WIN_MODE;

 typedef enum {
 AE_SET_FINISH = 0, 
 AE_SET_START
 }AE_SETTING_STATUS;

 typedef enum {
 AE_EXP_GAIN_NOT_ACTION_FOR_YUV = 0,
 AE_EXP_GAIN_NOT_ACTION_FOR_RGB,
 AE_EXP_GAIN_SET_POS_VD_RISING,
 AE_EXP_GAIN_SET_POS_VVALID_RISING,

 }AE_EXP_GAIN_SET_POS;

 typedef enum {
 AE_GAIN_AFTER_EXP_OFF = 0,
 AE_GAIN_AFTER_EXP_ON
 }AE_GAIN_AFTER_EXP;

 typedef enum {
 AE_NOT_READY = 0,
 AE_READY
 }AE_READY_STATUS;

 typedef enum {
 LUMA_NORMAL = 0,
 LUMA_CAL_COLOR_CHECKER,
 LUMA_CAL_LSC
 }CALCULATE_LUMA_MODE;

 typedef enum {
 AE_STATUS_SET_GAIN = 0,
 AE_STATUS_EFFECT_FRAME,
 AE_STATUS_PROC_TRIGGER,
 AE_STATUS_SET_EXPOSURE,
 }AE_INTR_FLOW_STATUS;

 typedef enum {
 AE_TABLE_AE = 0,
 AE_TABLE_GAIN,
 }AE_TABLE_SELECT;

 typedef enum {
 AE_PCCAM_320_240 = 0,
 AE_PCCAM_160_120,
 AE_PCCAM_SIZE_MAX,
 }AE_PCCAM_SRC_SIZE_MODE;

 typedef enum {
 SP1K_AE_TARGET_LUMA = 0, 
 SP1K_AE_EV_COMP, 
 SP1K_AE_FRAME_RATE, 
 SP1K_AE_METERING, 
 SP1K_AE_USING_AWB_GAIN, 
 SP1K_AE_CONVERAGE_ZONE, 
 SP1K_AE_GAINSHUTTERTBL, 

 SP1K_AE_LowLight, 
 SP1K_AE_IsReady, 
 SP1K_AE_EV_GET, 
 SP1K_AE_Count, 
 SP1K_AE_G_GAIN, 
 SP1K_AE_gId, 
 SP1K_AE_G_AETblSel, 
 SP1K_AE_G_CaptureCompensationGAIN, 
 SP1K_AE_FRAMERATE, 
 SP1K_AE_CAPTURE_ISO_MODE, 
 SP1K_AE_CURRENT_LUMA, 
 SP1K_AE_CAPTURE_ISO_VALUE, 
 SP1K_AE_CAPTURE_ANTISHAKE_FPS, 
 SP1K_AE_MAX_COVER_ZONE, 
 SP1K_AE_MIN_COVER_ZONE, 
 SP1K_AE_SPEED, 
 }SP1K_AE_PARAMETER_IDX;

 extern UINT8	G_AEEN;

 UINT8 aeInitial(void);
 void aeExposureGainSet(UINT8 vdCount);
 UINT8 aeGetCount(void);
 UINT8 aeMeterChangeSet(UINT8 mode);
 UINT8 aeMeteringModeGet(void);
 UINT8 aeStdLumiSet(UINT8 lumi);
 UINT8 aeExpCompSet(UINT8 light);
 SINT8 aeExpCompGet(void);
 SINT8 aeExpCompIdxGet(void);
 UINT8 aeFrameRateChangeSet(UINT16 framerate);
 void aeForceSet(void);
 void aeFrmInvalidSet(void);
 void aeModeEnableSet(UINT8 enable);
 UINT8 aeModeEnableGet(void);
 void aeWinModeSet(UINT8 aeMode);
 void aeGlobalGainEnableSet(UINT8 GlbGnEn);
 void aeWinConfigSet(UINT16 hSize, UINT16 vSize, UINT16 aeDispFactor);
 void aeTblSelSet(UINT8 AETblSel);
 void aeInfoShow(void);
 void aeCalculateLumaModeSet(UINT8);
 UINT8 aeCalculateLumaModeGet(void);
 void aeNormalizationFactorSet(UINT8 winMode);
 void aeNegativeEnableSet(UINT8 NegativeEn);
 UINT8 aeCalibrationCurLumaGet(void);
 void aeIntrCurGainValueGet(UINT8 *val);
 UINT8 aeIntrGIdGet(void);
 void aeConfigForIQTunning(UINT16 idx,UINT8 opt);
 UINT8 aeReadyGet(UINT8 mode);
 void aeProcReset(void);
 void aeParamSet(UINT8 selector,UINT16 val);
 UINT8 aeStatusGet(UINT8 selector, UINT8 *val);
 void aeFdAeModeSet(UINT8 enable);
 void aeGidStepSet(UINT8 luma);
 UINT8 aeFlowGetCurLuma(void);
 UINT16 aeFlowGetCurLumaU16(void);

 void aeSysDbgSet(void);
 void aeCurrentTableCheck(UINT8 tableSelect);
 void aeIntrDebug(void);
 void aePccamSrcSizeSet(UINT8 mode);

 void aeCallbackReg(UINT32 id, void *pfunct);

 void aeDynamicMeterChangeSet(void);
 void aeWindowDimensionGet(UINT8 * h,UINT8 * v);
 void aeWindowArrayGet(UINT8 *pArray);
 void aeFrameIntervalSet(UINT8 en);
 void aeCaptureHeaderAeInformationSet(UINT8 cfg,	void *point);

 typedef enum {
 SP1K_AE_PARA_TARGET_LUMA = SP1K_AE_TARGET_LUMA,							 
 SP1K_AE_PARA_EV_COMP = SP1K_AE_EV_COMP,									 
 SP1K_AE_PARA_FRAME_RATE = SP1K_AE_FRAME_RATE,							 
 SP1K_AE_PARA_METERING = SP1K_AE_METERING,								 
 SP1K_AE_PARA_GID = SP1K_AE_gId,											 
 SP1K_AE_PARA_CAP_ISO_MODE = SP1K_AE_CAPTURE_ISO_MODE,					 
 SP1K_AE_PARA_CAP_ISO_VALUE = SP1K_AE_CAPTURE_ISO_VALUE,					 
 SP1K_AE_PARA_CAP_ANTISHAKE_FPS = SP1K_AE_CAPTURE_ANTISHAKE_FPS,			 
 SP1K_AE_PARA_MAX_COVER_ZONE = SP1K_AE_MAX_COVER_ZONE,					 
 SP1K_AE_PARA_MIN_COVER_ZONE = SP1K_AE_MIN_COVER_ZONE,					 
 SP1K_AE_PARA_SPEED = SP1K_AE_SPEED,
 }SP1K_PARAMETER_SET_MODE;

 typedef enum {
 SP1K_AE_STS_LOW_LIGHT = SP1K_AE_LowLight,								 
 SP1K_AE_STS_IS_READY = SP1K_AE_IsReady,									 
 SP1K_AE_STS_EV = SP1K_AE_EV_GET,										 
 SP1K_AE_STS_COUNT = SP1K_AE_Count,										 
 SP1K_AE_STS_G_GAIN = SP1K_AE_G_GAIN,									 
 SP1K_AE_STS_GID = SP1K_AE_gId,											 
 SP1K_AE_STS_FREQ = SP1K_AE_G_AETblSel,									 
 SP1K_AE_STS_CAP_COMPENSATION_GAIN = SP1K_AE_G_CaptureCompensationGAIN,	 
 SP1K_AE_STS_FRAME_RATE = SP1K_AE_FRAMERATE,								 
 SP1K_AE_STS_CAP_ISO_MODE = SP1K_AE_CAPTURE_ISO_MODE,					 
 SP1K_AE_STS_CURRENT_LUMA = SP1K_AE_CURRENT_LUMA,						 
 SP1K_AE_STS_CAP_ISO_VALUE = SP1K_AE_CAPTURE_ISO_VALUE,					 
 SP1K_AE_STS_CAP_ANTISHAKE_FPS = SP1K_AE_CAPTURE_ANTISHAKE_FPS,			 
 }SP1K_STATUS_GET_MODE;

 typedef enum {
 SP1K_FRAME_RATE_60      = 0,		 
 SP1K_FRAME_RATE_50      = 1,		 
 SP1K_FRAME_RATE_30      = 2,		 
 SP1K_FRAME_RATE_25      = 3,		 
 SP1K_FRAME_RATE_24      = 4,		 
 SP1K_FRAME_RATE_20      = 5,		 
 SP1K_FRAME_RATE_15      = 6,		 
 SP1K_FRAME_RATE_12P5    = 7,		 
 SP1K_FRAME_RATE_SP_60HZ	= 252,		 
 SP1K_FRAME_RATE_SP_50HZ	= 253,		 
 SP1K_FRAME_RATE_TOT,
 }SP1K_FRAME_RATE_MODE;

 typedef enum {
 SP1K_FLICKER_60 = 60,		 
 SP1K_FLICKER_50 = 50,		 
 }SP1K_FLICKER_MODE;

 typedef enum {
 SP1K_AE_METER_MULTI=0,		 
 SP1K_AE_METER_CENTER,		 
 SP1K_AE_METER_SPOT,			 
 SP1K_AE_METER_USER_DEFINE,	 
 }SP1K_AE_METER_MODE;

 typedef enum {
 SP1K_AE_EXP_COMP_M20 = 0,		 
 SP1K_AE_EXP_COMP_M19,			 
 SP1K_AE_EXP_COMP_M18,			 
 SP1K_AE_EXP_COMP_M17,			 
 SP1K_AE_EXP_COMP_M16,			 
 SP1K_AE_EXP_COMP_M15,			 
 SP1K_AE_EXP_COMP_M14,			 
 SP1K_AE_EXP_COMP_M13,			 
 SP1K_AE_EXP_COMP_M12,			 
 SP1K_AE_EXP_COMP_M11,			 
 SP1K_AE_EXP_COMP_M10,			 
 SP1K_AE_EXP_COMP_M09,			 
 SP1K_AE_EXP_COMP_M08,			 
 SP1K_AE_EXP_COMP_M07,			 
 SP1K_AE_EXP_COMP_M06,			 
 SP1K_AE_EXP_COMP_M05,			 
 SP1K_AE_EXP_COMP_M04,			 
 SP1K_AE_EXP_COMP_M03,			 
 SP1K_AE_EXP_COMP_M02,			 
 SP1K_AE_EXP_COMP_M01,			 
 SP1K_AE_EXP_COMP_00,			 
 SP1K_AE_EXP_COMP_P01,			 
 SP1K_AE_EXP_COMP_P02,			 
 SP1K_AE_EXP_COMP_P03,			 
 SP1K_AE_EXP_COMP_P04,			 
 SP1K_AE_EXP_COMP_P05,			 
 SP1K_AE_EXP_COMP_P06,			 
 SP1K_AE_EXP_COMP_P07,			 
 SP1K_AE_EXP_COMP_P08,			 
 SP1K_AE_EXP_COMP_P09,			 
 SP1K_AE_EXP_COMP_P10,			 
 SP1K_AE_EXP_COMP_P11,			 
 SP1K_AE_EXP_COMP_P12,			 
 SP1K_AE_EXP_COMP_P13,			 
 SP1K_AE_EXP_COMP_P14,			 
 SP1K_AE_EXP_COMP_P15,			 
 SP1K_AE_EXP_COMP_P16,			 
 SP1K_AE_EXP_COMP_P17,			 
 SP1K_AE_EXP_COMP_P18,			 
 SP1K_AE_EXP_COMP_P19,			 
 SP1K_AE_EXP_COMP_P20			 
 }SP1K_AE_EXP_COMP_MODE;

 typedef enum {
 SP1K_AE_ISO_AUTO = 0,		 
 SP1K_AE_ISO_100,			 
 SP1K_AE_ISO_200,			 
 SP1K_AE_ISO_400,			 
 SP1K_AE_ISO_800,			 
 SP1K_AE_ISO_MAX,
 }SP1K_AE_ISO_MODE;

 typedef enum {
 SP1K_AE_JUMP_STEP_NORMAL = 0,
 SP1K_AE_JUMP_STEP_DIV2,
 SP1K_AE_JUMP_STEP_DIV3,
 SP1K_AE_JUMP_STEP_DIV4,
 }SP1K_AE_JUMP_STEP_SPEED;

 typedef enum {
 SP1K_AE_CALLBACK_USER_DEFINE_METER = 0,		 

 SP1K_AE_CALLBACK_MAX
 } SP1K_AE_CALLBACK_IDX;

 typedef enum {
 SP1K_HEADER_INFORMATION_AE_CONFIG = HEADER_INFORMATION_AE_CONFIG,                
 SP1K_HEADER_INFORMATION_FLASH_CONFIG = HEADER_INFORMATION_FLASH_CONFIG,          
 SP1K_HEADER_INFORMATION_BSHUTTER_CONFIG = HEADER_INFORMATION_BSHUTTER_CONFIG,    
 SP1K_HEADER_INFORMATION_MSHUTTER_CONFIG = HEADER_INFORMATION_MSHUTTER_CONFIG,    
 SP1K_HEADER_INFORMATION_CAPTURE_CONFIG = HEADER_INFORMATION_CAPTURE_CONFIG,      
 }SP1K_CAPTURE_HEADER_AE_INFORMATION_CONFIG;

 UINT8 HAL_JpegQTableWrite( UINT8 QTableIndex, UINT8 *yQTable, UINT8 *uvQTable, UINT8 zigzagOpt );
 UINT8 HAL_JpegQTableRead( UINT8 *QTableIndex, UINT8 *QTable, UINT8 zigzagOpt );
 UINT8 HAL_JpegMjpgTrig( UINT32 VLCAAddr, UINT32 VLCBAddr, UINT8 Option );
 void HAL_JpegMjpgAbort( void );
 UINT8 HAL_JpegEncode( UINT32 SrcAddr, UINT32 VLCAddr, UINT32 DCTDCAddr, UINT8 Option, UINT8 wait );
 UINT8 HAL_JpegDecode( UINT32 SrcAddr, UINT16 SrcHsize, UINT16 SrcVsize, UINT32 DstAddr, UINT8 PbRescalSize, UINT8 Option, UINT16 MCURstNo );
 UINT8 HAL_JpgZFactorSet( UINT16 hfactor, UINT16 vfactor, UINT16 SrcWidth, UINT16 SrcHeight, UINT16 DstWidth, UINT16 DstHeight, UINT8 option );
 UINT8 HAL_JpgROISet( UINT8 option, UINT16 SrcHsize, UINT16 SrcVsize, UINT16 SrcOffsetX, UINT16 SrcOffsetY, UINT16 DetHsize, UINT16 DetVsize, UINT8 rot );
 void HAL_JpegVlcBoundSet( UINT8 enable,UINT8 intrEn, UINT32 size );
 UINT8 HAL_JpegReadEncodeSize( UINT32 *Compsize );
 void HAL_JpegDecodeWait(void);
 UINT8 HAL_JpgInit(UINT32 jpgsrcaddr,UINT32 vlcaddr); 
 void HAL_JpegDataStampSet(UINT32 addrWord,UINT16 DramHsize,UINT16 xoff,UINT16 yoff,UINT8  xsize,UINT16 ysize,UINT8  thrhold);
 void HAL_JpegDataStampEnable(UINT8 en, UINT8 RevMode);
 void HAL_JpegSetVlcOffset(UINT16 offset);
 void HAL_JpegBypassScaleUp(UINT32 SrcYuvAddrWord, UINT16 SrcWidth, UINT16 SrcHeight, 
 UINT16 RoiWidth, UINT16 RoiHeight, UINT16 hOffset, UINT16 vOffset,
 UINT32 DstYuvAddrWord, UINT16 DstWidth, UINT16 DstHeight, UINT8 option);
 UINT8 HAL_JpgRoiEncSet(UINT16 SrcWidth, UINT16 SrcHeight, 
 UINT16 RoiWidth, UINT16 RoiHeight, UINT16 RoiXOff, UINT16 RoiYOff,
 UINT16 DstWidth, UINT16 DstHeight, UINT8 option); 
 void HAL_JpegSetSourceSize(UINT16 SrcHsize, UINT16 SrcVsize);
 void HAL_JpegSetDramHSize(UINT16 hsize);
 void HAL_JpegSetVlcSize(UINT16 VlcWidth, UINT16 VlcHeight);

 typedef enum {
 HAL_DISP_FIT_METHOD_SRC_STRETCH,
 HAL_DISP_FIT_METHOD_DST_OUTSIDE,
 HAL_DISP_FIT_METHOD_TOT,
 } HAL_DispFitMethod_e;

 typedef enum {
 HAL_DISP_PARA_SW_RST,
 HAL_DISP_PARA_CLK_ENABLE,
 HAL_DISP_PARA_CLK_CFG,
 HAL_DISP_PARA_CLK_PHASE_DLY_CFG,
 HAL_DISP_PARA_CLK_SIGNAL_POLAR_CFG,
 HAL_DISP_PARA_GLOB_ENABLE,
 HAL_DISP_PARA_IMG_LAYER_ENABLE,
 HAL_DISP_PARA_IMG_LAYER_FLIP,
 HAL_DISP_PARA_IMG_LAYER_MONOCHROME_ENABLE,
 HAL_DISP_PARA_OSD_LAYER_ENABLE,
 HAL_DISP_PARA_BG_COLOR,
 HAL_DISP_PARA_REQ_LENTH_SET,
 HAL_DISP_PARA_DEVICE_TYPE,
 HAL_DISP_PARA_DEVICE_TOTAL_REGION,
 HAL_DISP_PARA_DEVICE_ACTIVE_REGION,
 HAL_DISP_PARA_DEVICE_IMG_REGION_CROP,
 HAL_DISP_PARA_DEVICE_IMG_REGION_RENDER,
 HAL_DISP_PARA_DEVICE_OSD_REGION_CROP,
 HAL_DISP_PARA_DEVICE_OSD_REGION_RENDER,
 HAL_DISP_PARA_OSD_BLEND_FACTOR,
 HAL_DISP_PARA_INTERLACE_CFG,
 HAL_DISP_PARA_LCD_DITHER_ENABLE,
 HAL_DISP_PARA_UPS051_PIXEL_SHIFT_CFG,
 HAL_DISP_PARA_UPS051_LIGHTVIEW_ENABLE,
 HAL_DISP_PARA_UPS051_RGB888_ENABLE,
 HAL_DISP_PARA_UPS052_RGB_ONLY_ENABLE,
 HAL_DISP_PARA_UPS_HSD_DELAY_CFG,
 HAL_DISP_PARA_UPS_CLK_PHASE,
 HAL_DISP_PARA_UPS_LAYOUT_MODE,
 HAL_DISP_PARA_UPS_COLOR_FILTER_CFG,
 HAL_DISP_PARA_CCIR_OUTPUT_SEQUENCE_CFG,
 HAL_DISP_PARA_EDGE_TYPE_SET,
 HAL_DISP_PARA_EDGE_GAIN_SET,
 HAL_DISP_PARA_GAMMA_ENABLE,
 HAL_DISP_PARA_RB_SWITCH_SET,
 HAL_DISP_PARA_COLOR_MATRIX_TBL_CFG,
 HAL_DISP_PARA_COLOR_MATRIX_ENABLE,
 HAL_DISP_PARA_COLOR_MATRIX_FMT_CFG,
 HAL_DISP_PARA_TV_DAC_POWER_ENABLE,
 HAL_DISP_PARA_TV_SATURATION,
 HAL_DISP_PARA_TV_HUE,
 HAL_DISP_PARA_LCM_CMD_BUS_TYPE,
 HAL_DISP_PARA_LCM_CMD_BIT_WIDTH,
 HAL_DISP_PARA_LCM_CMD_PULSE_WIDTH,
 HAL_DISP_PARA_LCM_CMD_ACTIVE_LEVEL,
 HAL_DISP_PARA_LCM_DATA_FORMAT,
 HAL_DISP_PARA_LCM_RGB666_PACK_ENABLE,
 HAL_DISP_PARA_LCM_RGB666_FORMAT_10_ENABLE,
 HAL_DISP_PARA_LCM_PIXEL_ALIGN,
 HAL_DISP_PARA_LCM_COLOR_PACKAGE,
 HAL_DISP_PARA_LCM_SWITCH_G_ENABLE,
 HAL_DISP_PARA_LCM_CHANGE_RB_ENABLE,
 HAL_DISP_PARA_LCM_ONE_HALF_PIXEL_ENABLE,
 HAL_DISP_PARA_LCM_DITHER_ENABLE,
 HAL_DISP_PARA_MAX_ID,
 } HAL_DispParaId_e;

 typedef enum {
 HAL_DISP_DEV_RST_LOCAL = (0x01 << 0),
 HAL_DISP_DEV_RST_PNL   = (0x01 << 1),
 HAL_DISP_DEV_RST_TV    = (0x01 << 2),
 HAL_DISP_DEV_RST_ALL   = (HAL_DISP_DEV_RST_LOCAL | HAL_DISP_DEV_RST_PNL | HAL_DISP_DEV_RST_TV),
 } HAL_DispRstDev_e; 

 typedef enum {
 HAL_DISP_CLK_SRC_SYSPLL522M = 0,
 HAL_DISP_CLK_SRC_XTAL27M    = 1,
 } HAL_DispClkSrc_e;

 typedef enum {
 HAL_DISP_CLK_PHASE_DLY_0T = 0,
 HAL_DISP_CLK_PHASE_DLY_1T = 1,
 HAL_DISP_CLK_PHASE_DLY_2T = 2,
 HAL_DISP_CLK_PHASE_DLY_3T = 3,
 HAL_DISP_CLK_PHASE_DLY_4T = 4,
 HAL_DISP_CLK_PHASE_DLY_5T = 5,
 HAL_DISP_CLK_PHASE_DLY_6T = 6,
 HAL_DISP_CLK_PHASE_DLY_7T = 7,
 } HAL_DispClkPhaseDly_e;

 typedef enum {
 HAL_DISP_SIGNAL_CLK_NORMAL     = (0 << 3),
 HAL_DISP_SIGNAL_CLK_INVERSE    = (1 << 3),

 HAL_DISP_SIGNAL_VSYNC_ACT_LOW  = (0 << 4),
 HAL_DISP_SIGNAL_VSYNC_ACT_HIGH = (1 << 4),

 HAL_DISP_SIGNAL_HSYNC_ACT_LOW  = (0 << 5),
 HAL_DISP_SIGNAL_HSYNC_ACT_HIGH = (1 << 5),
 } HAL_DispSignalPolar_e;

 typedef enum {
 HAL_DISP_GLOB_BLEND_SRC_IMAGE = (0 << 4),
 HAL_DISP_GLOB_BLEND_SRC_BLACK = (1 << 4),
 } HAL_DispGlobBlendSrc_e;

 typedef enum {
 HAL_DISP_IMG_REQ_LENTH_16 = (0 << 4),
 HAL_DISP_IMG_REQ_LENTH_32 = (1 << 4),
 HAL_DISP_OSD_REQ_LENTH_16 = (0 << 5),
 HAL_DISP_OSD_REQ_LENTH_32 = (1 << 5),
 } HAL_DispReqLenth_e;

 typedef enum {
 HAL_DISP_DEVICE_TYPE_COMPOSITE_NTSC  = 0x00,
 HAL_DISP_DEVICE_TYPE_COMPOSITE_PAL   = 0x01,
 HAL_DEVICE_TYPE_CCIR656_NTSC        = 0x02,
 HAL_DEVICE_TYPE_CCIR656_PAL         = 0x03,
 HAL_DISP_DEVICE_TYPE_CCIR601_NTSC_8  = 0x04,
 HAL_DEVICE_TYPE_CCIR601_PAL_8       = 0x05,

 HAL_DEVICE_TYPE_UPS051              = 0x08,
 HAL_DEVICE_TYPE_UPS052              = 0x09,
 HAL_DEVICE_TYPE_LCM                 = 0x0A,

 HAL_DEVICE_TYPE_CCIR656_INTERLACE   = 0x10,
 HAL_DEVICE_TYPE_CCIR601_INTERLACE   = 0x13,
 HAL_DEVICE_TYPE_CCIR656_PROGRESSIVE = 0x13,
 HAL_DEVICE_TYPE_CCIR601_PROGRESSIVE = 0x13,
 } HAL_DispDeviceType_e;

 typedef enum {
 HAL_DISP_PROGRESSIVE = 0x00,
 HAL_DISP_INTERLACE   = 0x05,
 } HAL_DispInterlaceCfg_e;

 typedef enum {
 HAL_DISP_PIXEL_SHIFT_DISABLE = 0x00,
 HAL_DISP_PIXEL_SHIFT_ODD     = 0x10,
 HAL_DISP_PIXEL_SHIFT_EVEN    = 0x30,
 } HAL_DispUps051PixelShiftCfg_e;

 typedef enum {
 HAL_DISP_UPS052_HSD_DELAY_3T = (0 << 0),
 HAL_DISP_UPS052_HSD_NO_DELAY = (1 << 0),
 HAL_DISP_UPS052_HSD_DELAY_1T = (1 << 1),
 HAL_DISP_UPS052_HSD_DELAY_2T = (1 << 2),
 } HAL_DispUps052HsdDelayCfg_e;

 typedef enum {
 HAL_DISP_UPS051_HSD_RGB = (0 << 0),
 HAL_DISP_UPS051_HSD_BRG = (1 << 0),
 } HAL_DispUps051HsdDelayCfg_e;

 typedef enum {
 HAL_DISP_UPS_LAYOUT_MODE_DISABLE       = (0 << 4),
 HAL_DISP_UPS_LAYOUT_MODE_ODD_DELAY_1T  = (1 << 4),
 HAL_DISP_UPS_LAYOUT_MODE_EVEN_DELAY_1T = (3 << 4),
 } HAL_DispUpsLayoutMode_e;

 typedef enum {
 HAL_DISP_UPS051_ODD_RGB = (0 << 4),
 HAL_DISP_UPS051_ODD_GBR = (1 << 4),
 HAL_DISP_UPS051_ODD_BRG = (2 << 4),
 HAL_DISP_UPS051_ODD_BGR = (3 << 4),
 HAL_DISP_UPS051_ODD_RBG = (4 << 4),
 HAL_DISP_UPS051_ODD_GRB = (5 << 4),

 HAL_DISP_UPS052_ODD_RGB = (0 << 4),
 HAL_DISP_UPS052_ODD_GBR = (1 << 4),
 HAL_DISP_UPS052_ODD_BRG = (2 << 4),
 HAL_DISP_UPS052_ODD_RBG = (3 << 4),
 HAL_DISP_UPS052_ODD_BGR = (4 << 4),
 HAL_DISP_UPS052_ODD_GRB = (5 << 4),
 } HAL_DispUpsOddColorFilter_e;

 typedef enum {
 HAL_DISP_UPS051_EVEN_BRG = (0 << 0),
 HAL_DISP_UPS051_EVEN_RGB = (1 << 0),
 HAL_DISP_UPS051_EVEN_GBR = (2 << 0),
 HAL_DISP_UPS051_EVEN_GRB = (3 << 0),
 HAL_DISP_UPS051_EVEN_BGR = (4 << 0),
 HAL_DISP_UPS051_EVEN_RBG = (5 << 0),

 HAL_DISP_UPS052_EVEN_RGB = (0 << 0),
 HAL_DISP_UPS052_EVEN_GBR = (1 << 0),
 HAL_DISP_UPS052_EVEN_BRG = (2 << 0),
 HAL_DISP_UPS052_EVEN_RBG = (3 << 0),
 HAL_DISP_UPS052_EVEN_BGR = (4 << 0),
 HAL_DISP_UPS052_EVEN_GRB = (5 << 0),
 } HAL_DispUpsEvenColorFilter_e;

 typedef enum {
 HAL_DISP_COLOR_MATRIX_FMT_RGB   = (0 << 4),
 HAL_DISP_COLOR_MATRIX_FMT_YCBCR = (1 << 4),
 HAL_DISP_COLOR_MATRIX_FMT_YUV   = (2 << 4),
 } HAL_DispColorMtxCvsFmt_e;

 typedef enum {
 HAL_DISP_CCIR_YCRYCB = (0 << 0),
 HAL_DISP_CCIR_CRYCBY = (1 << 0),
 HAL_DISP_CCIR_YCBYCR = (2 << 0),
 HAL_DISP_CCIR_CBYCRY = (3 << 0),
 } HAL_DispCcirOutputSeqCfg_e;

 typedef enum {
 HAL_DISP_EDGE_TYPE_A = (0 << 0),  
 HAL_DISP_EDGE_TYPE_B = (1 << 0),  
 HAL_DISP_EDGE_TYPE_C = (2 << 0),  
 } HAL_DispEdgeType_e;

 typedef enum {
 HAL_DISP_LCM_CMD_BUS_I80 = (0 << 0),
 HAL_DISP_LCM_CMD_BUS_M68 = (1 << 0),
 } HAL_DispLcmCommandBusType_e;

 typedef enum {
 HAL_DISP_LCM_CMD_8_BIT   = (0 << 4),
 HAL_DISP_LCM_CMD_9_BIT   = (1 << 4),
 HAL_DISP_LCM_CMD_16_BIT  = (2 << 4),
 HAL_DISP_LCM_CMD_18_BIT  = (3 << 4),
 HAL_DISP_LCM_CMD_24_BIT  = (4 << 4),
 } HAL_DispLcmCommandBitWidth_e;

 typedef enum {
 HAL_DISP_LCM_DATA_TYPE_RGB565 = (0 << 4),
 HAL_DISP_LCM_DATA_TYPE_RGB666 = (1 << 4),
 HAL_DISP_LCM_DATA_TYPE_RGB444 = (2 << 4),
 HAL_DISP_LCM_DATA_TYPE_RGB888 = (3 << 4),
 } HAL_DispLcmDataType_e;

 typedef enum {

 HAL_DISP_LCM_DATA_FMT_B8_T444_0,  
 HAL_DISP_LCM_DATA_FMT_B8_T444_1,  

 HAL_DISP_LCM_DATA_FMT_B8_T565_0,  
 HAL_DISP_LCM_DATA_FMT_B8_T565_1,  
 HAL_DISP_LCM_DATA_FMT_B8_T565_2,  
 HAL_DISP_LCM_DATA_FMT_B8_T565_3,  
 HAL_DISP_LCM_DATA_FMT_B8_T565_4,  

 HAL_DISP_LCM_DATA_FMT_B8_T666_0,  
 HAL_DISP_LCM_DATA_FMT_B8_T666_1,  

 HAL_DISP_LCM_DATA_FMT_B9_T666_0,  
 HAL_DISP_LCM_DATA_FMT_B9_T666_1,  
 HAL_DISP_LCM_DATA_FMT_B9_T666_2,  

 HAL_DISP_LCM_DATA_FMT_B16_T444_0,  
 HAL_DISP_LCM_DATA_FMT_B16_T444_1,  
 HAL_DISP_LCM_DATA_FMT_B16_T444_2,  
 HAL_DISP_LCM_DATA_FMT_B16_T444_3,  

 HAL_DISP_LCM_DATA_FMT_B16_T565_0,  
 HAL_DISP_LCM_DATA_FMT_B16_T565_1,  

 HAL_DISP_LCM_DATA_FMT_B16_T666_0,  
 HAL_DISP_LCM_DATA_FMT_B16_T666_1,  
 HAL_DISP_LCM_DATA_FMT_B16_T666_2,  
 HAL_DISP_LCM_DATA_FMT_B16_T666_3,  
 HAL_DISP_LCM_DATA_FMT_B16_T666_4,  
 HAL_DISP_LCM_DATA_FMT_B16_T666_5,  
 HAL_DISP_LCM_DATA_FMT_B16_T666_6,  
 HAL_DISP_LCM_DATA_FMT_B16_T666_7,  
 HAL_DISP_LCM_DATA_FMT_B16_T666_8,  
 HAL_DISP_LCM_DATA_FMT_B16_T666_9,  
 HAL_DISP_LCM_DATA_FMT_B16_T666_10,  
 HAL_DISP_LCM_DATA_FMT_B16_T666_11,  

 HAL_DISP_LCM_DATA_FMT_B18_T666_0,  
 HAL_DISP_LCM_DATA_FMT_B18_T666_1,  

 HAL_DISP_LCM_DATA_FMT_B24_T888_0,  
 HAL_DISP_LCM_DATA_FMT_B24_T888_1,  
 } HAL_DispLcmDataFormat_e;

 typedef enum {
 HAL_DISP_LCM_PIXEL_ALIGN_NONE = (0 << 0),
 HAL_DISP_LCM_PIXEL_ALIGN_BIG  = (1 << 0),
 HAL_DISP_LCM_PIXEL_ALIGN_MID  = (2 << 0),
 HAL_DISP_LCM_PIXEL_ALIGN_BOTH = (3 << 0),
 } HAL_DispLcmPixelAlign_e;

 typedef enum {
 HAL_DISP_LCM_COLOR_PACKAGE_RGB  = (1 << 0),
 HAL_DISP_LCM_COLOR_PACKAGE_BGR  = (0 << 0),
 } HAL_DispLcmColorPackage_e;

 typedef enum {
 HAL_DISP_WAIT_VSYNC   = (1 << 0),
 HAL_DISP_WAIT_VVALID  = (1 << 1),
 HAL_DISP_WAIT_HSYNC   = (1 << 2),
 HAL_DISP_WAIT_HVALID  = (1 << 3),
 } HAL_DispWaitSource_e;

 typedef enum {
 HAL_DISP_WAIT_FALLING = 0,
 HAL_DISP_WAIT_RISING  = 1,
 HAL_DISP_WAIT_HIGH    = 2,
 HAL_DISP_WAIT_LOW     = 3,
 } HAL_DispWaitMode_e;

 typedef struct {
 UINT8 busWidth;
 UINT8 dataType;
 UINT8 dataCyc;
 UINT8 pnlRBChg;
 UINT8 pnlGChg;
 UINT8 pktMode;
 UINT8 bigEnd;
 UINT8 midEnd;
 } lcmSetItem_t; 

 void HAL_DispFitMethodSet(UINT8 method);
 UINT8 HAL_DispScaleSet(UINT16 hsize, UINT16 vsize, UINT16 zoomfac, 	UINT8 update);
 UINT8 HAL_DispImgPanSet(SINT16 xoff, SINT16 yoff);
 void HAL_Display_enable(void);
 UINT8 HAL_DispScaleUpdate(void);

 void HAL_DispRegUpdate(void);

 void HAL_DispPbFrameBufferSet(UINT32 addr, UINT16 w, UINT16 h);
 void HAL_DispPbFrameBufferGet(UINT32 *paddr, UINT16 *pw, UINT16 *ph);

 void HAL_DispOsdBuffAddrSet(UINT32 addrWord);
 void HAL_DispOsdBuffSizeSet(UINT16 buffW, UINT16 buffH);
 void HAL_DispOsdBuffAddrGet(UINT32 *pAddrWord);
 void HAL_DispOsdBuffSizeGet(UINT16* pBuffW,UINT16* pBuffH);

 UINT8 HAL_DispOsdScaleUpdate(void);

 UINT8 HAL_DispParaSet(UINT8 id, SINT16 para1, SINT16 para2, SINT16 para3, SINT16 para4, UINT8 upd);
 UINT8 HAL_DispOsdPaletteSet(UINT16 *paltTbl, UINT16 begIdx, UINT16 len);
 UINT8 HAL_DispGammaTableSet(UINT8 *gammaTbl);
 UINT8 HAL_DispOsdPaletteInit(UINT32 addr,UINT8 dataMode);

 UINT8 HAL_DispSignalWait(UINT8 src, UINT8 mode);

 UINT8 HAL_DispLcmCmdTableSend(UINT32 *cmdTbl);

 void HAL_DispImgXyGet(UINT32 *x0, UINT32 *y0, UINT32 *x1, UINT32 *y1);
 void HAL_DispImgXySet(UINT32 x0, UINT32 y0, UINT32 x1, UINT32 y1);
 void HAL_DispImgHvSet(UINT32 h0,UINT32 v0,UINT32 h1,UINT32 v1);
 void HAL_DispImgHvGet(UINT32 *h0,UINT32 *v0,UINT32 *h1,UINT32 *v1);
 void HAL_DispOsdHvSet(UINT32 h0,UINT32 v0,UINT32 h1,UINT32 v1);
 void HAL_DispOsdHvGet(UINT32 *h0,UINT32 *v0,UINT32 *h1,UINT32 *v1);
 void HAL_DispPnlInitHvGet(UINT16 *h0,UINT16 *h1,UINT16 *v0,UINT16 *v1);
 void HAL_DispPnlInitHvSet(UINT16 h0,UINT16 h1,UINT16 v0,UINT16 v1);
 void HAL_DispImgXyUpdate(UINT16 newImgW, UINT16 newImgH);
 void HAL_DispImgSizeGet(UINT16 *width,UINT16 *heigth);

 UINT8 HAL_UsbBulkInFromDRAM(UINT32 DRAMAddrWord, UINT16 nByte);
 UINT8 HAL_UsbBulkOutToDRAM(UINT32 DRAMAddrWord, UINT16 nByte);
 UINT8 HAL_UsbBulkInFromSRAM(UINT32 SRAMAddrByte, UINT16 nByte);
 UINT8 HAL_UsbBulkOutToSRAM(UINT32 SRAMAddrByte, UINT16 nByte);
 UINT8 HAL_UsbBulkIn(UINT32 *bulksize);
 UINT8 HAL_UsbBulkOut(UINT32 *bulksize);
 void HAL_UsbSwPlugOut(void);
 void HAL_UsbSwPlugOut_Intr(void);
 void HAL_UsbSwPlugIn(void);
 void HAL_UsbPccamInit(UINT8 FormatIndex);
 UINT8 HAL_UsbAutoCompress(UINT32 VLCAAddrWord, UINT32 VLCBAddrWord, UINT8 Option);
 UINT8 HAL_UsbBulkEnable(UINT8 BulkDir);
 void HAL_UsbForceFS(UINT8 usbconfigflag);
 UINT8 HAL_UsbParaSet(UINT8 selector, UINT8 value);
 UINT8 HAL_UsbBulkFlashAddrSet(UINT32 flashtype, UINT32 flashaddr);
 UINT32 HAL_UsbBulkFlashAddrGet(UINT32 flashtype);
 void HAL_UsbBulkAutoModeSet(UINT8 mode);
 void HAL_UsbEP7IntrDataTx(UINT8 *pdataAddr,UINT16 dataSize);

 typedef enum {
 HAL_AUD_SAMPLE_8BIT = 0,
 HAL_AUD_SAMPLE_16BIT = 1,
 } HAL_AudSampleBitMode_e;

 void HAL_AudStart(UINT32 mode, UINT32 smpRate, UINT32 smpBit, UINT32 chn);
 void HAL_AudSoundPlayCfg(UINT32 smpRate, UINT32 smpBit, UINT32 chn);
 void HAL_AudSoundPlayTrg(UINT32 index);
 void HAL_AudStop(UINT32 mode);
 void HAL_AudPause(void);
 void HAL_AudResume(void);
 void HAL_AudThdSet(UINT32 thd);
 void HAL_AudRingBufferSet(UINT32 sdramAddrByte, UINT32 sizeByte);
 void HAL_AudInIndexSet(UINT32 index);
 void HAL_AudOutIndexSet(UINT32 index);
 UINT8 HAL_AudIntStsGet(UINT16 mode);
 void HAL_AudVolumeSet(UINT8 volnum_integer,	UINT8 voldenum_decimal);
 void HAL_AudInIndexGet(UINT32 *index);
 void HAL_AudOutIndexGet(UINT32 *index);
 UINT8 HAL_AudRingBufCntRead(UINT32 *pAudBufCnt);
 void HAL_AudAdpcmSet(UINT8 mode, UINT8 on, UINT32 blockSize);
 void HAL_AudAgcWindowSet(UINT16 window);
 void HAL_AudAgcStateGet(UINT32 *eng, UINT32 *window, UINT8 *cnt);
 void HAL_AudPGAGainGet(UINT8 *gain,	UINT8 *boost);
 void HAL_AudPGAGainSet(UINT8 gain, UINT8 boost);
 UINT8 HAL_AudALCModeSet(UINT8 mode);
 UINT8 HAL_AudUsbModeSet(UINT8 mode);
 void HAL_AudAgcCfg(UINT32 dst1, UINT16 windowsize, UINT32 cmdaddr, UINT8 bitmode);
 void HAL_AudAgcValGet(UINT32 src1, UINT32 cmdaddr, UINT8 bitmode, UINT16 *agcval);
 void HAL_AudSpeakerPwrSet(UINT8 on);
 void HAL_AudPwrSave(UINT8 on);
 void HAL_AudSwRst(UINT8 rstval);
 void HAL_AudI2SSet(UINT8 en);

 typedef struct	
 {
 UINT8 swrst:1;
 UINT8 null0:3;
 UINT8 disclkgate:1;
 UINT8 null1:3;
 }moe_reg_module_control_t;

 typedef struct 
 {
 UINT8 cmdQueueIntr:1;
 UINT8 cmdIntr:1;
 UINT8 div0Intr:1;
 UINT8 shiftOverflow:1;
 UINT8 null0:4;

 }moe_reg_intr_t;

 typedef struct 
 {
 UINT8 queueBusy:1; 
 UINT8 cmdBusy:1; 
 UINT8 null0:6;

 }moe_reg_status_t;

 typedef struct 
 {	
 UINT8 func:1; 
 UINT8 exchange:1; 
 UINT8 sum:1; 
 UINT8 null0:1;

 UINT8 equation:3;

 UINT8 null1:1;
 }moe_reg_func_t;

 typedef struct
 {
 UINT8 null0:4;
 UINT8 src1ClipEn:1;
 UINT8 src2ClipEn:1;
 UINT8 dstClipEn:1;
 UINT8 null1:1;
 }moe_reg_opt_t;

 typedef struct
 {	
 UINT8 addr3:1; 
 UINT8 sign:1; 
 UINT8 format:1; 
 UINT8 shift:5; 
 }moe_reg_addr_t; 

 typedef enum dramZFUpdate_e {
 HAL_DRAM_ZUPDATE_CDSP,
 HAL_DRAM_ZUPDATE_PV,
 HAL_DRAM_ZUPDATE_PB,
 HAL_DRAM_ZUPDATE_OSD,
 HAL_DRAM_ZUPDATE_MPEG,
 HAL_DRAM_ZUPDATE_VIDEO,
 HAL_DRAM_ZUPDATE_ALL,
 }dramZFUpdate_t; 

 typedef enum
 {
 DRAM_RWFormat_8=0,	 
 DRAM_RWFormat_16,	 
 DRAM_RWFormat_32,	 
 }dram_rw_format_t; 

 typedef enum {
 HAL_DRAM_ZF_UPD_LAYER_PV  = (1 << 0),
 HAL_DRAM_ZF_UPD_LAYER_PB  = (1 << 1),
 HAL_DRAM_ZF_UPD_LAYER_OSD = (1 << 2),
 HAL_DRAM_ZF_UPD_CLEAR     = 0x00,
 } HAL_DramZfUpdSrc_e;

 UINT8 HAL_Dram1stPathFrmBufAddrGet(UINT8 idx, UINT32 *addrW);

 UINT8 HAL_Dram1stPathFrmBufAAddrSet(UINT32 AFBAddr);
 UINT8 HAL_Dram1stPathFrmBufBAddrSet(UINT32 BFBAddr);
 UINT8 HAL_Dram1stPathFrmBufCAddrSet(UINT32 CFBAddr);
 UINT8 HAL_Dram1stPathFrmBufDAddrSet(UINT32 DFBAddr);
 UINT8 HAL_Dram1stPathFrmBufAAddrSet_Intr(UINT32 AFBAddr);
 UINT8 HAL_Dram1stPathFrmBufBAddrSet_Intr(UINT32 BFBAddr);
 UINT8 HAL_Dram1stPathFrmBufCAddrSet_Intr(UINT32 CFBAddr);
 UINT8 HAL_Dram1stPathFrmBufDAddrSet_Intr(UINT32 DFBAddr);

 UINT8 HAL_Dram1stPathFrmBufSizeSet(UINT16, UINT16);
 UINT8 HAL_Dram1stPathFrmBufSizeGet(UINT16 *hSize, UINT16 *vSize); 

 UINT8 HAL_Dram2ndPathFrmBufAAddrSet(UINT32 AFBAddr);	    
 UINT8 HAL_Dram2ndPathFrmBufBAddrSet(UINT32 BFBAddr);
 UINT8 HAL_Dram2ndPathFrmBufCAddrSet(UINT32 CFBAddr);
 UINT8 HAL_Dram2ndPathFrmBufDAddrSet(UINT32 DFBAddr);
 UINT8 HAL_Dram2ndPathFrmBufSizeSet(UINT16);

 void  HAL_DramPreviewStart(UINT8 pbFreeze);
 UINT8 HAL_DramRawFrmBufSizeSet(UINT16, UINT16);
 UINT8 HAL_DramAVLCAddrSet(UINT32);
 UINT8 HAL_DramBVLCAddrSet(UINT32);
 UINT8 HAL_DramDoYuvSrcHSizeSet( UINT16 size );

 UINT8 HAL_DramRawFrmBufAddrSet(UINT32);

 UINT8 HAL_GprmDramDma(UINT32 srcAddrByte, UINT32 dstAddrByte, UINT32 sizeByte, UINT8 wait);
 UINT8 HAL_GprmDramDmaAdv(UINT32 srcAddrByte, UINT32 dstAddrByte, UINT32 sizeByte, UINT8 wait);

 UINT8 HAL_DramStartAddrSet(UINT32, UINT8);
 UINT8 HAL_DramWordDataRead(UINT8 *, UINT8 *);
 UINT8 HAL_DramWordDataWrite(UINT8, UINT8);
 void HAL_DramDispSrcSel(UINT8 srcSel);
 UINT8 HAL_DramDispSrcGet(void);

 UINT8 HAL_SetJPEGVscale(UINT16, UINT16);

 void HAL_DramPattSearch(UINT8 mode, UINT32 wordaddr, UINT32 length, UINT32 patt,
 UINT32 *first, UINT32 *total, UINT8 *hit);

 UINT8 HAL_DramVLCRdyClr(UINT8);

 UINT8 HAL_DramZUpdateWait(UINT8, UINT16 timeout);
 UINT8 HAL_IntrDramZUpdateWait(UINT8);

 void HAL_DramZUpdateClear(UINT8 val);
 void HAL_IntrDramZUpdateClear(UINT8 val);

 void HAL_DramZfUpdTrig(UINT8 layer);
 void HAL_IntrDramZfUpdTrig(UINT8 layer);

 UINT8 HAL_DramDRAMDarkAddrSet(UINT32 Addr);
 UINT8 HAL_DramDRAMDarkSizeSet(UINT16 hsize);

 void HAL_DramSnapRip(UINT8 type);

 void HAL_DramDispBufSel(UINT8 sel);
 void HAL_DramCodeSentry(UINT32 LowBoundaryAddrW,UINT32 HighBoundaryAddrW,UINT32 RedirectAddrW);

 void HAL_DramRWBytes(UINT32 addrW,UINT8* pt,UINT32 cnt,UINT8 isRead,UINT8 fmt);

 void HAL_DramFrmBufNumSet(UINT8 sel);

 UINT8 HAL_DramDmaWrite(void xdata* pSrc, UINT32 dramAddrWord, UINT16 sizeByte, UINT8 wait);
 UINT8 HAL_DramDmaRead(void xdata* pDst, UINT32 dramAddrWord, UINT16 sizeByte, UINT8 wait);

 void HAL_DramABBufAutoMap(UINT8 en);
 void HAL_DramWrYuvVDram1(UINT8 en);

 enum {
 USB_PARA_SINGLE_LUN_SHOW_NAND = 0,
 USB_PARA_MAX_ID,
 };

 void sp1kHalCamModeSet(sp1kCamMode_t CamMode);

 void sp1kHalJpegDecode( UINT32 SrcAddr, UINT16 SrcHsize, UINT16 SrcVsize, UINT32 DstAddr, UINT8 PbRescalSize,  UINT8 Option, UINT16 MCURstNo );

 void sp1kHalMjpgClipAbort( void );

 void sp1kHalJpegDecodeWait( void );

 UINT8 sp1kHalCtrlScreen(UINT8 attrib);

 typedef enum cdspsnaptype_s{
 CDSP_SNAP_8RAW,
 CDSP_SNAP_YUV,
 CDSP_SNAP_10RAW,	  
 } cdspsnaptype_t;

 typedef enum cdspsnapflow_s{
 CDSP_DO_RAW,
 CDSP_DO_FRONT,
 CDSP_DO_QV,
 CDSP_DO_YUV,
 CDSP_DO_YUV_QV,
 CDSP_DO_YUV2YUV,
 CDSP_DO_BLINK,
 } cdspsnapflow_t;

 typedef struct cdspjobtable_s {   
 UINT16 rhoffset 	   ;
 UINT16 rvoffset 	   ;
 UINT16 rhsize		   ;
 UINT16 rvsize		   ;
 UINT16 whoffset 	   ;
 UINT16 wvoffset 	   ;
 UINT16 hscaleaccini    ;
 UINT16 vscaleaccini    ;
 UINT16 clamphsize	   ;
 UINT8  skippixel	   ;
 UINT8  pixsw		   ;
 UINT8  dummyline	   ;
 UINT8  hmirren		   ;
 UINT8  vmirren		   ;
 } cdspjobtable_t;

 UINT32 snapCdspDoImg( UINT8 method );
 UINT32 cdspDoRaw(void);
 UINT32 cdspDoFront(void);	   
 UINT32 cdspDoQV(UINT8 rawType,UINT32 dstAddr,UINT16 dstHSize,UINT16 dstVSize);
 UINT32 cdspDoYuv(void);
 UINT8 cdspYuv2Yuv(UINT32 srcAddr,UINT32 dstAddr,UINT16 srcHSize,UINT16 srcVSize,UINT16 dstHSize, UINT16 dstVSize,UINT16 doVSize);
 UINT32 snapDoJpeg(UINT16 hfactor,UINT16 vfactor,UINT32 srcYuvAddr,UINT16 srcYuvWidth,UINT16 srcYuvHeight,UINT32 dstJpegVlcAddr,UINT16 dstWidth,UINT16 dstHeight);

 UINT16 cdspjobget(UINT16 srcHSize, UINT16 srcVSize, UINT16 cropHSize, UINT16 cropVSize, UINT16 dstHSize, UINT16 dstVSize, UINT16 jobHSize, UINT16 jobOutVSize, UINT8  pixelSw, 
 UINT8 yuvDoCdsp, UINT8 yuvScale, UINT8 get, cdspjobtable_t xdata* pJobTbl); 

 UINT32 snapVlcSizeGet(void);

 void sp1kVdAvgTimeSvcProc (void);

 void vdSvcProc(void* ptr	);

 UINT8 sp1kVdProcEnable(UINT8 flag, UINT8 en);

 void sp1kVdInit(void);

 UINT8 sp1kVdCountGet(void);

 UINT8 sp1kFrontDevActSet(UINT8 id);

 UINT8 sp1kFrontDevActGet(void);

 UINT8 sp1kFrontPowerOn(UINT8 id);

 UINT8 sp1kFrontPowerOff(UINT8 id);

 UINT8 sp1kFrontOpticalBlackStatusGet(UINT8 id);

 UINT8 sp1kFrontOpticalBlackCalibrate(UINT8 id);

 UINT8 sp1kFrontPreviewSizeGet(UINT8 id, UINT16 *pHSize, UINT16 *pVSize);

 UINT8 sp1kFrontSnapSizeGet(UINT8 id, UINT16 *pHSize, UINT16 *pVSize);

 UINT8 sp1kFrontHighQualityDigitalZoomCapabilityGet(UINT8 id);

 UINT8 sp1kFrontHighQualityDigitalZoomModeSet(UINT8 id, UINT8 en);

 UINT8 sp1kDiskTypeGet(void);
 UINT8 sp1kDiskRsvRead(UINT32 idx, UINT32 addr);
 UINT8 sp1kDiskRsvWrite(UINT32 idx, UINT32 addr);
 UINT8 sp1kDiskRsvSizeGet(UINT32 idx, UINT32 *offset, UINT32 *size);

 void sp1kFlashCacheCtrl(UINT8 on); 
 UINT8 flashCacheCtrlSts(void); 
 void flashLoadDramPara(UINT32 ramAddr);

 UINT8 SMC_RedunSram2Card(UINT32 startAddress, UINT16 blockAddress);
 UINT8 SMC_GetID(void);
 UINT8 SMC_Card2Sram(UINT32 phyAddr);
 UINT8 SMC_Sram2Card(UINT32 phyAddr, UINT16 blockNum);
 UINT8 SMC_Card2Dram(UINT32 phyAddr, UINT32 ramAddr);
 UINT8 SMC_Card2DramFast(UINT32 phyAddr, UINT32 ramAddr);

 UINT8 SMC_Dram2Card(UINT32 phyAddr, UINT16 blockNum, UINT32 ramAddr);
 UINT8 SMC_Card2Usb(UINT32 phyAddr);
 UINT8 SMC_Card2UsbFast(UINT32 phyAddr);
 UINT8 SMC_EraseOneBlock(UINT32 phyAddr);

 UINT8 SMC_Card2SramForExt(UINT32 phyAddr);
 UINT8 SMC_RedunCard2Sram(UINT32 phyAddr);

 UINT8 SMC_CheckEcc(void);
 void SMC_FillRedunArea(UINT16 blockAddress, UINT8 * badBlk);

 UINT8 SMC_Usb2Card(UINT32 startAddress, UINT16 blockAddress);
 UINT8 SMC_AutoCmdSet(UINT8 const xdata* pCmd, UINT8 CmdCnt, UINT32 Address);

 UINT8 NANDF_LWriteStart(UINT8 srcPath, UINT32 logAddr, UINT32 numSector);
 UINT8 NANDF_LWriteContinue(UINT8 srcPath, UINT32 logAddr, UINT32 ramAddr, UINT32 *pNumSector);
 UINT8 NANDF_LWriteEnd(UINT8 srcPath, UINT32 copySize, UINT8 *pEndContinuous);

 UINT8 SMC_Card2Sram(UINT32 startAddress);
 UINT8 SMC_Sram2Card(UINT32 startAddress, UINT16 blockAddress);

 UINT8 SMC_BadBlockMark(UINT16 blockAddress);

 UINT8 SMC_CardDetect(void);
 UINT8 SMC_Initialize(UINT8 doDOSInitial);
 UINT8 SMC_LCard2Sram(UINT32 logAddr);
 UINT8 SMC_LCard2Dram(UINT32 logAddr, UINT32 ramAddr, UINT32 size);
 UINT8 SMC_LCard2Usb(UINT32 logAddr, UINT32 size);
 UINT8 SMC_LCard2UsbExt(void);

 UINT8 SMC_LUsb2Card(UINT32 logAddr, UINT32 size);
 UINT8 SMC_LDram2Card(UINT32 logAddr, UINT32 ramAddr, UINT32 size);

 UINT8 SMC_ReadLpaToPpa(UINT32 logAddr, UINT32* phyAddr);
 UINT8 SMC_WriteLbaToPba(UINT32 logAddr, UINT32 *oldPhyAddr, UINT32 *newPhyAddr, UINT16 *blockNum);
 UINT16 SMC_CalcLbaField(UINT16 logBlockAddr);
 UINT16 SMC_SearchNewBlock(void);
 UINT8 SMC_FormatCheck(void);
 UINT8 SMC_BlockStsCheck(UINT32 phyAddress,UINT8 stsData, UINT8 stsData2);  
 UINT8 SMC_DataStsCheck(UINT8 stsData);
 UINT8 SMC_CisCheck(void);
 void SMC_MakeTable(UINT8 bank);
 UINT8 SMC_ParityCheck(UINT8 d1, UINT8 d2);
 UINT16 SMC_CalcLba(UINT8 d1, UINT8 d2);
 UINT8 SMC_EraseTotalBlock(void);  

 UINT8 SMC_SearchValidBlock(UINT32 *pPhyAddr);
 UINT8 SMC_CisIdi(UINT32 phyAddr);
 UINT16 SMC_GetBehavior(void);
 void SMC_SetError(UINT16 errCode);
 extern void SMC_ClearError(void);
 extern UINT16 SMC_GetError(void);
 void SMC_UpdateBankTable( UINT8 bank, UINT8 toSram );  
 void SMC_BuildBankTable( UINT8 bankCount );  
 UINT8 SMC_CorrectPage(UINT8 page, UINT32 ramAddr, UINT8 doCorrect);	 
 UINT32 SMC_Ecc1BitErrCount(void);				 
 UINT8 SMC_CheckValidBlock(UINT32 *pPhyAddr);

 void SMC_HwInitialize(void);

 void SMC_IdAddressLatchCycle(void);
 void SMC_ReadWriteAddressLatchCycle(UINT32 address);

 UINT8 SMC_CheckReady(void);
 UINT8 SMC_ReadIdData(void);

 UINT8 nandDetect(void);
 UINT8 nandRsvBlkParse(void);

 UINT8 nandRsvRead(UINT32 ,UINT32 );
 UINT8 nandRsvSizeGet(UINT32 idx,UINT32 *offset,	UINT32 *);

 UINT8 nandRsvWrite(UINT32 ,UINT32 );

 UINT8 NANDF_LDram2CardCache(UINT32 logAddr, UINT32 ramAddr, UINT32 size);

 void SMC_ReadRedunData2Sram(void);

 UINT8 NANDF_AccessReserveBlock(UINT8 path, UINT8 blkNum, UINT8 pageNum, UINT8 pageCount, UINT32 ramAddr);

 void SMC_NandAutoFlowCfg(UINT32 eccAddrWord, UINT32 redundAddrWord);
 UINT8 SMC_NandAutoReadStart(UINT32 startAddr,UINT32 dramAddr,UINT16 pageNum,UINT8 wait);
 UINT8 SMC_NandAutoWriteStart(UINT32 startAddr,UINT32 dramAddr,UINT16 pageNum,UINT8 wait);

 typedef enum osdFmt_e {
 OSD_FMT_8BIT = (UINT16)0x00,
 OSD_FMT_2BIT = (UINT16)0X01,
 } osdFmt_t;

 typedef struct gOsdFmt_s {
 osdFmt_t lang;  
 osdFmt_t icon;  
 } gOsdFmt_t;

 typedef struct osdPen_s {
 UINT32 color;
 UINT16 width;
 } osdPen_t;

 typedef struct osdMemCtrl_s {
 UINT32 addr;  
 UINT32 size;  
 } osdMemCtrl_t;

 typedef struct osdSrcSize_s
 {
 UINT32 iconSize;
 UINT32 EngSize;
 UINT32 FontSize;
 UINT32 StrSize;
 }osdSrcSize_t;

 typedef struct osdMem_s {
 osdMemCtrl_t dispA;  
 osdMemCtrl_t dispB;  
 osdMemCtrl_t datDraw;  
 osdMemCtrl_t icon;  
 osdMemCtrl_t cInfo;  
 osdMemCtrl_t char8;  
 osdMemCtrl_t char16;  
 osdMemCtrl_t res;  
 } osdMem_t;

 UINT32 osdMemSizeInit(void);
 UINT8 osdMemInit(void);
 void osdMemCbGet(osdMem_t** ppMem);
 void osdSrcSizeSet(osdSrcSize_t *pSrcSize);

 void distVectorGet();
 void distModeFuncSet(UINT8 mode);
 void distModeFuncGet(UINT8* mode);
 void distDispSet(void);
 void distEdgeAddrGet(UINT8 mode,UINT32* EdgeAddrA,UINT32* EdgeAddrB);
 void distMeOutAddrGet(UINT8 mode,UINT32* MeAddr0);
 void distGmvSet(UINT8 GmvX, UINT8 GmvY);
 void distGmvGet(UINT8* GmvX, UINT8* GmvY);
 void distClampSizeSet(UINT8 ClmpX, UINT8 ClmpY);
 void distClampSizeGet(UINT8* ClmpX, UINT8* ClmpY);
 void distSwitchSize(UINT16 SrcXsize, UINT16 SrcYsize);

 void distIntr();
 void distSvcProc(UINT8* pt);

 extern UINT8 xdata bUsbPwr;

 void INITIO_Initialize(void);

 void INITIO_SoftwareReset(void);
 void INITIO_SetCameraSize(void);  
 void INITIO_GetCameraSizeIndex(UINT16 index);  
 void INITIO_SetCameraSizeIndex(UINT16 index);  
 void INITIO_GetProductInfo(void);
 void INITIO_GetInformation(UINT16 infoType);

 typedef enum __FILE_LIST_E{
 __FILE_ID_LIB_3A__ = 0x0100,
 __FILE_ID_LIB_AE_FLOW__ = 0x0101,
 __FILE_ID_LIB_AE_API__ = 0x0102,
 __FILE_ID_LIB_FD_FLOW__ = 0x0103,
 __FILE_ID_LIB_FD_BLINK__ = 0x0104,
 __FILE_ID_LIB_FD_SMILE__ = 0x0105,
 __FILE_ID_LIB_FORNT_LOGIC__ = 0x0106,
 __FILE_ID_LIB_FORNT_VD__ = 0x0107,
 __FILE_ID_LIB_AF_FLOW__ = 0x0108,
 __FILE_ID_LIB_AF_API__ = 0x0109,

 __FILE_ID_LIB_AUDIO__ 	= 0x0200,

 __FILE_ID_LIB_CDSP__ = 0x0300,
 __FILE_ID_CDSP_TASK__	 = 0x0301,
 __FILE_ID_CDSP_FILTER__	 = 0x0302,
 __FILE_ID_LIB_CDSP_MISC__ = 0x0303,

 __FILE_ID_CALIBRATION__=0x0310,

 __FILE_ID_LIB_CMD__ = 0x0400,

 __FILE_ID_LIB_DISP__ = 0x0500,

 __FILE_ID_LIB_DOS__ = 0x0600,
 __FILE_ID_DOSFILE__ = 0x0601,

 __FILE_ID_LIB_DOS32__ = 0x0700,
 __FILE_ID_DIR_LOOKUP__ = 0x0701,
 __FILE_ID_DOS_DEL__ = 0x0702,
 __FILE_ID_DOS_FILERW__ = 0x0703,
 __FILE_ID_DOS_FORMAT__ = 0x0704,
 __FILE_ID_DOS_MOUNT__ = 0x0705,
 __FILE_ID_DOS_OPEN__ = 0x0706,
 __FILE_ID_DOS_RW__ = 0x0707,
 __FILE_ID_DOS_SUPPORT__ = 0x0708,
 __FILE_ID_FAT_RW__ = 0x0709,
 __FILE_ID_VFS_API__	= 0x070a,

 __FILE_ID_DOS_USR__ = 0x070b,

 __FILE_ID_LIB_DPOF__ = 0x0800,

 __FILE_ID_LIB_EXIF__ = 0x0900,

 __FILE_ID_LIB_GLOBAL__ = 0x0a00,
 __FILE_ID_INITIO__ = 0x0a01,
 __FILE_ID_GLOBAL_INIT__ = 0x0a02,

 __FILE_ID_LIB_HAL__ = 0x0b00,
 __FILE_ID_HAL_CDSP__ = 0x0b01,
 __FILE_ID_HAL_NAND__ = 0x0b02,
 __FILE_ID_HAL_SD__ = 0x0b03,
 __FILE_ID_HAL_FLASH__ = 0x0b04,
 __FILE_ID_HAL_GPRM__ = 0x0b05,
 __FILE_ID_HAL_CDSP_CORE__ = 0x0b06,
 __FILE_ID_HAL_CDSP_IQ__ = 0x0b07,
 __FILE_ID_HAL_SD_INTR__ = 0x0b08,
 __FILE_ID_HAL_CPU__ = 0x0b09,
 __FILE_ID_HAL_DMAC__ = 0x0b0a,
 __FILE_ID_HAL_MOE__ = 0x0b0b,

 __FILE_ID_LIB_INTR__ = 0x0c00,

 __FILE_ID_LIB_MISC__ = 0x0d00,
 __FILE_ID_BP_MAIN__ = 0x0d01,
 __FILE_ID_BP_FIND__ = 0x0d02,

 __FILE_ID_LIB_OS__ = 0x0e00,
 __FILE_ID_BOOTER__ = 0x0e01,

 __FILE_ID_LIB_OSD__ = 0x0f00,
 __FILE_ID_OSD_MEM__ = 0x0f01,	
 __FILE_ID_OSD_LINE__ = 0x0f02,
 __FILE_ID_OSD_DRAW__ = 0x0f03,
 __FILE_ID_OSD_BUFF__ = 0x0f04,
 __FILE_ID_OSD_INIT__ = 0x0f05,

 __FILE_ID_LIB_PB__ = 0x1000,

 __FILE_ID_LIB_PV__ = 0x1100,
 __FILE_ID_PV_TASK__ = 0x1101,
 __FILE_ID_PV_PHOTO_FRAME__ = 0x1102,
 __FILE_ID_PV_ZOOM__ = 0x1103,
 __FILE_ID_PB_PHOTOFRAME__=0X1104,

 __FILE_ID_LIB_SNAP__ = 0x1200,

 __FILE_ID_SNAP_TASK__	 = 0x1201,
 __FILE_ID_SNAP_DZOOM__ = 0x1202,
 __FILE_ID_SNAP_QTABLE__ = 0x1203,
 __FILE_ID_SNAP_ICATSTUB__ = 0x1204,
 __FILE_ID_SNAP_CORE__ = 0x1205,

 __FILE_ID_LIB_STORAGE__  = 0x1300,
 __FILE_ID_SMC_533__ = 0x1301,
 __FILE_ID_SMC_2533__ = 0x1302,
 __FILE_ID_SD_RW__ = 0x1303,
 __FILE_ID_DOSRAM__ = 0x1304,
 __FILE_ID_SMC__ = 0x1305,
 __FILE_ID_SD_533_BGWR__ = 0x1306,	
 __FILE_ID_SD_HW__ = 0x1307,
 __FILE_ID_NAND_AUTOFLOW__ = 0x1308,
 __FILE_ID_SD_BG__=0x1309,

 __FILE_ID_LIB_SYS__ = 0x1400,
 __FILE_ID_MAIN__ = 0x1401,
 __FILE_ID_SYS_SVC_DEF__ = 0x1402,
 __FILE_ID_SYS_MEM_INIT__ = 0x1403,

 __FILE_ID_LIB_USB__ = 0x1500,

 __FILE_ID_LIB_UTIL__ = 0x1600,
 __FILE_ID_DATESTAMP__ = 0x1601,
 __FILE_ID_FW_UPDATE__ = 0x1602,
 __FILE_ID_UTIL_CODE_SENTRY__ = 0x1603,

 __FILE_ID_LIB_VIDEO__ = 0x1700,
 __FILE_ID_VIDEO_CLIP__ = 0x1701,
 __FILE_ID_VIDEO_PB__ = 0x1702,
 __FILE_ID_VIDEO_DTSTMP__ = 0x1703,
 __FILE_ID_VIDEO_BRC__ = 0x1704,
 __FILE_ID_VIDEO_PBINIT__ = 0x1705,
 __FILE_ID_VIDEO_RECINIT__ = 0x1706,
 __FILE_ID_VIDEO_RECABORT__ = 0x1707,
 __FILE_ID_VIDEO_FUNC__ = 0x1708,
 __FILE_ID_VIDEO_MEM__ = 0x1709,
 __FILE_ID_TIMELAPSE__	= 0x1710,
 __FILE_ID_VIDEO_ZOOM__ = 0x1711,
 __FILE_ID_VIDEO_CAPTURE__ =0x1712,
 __FILE_ID_VIDEOPB_DUMMY__= 0x17ff,

 __FILE_ID_LIB_HOST__ = 0x2000,
 __FILE_ID_APP_INIT__ = 0x2001,
 __FILE_ID_APP_STS_STILLVIEW__ = 0x2002,
 __FILE_ID_APP_DEV_STORAGE__ = 0x2003,
 __FILE_ID_APP_STS_VIDEOVIEW__ = 0x2004,
 __FILE_ID_APP_DEV_DISP__ = 0x2005,
 __FILE_ID_APP_TP_SCAN__ = 0x2006,

 __FILE_ID_SENSOR_DRIVER__ = 0x2100,

 __FILE_ID_MOTION_DET__ = 0x2200,

 __FILE_ID_PANORAMA_API__ = 0x2300,

 __FILE_ID_LIB_MAX__ = 0xff00,
 } __FILE_LIST_T;

 UINT8 __ASSERT(UINT16 __FILE_ID, UINT16 __LINE_NUM, UINT8 __EXIT);

 UINT8 __INTR_ASSERT(UINT16 __FILE_ID, UINT16 __LINE_NUM, UINT8 __EXIT);

 void __PRINTF(UINT16 __FILE_ID, UINT16 __LINE_NUM);

 typedef enum {
 CSTR_ERROR, 				
 CSTR_SUCCESS, 			
 CSTR_FAIL, 					
 CSTR_CMD_UI_HELP, 		
 CSTR_CMD_MEM_HELP, 		
 CSTR_CMD_ADC_HELP, 		
 CSTR_CMD_ADC_TEST_S,		
 CSTR_CMD_ADC_TEST_E, 	
 CSTR_CMD_ADC_TAST_START,
 CSTR_CMD_ADC_TEST_INFO,	
 CSTR_CMD_BTN_TEST_INFO,	
 CSTR_CMD_BTN_TEST_NOERR, 
 CSTR_CMD_BTN_TEST_RETRY, 	
 } ConstStringList;

 typedef struct tagString {
 ConstStringList strId;
 UINT8 code* str;
 } ConstString_t;

 void dbgPrintf(UINT16 StringID);
 void dbgFuncTrack(UINT16 __FILE_ID, UINT16 __LINE_NUM) ;

 typedef enum {
 WDT_PROC_RELOAD        = 0x00,
 WDT_PROC_DBG_USB_ONLY  = 0x01,
 WDT_PROC_DBG_ALL       = 0x02,
 } wdtRstMode_t;

 void dbgWdtSet(UINT8 en);
 void dbgWdtFeed(UINT32 food);
 UINT32 dbgWdtCfg(UINT8 bWdtEn, wdtRstMode_t mode, UINT32 timeOut);

 extern UINT32 xdata G_TimerRef;

 void dbgBMInit(void);
 void dbgBMStart(UINT8 bTmrMode);
 void dbgBMShow(UINT8 bBMMode, UINT32* dwTimeTot, UINT32* dwTimeSeg, UINT32 dwLine);
 void dbgBMStop(void);

 typedef enum panelStatus {
 PANEL_STATUS_LCD = 0x00,
 PANEL_STATUS_NTSC = 0x01,
 PANEL_STATUS_PAL = 0x02,
 PANEL_STATUS_RESET = 0xFF,
 PANEL_STATUS_TOTAL = 0x04,
 } panelStatus_t;

 void appTVOutDetect(void); 
 void appHDMIDetect(void);
 void appPanelStatusGet(panelStatus_t* panelMode);
 void appPanelStatusSet(panelStatus_t panelMode);
 void appDispSet(void);
 UINT8 appPanelTypeGet(void);
 UINT8 appDispHdmiOpen(void);
 void appDispOpen();
 void appHdmiOsdSet(UINT8 on);
 void appPreviewCdspDispResSet(void);

 void appWhiteLedCtrl(void);

 void app_set_led(UINT8 falg);

 void app_set_lcd_backlight_status(UINT8 status);
 UINT8 app_get_lcd_backlight_status();

 void appPanelAutoProt();
 void app_set_close_lcd_backlight_time();

 void appUpdateStatsLightStatus();

 void appSetGpio6SpiWP(UINT8 falg);

 void app_set_speaker(UINT8 falg);

 void appSetWTDHeartbeat(UINT8 falg);

 void appFlashLedSet(UINT8 flag);
 void appSnapFlashAdjustsAe(UINT8 flag);
 void appSnapFlashLed(UINT8 flag);

 void appSensorPWDNSwitch(UINT8 falg);

 void AF_Set(UINT16 pos);
 UINT16 AF_GetPos(void);

 void AF_Switch(void);

 void appAFCtrl(UINT8 flag);

  UINT8 *frontSensorOpDevNameGet(void);
  void frontSensorOpFrameRateSet(UINT8 fps);
  void frontSensorOpCapabilityGet(frontCapabilityArg_t *parg);
  void frontSensorOpIntrCapabilityGet(frontCapabilityArg_t *parg);
  void frontSensorOpPowerOn(void);
  void frontSensorOpPowerOff(void);
  void frontSensorOpPreviewModeSet(void);
  void frontSensorOpPreviewSensorZoomModeSet(UINT16 factor);
  void frontSensorOpIntrPreviewSensorZoomModeSet(UINT16 factor);
  void frontSensorOpSnapModeSet(void);
  void frontSensorOpSnapShot(UINT8 first, UINT8 mode, UINT8 scaleUp);
  void frontSensorOpGainSet(UINT8 val, UINT8 isr, UINT8 upd);
  void frontSensorOpExposureTimeSet(frontExposureTimeArg_t *parg);
  void frontSensorOpIntrGainSet(UINT8 val, UINT8 isr, UINT8 upd);
  void frontSensorOpIntrExposureTimeSet(frontExposureTimeArg_t *parg);
  UINT8 frontSensorOpOpticalBlackStatusGet(void);
  UINT8 frontSensorOpOpticalBlackCalibrate(void);
  void frontSensorOpShutterOpen(void);
  void frontSensorOpShutterClose(void);
  void frontSensorOpIntrPreviewDynamicTempoalDenoiseSet(UINT8 en);

  void frontSensorOpAeTargetLuminanceSet(void *parg);
  void frontSensorOpAeExposureCompensationSet(void *parg);
  void frontSensorOpAeFrameRateSet(void *parg);
  void frontSensorOpAwbModeSet(void *parg);
  void frontSensorOpAfterSnapShot(void *parg);

 static void frontBeforePowerOn(void);
 static void frontAfterPowerOn(void);
 static void frontBeforePowerOff(void);
 static void frontAfterPowerOff(void);
 static void frontSnapExposureTimeSet(UINT8 mode,UINT8 scaleUp);
 static void frontBeforeSnapExposureTimeSet(void);
 static void frontGainTblValueTransfer(UINT16 value,UINT16 *numerator,UINT16 *denominator);
 static void frontPreviewModeSwitchPostProc(void);
 static void frontResTblBinRead(UINT16 fileId,	UINT16 byteAddress);
 static void frontResCmdBinSend(UINT16 fileId);

 extern UINT8 sensor_switch;

 static code UINT8 frontDevName[] = "SENSOR - OV OV5647 RAW";

  extern UINT8 TAG_CVT_S;

 typedef enum CvtTypeToX_u_0 {

 _ae20fps50hzTbl = 0x00C2,
 _ae20fps60hzTbl = 0x00C3,

 _ae30fps50hzTbl = 0x00C6,
 _ae30fps60hzTbl = 0x00C7,
 _ae60fps50hzTbl = 0x00C8,
 _ae60fps60hzTbl = 0x00C9,
 } _aeTbl;

 typedef enum CvtTypeToX_e {
 _gainTbl = 0x00CA,
 };

 typedef enum CvtTypeRmv_e {
 _frontInit0CmdTbl		= 0x00CB,
 _frontInit1CmdTbl		= 0x00CC,
 _frontInit2CmdTbl		= 0x00CD,
 _frontInit3CmdTbl		= 0x00CE,
 _frontInit4CmdTbl		= 0x00CF,

 _front20fpsPrevCmdTbl	= 0x00D1,

 _front30fpsPrevCmdTbl	= 0x00D3,
 _front60fpsPrevCmdTbl	= 0x00D4,
 _frontSnapCmdTbl		= 0x00D5,
 };

 typedef enum CvtTypeLnkAddr_e {
 __aeTbl = 0xa000,
 __gainTbl = 0xb000,
 };

  extern UINT8 TAG_CVT_E;

 static xdata frontAe_t aeTbl[140] _at_ 0xa000;

 xdata UINT16 gainTbl[241] _at_ 0xb000;//This segment had been converted to resource file: "gainTbl.BIN"

 static code UINT8 ae30fpsMaxIdxTbl[2] = {
  139,
  138,
 };

 static code UINT8 ae30fpsMinIdxTbl[2] = {
  0,
  0,
 };

 static code UINT8 ae50fpsMaxIdxTbl[2] = {
  129,
  128,
 };

 static code UINT8 ae50fpsMinIdxTbl[2] = {
  0,
  0,
 };

 static code UINT8 ae60fpsMaxIdxTbl[2] = {
  125,
  118,
 };

 static code UINT8 ae60fpsMinIdxTbl[2] = {
  0,
  0,
 };

 static code UINT8 ae20fpsMaxIdxTbl[2] = {
  125,
  124,
 };

 static code UINT8 ae20fpsMinIdxTbl[2] = {
  0,
  0,
 };

 static code UINT8 frontPrevToSnapBinningRateTbl[3] = {

  (1.6),

  4,
  4,
 };

 static code frontPreviewTemporalDenoise_t frontPrevTempDenoiseTbl[3] = {

 {   
  11 + 5,
  0,
  11 + 5,
  0,
  44850000L,
 31,
 6,
 },

 {   
  11 + 5,
  0,
  11 + 5,
  0,
  34660000L,
 31,
 6,
 },
 {   
  11,
  0,
  11,
  0,
  25200000L,
 31,
 6,
 },
 };

 static code frontPrevCapabDesc_t frontPrevCapabDesc[3] = {

 {   
  FRONT_SRAM_MODE_ENABLE,
  FRONT_BAYER_PTN_GBBRGR,
  1280,
  960,
  3,
  1,
  1630,
  984,
  11,
  0,
  11,
  0,
  48260000L,
  0,
  0,
  0,
  2,
  3,
  0,
  2,
  3,
 aeTbl,
 aeTbl,
 ae30fpsMaxIdxTbl,
 ae30fpsMinIdxTbl,
  100,
 gainTbl,
  240,
  0,
 &frontPrevTempDenoiseTbl[0],
  0,
 },

 {   
  FRONT_SRAM_MODE_ENABLE,
  FRONT_BAYER_PTN_GBBRGR,
  640,
  480,
  3,
  1,
  1132,
  738,
  11,
  0,
  11,
  0,
  50420000L,
  0,
  0,
  0,
  2,
  3,
  0,
  2,
  3,
 aeTbl,
 aeTbl,
 ae60fpsMaxIdxTbl,
 ae60fpsMinIdxTbl,
  100,
 gainTbl,
  112,
  0,
 &frontPrevTempDenoiseTbl[1],
  0,
 },
 {   
  FRONT_SRAM_MODE_ENABLE,
  FRONT_BAYER_PTN_GBBRGR,
  640,
  480,
  3,
  1,
  1132,
  738,
  11,
  0,
  11,
  0,
  25200000L,
  0,
  0,
  0,
  2,
  3,
  0,
  2,
  3,
 aeTbl,
 aeTbl,
 ae20fpsMaxIdxTbl,
 ae20fpsMinIdxTbl,
  100,
 gainTbl,
  112,
  0,
 &frontPrevTempDenoiseTbl[2],
  0,
 },
 };

 static code frontSnapCapabDesc_t frontSnapCapabDesc[1] = {
 {   
  FRONT_BAYER_PTN_GBBRGR,
  2560,
  1920,
  1,
  1,
  2700,
  1968,
  11,
  18,
  24,
  0,
  11,
  0,
  51000000L,
  41052631L,
  31200000L,
  0,
  0,
  0,
  2,
  3,
  0,
  2,
  3,
 },
 };

// static code UINT8 frontInit0CmdTbl[];//This segment had been removed to resource file: "frontInit0CmdTbl.BIN"

// static code UINT8 frontInit1CmdTbl[];//This segment had been removed to resource file: "frontInit1CmdTbl.BIN"

// static code UINT8 frontInit2CmdTbl[];//This segment had been removed to resource file: "frontInit2CmdTbl.BIN"

// static code UINT8 front30fpsPrevCmdTbl[];//This segment had been removed to resource file: "front30fpsPrevCmdTbl.BIN"

// static code UINT8 frontInit3CmdTbl[];//This segment had been removed to resource file: "frontInit3CmdTbl.BIN"

// static code UINT8 frontSnapCmdTbl[];//This segment had been removed to resource file: "frontSnapCmdTbl.BIN"

// static code UINT8 frontInit4CmdTbl[];//This segment had been removed to resource file: "frontInit4CmdTbl.BIN"

// static code UINT8 front60fpsPrevCmdTbl[];//This segment had been removed to resource file: "front60fpsPrevCmdTbl.BIN"

// static code UINT8 front20fpsPrevCmdTbl[];//This segment had been removed to resource file: "front20fpsPrevCmdTbl.BIN"

 static code UINT8 front30fpsPrevDz1xCmdTbl[1] = {
 0,
 };
 static UINT8 front30fpsPrevDz2xCmdTbl[1] = {
 0,
 };

 static UINT8 front60fpsPrevDz1xCmdTbl[1] = {
 0,
 };
 static UINT8 front60fpsPrevDz2xCmdTbl[1] = {
 0,
 };

 static UINT8 front20fpsPrevDz1xCmdTbl[1] = {
 0,
 };
 static UINT8 front20fpsPrevDz2xCmdTbl[1] = {
 0,
 };

 static UINT8 frontPrevMode = 0;
 static UINT8 frontSnapMode = 0;
 static UINT16 frontLastGain;		 
 static UINT16 frontLastGainIdx;		 
 static UINT32 frontLastShutter;		 
 static UINT16 frontSnapLastGain;	 
 static UINT32 frontSnapLastShutter;	 

 static UINT8 LoadBackfrontLastGainIdx;
 static UINT8 LoadBackfrontLastSetting = 0;

 static UINT8 frontPrevModeSave = 0;	 

 static UINT8 frontPowerOnStatus = 0;  

 static UINT8 frontPrevDyTdEn = 0;
 static UINT8 frontPrevDyTdSwitchPclk = 0;
 static UINT8 frontPrevDyTdSwitchExp = 0;
 static UINT8 frontPrevDyTdSwitchGain = 0;
 static UINT8 frontPrevDyTdSetFlow = 0;

 extern UINT8 flash_led_snap_adj_exp;

 UINT8
  frontSensorOpen(
 void
 ) 
 {

 frontOperatingFunc_t opFunc; 

 opFunc.expGainSetPos                           = AE_EXP_GAIN_SET_POS_VVALID_RISING;
 opFunc.gainAfterExp                            = AE_GAIN_AFTER_EXP_OFF;
 opFunc.pfOpDevNameGet                          = frontSensorOpDevNameGet;
 opFunc.pfOpFrameRateSet                        = frontSensorOpFrameRateSet;
 opFunc.pfOpCapabilityGet                       = frontSensorOpCapabilityGet;
 opFunc.pfOpIntrCapabilityGet                   = frontSensorOpIntrCapabilityGet;
 opFunc.pfOpPowerOn                             = frontSensorOpPowerOn;
 opFunc.pfOpPowerOff                            = frontSensorOpPowerOff;
 opFunc.pfOpPreviewModeSet                      = frontSensorOpPreviewModeSet;
 opFunc.pfOpPreviewSensorZoomModeSet            = frontSensorOpPreviewSensorZoomModeSet;
 opFunc.pfOpIntrPreviewSensorZoomModeSet        = frontSensorOpIntrPreviewSensorZoomModeSet;
 opFunc.pfOpSnapModeSet                         = frontSensorOpSnapModeSet;
 opFunc.pfOpSnapShot                            = frontSensorOpSnapShot;
 opFunc.pfOpGainSet                             = frontSensorOpGainSet;
 opFunc.pfOpExposureTimeSet                     = frontSensorOpExposureTimeSet;
 opFunc.pfOpIntrGainSet                         = frontSensorOpIntrGainSet;
 opFunc.pfOpIntrExposureTimeSet                 = frontSensorOpIntrExposureTimeSet;
 opFunc.pfOpOpticalBlackStatusGet               = frontSensorOpOpticalBlackStatusGet;
 opFunc.pfOpOpticalBlackCalibrate               = frontSensorOpOpticalBlackCalibrate;
 opFunc.pfOpShutterOpen                         = frontSensorOpShutterOpen;
 opFunc.pfOpShutterClose                        = frontSensorOpShutterClose;
 opFunc.pfOpIntrPreviewDynamicTempoalDenoiseSet = frontSensorOpIntrPreviewDynamicTempoalDenoiseSet;

 frontResTblBinRead(_gainTbl, __gainTbl);

 return frontDevOpen(&opFunc, sizeof(frontOperatingFunc_t));

 } 

  UINT8 *
  frontSensorOpDevNameGet(
 void
 )
 {
 return frontDevName;
 }

  void
  frontSensorOpFrameRateSet(
 UINT8 fps
 )
 {
 switch ( fps ) {
 case 30:
 case 25:
 printf("frontOpFrameRateSet = 30\n");
 frontPrevMode = 0;

 if (fps == 25) {
 printf("50Hz\n");
 frontResTblBinRead(_ae30fps50hzTbl, __aeTbl);
 }
 else {
 printf("60Hz\n");
 frontResTblBinRead(_ae30fps60hzTbl, __aeTbl);
 }   
 break;
 case 60:
 case 50:
 printf("frontOpFrameRateSet = 60\n");
 frontPrevMode = 1;

 if (fps == 50) {
 printf("50Hz\n");
 frontResTblBinRead(_ae60fps50hzTbl, __aeTbl);
 }
 else {
 printf("60Hz\n");
 frontResTblBinRead(_ae60fps60hzTbl, __aeTbl);
 }
 break;
 case 24:
 case 20:
 printf("frontOpFrameRateSet = 24\n");
 frontPrevMode = 2;

 if (fps == 20) {
 printf("50Hz\n");
 frontResTblBinRead(_ae20fps50hzTbl, __aeTbl);
 }
 else {
 printf("60Hz\n");
 frontResTblBinRead(_ae20fps60hzTbl, __aeTbl);
 }
 break;
 }
 }

  void
  frontSensorOpCapabilityGet(
 frontCapabilityArg_t *parg
 )
 {
 if ( parg->ppprevCap ) {
 *(parg->ppprevCap) = &frontPrevCapabDesc[frontPrevMode];
 }

 if ( parg->ppsnapCap ) {
 *(parg->ppsnapCap) = &frontSnapCapabDesc[frontSnapMode];
 }
 }

  void
  frontSensorOpIntrCapabilityGet(
 frontCapabilityArg_t *parg
 )
 {
 if ( parg->ppprevCap ) {
 *(parg->ppprevCap) = &frontPrevCapabDesc[frontPrevMode];
 }

 if ( parg->ppsnapCap ) {
 *(parg->ppsnapCap) = &frontSnapCapabDesc[frontSnapMode];
 }
 }

  void
  frontSensorOpPowerOn(
 void
 )
 {
 frontPrevCapabDesc_t *pcap;
 UINT16 addrt[] = {0x300a};
 UINT8 datat[] = {0XFF};
 UINT8 i;

 frontBeforePowerOn();
 pcap = &(frontPrevCapabDesc[frontPrevMode]);

  HAL_GpioByteFuncSet(GPIO_BYTE_TG0, 0xFF, 0x00);
  HAL_GpioByteDirSet(GPIO_BYTE_TG0, 0xFF, 0x04);
  HAL_GpioByteInGateSet(GPIO_BYTE_TG0, 0xFF, 0x00);
  HAL_GpioByteOutSet(GPIO_BYTE_TG0, 0xFF, 0x00);

  HAL_FrontParaSet(FRONT_PARA_MCLK_CFG, FRONT_MCLK_SRC_INT, pcap->mclkDiv, pcap->mclkPhase, 3, 0);
  HAL_FrontParaSet(FRONT_PARA_PCLK_CFG, (1 << 3), pcap->pclkDiv, pcap->pclkPhase, 0, 0);
  HAL_FrontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, 1, 1, 0, 0, 0);

  HAL_I2cDeviceAddrSet(0x6C);
  HAL_I2cRegClkCfgSet(I2C_SOURCE_DIV_10);
  HAL_I2cClkCfgSet(0);	

  HAL_GlobalTimerWait(300);

 frontResCmdBinSend(_frontInit1CmdTbl);
  HAL_GlobalTimerWait(5);
 frontResCmdBinSend(_frontInit2CmdTbl);
  HAL_GlobalTimerWait(50);

 frontAfterPowerOn();

 }

  void
  frontSensorOpPowerOff(
 void
 )
 {

 frontBeforePowerOff();

 frontAfterPowerOff();
 }

  void
  frontSensorOpPreviewModeSet(
 void
 )
 {
 frontPrevCapabDesc_t *pcap;

 pcap = &(frontPrevCapabDesc[frontPrevMode]);

  dbgFuncTrack(-1, (unsigned short)2526);
  HAL_FrontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);	 
  dbgFuncTrack(-1, (unsigned short)2528);

  HAL_FrontParaSet(FRONT_PARA_MCLK_CFG, FRONT_MCLK_SRC_INT, pcap->mclkDiv, pcap->mclkPhase, 0, 0);
  HAL_FrontParaSet(FRONT_PARA_PCLK_CFG, (1 << 3), pcap->pclkDiv, pcap->pclkPhase, 0, 0);
  HAL_FrontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, 1, 1, 0, 0, 0);
  HAL_FrontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
  HAL_FrontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
  HAL_FrontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
  HAL_FrontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

 if ( frontPrevMode == 0 ) {   

 frontResCmdBinSend(sensor_switch ? _front30fpsPrevCmdTbl : _frontInit3CmdTbl);
 }
 else if ( frontPrevMode == 1 ) {   
 frontResCmdBinSend(_front60fpsPrevCmdTbl);
 }
 else if (frontPrevMode == 2) {	 
 frontResCmdBinSend(_front20fpsPrevCmdTbl);
 }

 frontPreviewModeSwitchPostProc();

  HAL_CdspBayerPatternSet(FRONT_BAYER_PTN_GBBRGR);

  dbgFuncTrack(-1, (unsigned short)2555);
  HAL_FrontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);	 
  dbgFuncTrack(-1, (unsigned short)2557);

 }

  void
  frontSensorOpPreviewSensorZoomModeSet(
 UINT16 factor
 )
 {
 frontPrevCapabDesc_t *pcap;
 UINT8 pvGainIdxVal;
 SINT16 pv_gId = 0;

 pcap = &(frontPrevCapabDesc[frontPrevMode]);

 if (frontPrevSensorZoomSnapStatus == 1) {
 frontPrevSensorZoomSnapStatus = 0;
 return;
 }

 if ( frontPrevSensorZoomStatus == 0 ) {
 if ( frontPrevMode == 0 ) {   

 factor = factor;
 pvGainIdxVal = pvGainIdxVal;

 }
 else if (frontPrevMode == 1) {	 

 factor = factor;
 pvGainIdxVal = pvGainIdxVal;

 }
 else if (frontPrevMode == 2) {	 

 factor = factor;
 pvGainIdxVal = pvGainIdxVal;

 }
 }
 else if ( frontPrevSensorZoomStatus == 1) {
 if ( frontPrevMode == 0 ) {   

 factor = factor;
 pvGainIdxVal = pvGainIdxVal;

 }
 else if (frontPrevMode == 1) {	 

 factor = factor;
 pvGainIdxVal = pvGainIdxVal;

 }
 else if (frontPrevMode == 2) {	 

 factor = factor;
 pvGainIdxVal = pvGainIdxVal;

 }
 }

 }

  void
  frontSensorOpIntrPreviewSensorZoomModeSet(
 UINT16 factor
 )
 {
 frontPrevCapabDesc_t *pcap;
 UINT8 pvGainIdxVal;
 SINT16 pv_gId = 0, paraAry;

 pcap = &(frontPrevCapabDesc[frontPrevMode]);

 if (frontPrevSensorZoomSnapStatus == 1) {
 frontPrevSensorZoomSnapStatus = 0;
 return;
 }

 if ( frontPrevSensorZoomStatus == 0 ) {
 if ( frontPrevMode == 0 ) {   

 factor = factor;
 pvGainIdxVal = pvGainIdxVal;
 paraAry = paraAry;

 }
 else if (frontPrevMode == 1) {	 

 factor = factor;
 pvGainIdxVal = pvGainIdxVal;
 paraAry = paraAry;

 }
 else if (frontPrevMode == 2) {	 

 factor = factor;
 pvGainIdxVal = pvGainIdxVal;
 paraAry = paraAry;

 }
 }
 else if ( frontPrevSensorZoomStatus == 1) {
 if ( frontPrevMode == 0 ) {   

 factor = factor;
 pvGainIdxVal = pvGainIdxVal;
 paraAry = paraAry;

 }
 else if (frontPrevMode == 1) {	 

 factor = factor;
 pvGainIdxVal = pvGainIdxVal;
 paraAry = paraAry;

 }
 else if (frontPrevMode == 2) {	 

 factor = factor;
 pvGainIdxVal = pvGainIdxVal;
 paraAry = paraAry;

 }
 }

 }

  void
  frontSensorOpSnapModeSet(
 void
 )
 {
 frontSnapCapabDesc_t *pcap;

 pcap = &(frontSnapCapabDesc[frontSnapMode]);

  HAL_FrontParaSet(FRONT_PARA_MCLK_CFG, FRONT_MCLK_SRC_INT, pcap->mclkDiv, pcap->mclkPhase, 0, 0);	
  HAL_FrontParaSet(FRONT_PARA_PCLK_CFG, (1 << 3), pcap->pclkDiv, pcap->pclkPhase, 0, 0);
  HAL_FrontParaSet(FRONT_PARA_SYNC_INVERT_ENABLE, 1, 1, 0, 0, 0);
  HAL_FrontParaSet(FRONT_PARA_BYPASS_ENABLE, pcap->bypassHref, pcap->bypassVref, 0, 0, 0);
  HAL_FrontParaSet(FRONT_PARA_RESHAPE_ENABLE, pcap->reshapeHen, pcap->reshapeVen, 0, 0, 0);
  HAL_FrontParaSet(FRONT_PARA_RESHAPE_REGION, pcap->reshapeHfall, pcap->reshapeHrise, pcap->reshapeVfall, pcap->reshapeVrise, 0);
  HAL_FrontParaSet(FRONT_PARA_CROP_REGION, pcap->width, pcap->height, pcap->xoff, pcap->yoff, 0);

  HAL_CdspBayerPatternSet(FRONT_BAYER_PTN_GBBRGR);
 }

  void
  frontSensorOpSnapShot(
 UINT8 first,
 UINT8 mode,
 UINT8 scaleUp
 )
 {

 if ( frontPrevSensorZoom2xGainEn ) {
 frontPrevSensorZoomSnapStatus = 1;
 }

 if ( first != 0 ) {

  {	UINT8 sts;	do {(sts) = IE; IE &= 0x7f;} while(0);	HAL_FrontSignalWait(FRONT_WAIT_VSYNC,FRONT_WAIT_RISING,1);	do {((unsigned char volatile xdata *) 0)[0x2D00] |= 0x08;IE = (sts);} while(0);	};

 frontResCmdBinSend(sensor_switch ? _frontSnapCmdTbl : _frontInit4CmdTbl);

 frontSnapExposureTimeSet(mode,scaleUp);
 }

 if (mode != CDSP_DO_FRONT){   

  {	UINT8 sts;	do {(sts) = IE; IE &= 0x7f;} while(0);	HAL_FrontSignalWait(FRONT_WAIT_VSYNC,FRONT_WAIT_RISING,1);	do {((unsigned char volatile xdata *) 0)[0x2D00] |= 0x08;IE = (sts);} while(0);	};

  HAL_CdspVDramSkipWrSet(CDSP_SKIP_AWBAEEDGE,0xff);
  HAL_CdspVDramSkipWrSyncSet(1);

  {	UINT8 sts;	do {(sts) = IE; IE &= 0x7f;} while(0);	HAL_FrontSignalWait(FRONT_WAIT_VSYNC,FRONT_WAIT_RISING,1);	do {((unsigned char volatile xdata *) 0)[0x2D00] |= 0x08;IE = (sts);} while(0);	};

  {	UINT8 sts;	do {(sts) = IE; IE &= 0x7f;} while(0);	HAL_FrontSignalWait(FRONT_WAIT_VSYNC,FRONT_WAIT_FALLING,1);	do {((unsigned char volatile xdata *) 0)[0x2D00] |= 0x08;IE = (sts);} while(0);	};

  HAL_FrontParaSet(FRONT_PARA_FRONT_TO_CDSP_GATING_ENABLE, 1, 0, 0, 0, 1);

 HAL_CdspEofWait(1);

 }	
 else{  	 
 frontSnapCapabDesc_t *pcap;
 pcap = &(frontSnapCapabDesc[frontSnapMode]);

  {	UINT8 sts;	do {(sts) = IE; IE &= 0x7f;} while(0);	HAL_FrontSignalWait(FRONT_WAIT_VSYNC,FRONT_WAIT_RISING,1);	do {((unsigned char volatile xdata *) 0)[0x2D00] |= 0x08;IE = (sts);} while(0);	};

  HAL_CdspVDramSkipWrSyncSet(1);	
  HAL_CdspVDramSkipWrSet(CDSP_SKIP_ALL,0xff);

  {	UINT8 sts;	do {(sts) = IE; IE &= 0x7f;} while(0);	HAL_FrontSignalWait(FRONT_WAIT_VSYNC,FRONT_WAIT_RISING,1);	do {((unsigned char volatile xdata *) 0)[0x2D00] |= 0x08;IE = (sts);} while(0);	};

 if (scaleUp==1){
  HAL_FrontParaSet(FRONT_PARA_MCLK_CFG, FRONT_MCLK_SRC_INT, pcap->mclkDivBurstUp, pcap->mclkPhase, 0, 0);
 }
 else{
  HAL_FrontParaSet(FRONT_PARA_MCLK_CFG, FRONT_MCLK_SRC_INT, pcap->mclkDivBurst, pcap->mclkPhase, 0, 0);
 }
 }
 }

  void
  frontSensorOpGainSet(
 UINT8 val,
 UINT8 isr,
 UINT8 upd
 )
 {
 frontPrevCapabDesc_t *pcap;
 UINT8 cmdTbl[3*2];
 UINT8 TempGain = 0;

 pcap = &(frontPrevCapabDesc[frontPrevMode]);
 frontLastGain = pcap->gainTbl[val];
 frontLastGainIdx = val;

 cmdTbl[0] = 0x35;
 cmdTbl[1] = 0x0B;
 cmdTbl[2] = (UINT8)(frontLastGain & 0x00FF);

 cmdTbl[3] = 0x35;
 cmdTbl[4] = 0x0a;
 cmdTbl[5] = (UINT8)((frontLastGain & 0x0300) >> 8);

  HAL_I2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, isr, upd);

 }

  void
  frontSensorOpExposureTimeSet(
 frontExposureTimeArg_t *parg
 )
 {
 frontPrevCapabDesc_t *pcap;
 UINT16 ltotal, ftotal, espline;
 UINT32 pixclk, espline_16;
 UINT8 cmdTbl[3*3];

 pcap = &(frontPrevCapabDesc[frontPrevMode]);
 frontLastShutter = parg->val;

 ltotal = pcap->lineTot;
 ftotal = pcap->frameTot;
 pixclk = pcap->pclkFreq;

 espline = (pixclk * 10) / (ltotal * frontLastShutter);

 if (espline != 0) {
 if(espline > (ftotal - 6)) {
 espline = (ftotal - 6);

 }
 }
 else {
 espline = 1;
 }

 espline_16 = espline << 4;

 cmdTbl[0] = 0x35;
 cmdTbl[1] = 0x00;
 cmdTbl[2] = (UINT8)((espline_16 & 0x000F0000) >> 16);

 cmdTbl[3] = 0x35;
 cmdTbl[4] = 0x01;
 cmdTbl[5] = (UINT8)((espline_16 & 0x0000FF00) >> 8);

 cmdTbl[6] = 0x35;
 cmdTbl[7] = 0x02;
 cmdTbl[8] = (UINT8)(espline_16 & 0x000000FF);
  HAL_I2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

 }

  void
  frontSensorOpIntrGainSet(
 UINT8 val,
 UINT8 isr,
 UINT8 upd
 )
 {
 frontPrevCapabDesc_t *pcap;
 UINT8 cmdTbl[3*2];
 UINT8 TempGain = 0;

 pcap = &(frontPrevCapabDesc[frontPrevMode]);

 switch (frontPrevDyTdSwitchGain) {
 case 1:

 switch (frontPrevMode) {
 case 0:
 if (val >= 1) {
 val -= 1;
 }
 else {
 val = 0;
 }
 break;
 case 1:
 if (val >= 3) {
 val -= 3;
 }
 else {
 val = 0;
 }
 break;
 }

 frontPrevDyTdSwitchGain = 0;

 break;
 case 2:

 switch (frontPrevMode) {
 case 0:
 if (val < 48) {
 val += 1;
 }
 else {
 val = val-1;
 }
 break;
 case 1:
 if (val < 46) {
 val += 3;
 }
 else {
 val = val-1;
 }
 break;
 }
 frontPrevDyTdSwitchGain = 0;

 break;
 default:
 break;
 }

 frontLastGain = pcap->gainTbl[val];
 frontLastGainIdx = val;

 cmdTbl[0] = 0x35;
 cmdTbl[1] = 0x0B;
 cmdTbl[2] = (UINT8)(frontLastGain & 0x00FF);

 cmdTbl[3] = 0x35;
 cmdTbl[4] = 0x0a;
 cmdTbl[5] = (UINT8)((frontLastGain & 0x0300) >> 8);
  HAL_I2cCmdTableSend_Intr(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, isr, upd);

 }

  void
  frontSensorOpIntrExposureTimeSet(
 frontExposureTimeArg_t *parg
 )
 {
 frontPrevCapabDesc_t *pcap;
 UINT16 ltotal, ftotal, espline;
 UINT32 pixclk, espline_16;
 UINT8 cmdTbl[3*3];

 pcap = &(frontPrevCapabDesc[frontPrevMode]);
 frontLastShutter = parg->val;

 ltotal = pcap->lineTot;
 ftotal = pcap->frameTot;

 if (frontPrevDyTdEn == FRONT_PV_DYNAMIC_TD_ENABLE) {
 pixclk = pcap->prevDynamicTDTbl->pclkFreq;
 }
 else {
 pixclk = pcap->pclkFreq;
 }

 espline = (pixclk * 10) / (ltotal * frontLastShutter);

 if (espline != 0) {
 if(espline > (ftotal - 6)) {
 espline = (ftotal - 6);

 }
 }
 else {
 espline = 1;
 }

 espline_16 = espline << 4;

 cmdTbl[0] = 0x35;
 cmdTbl[1] = 0x00;
 cmdTbl[2] = (UINT8)((espline_16 & 0x000F0000) >> 16);

 cmdTbl[3] = 0x35;
 cmdTbl[4] = 0x01;
 cmdTbl[5] = (UINT8)((espline_16 & 0x0000FF00) >> 8);

 cmdTbl[6] = 0x35;
 cmdTbl[7] = 0x02;
 cmdTbl[8] = (UINT8)(espline_16 & 0x000000Ff);
  HAL_I2cCmdTableSend_Intr(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, parg->isr, parg->upd);

 }

  UINT8
  frontSensorOpOpticalBlackStatusGet(
 void
 )
 {

 return 0;
 }

  UINT8
  frontSensorOpOpticalBlackCalibrate(
 void
 )
 {

 return 0;
 }

  void
  frontSensorOpShutterOpen(
 void
 )
 {

 }

  void
  frontSensorOpShutterClose(
 void
 )
 {

 }

  void
  frontSensorOpIntrPreviewDynamicTempoalDenoiseSet(
 UINT8 en
 )
 {
 frontPrevCapabDesc_t *pcap;
 SINT16 paraAry[4];
 UINT8 upd;

 if (frontPrevDyTdSetFlow == 0) {

 frontPrevDyTdSetFlow = 1;

 frontPrevDyTdEn = en;
 frontPrevDyTdSwitchPclk = 1;
 frontPrevDyTdSwitchExp = 1;

 if (frontPrevDyTdEn == FRONT_PV_DYNAMIC_TD_DISABLE) {
 frontPrevDyTdSwitchGain = 1; 
 }
 else {
 frontPrevDyTdSwitchGain = 2; 
 }				
 }
 else {

 frontPrevDyTdSetFlow = 0;

 pcap = &(frontPrevCapabDesc[frontPrevMode]);

 if (frontPrevDyTdSwitchPclk == 1) {
 frontPrevDyTdSwitchPclk = 0;

 if (frontPrevDyTdEn == FRONT_PV_DYNAMIC_TD_ENABLE) {

 paraAry[0] = FRONT_MCLK_SRC_INT;
 paraAry[1] = pcap->prevDynamicTDTbl->mclkDiv;
 paraAry[2] = pcap->prevDynamicTDTbl->mclkPhase;
 paraAry[3] = 0;
 upd= 0;
 HAL_IntrFrontParaMClkCfgSet(&paraAry[0],upd);
 paraAry[0] = (1 << 3);
 paraAry[1] = pcap->prevDynamicTDTbl->pclkDiv;
 paraAry[2] = pcap->prevDynamicTDTbl->pclkPhase;
 paraAry[3] = 0;
 upd= 0;
 HAL_IntrFrontParaPClkCfgSet(&paraAry[0],upd);
 }
 else {

 paraAry[0] = FRONT_MCLK_SRC_INT;
 paraAry[1] = pcap->mclkDiv;
 paraAry[2] = pcap->mclkPhase;
 paraAry[3] = 0;
 upd= 0;
 HAL_IntrFrontParaMClkCfgSet(&paraAry[0],upd);
 paraAry[0] = (1 << 3);
 paraAry[1] = pcap->pclkDiv;
 paraAry[2] = pcap->pclkPhase;
 paraAry[3] = 0;
 upd= 0;
 HAL_IntrFrontParaPClkCfgSet(&paraAry[0],upd);
 }
 }
 }
 }

  void
  frontSensorOpAeTargetLuminanceSet(
 void *parg
 )
 {
 parg = parg;

 }

  void
  frontSensorOpAeExposureCompensationSet(
 void *parg
 )
 {
 parg = parg;

 }

  void
  frontSensorOpAeFrameRateSet(
 void *parg
 )
 {
 parg = parg;

 }

  void
  frontSensorOpAwbModeSet(
 void *parg
 )
 {
 parg = parg;

 }

  void
  frontSensorOpAfterSnapShot(
 void *parg
 )
 {
 parg = parg;

 }

 static void
 frontBeforePowerOn(
 void
 )
 {

  HAL_GpioByteFuncSet(GPIO_BYTE_GEN0, (1 << ( 0 & 0x07)), (1 << ( 0 & 0x07)));   
  HAL_GpioByteDirSet(GPIO_BYTE_GEN0, (1 << ( 0 & 0x07)), (1 << ( 0 & 0x07)));
  HAL_GpioByteOutSet(GPIO_BYTE_GEN0, (1 << ( 0 & 0x07)), (1 << ( 0 & 0x07)));
  HAL_GlobalTimerWait(20);

  HAL_GpioByteOutSet(GPIO_BYTE_GEN0, (1 << ( 0 & 0x07)), (0 << ( 0 & 0x07)));
  HAL_GlobalTimerWait(100);

  HAL_GpioByteOutSet(GPIO_BYTE_GEN0, (1 << ( 0 & 0x07)), (1 << ( 0 & 0x07)));
  HAL_GlobalTimerWait(20);

 }

 static void
 frontAfterPowerOn(
 void
 )
 {

 }

 static void
 frontBeforePowerOff(
 void
 )
 {

 }

 static void
 frontAfterPowerOff(
 void
 )
 {

 }

 static void
 frontSnapExposureTimeSet(
 UINT8 mode,
 UINT8 scaleUp
 )
 {
 frontSnapCapabDesc_t *pcap;
 UINT16 ltotal, ftotal, espline;
 UINT32 pixclk, espline_16;
 UINT8 cmdTbl[3*7];
 UINT16 s_gain;

 UINT16 compensation_line = 0;

 CaptureHeaderInformation cap;

 frontBeforeSnapExposureTimeSet();

 pcap = &(frontSnapCapabDesc[frontSnapMode]);

 ltotal = pcap->lineTot;
 ftotal = pcap->frameTot;

 if(mode == CDSP_DO_FRONT){
 if (scaleUp==1){
 pixclk = pcap->pclkFreqBurstUp;
 }
 else{
 pixclk = pcap->pclkFreqBurst;
 }
 }
 else{
 pixclk = pcap->pclkFreq;
 }

 espline = (UINT32)(pixclk * 10 * (1.6) ) / (UINT32)(ltotal * frontSnapLastShutter); 

 s_gain = frontSnapLastGain;

 if (espline != 0) {
 if(espline > (ftotal - 6)) {

 compensation_line = espline - ftotal + 6; 

 }
 }
 else {
 espline = 1;
 }

 espline_16 = (UINT32)espline << 4;
 if (flash_led_snap_adj_exp) {
 espline_16 <<= 1;
 }

 cmdTbl[0] = 0x35;
 cmdTbl[1] = 0x0c;
 cmdTbl[2] = 0x00;  

 cmdTbl[3] = 0x35;
 cmdTbl[4] = 0x0d;
 cmdTbl[5] = 0x00;  

 cmdTbl[6] = 0x35;
 cmdTbl[7] = 0x00;
 cmdTbl[8] = (UINT8)((espline_16 & 0x000F0000) >> 16);

 cmdTbl[9]  = 0x35;
 cmdTbl[10] = 0x01;
 cmdTbl[11] = (UINT8)((espline_16 & 0x0000FF00) >> 8);

 cmdTbl[12] = 0x35;
 cmdTbl[13] = 0x02;
 cmdTbl[14] = (UINT8)(espline_16 & 0x000000FF);

 printf("pre-s_gain=%d\n",s_gain);

 cmdTbl[15]  = 0x35;
 cmdTbl[16] = 0x0B;
 cmdTbl[17] = (UINT8)(s_gain & 0x00FF);

 cmdTbl[18]  = 0x35;
 cmdTbl[19] = 0x0a;
 cmdTbl[20] = (UINT8)((s_gain & 0x0300) >> 8);

  HAL_I2cCmdTableSend(cmdTbl, 2, sizeof(cmdTbl)/3, I2C_TRANS_MODE_NORMAL, 0, 0);

 cap.snapPclk = pixclk;
 cap.espline = espline;
 cap.compensationLine = compensation_line;
 cap.lineTot = ltotal;
 cap.frameTot = ftotal;
 cap.gain = s_gain;
 aeCaptureHeaderAeInformationSet(HEADER_INFORMATION_CAPTURE_CONFIG,&cap);

 }

 static void
 frontBeforeSnapExposureTimeSet(
 void
 )
 {
 frontPrevCapabDesc_t *pcap;
 UINT8 compensationGainIdx = 0, lastGainIdx, pvGainIdxVal;
 UINT8 isoCfg, isoValue, isoGainIdx, autoGainIdx, adjustGainIdx, pvBanding, capBanding, pwrFreq, limitShutterFps;
 UINT16 maxGainTblIdx;
 UINT16 isoNumerator = 0, autoNumerator = 0;
 UINT16 isoDenominator = 0, autoDenominator = 0;
 UINT32 isoShutter, limitShutter, bandingShutter, adjustShutter;

 UINT8 minIdx, maxIdx;

 pcap = &(frontPrevCapabDesc[frontPrevMode]);

 maxGainTblIdx = (UINT16)((sizeof(gainTbl)/sizeof(UINT16))-1);

 aeStatusGet(SP1K_AE_CAPTURE_ISO_MODE,&isoCfg);

  aeStatusGet(SP1K_AE_G_GAIN, &pvGainIdxVal);

  aeStatusGet(SP1K_AE_G_CaptureCompensationGAIN, &compensationGainIdx);

  aeStatusGet(SP1K_AE_G_AETblSel, &pwrFreq);

  aeStatusGet(SP1K_AE_STS_CAP_ANTISHAKE_FPS, &limitShutterFps);

 autoGainIdx = pvGainIdxVal + compensationGainIdx;

 switch (isoCfg) {
 case SP1K_AE_ISO_AUTO:			
 if (autoGainIdx > maxGainTblIdx) {
 autoGainIdx = maxGainTblIdx;
 }
 break;
 case SP1K_AE_ISO_100:

 isoGainIdx = 0;
  do {	if (!(!(isoGainIdx > maxGainTblIdx))) {	__ASSERT(__FILE_ID_SENSOR_DRIVER__, (unsigned short)3961, (1));	}	} while (0);
 break;
 case SP1K_AE_ISO_200:

 isoGainIdx = 16;
  do {	if (!(!(isoGainIdx > maxGainTblIdx))) {	__ASSERT(__FILE_ID_SENSOR_DRIVER__, (unsigned short)3966, (1));	}	} while (0);
 break;
 case SP1K_AE_ISO_400:

 isoGainIdx = 32;
  do {	if (!(!(isoGainIdx > maxGainTblIdx))) {	__ASSERT(__FILE_ID_SENSOR_DRIVER__, (unsigned short)3971, (1));	}	} while (0);
 break;
 case SP1K_AE_ISO_800:

 isoGainIdx = 48;
  do {	if (!(!(isoGainIdx > maxGainTblIdx))) {	__ASSERT(__FILE_ID_SENSOR_DRIVER__, (unsigned short)3976, (1));	}	} while (0);
 break;
 default:

  do {	if (!(0)) {	__ASSERT(__FILE_ID_SENSOR_DRIVER__, (unsigned short)3980, (1));	}	} while (0);
 break;
 }

 if (isoCfg != SP1K_AE_ISO_AUTO) {

 isoValue = isoCfg;

 frontGainTblValueTransfer(pcap->gainTbl[autoGainIdx],&autoNumerator,&autoDenominator);

 frontGainTblValueTransfer(pcap->gainTbl[isoGainIdx],&isoNumerator,&isoDenominator);

 isoShutter = (UINT32)isoNumerator * frontLastShutter * autoDenominator / (isoDenominator * autoNumerator);

 if (limitShutterFps != 0) {

 limitShutter = (UINT32)((UINT32)limitShutterFps * 10 * (1.6) );
 if (isoShutter < limitShutter) {
 isoShutter = limitShutter;

 }
 }

 bandingShutter = (UINT32)10*pwrFreq*2;

 pvBanding = ((frontLastShutter > bandingShutter) ? 1 : 0);

 capBanding = ((isoShutter > bandingShutter*(1.6) ) ? 1 : 0);

 if (pvBanding == 0 && capBanding == 1) {
 lastGainIdx = autoGainIdx;
 frontSnapLastGain = pcap->gainTbl[lastGainIdx];
 frontSnapLastShutter = frontLastShutter;
 }
 else {
 lastGainIdx = isoGainIdx;
 frontSnapLastGain = pcap->gainTbl[lastGainIdx];
 frontSnapLastShutter = isoShutter;
 }

 }
 else {

 lastGainIdx = autoGainIdx;

 if ( lastGainIdx > 32 ) {
 isoValue = SP1K_AE_ISO_800;
 }
 else if ( lastGainIdx > 16 ) {
 isoValue = SP1K_AE_ISO_400;
 }
 else if ( lastGainIdx > 0 ) {
 isoValue = SP1K_AE_ISO_200;
 }
 else {
 isoValue = SP1K_AE_ISO_100;
 }

 if (limitShutterFps != 0) {

 limitShutter = (UINT32)((UINT32)limitShutterFps * 10 * (1.6) );

 adjustShutter = frontLastShutter;

 while ( adjustShutter < limitShutter ) {

 if (isoValue != (SP1K_AE_ISO_MAX - 1)) {

 if ( isoValue < (SP1K_AE_ISO_MAX - 1) ) {
 isoValue++;
 adjustShutter = (adjustShutter << 1);

 }

 switch (isoValue) {
 case SP1K_AE_ISO_800:
 adjustGainIdx = lastGainIdx + (48 - 32);
 break;
 case SP1K_AE_ISO_400:
 adjustGainIdx = lastGainIdx + (32 - 16);
 break;
 case SP1K_AE_ISO_200:
 adjustGainIdx = lastGainIdx + (16 - 0);
 break;
 case SP1K_AE_ISO_100:
 adjustGainIdx = lastGainIdx;
 break;
 }
 lastGainIdx = adjustGainIdx;
 }
 else {
 adjustShutter = limitShutter;
 break;
 }
 }

 frontSnapLastGain = pcap->gainTbl[lastGainIdx];			
 frontSnapLastShutter = adjustShutter;

 }
 else {
 frontSnapLastGain = pcap->gainTbl[lastGainIdx];
 frontSnapLastShutter = frontLastShutter;
 }
 }

  aeParamSet(SP1K_AE_CAPTURE_ISO_VALUE, isoValue);

 if (pvGainIdxVal == lastGainIdx) {

 LoadBackfrontLastSetting = 1;

 if (pwrFreq == SP1K_FLICKER_60) {
 minIdx = pcap->aeMinIdxTbl[0];
 maxIdx = pcap->aeMaxIdxTbl[0];
 }
 else {
 minIdx = pcap->aeMinIdxTbl[1];
 maxIdx = pcap->aeMaxIdxTbl[1];
 }

 if (pvGainIdxVal == minIdx) {
 LoadBackfrontLastGainIdx = pvGainIdxVal + 1;
 }
 else { 
 LoadBackfrontLastGainIdx = pvGainIdxVal - 1;
 }		
 }
 else {
 LoadBackfrontLastGainIdx = pvGainIdxVal;
 }

 }

 static void
 frontGainTblValueTransfer(
 UINT16 value,
 UINT16 *numerator,
 UINT16 *denominator
 )
 {

 *numerator = (16+(value&0x000F));
 *denominator = (((16/((value&0x0010)?2:1))/((value&0x0020)?2:1))/((value&0x0040)?2:1));	

 }

 static void
 frontPreviewModeSwitchPostProc(
 void
 )
 {
 frontPrevCapabDesc_t *pcap;
 UINT8 pvFreq, gIdval;
 SINT16 pv_gId = 0;
 UINT8 minIdx, maxIdx;

 pcap = &(frontPrevCapabDesc[frontPrevMode]);

 frontPrevSensorZoomStatus = 0;
 frontPrevSensorZoom2xGainEn = 0;

 if (frontPowerOnStatus == 1) {

 if (frontPrevModeSave != frontPrevMode) { 
  aeStatusGet(SP1K_AE_G_AETblSel,&pvFreq);
  aeStatusGet(SP1K_AE_gId, &gIdval);
 pv_gId = gIdval;

 if (frontPrevModeSave == 2 && frontPrevMode == 1) {

 }
 else if (frontPrevModeSave == 2 && frontPrevMode == 0) {

 }
 else if (frontPrevModeSave == 1 && frontPrevMode == 0) {
 pv_gId += 20;

 }
 else if (frontPrevModeSave == 1 && frontPrevMode == 2) {

 }
 else if (frontPrevModeSave == 0 && frontPrevMode == 1) {
 pv_gId -= 20;

 }
 else if (frontPrevModeSave == 0 && frontPrevMode == 2) {

 }

 if (pvFreq == SP1K_FLICKER_60) {
 minIdx = pcap->aeMinIdxTbl[0];
 maxIdx = pcap->aeMaxIdxTbl[0];
 }
 else {
 minIdx = pcap->aeMinIdxTbl[1];
 maxIdx = pcap->aeMaxIdxTbl[1];
 }

 if(pv_gId < minIdx) {
 pv_gId = minIdx;

 }

 if(pv_gId > maxIdx) {
 pv_gId = maxIdx;

 }

  aeParamSet(SP1K_AE_gId,(UINT8)pv_gId);

 if (pvFreq == SP1K_FLICKER_60) {
 frontExposureTimeSet(0,pcap->ae60hzTbl[pv_gId].shutter,1,0);
 frontGainSet(0,pcap->ae60hzTbl[pv_gId].gain,1,0);
 }
 else { 
 frontExposureTimeSet(0,pcap->ae50hzTbl[pv_gId].shutter,1,0);
 frontGainSet(0,pcap->ae50hzTbl[pv_gId].gain,1,0);
 }

 frontPrevModeSave = frontPrevMode;
 }
 else {

 if (LoadBackfrontLastSetting == 1) {
 frontExposureTimeSet(0,frontLastShutter,1,0);
  HAL_FrontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
 frontGainSet(0,LoadBackfrontLastGainIdx,1,0);
  HAL_FrontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
 frontGainSet(0,frontLastGainIdx,1,0);

 LoadBackfrontLastSetting = 0;
 }
 else {

 frontExposureTimeSet(0,frontLastShutter,1,0);
  HAL_FrontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);
 frontGainSet(0,frontLastGainIdx,1,0);
  HAL_FrontSignalWait(FRONT_WAIT_VSYNC, FRONT_WAIT_FALLING, 1);

 }
 }
 }
 else {
 frontPrevModeSave = frontPrevMode;
 frontPowerOnStatus = 1;
 }
 }

 static void
 frontResCmdBinSend(
 UINT16 fileId
 )
 {

 UINT32 size;
 UINT8 sts;

  sp1kDiskRsvSizeGet(fileId, ((void *) 0), &size);  
 if (size > (12UL * 1024UL)) {
  do {	if (!(!(size > (12UL * 1024UL)))) {	__ASSERT(__FILE_ID_SENSOR_DRIVER__, (unsigned short)4321, (0));	}	} while (0); 
 return;
 }

 do {
 sts = sp1kDiskRsvRead(fileId, ((sysMemSizeGet(MEM_IDX_TEXT)+(((UINT16)G_FPGAMemPool - 0x4000)>>1))<<1));
 }while(!sts);

 sts = HAL_I2cCmdTableSend(G_FPGAMemPool, 2, size/3, I2C_TRANS_MODE_NORMAL, 0, 0);

 if (sts == 1) {
 printf("Sensor command table (0x%x)\n",fileId);
  do {	if (!(0)) {	__ASSERT(__FILE_ID_SENSOR_DRIVER__, (unsigned short)4339, (1));	}	} while (0);
 }

 }

 static void
 frontResTblBinRead(
 UINT16 fileId,
 UINT16 byteAddress
 )
 {

 UINT32 size;
 UINT8 sts;

 if (byteAddress & 0x0001) {
  do {	if (!(!(byteAddress & 0x0001))) {	__ASSERT(__FILE_ID_SENSOR_DRIVER__, (unsigned short)4426, (0));	}	} while (0); 
 return;
 }

  sp1kDiskRsvSizeGet(fileId, ((void *) 0), &size);  
 if (size > (4UL * 1024UL)) {
  do {	if (!(!(size > (4UL * 1024UL)))) {	__ASSERT(__FILE_ID_SENSOR_DRIVER__, (unsigned short)4433, (0));	}	} while (0); 
 return;
 }

 do {
 sts = sp1kDiskRsvRead(fileId, ((sysMemSizeGet(MEM_IDX_TEXT) + ((byteAddress - 0x4000) >> 1)) << 1));
 }while(!sts);

 }
