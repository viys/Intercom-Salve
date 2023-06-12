#ifndef KING_ACCESS_NW
#define KING_ACCESS_NW

#include "kingdef.h"

typedef enum{
    ACCESS_NW_TYPE_UNKNOWN,
    ACCESS_NW_TYPE_GERAN,
    ACCESS_NW_TYPE_UTRAN,
    ACCESS_NW_TYPE_EUTRAN,
    ACCESS_NW_TYPE_CDMA2000,
    ACCESS_NW_TYPE_IWLAN,
    ACCESS_NW_TYPE_NGRAN,
    ACCESS_NW_TYPE_MAX = 0x7FFFFFFF
}ACCESS_NW_TYPE;

typedef enum{
    GERAN_BAND_0,
    GERAN_BAND_T380,
    GERAN_BAND_T410,
    GERAN_BAND_450,
    GERAN_BAND_480,
    GERAN_BAND_710,
    GERAN_BAND_750,
    GERAN_BAND_T810,
    GERAN_BAND_850,
    GERAN_BAND_P900,
    GERAN_BAND_E900,
    GERAN_BAND_R900,
    GERAN_BAND_12,
    GERAN_BAND_1800,
    GERAN_BAND_PCS1900,
    GERAN_BAND_ER900,
    GERAN_BAND_MAX = 0x7FFFFFFF
}GERAN_BAND;

typedef enum{
    CDMA_BAND_BC0,
    CDMA_BAND_BC1,
    CDMA_BAND_BC2,
    CDMA_BAND_BC3,
    CDMA_BAND_BC4,
    CDMA_BAND_BC5,
    CDMA_BAND_BC6,
    CDMA_BAND_BC7,
    CDMA_BAND_BC8,
    CDMA_BAND_BC9,
    CDMA_BAND_BC10,
    CDMA_BAND_BC11,
    CDMA_BAND_BC12,
    CDMA_BAND_BC13,
    CDMA_BAND_BC14,
    CDMA_BAND_BC15,
    CDMA_BAND_BC16,
    CDMA_BAND_BC17,
    CDMA_BAND_BC18,
    CDMA_BAND_BC19,
    CDMA_BAND_BC20,
    CDMA_BAND_BC21,
    CDMA_BAND_MAX = 0x7FFFFFFF
}CDMA_BAND;

typedef enum{
    UTRAN_BAND_0,
    UTRAN_BAND_1,
    UTRAN_BAND_2,
    UTRAN_BAND_3,
    UTRAN_BAND_4,
    UTRAN_BAND_5,
    UTRAN_BAND_6,
    UTRAN_BAND_7,
    UTRAN_BAND_8,
    UTRAN_BAND_9,
    UTRAN_BAND_10,
    UTRAN_BAND_11,
    UTRAN_BAND_12,
    UTRAN_BAND_13,
    UTRAN_BAND_14,
    UTRAN_BAND_15,
    UTRAN_BAND_16,
    UTRAN_BAND_17,
    UTRAN_BAND_18,
    UTRAN_BAND_19,
    UTRAN_BAND_20,
    UTRAN_BAND_21,
    UTRAN_BAND_22,
    UTRAN_BAND_23,
    UTRAN_BAND_24,
    UTRAN_BAND_25,
    UTRAN_BAND_26,
    UTRAN_BAND_A = 101,
    UTRAN_BAND_B = 102,
    UTRAN_BAND_C = 103,
    UTRAN_BAND_D = 104,
    UTRAN_BAND_E = 105,
    UTRAN_BAND_F = 106,
    UTRAN_BAND_MAX = 0x7FFFFFFF
}UTRAN_BAND;

typedef enum{
    EUTRAN_BAND_0,
    EUTRAN_BAND_1,
    EUTRAN_BAND_2,
    EUTRAN_BAND_3,
    EUTRAN_BAND_4,
    EUTRAN_BAND_5,
    EUTRAN_BAND_6,
    EUTRAN_BAND_7,
    EUTRAN_BAND_8,
    EUTRAN_BAND_9,
    EUTRAN_BAND_10,
    EUTRAN_BAND_11,
    EUTRAN_BAND_12,
    EUTRAN_BAND_13,
    EUTRAN_BAND_14,
    EUTRAN_BAND_15,
    EUTRAN_BAND_16,
    EUTRAN_BAND_17,
    EUTRAN_BAND_18,
    EUTRAN_BAND_19,
    EUTRAN_BAND_20,
    EUTRAN_BAND_21,
    EUTRAN_BAND_22,
    EUTRAN_BAND_23,
    EUTRAN_BAND_24,
    EUTRAN_BAND_25,
    EUTRAN_BAND_26,
    EUTRAN_BAND_27,
    EUTRAN_BAND_28,
    EUTRAN_BAND_29,
    EUTRAN_BAND_30,
    EUTRAN_BAND_31,
    EUTRAN_BAND_32,
    EUTRAN_BAND_33,
    EUTRAN_BAND_34,
    EUTRAN_BAND_35,
    EUTRAN_BAND_36,
    EUTRAN_BAND_37,
    EUTRAN_BAND_38,
    EUTRAN_BAND_39,
    EUTRAN_BAND_40,
    EUTRAN_BAND_41,
    EUTRAN_BAND_42,
    EUTRAN_BAND_43,
    EUTRAN_BAND_44,
    EUTRAN_BAND_45,
    EUTRAN_BAND_46,
    EUTRAN_BAND_47,
    EUTRAN_BAND_48,
    EUTRAN_BAND_49,
    EUTRAN_BAND_50,
    EUTRAN_BAND_51,
    EUTRAN_BAND_52,
    EUTRAN_BAND_53,
    EUTRAN_BAND_54,
    EUTRAN_BAND_55,
    EUTRAN_BAND_56,
    EUTRAN_BAND_57,
    EUTRAN_BAND_58,
    EUTRAN_BAND_59,
    EUTRAN_BAND_60,
    EUTRAN_BAND_61,
    EUTRAN_BAND_62,
    EUTRAN_BAND_63,
    EUTRAN_BAND_64,
    EUTRAN_BAND_65,
    EUTRAN_BAND_66,
    EUTRAN_BAND_67,
    EUTRAN_BAND_68,
    EUTRAN_BAND_69,
    EUTRAN_BAND_70,
    EUTRAN_BAND_MAX = 0x7FFFFFFF
}EUTRAN_BAND;

typedef enum{
    NGRAN_BAND_0,
    NGRAN_BAND_1,
    NGRAN_BAND_2,
    NGRAN_BAND_3,
    NGRAN_BAND_4,
    NGRAN_BAND_5,
    NGRAN_BAND_6,
    NGRAN_BAND_7,
    NGRAN_BAND_8,
    NGRAN_BAND_9,
    NGRAN_BAND_10,
    NGRAN_BAND_11,
    NGRAN_BAND_12,
    NGRAN_BAND_13,
    NGRAN_BAND_14,
    NGRAN_BAND_15,
    NGRAN_BAND_16,
    NGRAN_BAND_17,
    NGRAN_BAND_18,
    NGRAN_BAND_19,
    NGRAN_BAND_20,
    NGRAN_BAND_21,
    NGRAN_BAND_22,
    NGRAN_BAND_23,
    NGRAN_BAND_24,
    NGRAN_BAND_25,
    NGRAN_BAND_26,
    NGRAN_BAND_27,
    NGRAN_BAND_28,
    NGRAN_BAND_29,
    NGRAN_BAND_30,
    NGRAN_BAND_31,
    NGRAN_BAND_32,
    NGRAN_BAND_33,
    NGRAN_BAND_34,
    NGRAN_BAND_35,
    NGRAN_BAND_36,
    NGRAN_BAND_37,
    NGRAN_BAND_38,
    NGRAN_BAND_39,
    NGRAN_BAND_40,
    NGRAN_BAND_41,
    NGRAN_BAND_42,
    NGRAN_BAND_43,
    NGRAN_BAND_44,
    NGRAN_BAND_45,
    NGRAN_BAND_46,
    NGRAN_BAND_47,
    NGRAN_BAND_48,
    NGRAN_BAND_49,
    NGRAN_BAND_50,
    NGRAN_BAND_51,
    NGRAN_BAND_52,
    NGRAN_BAND_53,
    NGRAN_BAND_54,
    NGRAN_BAND_55,
    NGRAN_BAND_56,
    NGRAN_BAND_57,
    NGRAN_BAND_58,
    NGRAN_BAND_59,
    NGRAN_BAND_60,
    NGRAN_BAND_61,
    NGRAN_BAND_62,
    NGRAN_BAND_63,
    NGRAN_BAND_64,
    NGRAN_BAND_65,
    NGRAN_BAND_66,
    NGRAN_BAND_67,
    NGRAN_BAND_68,
    NGRAN_BAND_69,
    NGRAN_BAND_70,
    NGRAN_BAND_71,
    NGRAN_BAND_72,
    NGRAN_BAND_73,
    NGRAN_BAND_74,
    NGRAN_BAND_75,
    NGRAN_BAND_76,
    NGRAN_BAND_77,
    NGRAN_BAND_78,
    NGRAN_BAND_79,
    NGRAN_BAND_80,
    NGRAN_BAND_81,
    NGRAN_BAND_82,
    NGRAN_BAND_83,
    NGRAN_BAND_84,
    NGRAN_BAND_85,
    NGRAN_BAND_86,
    NGRAN_BAND_87,
    NGRAN_BAND_88,
    NGRAN_BAND_89,
    NGRAN_BAND_90,
    NGRAN_BAND_91,
    NGRAN_BAND_92,
    NGRAN_BAND_93,
    NGRAN_BAND_94,
    NGRAN_BAND_95,
    NGRAN_BAND_96,
    NGRAN_BAND_97,
    NGRAN_BAND_98,
    NGRAN_BAND_99,
    NGRAN_BAND_100,
    NGRAN_BAND_101,
    NGRAN_BAND_102,
    NGRAN_BAND_103,
    NGRAN_BAND_104,
    NGRAN_BAND_105,
    NGRAN_BAND_106,
    NGRAN_BAND_107,
    NGRAN_BAND_108,
    NGRAN_BAND_109,
    NGRAN_BAND_110,
    NGRAN_BAND_111,
    NGRAN_BAND_112,
    NGRAN_BAND_113,
    NGRAN_BAND_114,
    NGRAN_BAND_115,
    NGRAN_BAND_116,
    NGRAN_BAND_117,
    NGRAN_BAND_118,
    NGRAN_BAND_119,
    NGRAN_BAND_120,
    NGRAN_BAND_121,
    NGRAN_BAND_122,
    NGRAN_BAND_123,
    NGRAN_BAND_124,
    NGRAN_BAND_125,
    NGRAN_BAND_126,
    NGRAN_BAND_127,
    NGRAN_BAND_128,
    NGRAN_BAND_129,
    NGRAN_BAND_130,
    NGRAN_BAND_131,
    NGRAN_BAND_132,
    NGRAN_BAND_133,
    NGRAN_BAND_134,
    NGRAN_BAND_135,
    NGRAN_BAND_136,
    NGRAN_BAND_137,
    NGRAN_BAND_138,
    NGRAN_BAND_139,
    NGRAN_BAND_140,
    NGRAN_BAND_141,
    NGRAN_BAND_142,
    NGRAN_BAND_143,
    NGRAN_BAND_144,
    NGRAN_BAND_145,
    NGRAN_BAND_146,
    NGRAN_BAND_147,
    NGRAN_BAND_148,
    NGRAN_BAND_149,
    NGRAN_BAND_150,
    NGRAN_BAND_151,
    NGRAN_BAND_152,
    NGRAN_BAND_153,
    NGRAN_BAND_154,
    NGRAN_BAND_155,
    NGRAN_BAND_156,
    NGRAN_BAND_157,
    NGRAN_BAND_158,
    NGRAN_BAND_159,
    NGRAN_BAND_160,
    NGRAN_BAND_161,
    NGRAN_BAND_162,
    NGRAN_BAND_163,
    NGRAN_BAND_164,
    NGRAN_BAND_165,
    NGRAN_BAND_166,
    NGRAN_BAND_167,
    NGRAN_BAND_168,
    NGRAN_BAND_169,
    NGRAN_BAND_170,
    NGRAN_BAND_171,
    NGRAN_BAND_172,
    NGRAN_BAND_173,
    NGRAN_BAND_174,
    NGRAN_BAND_175,
    NGRAN_BAND_176,
    NGRAN_BAND_177,
    NGRAN_BAND_178,
    NGRAN_BAND_179,
    NGRAN_BAND_180,
    NGRAN_BAND_181,
    NGRAN_BAND_182,
    NGRAN_BAND_183,
    NGRAN_BAND_184,
    NGRAN_BAND_185,
    NGRAN_BAND_186,
    NGRAN_BAND_187,
    NGRAN_BAND_188,
    NGRAN_BAND_189,
    NGRAN_BAND_190,
    NGRAN_BAND_191,
    NGRAN_BAND_192,
    NGRAN_BAND_193,
    NGRAN_BAND_194,
    NGRAN_BAND_195,
    NGRAN_BAND_196,
    NGRAN_BAND_197,
    NGRAN_BAND_198,
    NGRAN_BAND_199,
    NGRAN_BAND_200,
    NGRAN_BAND_201,
    NGRAN_BAND_202,
    NGRAN_BAND_203,
    NGRAN_BAND_204,
    NGRAN_BAND_205,
    NGRAN_BAND_206,
    NGRAN_BAND_207,
    NGRAN_BAND_208,
    NGRAN_BAND_209,
    NGRAN_BAND_210,
    NGRAN_BAND_211,
    NGRAN_BAND_212,
    NGRAN_BAND_213,
    NGRAN_BAND_214,
    NGRAN_BAND_215,
    NGRAN_BAND_216,
    NGRAN_BAND_217,
    NGRAN_BAND_218,
    NGRAN_BAND_219,
    NGRAN_BAND_220,
    NGRAN_BAND_221,
    NGRAN_BAND_222,
    NGRAN_BAND_223,
    NGRAN_BAND_224,
    NGRAN_BAND_225,
    NGRAN_BAND_226,
    NGRAN_BAND_227,
    NGRAN_BAND_228,
    NGRAN_BAND_229,
    NGRAN_BAND_230,
    NGRAN_BAND_231,
    NGRAN_BAND_232,
    NGRAN_BAND_233,
    NGRAN_BAND_234,
    NGRAN_BAND_235,
    NGRAN_BAND_236,
    NGRAN_BAND_237,
    NGRAN_BAND_238,
    NGRAN_BAND_239,
    NGRAN_BAND_240,
    NGRAN_BAND_241,
    NGRAN_BAND_242,
    NGRAN_BAND_243,
    NGRAN_BAND_244,
    NGRAN_BAND_245,
    NGRAN_BAND_246,
    NGRAN_BAND_247,
    NGRAN_BAND_248,
    NGRAN_BAND_249,
    NGRAN_BAND_250,
    NGRAN_BAND_251,
    NGRAN_BAND_252,
    NGRAN_BAND_253,
    NGRAN_BAND_254,
    NGRAN_BAND_255,
    NGRAN_BAND_256,
    NGRAN_BAND_257,
    NGRAN_BAND_258,
    NGRAN_BAND_259,
    NGRAN_BAND_260,
    NGRAN_BAND_261,
    NGRAN_BAND_262,
    NGRAN_BAND_263,
    NGRAN_BAND_264,
    NGRAN_BAND_265,
    NGRAN_BAND_MAX = 0x7FFFFFFF
}NGRAN_BAND;

typedef enum{
    NW_PRIORITY_HIGH,
    NW_PRIORITY_MID,
    NW_PRIORITY_LOW,
    NW_PRIORITY_MAX = 0x7FFFFFFF
}NETWORK_PRIORITY;

typedef struct _tagNetworkIdentityGERAN{
    uint16 mcc;
    uint16 mnc;
    uint16 mnc_digit_num;
    uint32 cid;
    uint16 bsic;
    uint16 lac;
    uint16 arfcn;
    GERAN_BAND geran_band;
    int16  Dbm;
    int16  snr;
    uint16  TimingAdvance;
}NETWORK_ID_GERAN;

typedef struct _tagNetworkIdentityCDMA{
    uint16 mcc;
    uint16 mnc;
    uint16 mnc_digit_num;
    int32  Latitude;
    int32  Longitude;
    uint16 BasestationId;
    uint16 NetworkId;
    uint16 SystemId;
    CDMA_BAND cdma_band;
    int16  Dbm;
    int16  CdmaEcio;
    int16  snr;
}NETWORK_ID_CDMA;

typedef struct _tagNetworkIdentityUTRAN{
    uint16 mcc;
    uint16 mnc;
    uint16 mnc_digit_num;
    uint32 cid;
    uint16 lac;
    uint16 uarfcn;
    uint16 Psc;
    UTRAN_BAND utran_band;
    int16  Dbm;
    int16  EcNo;
}NETWORK_ID_UTRAN;

typedef struct _tagNetworkIdentityEUTRAN{
    uint16 mcc;
    uint16 mnc;
    uint16 mnc_digit_num;
    uint32 cid;
    uint16 earfcn;
    uint16 pci;
    uint16 tac;
    uint16 Bandwidth;
    EUTRAN_BAND eutran_band;
    int16  Dbm;
    int16  cqi;
    int16  rsrp;
    int16  rsrq;
    int16  rssi;
    int16  snr;
    uint16 TimingAdvance;
}NETWORK_ID_EUTRA;

typedef struct _tagCellIdentityNR{
    uint16 mcc;
    uint16 mnc;
    uint16 mnc_digit_num;
    uint64 nci;
    uint32 Nrarfcn;
    uint16 pci;
    uint16 Tac;
    NGRAN_BAND  ngran_band;
    int16  Dbm;
    int16  rsrp;
    int16  rsrq;
    int16  rssi;
    int16  snr;
}NETWORK_ID_NR;

typedef struct _tagAvailableNetworkInfo{
    ACCESS_NW_TYPE Type;
    NETWORK_PRIORITY Priority;
    union{
        NETWORK_ID_GERAN geran;
        NETWORK_ID_CDMA  cdma;
        NETWORK_ID_UTRAN utran;
        NETWORK_ID_EUTRA eutran;
        NETWORK_ID_NR    nr;
    };
}NETWORK_INFO;

/**
 * ���ط���С����Ϣ
 *
 * @param[out] nw_info  ����С����Ϣ
 * @return 0 success  -1 fail
 */
int KING_ServedNetworkInfoGet(NETWORK_INFO *nw_info);

/**
 * ������С����Ϣ
 *
 * @param[inout] nw_info  ָ����С����Ϣ��input nw_info->type��С������
 * @param[in] index  ָ����ȡ�ڼ�����С��
 * @return 0 success  -1 fail
 */
int KING_NeighborCellInfoGet(NETWORK_INFO *nw_info, uint32 index);

#endif