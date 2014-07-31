#ifndef NRFPP_BLE_SERVICE_HPP
#define NRFPP_BLE_SERVICE_HPP

#include "ble_characteristic.hpp"

extern "C" {
#include "ble.h"
#include "ble_srv_common.h"
#include "wrappers.h"
}

#include <list>

namespace nrfpp {

class BLEService 
{
    #define UUID_BASE {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}

public:
    typedef std::list<BLECharacteristic*> Characteristics;
    BLEService(uint16_t uuid, uint8_t uuid_type);

public:
    virtual void on_event(ble_evt_t* evt);
    void     add_characteristic(BLECharacteristic* ch);
    bool     is_good() const { return good_; }
    uint16_t uuid() const { return uuid_; }
    uint8_t  uuid_type() const { return uuid_type_; }

protected:
    void on_connect(ble_evt_t* evt);
    void on_disconnect(ble_evt_t* evt);
    void on_write(ble_evt_t* evt);
    void on_timeout(ble_evt_t* evt);

protected:
    uint16_t uuid_;
    uint8_t  uuid_type_;
    // handles
    uint16_t service_handle_;
    uint16_t connection_handle_;
    // characteristics
    Characteristics characteristics_;
    // status
    bool  good_;
};

}

#endif //NRFPP_BLE_SERVICE_HPP
