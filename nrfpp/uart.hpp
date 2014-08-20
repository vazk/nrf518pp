#ifndef NRFPP_UART_HPP
#define NRFPP_UART_HPP

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include <stdint.h>

namespace nrfpp 
{

class UART
{
public:
    enum Bitrate {
        BR_1200   = UART_BAUDRATE_BAUDRATE_Baud1200,
        BR_2400   = UART_BAUDRATE_BAUDRATE_Baud2400,
        BR_4800   = UART_BAUDRATE_BAUDRATE_Baud4800,
        BR_9600   = UART_BAUDRATE_BAUDRATE_Baud9600,
        BR_14400  = UART_BAUDRATE_BAUDRATE_Baud14400,
        BR_19200  = UART_BAUDRATE_BAUDRATE_Baud19200,
        BR_28800  = UART_BAUDRATE_BAUDRATE_Baud28800,
        BR_38400  = UART_BAUDRATE_BAUDRATE_Baud38400,
        BR_57600  = UART_BAUDRATE_BAUDRATE_Baud57600,
        BR_115200 = UART_BAUDRATE_BAUDRATE_Baud115200,
        BR_230400 = UART_BAUDRATE_BAUDRATE_Baud230400,
        BR_460800 = UART_BAUDRATE_BAUDRATE_Baud460800,
        BR_921600 = UART_BAUDRATE_BAUDRATE_Baud921600
    };
    struct Config 
    {
        Bitrate bitrate;
        uint8_t rx_pin;
        uint8_t tx_pin;
        uint8_t cts_pin;
        uint8_t rts_pin;
        bool hardware_control;
        bool blocking;
    };

public:
    UART(const Config& config);

    void  put(uint8_t b);
    void  get(uint8_t* b);
    bool  get_timeout(uint8_t* b, uint32_t ms);
};

}


#endif //NRFPP_UART_HPP
