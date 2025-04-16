/***************************************************************************//**
* \file mtb_hal_hw_types_adc_epass.h
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

#pragma once

#include "cy_pdl.h"
#include "mtb_hal_hw_types_clock.h"

#if defined(CY_IP_MXS40EPASS_ESAR)

 /**
 * \ingroup group_hal_availability
 * \{
 */

#if !defined(MTB_HAL_DRIVER_AVAILABLE_ADC)
/** Macro specifying whether the ADC driver is available for the current device */
#define MTB_HAL_DRIVER_AVAILABLE_ADC (1u)
#endif // !defined(MTB_HAL_DRIVER_AVAILABLE_ADC)

/** \} group_hal_availability */

#if !defined(_MTB_HAL_DRIVER_AVAILABLE_ADC_SAR)
#define _MTB_HAL_DRIVER_AVAILABLE_ADC_SAR (1u)
#endif // !defined(_MTB_HAL_DRIVER_AVAILABLE_ADC_SAR)


/* Maximum number of channels across all ADC instances. For XMC7, we have to compute this */
#if (((CY_IP_MXS40EPASS_ESAR_INSTANCES < 2) || \
    (PASS_SAR_SLICE_NR0_SAR_SAR_CHAN_NR > PASS_SAR_SLICE_NR1_SAR_SAR_CHAN_NR)) \
    && ((CY_IP_MXS40EPASS_ESAR_INSTANCES < 3) || \
    (PASS_SAR_SLICE_NR0_SAR_SAR_CHAN_NR > PASS_SAR_SLICE_NR2_SAR_SAR_CHAN_NR)))
    #define CY_SAR_MAX_NUM_CHANNELS (PASS_SAR_SLICE_NR0_SAR_SAR_CHAN_NR)
#elif ((CY_IP_MXS40EPASS_ESAR_INSTANCES < 2) /* If we got to this point, we know neither 0 isn't
                                                    the max */ \
    && (CY_IP_MXS40EPASS_ESAR_INSTANCES < 3 || \
        (PASS_SAR_SLICE_NR1_SAR_SAR_CHAN_NR > PASS_SAR_SLICE_NR2_SAR_SAR_CHAN_NR)))
        #define CY_SAR_MAX_NUM_CHANNELS (PASS_SAR_SLICE_NR1_SAR_SAR_CHAN_NR)
#elif (CY_IP_MXS40EPASS_ESAR_INSTANCES < 4) /* If we got to this point, we know neither 0 nor 1
                                                   is the max */
        #define CY_SAR_MAX_NUM_CHANNELS (PASS_SAR_SLICE_NR2_SAR_SAR_CHAN_NR)
#else // if (((CY_IP_MXS40EPASS_ESAR_INSTANCES < 2) || (PASS_SAR_SLICE_NR0_SAR_SAR_CHAN_NR >
// PASS_SAR_SLICE_NR1_SAR_SAR_CHAN_NR)) && ((CY_IP_MXS40EPASS_ESAR_INSTANCES < 3) ||
// (PASS_SAR_SLICE_NR0_SAR_SAR_CHAN_NR > PASS_SAR_SLICE_NR2_SAR_SAR_CHAN_NR)))
    #error "Unhandled ADC instance count"
#endif // if (((CY_IP_MXS40EPASS_ESAR_INSTANCES < 2) || (PASS_SAR_SLICE_NR0_SAR_SAR_CHAN_NR >
// PASS_SAR_SLICE_NR1_SAR_SAR_CHAN_NR)) && ((CY_IP_MXS40EPASS_ESAR_INSTANCES < 3) ||
// (PASS_SAR_SLICE_NR0_SAR_SAR_CHAN_NR > PASS_SAR_SLICE_NR2_SAR_SAR_CHAN_NR)))

/**
 * @brief ADC object
 *
 * Application code should not rely on the specific contents of this struct.
 * They are considered an implementation detail which is subject to change
 * between platforms and/or HAL releases.
 */
typedef struct
{
    struct _mtb_hal_adc_channel_s*      channel_config[CY_SAR_MAX_NUM_CHANNELS];
    PASS_SAR_Type*                      base;
    const mtb_hal_clock_t*              clock;
    bool                                continuous_scanning;
} mtb_hal_adc_t;


/**
 * @brief ADC configurator struct
 *
 * This struct allows a configurator to provide block configuration information
 * to the HAL. Because configurator-generated configurations are platform
 * specific, the contents of this struct is subject to change between platforms
 * and/or HAL releases.
 */
typedef struct
{
    PASS_SAR_Type*                      base;
    cy_stc_sar2_config_t const*         config;
    const mtb_hal_clock_t*              clock;
    uint8_t                             num_channels;
} mtb_hal_adc_configurator_t;

/**
 * @brief ADC channel object
 *
 * Application code should not rely on the specific contents of this struct.
 * They are considered an implementation detail which is subject to change
 * between platforms and/or HAL releases.
 */
typedef struct _mtb_hal_adc_channel_s   /* Struct given an explicit name to make the forward
                                           declaration above work */
{
    mtb_hal_adc_t*                      adc;
    uint8_t                             channel_idx;
    uint8_t                             channel_msk;
    bool                                avg_enabled;
} mtb_hal_adc_channel_t;

#endif // if defined(CY_IP_MXS40EPASS_ESAR)
