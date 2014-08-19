#include "ble_peripheral_device.hpp"
#include "ble_service.hpp"
#include "ble_advertising_data.hpp"
#include "ble_app_timer.hpp"

extern "C" {
#include "ble_types.h"
#include "ble_srv_common.h"
#include "ble_conn_params.h"
#include "ble_advdata.h"
#include "ble_gap.h"
#include "wrappers.h"
}

#include <vector>
#include <assert.h>

void 
app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    // do whatever
    assert(0);
    while(true);
}

void 
assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    // do whatever...
    assert(0);
    while(true);
}

void 
conn_params_error_handler(uint32_t nrf_error)
{
    assert(0);
    // do whatever...
    while(true);
}






namespace nrfpp {

// initialization of static members...
bool BLEPeripheralDevice::good_ = false;
BLEPeripheralDevice::Params                BLEPeripheralDevice::dp_;
BLEPeripheralDevice::SecurityParams        BLEPeripheralDevice::sp_;
BLEPeripheralDevice::ConnectionParams      BLEPeripheralDevice::cp_;
BLEPeripheralDevice::ServiceArray          BLEPeripheralDevice::services_;
BLEPeripheralDevice::AdvertisingDataArray  BLEPeripheralDevice::advertising_data_;
ble_gap_sec_params_t   BLEPeripheralDevice::sec_params_;
ble_gap_adv_params_t   BLEPeripheralDevice::adv_params_;

void
BLEPeripheralDevice::init(const BLEPeripheralDevice::Params& dp,
                          const BLEPeripheralDevice::SecurityParams& sp,
                          const BLEPeripheralDevice::ConnectionParams& cp)
{
    dp_ = dp;
    sp_ = sp;
    cp_ = cp;

    // start BLE stack
    start();
    if(!good_) return;

    // initialize GAP params
    init_gap();
    if(!good_) return;
}

void 
BLEPeripheralDevice::start()
{
    SOFTDEVICE_HANDLER_INIT(dp_.cconf, false);
    uint32_t result = softdevice_ble_evt_handler_set(peripheraldevice_event_dispatcher);
    good_ = (result == NRF_SUCCESS);

    if(!good_) return;

    result = softdevice_sys_evt_handler_set(system_event_dispatcher);
    good_ = (result == NRF_SUCCESS);
}

void BLEPeripheralDevice::stop()
{
    uint32_t result = softdevice_handler_sd_disable();
    good_ = (result == NRF_SUCCESS);
}

void 
BLEPeripheralDevice::idle()
{
    uint32_t result = nrfpp_sd_app_evt_wait();
    good_ = (result == NRF_SUCCESS);
}

void 
BLEPeripheralDevice::peripheraldevice_event_dispatcher(ble_evt_t* evt)
{
    // standard event handler for system/connectivity events
    ble_conn_params_on_ble_evt(evt);
    // application's ble-stack evend handler
    uint32_t result;
    static uint16_t s_conn_handle = BLE_CONN_HANDLE_INVALID;
    static ble_gap_evt_auth_status_t s_auth_status;
    ble_gap_enc_info_t* enc_info;

    switch (evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            s_conn_handle = evt->evt.gap_evt.conn_handle;
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            s_conn_handle = BLE_CONN_HANDLE_INVALID;
            start_advertising();
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            result = nrfpp_sd_ble_gap_sec_params_reply(s_conn_handle, 
                                                      BLE_GAP_SEC_STATUS_SUCCESS, 
                                                      &sec_params_);
            good_ = (result == NRF_SUCCESS);
            break;
            
        case BLE_GAP_EVT_AUTH_STATUS:
            s_auth_status = evt->evt.gap_evt.params.auth_status;
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            result = nrfpp_sd_ble_gatts_sys_attr_set(s_conn_handle, NULL, 0);
            good_ = (result == NRF_SUCCESS);
            break;

        case BLE_GAP_EVT_SEC_INFO_REQUEST:
            enc_info = &s_auth_status.periph_keys.enc_info;
            if(enc_info->div == evt->evt.gap_evt.params.sec_info_request.div) {
                result = nrfpp_sd_ble_gap_sec_info_reply(s_conn_handle, enc_info, NULL);
                good_ = (result == NRF_SUCCESS);
            } else {
                // No keys found for this device.
                result = nrfpp_sd_ble_gap_sec_info_reply(s_conn_handle, NULL, NULL);
                good_ = (result == NRF_SUCCESS);
            }
            break;

        case BLE_GAP_EVT_TIMEOUT:
            if (evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISEMENT) {
                // vazk
                /*
                nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);
                result = app_button_disable();
                APP_ERROR_CHECK(result);
                
                if (result == NRF_SUCCESS)
                {
                    // Configure buttons with sense level low as wakeup source.
                    nrf_gpio_cfg_sense_input(BLE_BUTTON_PIN_NO,
                                             BUTTON_PULL,
                                             NRF_GPIO_PIN_SENSE_LOW);
                
                    nrf_gpio_cfg_sense_input(GZLL_BUTTON_PIN_NO,
                                             BUTTON_PULL,
                                             NRF_GPIO_PIN_SENSE_LOW);

                    // Go to system-off mode.
                    // (this function will not return; wakeup will cause a reset)
                    result = sd_power_system_off();
                    APP_ERROR_CHECK(result);
                }
                */
            }
            break;
            
        default: 
            // No implementation needed.
            break;
    }

    // service event handlers
    std::list<BLEService*>::iterator sit = services_.begin();
    std::list<BLEService*>::iterator esit = services_.end();
    for(; sit != esit; ++sit) {
        (*sit)->on_event(evt);
    }
}

void 
BLEPeripheralDevice::system_event_dispatcher(uint32_t evt)
{
    assert(0);
}

void 
BLEPeripheralDevice::add_service(BLEService* service)
{
    services_.push_back(service);
}

void 
BLEPeripheralDevice::add_advertising_data(BLEAdvertisingData* advdata)
{
    advertising_data_.push_back(advdata);
}

void 
BLEPeripheralDevice::start_advertising()
{
    // initialize advertising parameters (used when starting advertising).
    update_advertising();
    if(!good_) return;

    // initialize connection parameters
    init_connection();
    
    // initialize security parameters
    init_security();
    
    // start advertising
    memset(&adv_params_, 0, sizeof(adv_params_));
    adv_params_.type         = BLE_GAP_ADV_TYPE_ADV_IND;
    adv_params_.p_peer_addr  = NULL; 
    adv_params_.fp           = BLE_GAP_ADV_FP_ANY;
    adv_params_.interval     = dp_.adv_interval_ms/0.625; // to convert to required units
    adv_params_.timeout      = dp_.adv_timeout_s;
    uint32_t result = nrfpp_sd_ble_gap_adv_start(&adv_params_);
    good_ = (result == NRF_SUCCESS);
}

bool 
BLEPeripheralDevice::is_good()
{
    return good_;
}




void 
BLEPeripheralDevice::init_gap()
{
    uint32_t                result;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    result = nrfpp_sd_ble_gap_device_name_set(&sec_mode, 
                                             (const uint8_t*)dp_.name.c_str(), 
                                             strlen(dp_.name.c_str()));
    good_ = (result == NRF_SUCCESS);
    if(!good_) return;

    result = nrfpp_sd_ble_gap_appearance_set(BLE_APPEARANCE_UNKNOWN);
    good_ = (result == NRF_SUCCESS);
    if(!good_) return;
    
    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval   = MSEC_TO_UNITS(dp_.min_conn_interval_ms, UNIT_1_25_MS);
    gap_conn_params.max_conn_interval   = MSEC_TO_UNITS(dp_.max_conn_interval_ms, UNIT_1_25_MS);
    gap_conn_params.slave_latency       = MSEC_TO_UNITS(dp_.slave_latency_ms, UNIT_1_25_MS);
    gap_conn_params.conn_sup_timeout    = MSEC_TO_UNITS(dp_.conn_supervisory_timeout_ms, UNIT_10_MS);

    result = nrfpp_sd_ble_gap_ppcp_set(&gap_conn_params);
    good_ = (result == NRF_SUCCESS);
}

void 
BLEPeripheralDevice::update_advertising()
{

    // initialize advertising
    ble_advdata_t advdata;
    uint8_t flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    
    ble_uuid_t* adv_uuids = new ble_uuid_t[services_.size()];
    //ble_uuid_t adv_uuids[1];
    size_t i = 0;
    ServiceArray::iterator sit = services_.begin();
    ServiceArray::iterator esit = services_.end();
    for(i = 0; sit != esit; ++sit, ++i) {
        adv_uuids[i].uuid = (*sit)->uuid();
        adv_uuids[i].type = BLE_UUID_TYPE_BLE;//(*sit)->uuid_type();
    }

    ble_advdata_service_data_t* service_data = 
                new ble_advdata_service_data_t[advertising_data_.size()];
    AdvertisingDataArray::iterator ait = advertising_data_.begin();
    AdvertisingDataArray::iterator eait = advertising_data_.end();
    for(i = 0; ait != eait; ++ait, ++i) {
        service_data[0].service_uuid = (*ait)->uuid();
        service_data[0].data.size    = (*ait)->size();
        service_data[0].data.p_data  = (*ait)->data();
    }


     
    //ble_uuid_t adv_uuids[] = 
    //{
    //    //{BLE_UUID_BATTERY_SERVICE, BLE_UUID_TYPE_BLE}, 
    //    {0xABCD, BLE_UUID_TYPE_BLE},
    //};

    // build and set advertising data.
    memset(&advdata, 0, sizeof(advdata));
    
    advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance      = true;
    advdata.flags.size              = sizeof(flags);
    advdata.flags.p_data            = &flags;
    advdata.service_data_count      = advertising_data_.size();
    advdata.p_service_data_array    = service_data;
    advdata.uuids_complete.uuid_cnt = services_.size();
    //advdata.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
    advdata.uuids_complete.p_uuids  = adv_uuids;
    
    uint32_t result = ble_advdata_set(&advdata, NULL);
    good_ = (result == NRF_SUCCESS);
    delete[] adv_uuids;
    delete[] service_data;
}

void 
BLEPeripheralDevice::init_connection()
{
    uint32_t result;
    ble_conn_params_init_t connection_params_init;

    BLEApplication& app = BLEApplication::instance();
    assert(app.is_initialized()); 
    
    memset(&connection_params_init, 0, sizeof(connection_params_init));
    connection_params_init.p_conn_params                  = NULL;
    connection_params_init.first_conn_params_update_delay = 
               APP_TIMER_TICKS(cp_.conn_first_params_update_delay_ms, app.config().timer_prescaler);
    connection_params_init.next_conn_params_update_delay  = 
               APP_TIMER_TICKS(cp_.conn_next_params_update_delay_ms, app.config().timer_prescaler);
    connection_params_init.max_conn_params_update_count   = cp_.conn_max_negotiation_attempts;
    connection_params_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    connection_params_init.disconnect_on_fail             = true;
    connection_params_init.evt_handler                    = NULL;
    connection_params_init.error_handler                  = conn_params_error_handler;
    
    result = ble_conn_params_init(&connection_params_init);
    good_ = (result == NRF_SUCCESS);
}


void 
BLEPeripheralDevice::init_security()
{
    memset(&sec_params_, 0, sizeof(sec_params_));
    sec_params_.timeout      = sp_.sec_timeout_s;
    sec_params_.bond         = sp_.sec_bond;
    sec_params_.mitm         = sp_.sec_mitm;
    sec_params_.io_caps      = sp_.sec_io_capabilities;
    sec_params_.oob          = sp_.sec_oob;  
    sec_params_.min_key_size = sp_.sec_min_key_size;
    sec_params_.max_key_size = sp_.sec_max_key_size;
}

}


