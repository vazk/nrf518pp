#include "wrappers.h"

uint32_t nrfpp_sd_ble_gap_appearance_set(uint16_t appearance)
{
    return sd_ble_gap_appearance_set(appearance);
}

uint32_t nrfpp_sd_ble_gap_device_name_set(ble_gap_conn_sec_mode_t const* const write_perm,
                                         uint8_t const                * const dev_name,
                                         uint16_t                              len)
{
    return sd_ble_gap_device_name_set(write_perm, dev_name, len);
}

uint32_t nrfpp_sd_ble_gap_ppcp_set(ble_gap_conn_params_t const* const conn_params)
{
    return sd_ble_gap_ppcp_set(conn_params);
}


uint32_t nrfpp_sd_ble_gap_adv_start(ble_gap_adv_params_t const* const adv_params)
{
    return sd_ble_gap_adv_start(adv_params);
}

uint32_t nrfpp_sd_ble_gap_sec_params_reply(uint16_t                          conn_handle,
                                          uint8_t                           sec_status,
                                          ble_gap_sec_params_t const* const sec_params)
{
    return sd_ble_gap_sec_params_reply(conn_handle, sec_status, sec_params);
}

uint32_t nrfpp_sd_ble_gatts_sys_attr_set(uint16_t             conn_handle,
                                        uint8_t const* const sys_attr_data,
                                        uint16_t             len)
{
    return sd_ble_gatts_sys_attr_set(conn_handle, sys_attr_data, len);
}

uint32_t nrfpp_sd_ble_gap_sec_info_reply(uint16_t                         conn_handle,
                                        ble_gap_enc_info_t  const* const enc_info,
                                        ble_gap_sign_info_t const* const sign_info)
{
    return sd_ble_gap_sec_info_reply(conn_handle, enc_info, sign_info);
}

uint32_t nrfpp_sd_app_evt_wait(void)
{
    return sd_app_evt_wait();
}

uint32_t nrfpp_sd_ble_gatts_service_add(uint8_t                 type,
                                       ble_uuid_t const* const uuid,
                                       uint16_t* const         handle)
{
    return sd_ble_gatts_service_add(type, uuid, handle);
}

uint32_t nrfpp_sd_ble_gatts_characteristic_add(uint16_t                   service_handle,
                                              ble_gatts_char_md_t const* const char_md,
                                              ble_gatts_attr_t const*    const attr_char_value,
                                              ble_gatts_char_handles_t*  const handles)
{
    return sd_ble_gatts_characteristic_add(service_handle, char_md, attr_char_value, handles);
}

uint32_t nrfpp_sd_ble_uuid_vs_add(ble_uuid128_t const* vs_uuid, uint8_t* uuid_type)
{
    return sd_ble_uuid_vs_add(vs_uuid, uuid_type);
}

uint32_t nrfpp_sd_nvic_SetPriority(IRQn_Type IRQn, nrf_app_irq_priority_t priority)
{
    return sd_nvic_SetPriority(IRQn, priority);
}

uint32_t nrfpp_sd_nvic_EnableIRQ(IRQn_Type IRQn)
{
    return sd_nvic_EnableIRQ(IRQn);
}

uint32_t nrfpp_sd_ppi_channel_assign(uint8_t channel_num, 
                                     const volatile void* evt_endpoint, 
                                     const volatile void* task_endpoint)
{
    return sd_ppi_channel_assign(channel_num, 
                                 evt_endpoint, 
                                 task_endpoint);
}

uint32_t nrfpp_sd_ppi_channel_enable_set(uint32_t channel_enable_set_msk)
{
    return sd_ppi_channel_enable_set(channel_enable_set_msk);
}
