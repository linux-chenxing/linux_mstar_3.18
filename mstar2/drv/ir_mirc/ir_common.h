#ifndef _PROTOCOLS_COMMON_
#define _PROTOCOLS_COMMON_

/**************************** IR Common Definition ********************************/
//IR shotcount Status  enum
typedef enum{
    STATE_INACTIVE = 0,
    STATE_HEADER_SPACE,
    STATE_BIT_DATA,
    STATE_BIT_PULSE,
    STATE_BIT_SPACE,
    STATE_BIT_TRAILER,
}IR_Status_e;

#define XTAL_CLOCK_FREQ             12000000    //12 MHz
#define IR_CKDIV_NUM          ((XTAL_CLOCK_FREQ+500000)/1000000)
#define IR_CLK                (XTAL_CLOCK_FREQ/1000000)
#define irGetMinCnt(time, tolerance)   ((u32)(((double)time*((double)IR_CLK)/(IR_CKDIV_NUM+1))*((double)1-tolerance)))
#define irGetMaxCnt(time, tolerance)   ((u32)(((double)time*((double)IR_CLK)/(IR_CKDIV_NUM+1))*((double)1+tolerance)))


static inline bool eq_margin(unsigned d, unsigned d1, unsigned d2)
{
    return ((d > d1) && (d < d2));
}
#define FALSE           0
#define TRUE            1

/***********************************************************************************/
//&step 1: IR KEYMAPS  Define
#define NAME_KEYMAP_MSTAR_TV                  "ir-mstar-tv"
#define NAME_KEYMAP_TCL_RCA_TV                "ir-tcl-rca-tv"
#define NAME_KEYMAP_TCL_TV                    "ir-tcl-tv"
#define NAME_KEYMAP_HISENSE_TV                "ir-hisense-tv"
#define NAME_KEYMAP_CHANGHONG_TV              "ir-changhong-tv"
#define NAME_KEYMAP_HAIER_TV                  "ir-haier-tv"
#define NAME_KEYMAP_KONKA_TV                  "ir-kanka-tv"
#define NAME_KEYMAP_SKYWORTH_TV               "ir-skyworth-tv"
#define NAME_KEYMAP_P7051_STB                 "ir-p7051-stb"  //panasonic 7051 stb ir keymap
#define NAME_KEYMAP_KATHREIN_TV               "ir-Kathrein-tv"
#define NAME_KEYMAP_RC5_TV                    "ir-rc5-tv"

#define    NUM_KEYMAP_MSTAR_TV                0x807F
#define    NUM_KEYMAP_TCL_TV                  0xF0
#define    NUM_KEYMAP_CHANGHONG_TV            0x40BF
#define    NUM_KEYMAP_HISENSE_TV              0x00BF
#define    NUM_KEYMAP_SKYWORTH_TV             0x0E0E
#define    NUM_KEYMAP_P7051_STB               0x484E5958
#define    NUM_KEYMAP_KATHREIN_TV             0x8046
#define    NUM_KEYMAP_TCL_RCA_TV              0xF000
#define    NUM_KEYMAP_MAX                     0xFFFFFFFF
#define    NUM_KEYMAP_RC5_TV                  0x0008

typedef enum{
    IR_TYPE_FULLDECODE_MODE = 0,   /* NEC full deocoder mode */
    IR_TYPE_RAWDATA_MODE,          /* NEC raw decoder mode*/
    IR_TYPE_HWRC5_MODE,            /* RC5 decoder mode*/
    IR_TYPE_HWRC5X_MODE,           /* RC5_ext decoder mode*/
    IR_TYPE_HWRC6_MODE,            /* RC6_mode0 decoder mode*/
    IR_TYPE_SWDECODE_MODE,         /* SW decoder mode*/
    IR_TYPE_MAX_MODE
}IR_Mode_e;

/***********************************************************************************/
//&step2:  IR  protocols Type enum
typedef enum{
    IR_TYPE_NEC = 0,        /* NEC protocol */
    IR_TYPE_RC5,            /* RC5 protocol*/
    IR_TYPE_RC6,            /* RC6 protocol*/
    IR_TYPE_RCMM,           /* RCMM protocol*/
    IR_TYPE_KONKA,          /* Konka protocol*/
    IR_TYPE_HAIER,          /* Haier protocol*/
    IR_TYPE_RCA,            /*TCL RCA protocol**/
    IR_TYPE_P7051,          /*Panasonic 7051 protocol**/
    IR_TYPE_TOSHIBA,        /*Toshiba protocol*/
    IR_TYPE_RC5X,           /* RC5 ext protocol*/
    IR_TYPE_RC6_MODE0,      /* RC6  mode0 protocol*/
    IR_TYPE_MAX,
}IR_Type_e;


//Description  of IR
typedef struct IR_Profile_s {
    IR_Type_e eIRType;
    u32 u32HeadCode;
    u32 u32IRSpeed;
}IR_Profile_t;

/***********************************************************************************/
//&step3:  Add protocol spec Description
//------------------------- NEC [ Do not modify ]----------------------------//
//Standard NEC Timming Spec
#define NEC_NBITS		32
#define NEC_HEADER_PULSE	9000//us
#define NEC_HEADER_SPACE	4500
#define NEC_REPEAT_SPACE	2250
#define NEC_BIT_PULSE		560
#define NEC_BIT_0_SPACE		560
#define NEC_BIT_1_SPACE		1680

#define NEC_REPEAT_TIMEOUT 140UL   // > nec cycle 110ms

//low/up bound Coefficient   [Calculate The Specific Timming Data According To The Differ Coefficient]
#define NEC_HEADER_PULSE_LWB	irGetMinCnt(NEC_HEADER_PULSE,0.3)   //9000  *(1-0.2) us
#define NEC_HEADER_PULSE_UPB	irGetMaxCnt(NEC_HEADER_PULSE,0.3)   //9000 *(1+0.2) us

#define NEC_HEADER_SPACE_LWB	irGetMinCnt(NEC_HEADER_SPACE,0.25)   //4500 *(1-0.2) us
#define NEC_HEADER_SPACE_UPB	irGetMaxCnt(NEC_HEADER_SPACE,0.25)   //4500 *(1+0.2) us

#define NEC_REPEAT_SPACE_LWB	irGetMinCnt(NEC_REPEAT_SPACE,0.2)   //2250*(1-0.2) us
#define NEC_REPEAT_SPACE_UPB	irGetMaxCnt(NEC_REPEAT_SPACE,0.2)   //2250*(1+0.2) us

#define NEC_BIT_PULSE_LWB		irGetMinCnt(NEC_BIT_PULSE,0.2)      //560*(1-0.2) us
#define NEC_BIT_PULSE_UPB		irGetMaxCnt(NEC_BIT_PULSE,0.2)      //560*(1+0.2) us

#define NEC_BIT_0_SPACE_LWB		irGetMinCnt(NEC_BIT_0_SPACE,0.35)    //560*(1-0.2) us
#define NEC_BIT_0_SPACE_UPB		irGetMaxCnt(NEC_BIT_0_SPACE,0.35)    //560*(1+0.2) us

#define NEC_BIT_1_SPACE_LWB		irGetMinCnt(NEC_BIT_1_SPACE,0.2)    //1680*(1-0.2) us
#define NEC_BIT_1_SPACE_UPB		irGetMaxCnt(NEC_BIT_1_SPACE,0.2)    //1680*(1+0.2) us

typedef struct IR_NEC_Spec_s {
    IR_Status_e eStatus;    //nec decoder status
    u8 u8BitCount;          //nec bitcount
    u8 u8RepeatTimes;       //nec repeat filter times [if u8RepeatTimes == speed , return repeat flag=1 &return ture , otherwise return false]
    u32 u32DataBits;        //nec data record  [ex:power key 0x46=0100,0110]
} IR_NEC_Spec_t;

int nec_decode_init(void);
void nec_decode_exit(void);

//--------------------------- RCA  [ Do not modify ]---------------------------//

#define RCA_NBITS		24
#define RCA_UNIT		500     //us
#define RCA_HEADER_PULSE  4000  //(8  * RCA_UNIT)
#define RCA_HEADER_SPACE  4000  //(8  * RCA_UNIT)
#define RCA_BIT_PULSE	  500	//(1  * RCA_UNIT)
#define RCA_BIT_0_SPACE	  1000  //(2  * RCA_UNIT)
#define RCA_BIT_1_SPACE	  2000  //(4  * RCA_UNIT)

//low/up bound Coefficient   [Calculate The Specific Timming Data According To The Differ Coefficient]
#define RCA_HEADER_PULSE_LWB	irGetMinCnt(RCA_HEADER_PULSE,0.2)
#define RCA_HEADER_PULSE_UPB	irGetMaxCnt(RCA_HEADER_PULSE,0.2)

#define RCA_HEADER_SPACE_LWB	irGetMinCnt(RCA_HEADER_SPACE,0.2)
#define RCA_HEADER_SPACE_UPB	irGetMaxCnt(RCA_HEADER_SPACE,0.2)

#define RCA_BIT_PULSE_LWB		irGetMinCnt(RCA_BIT_PULSE,0.2)
#define RCA_BIT_PULSE_UPB		irGetMaxCnt(RCA_BIT_PULSE,0.2)

#define RCA_BIT_0_SPACE_LWB		irGetMinCnt(RCA_BIT_0_SPACE,0.2)
#define RCA_BIT_0_SPACE_UPB		irGetMaxCnt(RCA_BIT_0_SPACE,0.2)

#define RCA_BIT_1_SPACE_LWB		irGetMinCnt(RCA_BIT_1_SPACE,0.2)
#define RCA_BIT_1_SPACE_UPB		irGetMaxCnt(RCA_BIT_1_SPACE,0.2)

/*add TCL RCA protocol*/
typedef struct IR_RCA_Spec_s{
    IR_Status_e eStatus;
    u8 u8BitCount;
    u8 u8RepeatTimes;
    u32 u32DataBits;
} IR_RCA_Spec_t;
int rca_decode_init(void);
void rca_decode_exit(void);

//--------------------------- Panasonic 7051 [ Do not modify ]---------------------------//

#define P7051_NBITS		        48
#define P7051_HEADER_PULSE	    3640
#define P7051_HEADER_SPACE	    1800
#define P7051_BIT_PULSE		    380
#define P7051_BIT_0_SPACE		380
#define P7051_BIT_1_SPACE		1350

//low/up bound Coefficient   [Calculate The Specific Timming Data According To The Differ Coefficient]
#define P7051_HEADER_PULSE_LWB	irGetMinCnt(P7051_HEADER_PULSE,0.2)
#define P7051_HEADER_PULSE_UPB	irGetMaxCnt(P7051_HEADER_PULSE,0.2)

#define P7051_HEADER_SPACE_LWB	irGetMinCnt(P7051_HEADER_SPACE,0.2)
#define P7051_HEADER_SPACE_UPB	irGetMaxCnt(P7051_HEADER_SPACE,0.2)

#define P7051_BIT_PULSE_LWB		irGetMinCnt(P7051_BIT_PULSE,0.3)
#define P7051_BIT_PULSE_UPB		irGetMaxCnt(P7051_BIT_PULSE,0.3)

#define P7051_BIT_0_SPACE_LWB	irGetMinCnt(P7051_BIT_0_SPACE,0.3)
#define P7051_BIT_0_SPACE_UPB	irGetMaxCnt(P7051_BIT_0_SPACE,0.3)

#define P7051_BIT_1_SPACE_LWB	irGetMinCnt(P7051_BIT_1_SPACE,0.2)
#define P7051_BIT_1_SPACE_UPB	irGetMaxCnt(P7051_BIT_1_SPACE,0.2)

/*add Panasonic 7051 protocol*/
typedef struct IR_P7051_Spec_s{
    IR_Status_e eStatus;
    u8 u8BitCount;
    u8 u8RepeatTimes;
    u64 u64DataBits;
} IR_P7051_Spec_t;

int p7051_decode_init(void);
void  p7051_decode_exit(void);

//--------------------------- RC5  [ Do not modify ]---------------------------//

#define RC5_NBITS		        14
#define RC5_BIT_MIN		  889  //us
#define RC5_BIT_MAX       1778 //us

#define RC5_BIT_MIN_LWB		irGetMinCnt(RC5_BIT_MIN,0.2)  //us
#define RC5_BIT_MIN_UPB		irGetMaxCnt(RC5_BIT_MIN,0.2)  //us

#define RC5_BIT_MAX_LWB       irGetMinCnt(RC5_BIT_MAX,0.2) //us
#define RC5_BIT_MAX_UPB       irGetMaxCnt(RC5_BIT_MAX,0.2) //us


/*add Philips RC5 protocol*/
typedef struct IR_RC5_Spec_s{
    IR_Status_e eStatus;
    u8 u8BitCount;
    u8 u8RepeatTimes;
    u64 u64DataBits;
} IR_RC5_Spec_t;

int rc5_decode_init(void);
void  rc5_decode_exit(void);

//--------------------------- RC6  [ Do not modify ]---------------------------//

#define RC6_MODE0_NBITS		        16
#define RC6_MODE6A_32_NBITS		    32
#define MODE_BITS 8
#define TRAILER_BITS 12

#define RC6_HEADER_PULSE  2666 //us
#define RC6_HEADER_SPACE  889  //us
#define RC6_BIT_MIN		  444  //us
#define RC6_BIT_MAX       889  //us
#define RC6_TRAILER_MIN   889  //us
#define RC6_TRAILER_MAX   1333 //us

#define RC6_HEADER_PULSE_LWB	irGetMinCnt(RC6_HEADER_PULSE,0.2)
#define RC6_HEADER_PULSE_UPB	irGetMaxCnt(RC6_HEADER_PULSE,0.2)

#define RC6_HEADER_SPACE_LWB	irGetMinCnt(RC6_HEADER_SPACE,0.2)
#define RC6_HEADER_SPACE_UPB	irGetMaxCnt(RC6_HEADER_SPACE,0.2)

#define RC6_BIT_MIN_LWB		    irGetMinCnt(RC6_BIT_MIN,0.3)  //us
#define RC6_BIT_MIN_UPB		    irGetMaxCnt(RC6_BIT_MIN,0.3)  //us

#define RC6_BIT_MAX_LWB         irGetMinCnt(RC6_BIT_MAX,0.25) //us
#define RC6_BIT_MAX_UPB         irGetMaxCnt(RC6_BIT_MAX,0.25) //us

#define RC6_TRAILER_MIN_LOB     irGetMinCnt(RC6_TRAILER_MIN,0.2) //us
#define RC6_TRAILER_MIN_UPB     irGetMaxCnt(RC6_TRAILER_MIN,0.2) //us

#define RC6_TRAILER_MAX_LOB     irGetMinCnt(RC6_TRAILER_MAX,0.2) //us
#define RC6_TRAILER_MAX_UPB     irGetMaxCnt(RC6_TRAILER_MAX,0.2) //us




/*add Philips RC6 protocol*/
typedef struct IR_RC6_Spec_s{
    IR_Status_e eStatus;
    u8 u8DataFlag;
    u8 u8RC6Mode;
    u8 u8Toggle;
    u8 u8BitCount;
    u8 u8RepeatTimes;
    u64 u64DataBits;
} IR_RC6_Spec_t;

int rc6_decode_init(void);
void  rc6_decode_exit(void);
//--------------------------- Toshiba [ Do not modify ]---------------------------//

#define TOSHIBA_NBITS		     32
#define TOSHIBA_HEADER_PULSE	 4500
#define TOSHIBA_HEADER_SPACE	 4500
#define TOSHIBA_BIT_PULSE		 560
#define TOSHIBA_BIT_0_SPACE		 560
#define TOSHIBA_BIT_1_SPACE		 1690

#define TOSHIBA_REPEAT_TIMEOUT 140UL   // > toshiba  cycle 110ms

//low/up bound Coefficient   [Calculate The Specific Timming Data According To The Differ Coefficient]
#define TOSHIBA_HEADER_PULSE_LWB	irGetMinCnt(TOSHIBA_HEADER_PULSE,0.2)
#define TOSHIBA_HEADER_PULSE_UPB	irGetMaxCnt(TOSHIBA_HEADER_PULSE,0.2)

#define TOSHIBA_HEADER_SPACE_LWB	irGetMinCnt(TOSHIBA_HEADER_SPACE,0.2)
#define TOSHIBA_HEADER_SPACE_UPB	irGetMaxCnt(TOSHIBA_HEADER_SPACE,0.2)

#define TOSHIBA_BIT_PULSE_LWB		irGetMinCnt(TOSHIBA_BIT_PULSE,0.3)
#define TOSHIBA_BIT_PULSE_UPB		irGetMaxCnt(TOSHIBA_BIT_PULSE,0.3)

#define TOSHIBA_BIT_0_SPACE_LWB	irGetMinCnt(TOSHIBA_BIT_0_SPACE,0.3)
#define TOSHIBA_BIT_0_SPACE_UPB	irGetMaxCnt(TOSHIBA_BIT_0_SPACE,0.3)

#define TOSHIBA_BIT_1_SPACE_LWB	irGetMinCnt(TOSHIBA_BIT_1_SPACE,0.2)
#define TOSHIBA_BIT_1_SPACE_UPB	irGetMaxCnt(TOSHIBA_BIT_1_SPACE,0.2)

/*add Panasonic 7051 protocol*/
typedef struct IR_Toshiba_Spec_s{
    IR_Status_e eStatus;    //Toshiba decoder status
    u8 u8BitCount;          //Toshiba bitcount
    u8 u8RepeatTimes;       //Toshiba repeat filter times [if u8RepeatTimes == speed , return repeat flag=1 &return ture , otherwise return false]
    u32 u32DataBits;        //Toshiba data record  [ex:power key 0x46=0100,0110]
} IR_Toshiba_Spec_t;

int toshiba_decode_init(void);
void  toshiba_decode_exit(void);

#endif
