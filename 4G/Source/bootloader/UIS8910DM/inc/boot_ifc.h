/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _DRV_IFC_H_
#define _DRV_IFC_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IFC direction
 */
typedef enum
{
    DRV_IFC_RX, ///< RX, from peripherals to memory
    DRV_IFC_TX  ///< TX, from memory to peripherals
} bootIfcDirection_t;

/**
 * @brief IFC channel data structure
 *
 * *Don't* access the members directly.
 */
typedef struct
{
    //! @cond Doxygen_Suppress
    const void *ifc_desc;
    volatile void *hwp_channel;
    //! @endcond
} bootIfcChannel_t;

/**
 * @brief initialize IFC channel data structure
 *
 * @param ch        IFC channel to be initialized
 * @param name      device name
 * @param dir       IFC direction
 * @return
 *      - true on success
 *      - false if the device name or direction is not supported by IFC
 */
bool bootIfcChannelInit(bootIfcChannel_t *ch, uint32_t name, bootIfcDirection_t dir);

/**
 * @brief request a hardware IFC channel
 *
 * @param ch        IFC channel data structure, must be valid
 * @return
 *      - true if a hardware channel is requested
 *      - false if there are no available hardware channels
 */
bool bootIfcRequestChannel(bootIfcChannel_t *ch);

/**
 * @brief request and wait a hardware IFC channel
 *
 * It can't be called inside ISR.
 *
 * @param ch        IFC channel data structure, must be valid
 */
void bootIfcWaitChannel(bootIfcChannel_t *ch);

/**
 * @brief release the hardware channel
 *
 * @param ch        IFC channel data structure, must be valid
 */
void bootIfcReleaseChannel(bootIfcChannel_t *ch);

/**
 * @brief whether there is requested hardware channel
 *
 * @param ch        IFC channel data structure, must be valid
 * @return
 *      - true if a hardware channel is requested
 *      - false if no hardware channels are requested
 */
bool bootIfcReady(bootIfcChannel_t *ch);

/**
 * @brief flush hardware IFC channel
 *
 * This should be only called for RX channel. At flush, IFC controller
 * will pause to fetch data from hardware module. For example, when
 * \a bootIfcFlush is called for UART RX channel, UART arriving data
 * won't be transfered to memory configured in IFC. Rather, they will
 * be put to UART FIFO.
 *
 * @param ch        IFC channel data structure, must be valid
 */
void bootIfcFlush(bootIfcChannel_t *ch);

/**
 * @brief clear flush state of IFC channel
 *
 * It should be only called on *flushed* IFC channel. After it is called,
 * IFC will continue to fetch data from hardware module.
 *
 * @param ch        IFC channel data structure, must be valid
 */
void bootIfcClearFlush(bootIfcChannel_t *ch);

/**
 * @brief get current transfer count
 *
 * \a TC will be decreased from the initial value. Current transfer count
 * is the remaining byte count for the configured DMA transfer.
 *
 * @param ch        IFC channel data structure, must be valid
 * @return
 *      - \a TC
 */
uint32_t bootIfcGetTC(bootIfcChannel_t *ch);

/**
 * @brief start a transfer
 *
 * Caller should take care cache coherence before.
 *
 * @param ch        IFC channel data structure, must be valid
 * @param address   DMA address
 * @param size      DMA size
 */
void bootIfcStart(bootIfcChannel_t *ch, const void *address, uint32_t size);

/**
 * @brief stop a transfer
 *
 * After stop, *TC* will be set to 0. If the *TC* before stop, it is needed
 * to call \p bootIfcGetTC before stop.
 *
 * @param ch        IFC channel data structure, must be valid
 */
void bootIfcStop(bootIfcChannel_t *ch);

/**
 * @brief extend current transfer
 *
 * Caller should take care cache coherence before.
 *
 * When the next DMA address is adjacent to the previous started DMA end
 * address, \a bootIfcExtend can be called to transfer mode bytes. For RX
 * channel, it means receiving more bytes. For TX channel, it means sending
 * more bytes.
 *
 * It can be called even the DMA is on going.
 *
 * @param ch        IFC channel data structure, must be valid
 * @param size      extend DMA size
 */
void bootIfcExtend(bootIfcChannel_t *ch, uint32_t size);

/**
 * @brief whether the IFC fifo is empty
 *
 * @param ch        IFC channel data structure, must be valid
 * @return
 *      - true if the hardware IFC channel FIFO is empty
 *      - false otherwise
 */
bool bootIfcIsFifoEmpty(bootIfcChannel_t *ch);

/**
 * @brief whether the hardware is running
 *
 * @param ch        IFC channel data structure, must be valid
 * @return
 *      - true if the hardware IFC channel is started, and not finished
 *      - false otherwise
 */
bool bootIfcIsRunning(bootIfcChannel_t *ch);

/**
 * @brief wait IFC channel is done
 *
 * @param ch        IFC channel data structure, must be valid
 */
void bootIfcWaitDone(bootIfcChannel_t *ch);

#ifdef __cplusplus
}
#endif

#endif
