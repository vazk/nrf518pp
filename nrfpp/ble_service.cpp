#include "ble_service.hpp"

namespace nrfpp {

BLEService::BLEService(uint16_t uuid, uint8_t uuid_type)
 : uuid_(uuid),
   uuid_type_(uuid_type),
   good_(false)
{
    uint32_t   result;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    connection_handle_ = BLE_CONN_HANDLE_INVALID;
    
    // Add service
    ble_uuid128_t base_uuid = UUID_BASE;
    result = nrfpp_sd_ble_uuid_vs_add(&base_uuid, &uuid_type_);
    good_ = (result == NRF_SUCCESS);
    if(!good_) return;
    
    ble_uuid.type = uuid_type_;
    ble_uuid.uuid = uuid_;

    result = nrfpp_sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, 
                                           &ble_uuid, &service_handle_);
    good_ = (result == NRF_SUCCESS);
}

void 
BLEService::on_event(ble_evt_t* evt)
{
    switch(evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(evt);
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(evt);
            break;
            
        case BLE_GATTS_EVT_WRITE:
            on_write(evt);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            on_timeout(evt);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            on_authorize_rw_request(evt);
            break;
        // add all the rest
            
        default:
            break;
    }
}

void 
BLEService::on_connect(ble_evt_t* evt)
{
    connection_handle_ = evt->evt.gap_evt.conn_handle;
    // tbd: characteristics may need to know about the connection event...
    ble_gap_evt_connected_t* evt_connect = &evt->evt.gap_evt.params.connected;
    Characteristics::iterator cit = characteristics_.begin();
    Characteristics::iterator ecit = characteristics_.end();
    for(; cit != ecit; ++cit) {
        (*cit)->on_connect(evt_connect);
    }
}

void 
BLEService::on_disconnect(ble_evt_t* evt)
{
    connection_handle_ = BLE_CONN_HANDLE_INVALID;
    // tbd: characteristics may need to know about the disconnection event...
    ble_gap_evt_disconnected_t* evt_disconnect = &evt->evt.gap_evt.params.disconnected;
    Characteristics::iterator cit = characteristics_.begin();
    Characteristics::iterator ecit = characteristics_.end();
    for(; cit != ecit; ++cit) {
        (*cit)->on_disconnect(evt_disconnect);
    }
}

void 
BLEService::on_write(ble_evt_t* evt)
{
    ble_gatts_evt_write_t* evt_write = &evt->evt.gatts_evt.params.write;
    Characteristics::iterator cit = characteristics_.begin();
    Characteristics::iterator ecit = characteristics_.end();
    for(; cit != ecit; ++cit) {
        (*cit)->on_write(evt_write);
    }
}

void 
BLEService::on_timeout(ble_evt_t* evt)
{
    ble_gatts_evt_timeout_t* evt_to = &evt->evt.gatts_evt.params.timeout;
    Characteristics::iterator cit = characteristics_.begin();
    Characteristics::iterator ecit = characteristics_.end();
    for(; cit != ecit; ++cit) {
        (*cit)->on_timeout(evt_to);
    }
}

void 
BLEService::on_authorize_rw_request(ble_evt_t* evt)
{
    ble_gatts_evt_rw_authorize_request_t* evt_auth_rw = &evt->evt.gatts_evt.params.authorize_request;
    Characteristics::iterator cit = characteristics_.begin();
    Characteristics::iterator ecit = characteristics_.end();
    for(; cit != ecit; ++cit) {
        (*cit)->on_authorize_rw_request(evt_auth_rw, connection_handle_);
    }
}




void
BLEService::add_characteristic(BLECharacteristic* ch) 
{
    characteristics_.push_back(ch);
    good_ = ch->register_self(service_handle_, uuid_, uuid_type_);
}

}
