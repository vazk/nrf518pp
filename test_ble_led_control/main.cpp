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
#include "nrfpp/uart.hpp"


using nrfpp::BLEApplication;
using nrfpp::BLEPeripheralDevice;
using nrfpp::BLEService;
using nrfpp::BLECharacteristic;
using nrfpp::UART;


class BLELedControllerCharacteristic : public BLECharacteristic
{
    enum { LED_PIN = 5 };
public:
    BLELedControllerCharacteristic(uint16_t char_uuid, const char* char_name)
     : BLECharacteristic(char_uuid, char_name, true, true, true, true, 1, nrfpp::LOC_STACK)
    {
        nrf_gpio_cfg_output(LED_PIN);
    } 
protected:
    void on_connect(ble_gap_evt_connected_t* evt)
    {
        printf("on_connect!\n");
        //grant_authorization(nrfpp::AUTH_READWRITE, conn_handle);
    }

    void on_disconnect(ble_gap_evt_disconnected_t* evt)
    {
        printf("on_disconnect!\n");
    }


    void on_write(ble_gatts_evt_write_t* evt)
    {
        printf("on_write!\n");
    }

    void on_authorize_rw_request(ble_gatts_evt_rw_authorize_request_t* evt,
                                 uint16_t conn_handle)
    {
        printf("on_auth_rw! ");
        switch((nrfpp::AuthorizationEN)evt->type) {
            case nrfpp::AUTH_READ : 
                printf("read\n");
                break;
            case nrfpp::AUTH_WRITE : 
                printf("write[%d]\n", (int)evt->request.write.data[0]);
                if(evt->request.write.data[0]) {
                    nrf_gpio_pin_set(LED_PIN);
                } else {
                    nrf_gpio_pin_clear(LED_PIN);
                }

                break;
            default:
                printf("\n");
        }
        grant_authorization((nrfpp::AuthorizationEN)evt->type, conn_handle);
    }
};


int main(void)
{
    // instantiate and initialize the application scheduler.
    BLEApplication& app = BLEApplication::instance();
    app.initialize(BLEApplication::Config());

    UART& uart = UART::instance();
    UART::Config config = {UART::BR_38400,3,7,0,0,false};
    uart.initialize(config);
    printf("UART is up!\n");

    // setup all the required parameters for initialization of BLEPeripheralDevice
    BLEPeripheralDevice::Params dp;
    dp.name = "nrfpp-led";
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

    // instantiate a service and a characteristic
    BLEService* bsa = new BLEService(0x1523, BLE_UUID_TYPE_BLE);
    BLECharacteristic* cha = new BLELedControllerCharacteristic(0x1524, "led-ctrl-ch");
    // add the characteristic to the service
    bsa->add_characteristic(cha);
    // now add the service to the peripheral device 
    pdevice.add_service(bsa);

    // start the device
    pdevice.start_advertising();
    
    // application scheduler and softdevice event handler loops.
    while(true) {
        app.idle();
        pdevice.idle();
    }

    return 0;
}
