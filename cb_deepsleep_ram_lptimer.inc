#if (MTB_HAL_DRIVER_AVAILABLE_LPTIMER)
extern cy_en_syspm_status_t _mtb_hal_syspm_handle_lptimer(cy_stc_syspm_callback_params_t* params,
                                                          cy_en_syspm_callback_mode_t mode);
extern cy_stc_syspm_callback_params_t _mtb_hal_syspm_cb_params_default;

//--------------------------------------------------------------------------------------------------
// _mtb_hal_syspm_ensure_cb_registered_dsram_lptimer
//--------------------------------------------------------------------------------------------------
static inline bool _mtb_hal_syspm_ensure_cb_registered_dsram_lptimer(void)
{
    static cy_stc_syspm_callback_t cb_deepsleep_ram =
    {
        .callback       = _mtb_hal_syspm_handle_lptimer,
        .type           = CY_SYSPM_DEEPSLEEP_RAM,
        // We need to start the LPTimer before transition and stop it after the transition
        .skipMode       = (CY_SYSPM_SKIP_CHECK_READY | CY_SYSPM_SKIP_CHECK_FAIL),
        .callbackParams = &_mtb_hal_syspm_cb_params_default,
        .prevItm        = NULL,
        .nextItm        = NULL,
        /* We want this to be the last to run before DeepSleep entry, so that the LPTimer
         * delay corresponds as closely as possible to the actual time asleep. This also
         * means this will be the first to run on wakeup, which is also what we want
         * so that we can accurately capture the time spent actually asleep for debugging
         */
        .order          = 255u,
    };

    return Cy_SysPm_RegisterCallback(&cb_deepsleep_ram);
}


#endif // (MTB_HAL_DRIVER_AVAILABLE_LPTIMER)