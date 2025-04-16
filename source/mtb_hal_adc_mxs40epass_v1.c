/***************************************************************************//**
* \file mtb_hal_adc_mxs40epass_v1.c
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

#include "mtb_hal_adc.h"
#include <stdlib.h>
#include "cy_pdl.h"
#include "mtb_hal_adc_mxs40epass_v1.h"
#include "mtb_hal_system.h"


#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */

#if defined(CY_IP_MXS40EPASS_ESAR_INSTANCES)

/*******************************************************************************
*                           Defines
*******************************************************************************/


/*******************************************************************************
*                           Typedefs
*******************************************************************************/


/*******************************************************************************
*                        Public Functions
*******************************************************************************/

//--------------------------------------------------------------------------------------------------
// _mtb_hal_adc_first_enabled
//--------------------------------------------------------------------------------------------------
uint8_t _mtb_hal_adc_first_enabled(const mtb_hal_adc_t* obj) /* Or first channel, if no channel
                                                                    is enabled */
{
    uint8_t first_enabled = 0u;
    for (uint8_t i = 0; i < _MTB_HAL_SAR_MAX_NUM_CHANNELS; ++i)
    {
        if ((NULL != obj->channel_config[i]) && obj->base->CH[i].ENABLE)
        {
            first_enabled = i;
            break;
        }
    }

    return first_enabled;
}


//--------------------------------------------------------------------------------------------------
// _mtb_hal_adc_last_enabled
//--------------------------------------------------------------------------------------------------
uint8_t _mtb_hal_adc_last_enabled(const mtb_hal_adc_t* obj) /* Or last channel, if no channel is
                                                                   enabled */
{
    uint8_t last_enabled = _MTB_HAL_SAR_MAX_NUM_CHANNELS - 1u;
    /* NOT uint, or the loop will never terminate */
    for (int i = _MTB_HAL_SAR_MAX_NUM_CHANNELS - 1; i >= 0; --i)
    {
        if ((NULL != obj->channel_config[i]) && obj->base->CH[i].ENABLE)
        {
            last_enabled = i;
            break;
        }
    }

    return last_enabled;
}


//--------------------------------------------------------------------------------------------------
// _mtb_hal_adc_update_interrupt_mask
//--------------------------------------------------------------------------------------------------
void _mtb_hal_adc_update_interrupt_mask(const mtb_hal_adc_t* obj)
{
    uint8_t last_channel = _mtb_hal_adc_last_enabled(obj);
    uint32_t current_mask = Cy_SAR2_Channel_GetInterruptMask(obj->base, last_channel);
    uint32_t new_mask;
    new_mask = current_mask & ~CY_SAR2_INT_GRP_DONE;

    Cy_SAR2_Channel_ClearInterrupt(obj->base, last_channel, new_mask & (~current_mask));
    Cy_SAR2_Channel_SetInterruptMask(obj->base, last_channel, new_mask);
}


//--------------------------------------------------------------------------------------------------
// _mtb_hal_adc_read
//--------------------------------------------------------------------------------------------------
int32_t _mtb_hal_adc_read(const mtb_hal_adc_channel_t* obj)
{
    uint16_t timeout = 1000;
    if (!obj->adc->continuous_scanning)
    {
        _mtb_hal_adc_update_interrupt_mask(obj->adc);
        Cy_SAR2_Channel_SoftwareTrigger(obj->adc->base, _mtb_hal_adc_first_enabled(obj->adc));

        /* Wait until transfer has started */
        while (_mtb_hal_adc_is_conversion_complete(obj) && timeout > 0)
        {
            mtb_hal_system_delay_us(1);
            timeout--;
        }
    }

    /* Wait for the busy register bit to clear, or until the results are valid if
     * in continuous mode */
    timeout = 1000;
    while (!_mtb_hal_adc_is_conversion_complete(obj) && timeout > 0)
    {
        mtb_hal_system_delay_us(1);
        timeout--;
    }

    /* Cy_SAR2_Channel_GetResult returns 12-bit unsigned value, which represent ADC count value in
     * range from 0V to 3.3V/5V (depends on VDDA). Casting it to signed 32 bit int as per
     * mtb_hal_adc_read return value description.
     */
    int32_t result = (int32_t)(Cy_SAR2_Channel_GetResult(obj->adc->base, obj->channel_idx,
                                                         NULL /* We don't need the status bits */));

    return result;
}


#endif /* defined(CY_IP_MXS40EPASS_ESAR_INSTANCES) */

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */
