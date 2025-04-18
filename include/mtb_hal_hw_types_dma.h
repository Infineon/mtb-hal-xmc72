/***************************************************************************//**
* \file mtb_hal_hw_types_dma.h
*
*********************************************************************************
* \copyright
* Copyright 2024-2025 Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation
*
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/**
 * \brief
 * Provides implementation specific values for types that are part of the
 * portable HAL DMA API.
 *
 * \addtogroup group_hal_impl_hw_types Specific Hardware Types
 * \{
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mtb_hal_impl_types.h"
#include "mtb_hal_hw_types_dma_dmac.h"
#include "mtb_hal_hw_types_dma_dw.h"

#if defined(_MTB_HAL_DRIVER_AVAILABLE_DMA_DW) || defined(_MTB_HAL_DRIVER_AVAILABLE_DMA_DMAC)
 /**
 * \ingroup group_hal_availability
 * \{
 */

#if !defined(MTB_HAL_DRIVER_AVAILABLE_DMA)
/** Macro specifying whether the DMA driver is available for the current device */
#define MTB_HAL_DRIVER_AVAILABLE_DMA (1u)
#endif // !defined(MTB_HAL_DRIVER_AVAILABLE_DMA)

/** \} group_hal_availability */

#endif

#if defined(MTB_HAL_DRIVER_AVAILABLE_DMA)
/** Implementation specific header for DMA */
#define MTB_HAL_DMA_IMPL_HEADER           "mtb_hal_dma_impl.h"
#
/**
 * @brief Aligns a symbol as required by the DMA hardware
 *
 * If DCACHE is present, it takes precedence. Otherwise,
 * defer to the alignment defined by the DMAC hardware, if any
 */
#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    #define _MTB_HAL_DMA_ALIGN        CY_ALIGN(__SCB_DCACHE_LINE_SIZE)
#elif defined(_MTB_HAL_DMA_ALIGN_DMAC)
    // Macro value not in parentheses because it needs to expand out
    #define _MTB_HAL_DMA_ALIGN _MTB_HAL_DMA_ALIGN_DMAC
#else
    #define _MTB_HAL_DMA_ALIGN
#endif


/** DMA type */
typedef enum
{
    MTB_HAL_DMA_DW = 0,
    MTB_HAL_DMA_DMAC = 1
} mtb_hal_dma_type_t;

/**
 * @brief DMA object
 *
 * Application code should not rely on the specific contents of this struct.
 * They are considered an implementation detail which is subject to change
 * between platforms and/or HAL releases.
 */
typedef struct
{
    union
    {
        #if defined (_MTB_HAL_DRIVER_AVAILABLE_DMA_DW)
        _mtb_hal_dw_base_t* dw_base;
        #endif
        #if defined (_MTB_HAL_DRIVER_AVAILABLE_DMA_DMAC)
        _mtb_hal_dmac_base_t* dmac_base;
        #endif
    } base;
    union
    {
        #if defined (_MTB_HAL_DRIVER_AVAILABLE_DMA_DW)
        _mtb_hal_dw_descriptor_t* dw;
        #endif
        #if defined (_MTB_HAL_DRIVER_AVAILABLE_DMA_DMAC)
        _mtb_hal_dmac_descriptor_t* dmac;
        #endif
    } descriptor;
    mtb_hal_dma_type_t                       dma_type;
    uint32_t                                 channel;
    uint16_t                                 expected_bursts;
    uint32_t                                 direction; /* really a mtb_hal_dma_direction_t */
    uint32_t                                 irq_cause;
    _mtb_hal_event_callback_data_t           callback_data;
} mtb_hal_dma_t;

/**
 * @brief DMA configurator struct
 *
 * This struct allows a configurator to provide block configuration information
 * to the HAL. Because configurator-generated configurations are platform
 * specific, the contents of this struct is subject to change between platforms
 * and/or HAL releases.
 */
typedef struct
{
    mtb_hal_dma_type_t                       dma_type;
    uint32_t                                 channel;
    struct
    {
        union
        {
            #if defined(_MTB_HAL_DRIVER_AVAILABLE_DMA_DW)
            _mtb_hal_dw_base_t const*         dw_base;
            #endif
            #if defined(_MTB_HAL_DRIVER_AVAILABLE_DMA_DMAC)
            _mtb_hal_dmac_base_t const*       dmac_base;
            #endif
        };

        union
        {
            #if defined(_MTB_HAL_DRIVER_AVAILABLE_DMA_DW)
            _mtb_hal_dw_descriptor_t *   dw_descriptor;
            #endif
            #if defined(_MTB_HAL_DRIVER_AVAILABLE_DMA_DMAC)
            _mtb_hal_dmac_descriptor_t * dmac_descriptor;
            #endif
        };
    };
} mtb_hal_dma_configurator_t;

#endif // defined(MTB_HAL_DRIVER_AVAILABLE_DMA)

/** \} group_hal_impl_hw_types */


#if defined(__cplusplus)
}
#endif /* __cplusplus */
