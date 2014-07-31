#include <stdint.h>
#include "ble_types.h"
#include "ble_gap.h"
#include "ble_gatts.h"
#include "nrf_soc.h"
#include "ble.h"

uint32_t nrfx_sd_ble_gap_appearance_set(uint16_t appearance);


uint32_t nrfx_sd_ble_gap_device_name_set(ble_gap_conn_sec_mode_t const* const p_write_perm,
                                         uint8_t const                * const p_dev_name,
                                         uint16_t                             len);

uint32_t nrfx_sd_ble_gap_ppcp_set(ble_gap_conn_params_t const* const p_conn_params);

uint32_t nrfx_sd_ble_gap_adv_start(ble_gap_adv_params_t const* const p_adv_params);

uint32_t nrfx_sd_ble_gap_sec_params_reply(uint16_t                          conn_handle,
                                          uint8_t                           sec_status,
                                          ble_gap_sec_params_t const* const p_sec_params);

uint32_t nrfx_sd_ble_gatts_sys_attr_set(uint16_t             conn_handle,
                                        uint8_t const* const p_sys_attr_data,
                                        uint16_t             len);

uint32_t nrfx_sd_ble_gap_sec_info_reply(uint16_t                         conn_handle,
                                        ble_gap_enc_info_t  const* const p_enc_info,
                                        ble_gap_sign_info_t const* const p_sign_info);

uint32_t nrfx_sd_app_evt_wait(void);

uint32_t nrfx_sd_ble_gatts_service_add(uint8_t                 type,
                                       ble_uuid_t const* const uuid,
                                       uint16_t*         const handle);

uint32_t nrfx_sd_ble_gatts_characteristic_add(uint16_t                   service_handle,
                                              ble_gatts_char_md_t const* const char_md,
                                              ble_gatts_attr_t const*    const attr_char_value,
                                              ble_gatts_char_handles_t*  const handles);

uint32_t nrfx_sd_ble_uuid_vs_add(ble_uuid128_t const* vs_uuid, uint8_t* uuid_type);
