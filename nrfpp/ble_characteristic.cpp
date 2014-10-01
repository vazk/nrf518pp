#include "ble_characteristic.hpp"
extern "C" {
#include "wrappers.h"
}
#include <string.h>

namespace nrfpp {

BLECharacteristic::BLECharacteristic(uint16_t char_uuid, const char* char_name,
                                     bool allow_read, bool auth_read, 
                                     bool allow_write, bool auth_write,
                                     uint8_t len, BufferLocEN loc)
{
    memset(&char_md_, 0, sizeof(char_md_));
    // characteristic metadata
    char_md_.char_props.read   = 1;
    char_md_.char_props.write  = 1;
    char_md_.p_char_user_desc  = (uint8_t*)char_name;
    char_md_.char_user_desc_max_size  = 12;
    char_md_.char_user_desc_size  = strlen(char_name);
    char_md_.p_char_pf         = NULL;
    char_md_.p_user_desc_md    = NULL;
    char_md_.p_cccd_md         = NULL;
    char_md_.p_sccd_md         = NULL;
    
    memset(&attr_md_, 0, sizeof(attr_md_));

    if(allow_read) {
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md_.read_perm);
    } else {
        BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md_.read_perm);
    }
    if(allow_write) {
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md_.write_perm);
    } else {
        BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md_.write_perm);
    }
    attr_md_.vlen       = len;
    attr_md_.vloc       = loc;
    attr_md_.rd_auth    = auth_read;
    attr_md_.wr_auth    = auth_write;
    

    ble_uuid_.uuid = char_uuid;

    memset(&attr_value_, 0, sizeof(attr_value_));

    attr_value_.p_uuid       = &ble_uuid_;
    attr_value_.p_attr_md    = &attr_md_;
    attr_value_.init_len     = sizeof(uint8_t);
    attr_value_.init_offs    = 0;
    attr_value_.max_len      = sizeof(uint8_t);
    attr_value_.p_value      = NULL;
}

bool
BLECharacteristic::register_self(uint16_t service_handle, 
                                 uint16_t uuid, uint8_t uuid_type)
{
    ble_uuid_.type = uuid_type;
    uint32_t result = nrfpp_sd_ble_gatts_characteristic_add(service_handle, 
                                                            &char_md_, 
                                                            &attr_value_,
                                                            &char_handles_);
    return (result == NRF_SUCCESS);
}

bool
BLECharacteristic::grant_authorization(nrfpp::AuthorizationEN auth, uint16_t conn_handle)
{

    ble_gatts_rw_authorize_reply_params_t auth_reply;
    auth_reply.type = (uint8_t)auth;
    switch(auth) {
        case nrfpp::AUTH_READ: 
            auth_reply.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;
            break;
        case nrfpp::AUTH_WRITE: 
            auth_reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
            break;
        default: return false; 
    }
    uint32_t result = nrfpp_sd_ble_gatts_rw_authorize_reply(conn_handle, &auth_reply);
    return (result == NRF_SUCCESS);
}

void
BLECharacteristic::on_connect(ble_gap_evt_connected_t* evt)
{
    // evt->data[..]
    // evt->len
}

void
BLECharacteristic::on_disconnect(ble_gap_evt_disconnected_t* evt)
{
    // evt->data[..]
    // evt->len
}


void
BLECharacteristic::on_write(ble_gatts_evt_write_t* evt)
{
    // evt->data[..]
    // evt->len
}

void
BLECharacteristic::on_timeout(ble_gatts_evt_timeout_t* evt)
{
    // evt->data[..]
    // evt->len
}

void
BLECharacteristic::on_authorize_rw_request(ble_gatts_evt_rw_authorize_request_t* evt,
                                           uint16_t conn_handle)
{
    // evt->data[..]
    // evt->len
}



}
