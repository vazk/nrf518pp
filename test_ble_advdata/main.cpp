#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <vector>
#include <set>
#include <map>

#include "nrfpp/ble_peripheral_device.hpp"
#include "nrfpp/ble_service.hpp"
#include "nrfpp/ble_characteristic.hpp"
#include "nrfpp/ble_advertising_data.hpp"
#include "nrfpp/ble_app_timer.hpp"


using nrfpp::BLEApplication;
using nrfpp::BLEPeripheralDevice;
using nrfpp::BLEService;
using nrfpp::BLECharacteristic;

class TestAdvData : public nrfpp::BLEAdvertisingData
{
public:
    TestAdvData() : BLEAdvertisingData(0x1527), byte_(0) {}
    uint16_t size() { return 1; }
    uint8_t* data() { return &byte_; }
    void incr() { byte_++; }
private:
    uint8_t byte_;
};


class AdvDataUpdater
{
public:
    AdvDataUpdater(TestAdvData* tad) {}
    static void on_timer(void* context) {
        TestAdvData* tad = (TestAdvData*)context;
        tad->incr();
        BLEPeripheralDevice::instance().update_advertising();
    }
    
};

int main(void)
{
    // instantiate and initialize the application scheduler.
    BLEApplication& app = BLEApplication::instance();
    app.initialize(BLEApplication::Config());

    // setup all the required parameters for initialization of BLEPeripheralDevice
    BLEPeripheralDevice::Params dp;
    dp.name = "ble_advdata";
    dp.cconf = nrfpp::SYNTH_250;
    dp.adv_interval_ms = 25;
    dp.adv_timeout_s = 0;

    dp.min_conn_interval_ms = 500;
    dp.max_conn_interval_ms = 1000;
    dp.slave_latency_ms = 0;
    dp.conn_supervisory_timeout_ms = 4000;

    BLEPeripheralDevice::SecurityParams sp;
    sp.sec_timeout_s = 30;
    sp.sec_bond = true;
    sp.sec_mitm = false;
    sp.sec_io_capabilities = BLE_GAP_IO_CAPS_NONE;
    sp.sec_oob = false;
    sp.sec_min_key_size = 7;
    sp.sec_max_key_size = 16;

    BLEPeripheralDevice::ConnectionParams cp;
    cp.conn_max_negotiation_attempts = 3;
    cp.conn_first_params_update_delay_ms = 5000;
    cp.conn_next_params_update_delay_ms = 10000;

    // get the reference to the singleton object of BLEPeripheralDevice
    BLEPeripheralDevice& pdevice = BLEPeripheralDevice::instance();
    pdevice.init(dp, sp, cp);

    // also add simple adverticement data 
    TestAdvData* tad = new TestAdvData();
    pdevice.add_advertising_data(tad);

    // set up this timer to modify the adverticement data periodically
    nrfpp::BLEAppTimer<AdvDataUpdater> adu_timer(nrfpp::TIMER_REPEATED, tad);
    bool status = adu_timer.is_good();
    adu_timer.start(800);
    status = adu_timer.is_good();

    // start the device
    pdevice.start_advertising();
    
    // application scheduler and softdevice event handler loops.
    while(true) {
        app.idle();
        pdevice.idle();
    }

    return 0;
}
