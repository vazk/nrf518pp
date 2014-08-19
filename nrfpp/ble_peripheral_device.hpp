#ifndef NRFPP_BLE_PERIPHERAL_DEVICE_HPP
#define NRFPP_BLE_PERIPHERAL_DEVICE_HPP

extern "C" {
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include "ble.h"
#include "ble_gap.h"
#include "nrf_assert.h"
#include "softdevice_handler.h"
}

#include <list>
#include <string>
#include <stdint.h>

namespace nrfpp {
    enum ClockConfEN {
         SYNTH_250 = NRF_CLOCK_LFCLKSRC_SYNTH_250_PPM,
         XTAL_500 = NRF_CLOCK_LFCLKSRC_XTAL_500_PPM,
         XTAL_250 = NRF_CLOCK_LFCLKSRC_XTAL_250_PPM,
         XTAL_150 = NRF_CLOCK_LFCLKSRC_XTAL_150_PPM,
         XTAL_100 = NRF_CLOCK_LFCLKSRC_XTAL_100_PPM,
         XTAL_75 = NRF_CLOCK_LFCLKSRC_XTAL_75_PPM,
         XTAL_50 = NRF_CLOCK_LFCLKSRC_XTAL_50_PPM,
         XTAL_30 = NRF_CLOCK_LFCLKSRC_XTAL_30_PPM,
         XTAL_20 = NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, 
         RC_250 = NRF_CLOCK_LFCLKSRC_RC_250_PPM_250MS_CALIBRATION,
         RC_500 = NRF_CLOCK_LFCLKSRC_RC_250_PPM_500MS_CALIBRATION,
         RC_1000 = NRF_CLOCK_LFCLKSRC_RC_250_PPM_1000MS_CALIBRATION,
         RC_2000 = NRF_CLOCK_LFCLKSRC_RC_250_PPM_2000MS_CALIBRATION,
         RC_4000 = NRF_CLOCK_LFCLKSRC_RC_250_PPM_4000MS_CALIBRATION,
         RC_8000 = NRF_CLOCK_LFCLKSRC_RC_250_PPM_8000MS_CALIBRATION,    
    };
}

namespace nrfpp {

class BLEAdvertisingData;
class BLEService;

class BLEPeripheralDevice
{
public:
    struct Params 
    {
        std::string name;
        ClockConfEN cconf;
        // advertising
        uint32_t adv_interval_ms;
        uint32_t adv_timeout_s;
        //
        uint32_t min_conn_interval_ms;
        uint32_t max_conn_interval_ms;
        uint32_t slave_latency_ms;
        uint32_t conn_supervisory_timeout_ms;
    };
    struct SecurityParams
    {
        uint32_t sec_timeout_s;
        bool     sec_bond;
        bool     sec_mitm;
        uint32_t sec_io_capabilities;
        bool     sec_oob;
        uint32_t sec_min_key_size;
        uint32_t sec_max_key_size;
    };
    struct ConnectionParams
    {
        uint32_t conn_max_negotiation_attempts;
        uint32_t conn_first_params_update_delay_ms;
        uint32_t conn_next_params_update_delay_ms;
    };
    typedef std::list<BLEAdvertisingData*> AdvertisingDataArray;
    typedef std::list<BLEService*>         ServiceArray;

private:
    BLEPeripheralDevice()
     : good_(false) 
    {}

public:
    static BLEPeripheralDevice& instance();
    
public:
    void init(const Params& dp, 
                     const SecurityParams& sp,
                     const ConnectionParams& cp);
    void add_service(BLEService* service);
    void add_advertising_data(BLEAdvertisingData* adv_data);

    void start_advertising();
    void update_advertising();

    void start();
    void stop();

    void idle();
    
    bool is_good();

private:
    static void peripheraldevice_event_dispatcher(ble_evt_t* evt);
    static void system_event_dispatcher(uint32_t evt);
    void init_gap();
    void init_connection();
    void init_security();

private:
    bool good_;
    Params dp_;
    SecurityParams   sp_;
    ConnectionParams cp_;
    ble_gap_sec_params_t sec_params_;
    ble_gap_adv_params_t adv_params_;
    AdvertisingDataArray advertising_data_;
    ServiceArray         services_;
};

}

#endif //NRFPP_BLE_PERIPHERAL_DEVICE_HPP
