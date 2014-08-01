#ifndef NRFPP_BLE_ADVERTISEMENT_DATA_HPP
#define NRFPP_BLE_ADVERTISEMENT_DATA_HPP

#include <stdint.h>

namespace nrfpp {

class BLEAdvertisingData
{
public:
    BLEAdvertisingData(uint16_t uuid) : uuid_(uuid) {}
    virtual uint16_t size() = 0;
    virtual uint8_t* data() = 0;
    uint16_t uuid() { return uuid_; }

protected:
    uint16_t    uuid_;
};

}

#endif // NRFPP_BLE_ADVERTISEMENT_DATA_HPP
