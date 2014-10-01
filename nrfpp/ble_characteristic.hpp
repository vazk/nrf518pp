#ifndef NRFPP_BLE_CHARACTERISTIC_HPP
#define NRFPP_BLE_CHARACTERISTIC_HPP

extern "C" {
#include "ble_gatts.h"
}

namespace nrfpp {

enum BufferLocEN {
    LOC_INVALID = BLE_GATTS_VLOC_INVALID,
    LOC_STACK   = BLE_GATTS_VLOC_STACK,
    LOC_USER    = BLE_GATTS_VLOC_USER
};

enum AuthorizationEN{
    AUTH_NONE = BLE_GATTS_AUTHORIZE_TYPE_INVALID,
    AUTH_READ = BLE_GATTS_AUTHORIZE_TYPE_READ,
    AUTH_WRITE = BLE_GATTS_AUTHORIZE_TYPE_WRITE
};


class BLECharacteristic
{

public:
    BLECharacteristic(uint16_t char_uuid, const char* char_name,
                      bool allow_read, bool auth_read, 
                      bool allow_write, bool auth_write,
                      uint8_t len, BufferLocEN loc);
    bool register_self(uint16_t service_handle,
                       uint16_t uuid, uint8_t uuid_type);

public:
    bool grant_authorization(AuthorizationEN auth, uint16_t conn_handle);

public:
    virtual void on_connect(ble_gap_evt_connected_t* evt);
    virtual void on_disconnect(ble_gap_evt_disconnected_t* evt);
    virtual void on_write(ble_gatts_evt_write_t* evt);
    virtual void on_timeout(ble_gatts_evt_timeout_t* evt);
    virtual void on_authorize_rw_request(ble_gatts_evt_rw_authorize_request_t* evt,
                                         uint16_t conn_handle);

protected:
    ble_uuid_t                ble_uuid_;
    ble_gatts_attr_md_t       attr_md_;
    ble_gatts_char_md_t       char_md_;
    ble_gatts_attr_t          attr_value_;
    ble_gatts_char_handles_t  char_handles_;
};

}

#endif // NRFPP_BLE_CHARACTERISTIC_HPP
