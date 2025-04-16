/***************************************************************************//**
* \file mtb_hal_adc_mxs40epass_v1.h
*
* \brief
* Provides common API declarations of the mxs40epass_v1 driver
*
********************************************************************************
* \copyright
* Copyright 2024 Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.
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

#include "mtb_hal_adc.h"
#include <stdlib.h>
#include "cy_pdl.h"

#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#if defined(CY_IP_MXS40EPASS_ESAR_INSTANCES)

/*******************************************************************************
*                           Defines
*******************************************************************************/

#define _MTB_HAL_SAR_MAX_NUM_CHANNELS           CY_SAR_MAX_NUM_CHANNELS
#define _MTB_HAL_ADC_MIN_ACQUISITION_TIME_NS    300UL
#define _MTB_HAL_ADC_INTERNAL_VREF_MV           1200UL


/*******************************************************************************
*                           Typedefs
*******************************************************************************/

typedef PASS_SAR_Type _MTB_HAL_ADC_SAR_Type;


/*******************************************************************************
*                        Public Function Prototypes
*******************************************************************************/

int32_t _mtb_hal_adc_read(const mtb_hal_adc_channel_t* obj);
void _mtb_hal_adc_update_interrupt_mask(const mtb_hal_adc_t* obj);
uint8_t _mtb_hal_adc_first_enabled(const mtb_hal_adc_t* obj);
uint8_t _mtb_hal_adc_last_enabled(const mtb_hal_adc_t* obj);

/*******************************************************************************
*       Inlined functions
*******************************************************************************/

//--------------------------------------------------------------------------------------------------
// _mtb_hal_adc_setup
//--------------------------------------------------------------------------------------------------
__STATIC_INLINE cy_rslt_t _mtb_hal_adc_setup(mtb_hal_adc_t* obj,
                                             const mtb_hal_adc_configurator_t* config,
                                             mtb_hal_adc_channel_t** channels)
{
    cy_rslt_t status = CY_RSLT_SUCCESS;

    obj->continuous_scanning = config->config->channelConfig[0]->triggerSelection ==
                               CY_SAR2_TRIGGER_CONTINUOUS ? true : false;

    /* Setup channels */
    for (uint32_t channel_idx = 0; channel_idx < config->num_channels; channel_idx++)
    {
        mtb_hal_adc_channel_t* channel = channels[channel_idx];
        memset(channel, 0, sizeof(mtb_hal_adc_channel_t));

        channel->adc = obj;
        channel->channel_idx = channel_idx;
        obj->channel_config[channel_idx] = channel;
    }

    return status;
}


//--------------------------------------------------------------------------------------------------
// _mtb_hal_adc_is_conversion_complete
//--------------------------------------------------------------------------------------------------
__STATIC_INLINE bool _mtb_hal_adc_is_conversion_complete(const mtb_hal_adc_channel_t* obj)
{
    if (obj->adc->continuous_scanning)
    {
        // Can't rely on BUSY status, as it's never cleared in continuous mode
        return (obj->adc->base->RESULT_VALID & (uint32_t)(1u << (obj->channel_idx)));
    }
    return ((obj->adc->base->STATUS & PASS_SAR_STATUS_BUSY_Msk) == 0UL);
}


#define mtb_hal_adc_is_conversion_complete(obj)  _mtb_hal_adc_is_conversion_complete(obj)


//--------------------------------------------------------------------------------------------------
// _mtb_hal_adc_read_latest
//--------------------------------------------------------------------------------------------------
__STATIC_INLINE cy_rslt_t _mtb_hal_adc_read_latest(const mtb_hal_adc_channel_t* obj,
                                                   int32_t* result)
{
    *result = Cy_SAR2_Channel_GetResult(obj->adc->base, obj->channel_idx, NULL);
    return CY_RSLT_SUCCESS;
}


#define mtb_hal_adc_read_latest(obj, result) _mtb_hal_adc_read_latest(obj, result)

//--------------------------------------------------------------------------------------------------
// _mtb_hal_adc_start_convert
//--------------------------------------------------------------------------------------------------
__STATIC_INLINE cy_rslt_t _mtb_hal_adc_start_convert(mtb_hal_adc_t* obj)
{
    /* Continuous vs. single-shot is handled via the trigger configuration */
    Cy_SAR2_Channel_SoftwareTrigger(obj->base, _mtb_hal_adc_first_enabled(obj));
    return CY_RSLT_SUCCESS;
}


#define mtb_hal_adc_start_convert(obj) _mtb_hal_adc_start_convert(obj)

//--------------------------------------------------------------------------------------------------
// _mtb_hal_adc_counts_to_u16
//--------------------------------------------------------------------------------------------------
__STATIC_INLINE uint16_t _mtb_hal_adc_counts_to_u16(const mtb_hal_adc_channel_t* obj,
                                                    int32_t signed_result)
{
    CY_UNUSED_PARAMETER(obj);
    const uint8_t RESULT_SCALING_FACTOR = UINT16_MAX / 0xFFF; // constant 12-bit SAR resolution

    uint16_t unsigned_result = (uint16_t)((uint32_t)(signed_result) & 0xFFFF);
    /* The SAR provides a 12-bit result, but this API is defined to fill a full 16-bit range */
    uint16_t scaled_result = unsigned_result * RESULT_SCALING_FACTOR;
    return scaled_result;
}


//--------------------------------------------------------------------------------------------------
// _mtb_hal_adc_enable
//--------------------------------------------------------------------------------------------------
__STATIC_INLINE void _mtb_hal_adc_enable(mtb_hal_adc_t* obj)
{
    Cy_SAR2_Enable(obj->base);
}


//--------------------------------------------------------------------------------------------------
// _mtb_hal_adc_disable
//--------------------------------------------------------------------------------------------------
__STATIC_INLINE void _mtb_hal_adc_disable(mtb_hal_adc_t* obj)
{
    Cy_SAR2_Disable(obj->base);
}


#endif /* defined(CY_IP_MXS40EPASS_ESAR_INSTANCES) */

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */
