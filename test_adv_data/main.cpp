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
        nrfpp::BLEPeripheralDevice::update_advertising();
    }
    
};



#define SCHED_MAX_EVENT_DATA_SIZE       sizeof(app_timer_event_t) /**< Maximum size of scheduler events. Note that scheduler BLE stack events do not contain any data, as the events are being pulled from the stack in the event handler. */
#define SCHED_QUEUE_SIZE                10 /**< Maximum number of events in the scheduler queue. */

using  nrfpp::BLEPeripheralDevice;
using  nrfpp::BLEService;
using  nrfpp::BLECharacteristic;

int main(void)
{
    BLEPeripheralDevice::Params dp;
    dp.name = "poxosBLE";
    dp.cconf = nrfpp::SYNTH_250;
    dp.adv_interval_ms = 25;
    dp.adv_timeout_s = 180;

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

    BLEPeripheralDevice::init(dp, sp, cp);

    BLEService* bs = new BLEService(0x1523, BLE_UUID_TYPE_BLE);
    BLECharacteristic* ch = new BLECharacteristic(0x1524, "bla",
                                                  false, false, 
                                                  true, false, 
                                                  1, nrfpp::LOC_STACK);
    bs->add_characteristic(ch);
    
    BLEPeripheralDevice::add_service(bs);

    TestAdvData* tad = new TestAdvData();
    BLEPeripheralDevice::add_advertising_data(tad);

    nrfpp::BLEAppTimer<AdvDataUpdater> adu_timer(nrfpp::TIMER_REPEATED, tad);
    bool status = adu_timer.is_good();
    adu_timer.start(800);
    status = adu_timer.is_good();

    BLEPeripheralDevice::start_advertising();

    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);

    while(true) {
        BLEPeripheralDevice::idle();
    }

    return 0;
}
