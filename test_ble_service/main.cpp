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


int main(void)
{
    // instantiate and initialize the application scheduler.
    BLEApplication& app = BLEApplication::instance();
    app.initialize(BLEApplication::Config());

    // setup all the required parameters for initialization of BLEPeripheralDevice
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

    // get the reference to the singleton object of BLEPeripheralDevice
    BLEPeripheralDevice& pdevice = BLEPeripheralDevice::instance();
    pdevice.init(dp, sp, cp);

    // instantiate a service and a characteristic
    BLEService* bs = new BLEService(0x1523, BLE_UUID_TYPE_BLE);
    BLECharacteristic* ch = new BLECharacteristic(0x1524, "bla",
                                                  false, false, 
                                                  true, false, 
                                                  1, nrfpp::LOC_STACK);
    // add the characteristic to the service
    bs->add_characteristic(ch);
    // now add the service to the peripheral device 
    pdevice.add_service(bs);

    // start the device
    pdevice.start_advertising();
    
    // application scheduler and softdevice event handler loops.
    while(true) {
        app.idle();
        pdevice.idle();
    }

    return 0;
}
