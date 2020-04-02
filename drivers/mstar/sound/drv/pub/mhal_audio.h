#ifndef __MHAL_AUDIO_H__

#define __MHAL_AUDIO_H__
#include "mhal_audio_datatype.h"

/**
*  @file mhal_audio.h
*  @brief audio driver APIs
*/

/**
* \brief Init audio HW
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_Init(void);

/**
* \brief Config audio I2S output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigI2sOut(MHAL_AUDIO_DEV AoutDevId, MHAL_AUDIO_I2sCfg_t *pstI2sConfig);

/**
* \brief Config audio I2S input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigI2sIn(MHAL_AUDIO_DEV AinDevId, MHAL_AUDIO_I2sCfg_t *pstI2sConfig);

/**
* \brief Config audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigPcmOut(MHAL_AUDIO_DEV AoutDevId, MHAL_AUDIO_PcmCfg_t *pstDmaConfig);

/**
* \brief Config audio PCM input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigPcmIn(MHAL_AUDIO_DEV AinDevId, MHAL_AUDIO_PcmCfg_t *pstDmaConfig);


/**
* \brief Open audio PCM output device, should be called after configPcmOut
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_OpenPcmOut(MHAL_AUDIO_DEV AoutDevId);

/**
* \brief Open audio PCM input device, should be called after configPcmIn
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_OpenPcmIn(MHAL_AUDIO_DEV AinDevId);


/**
* \brief Close audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ClosePcmOut(MHAL_AUDIO_DEV AoutDevId);

/**
* \brief Close audio PCM input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ClosePcmIn(MHAL_AUDIO_DEV AinDevId);

/**
* \brief Start audio PCM output device, start playback
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StartPcmOut(MHAL_AUDIO_DEV AoutDevId);

/**
* \brief Start audio PCM input device, start recording
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StartPcmIn(MHAL_AUDIO_DEV AinDevId);

/**
* \brief Stop audio PCM output device, stop playback
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StopPcmOut(MHAL_AUDIO_DEV AoutDevId);

/**
* \brief Stop audio PCM input device, stop recording
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StopPcmIn(MHAL_AUDIO_DEV AinDevId);

/**
* \brief Pause audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_PausePcmOut(MHAL_AUDIO_DEV AoutDevId);


/**
* \brief Resume audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ResumePcmOut(MHAL_AUDIO_DEV AoutDevId);


/**
* \brief Read audio raw data from PCM input device(period unit)
* \return value>0 => read data bytes, value<0 => error number
*/
MS_S32 MHAL_AUDIO_ReadDataIn(MHAL_AUDIO_DEV AinDevId, VOID *pRdBuffer, MS_U32 u32Size, MS_BOOL bBlock);

/**
* \brief Write audio raw data to PCM output device(period unit)
* \return value>0 => write data bytes, value<0 => error number
*/
MS_S32 MHAL_AUDIO_WriteDataOut(MHAL_AUDIO_DEV AoutDevId, VOID *pWrBuffer, MS_U32 u32Size, MS_BOOL bBlock);

/**
* \brief Get PCM device status(empty or not, if empty, reset status automatically)
* \return 1=> xun , 0=>normal
*/
MS_BOOL MHAL_AUDIO_IsPcmOutXrun(MHAL_AUDIO_DEV AoutDevId);

/**
* \brief Get PCM device status(full or not, if full, reset status automatically)
* \return 1=> xun , 0=>normal
*/
MS_BOOL MHAL_AUDIO_IsPcmInXrun(MHAL_AUDIO_DEV AinDevId);

/**
* \brief Set output path gain
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_SetGainOut(MHAL_AUDIO_DEV AoutDevId, MS_S16 s16Gain);



#endif //__MHAL_AUDIO_H__