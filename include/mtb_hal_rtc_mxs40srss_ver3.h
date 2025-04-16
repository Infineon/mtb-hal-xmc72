/***************************************************************************//**
* \file mtb_hal_rtc_mxs40srss_ver3.h
*
* \brief
* Provides common API declarations of the mtb_hal_rtc_mxs40srss_ver3 driver
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

#include "cy_pdl.h"


#if defined(__cplusplus)
extern "C" {
#endif /* defined(__cplusplus) */


/*******************************************************************************
*                           Defines
*******************************************************************************/
#define _MTB_HAL_RTC_BREG (BACKUP->BREG[SRSS_BACKUP_NUM_BREG-1])

/*******************************************************************************
*                           Typedefs
*******************************************************************************/


/*******************************************************************************
*                        Public Function Prototypes
*******************************************************************************/

#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */
