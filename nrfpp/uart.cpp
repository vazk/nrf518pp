#include "uart.hpp"

namespace nrfpp 
{

UART::UART(const UART::Config& config)
{
    // setup the rx/tx pins
    nrf_gpio_cfg_output(config.tx_pin);
    nrf_gpio_cfg_input(config.rx_pin, NRF_GPIO_PIN_NOPULL);
    NRF_UART0->PSELTXD = config.tx_pin;
    NRF_UART0->PSELRXD = config.rx_pin;
    // setup the cts and rts pins if hardware control is requested
    if (config.hardware_control) {
        nrf_gpio_cfg_output(config.rts_pin);
        nrf_gpio_cfg_input(config.cts_pin, NRF_GPIO_PIN_NOPULL);
        NRF_UART0->PSELCTS = config.cts_pin;
        NRF_UART0->PSELRTS = config.rts_pin;
        NRF_UART0->CONFIG  = (UART_CONFIG_HWFC_Enabled << UART_CONFIG_HWFC_Pos);
    }
    // set the baudrate
    NRF_UART0->BAUDRATE      = (config.bitrate << UART_BAUDRATE_BAUDRATE_Pos);
    // and enable the driver
    NRF_UART0->ENABLE        = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
    NRF_UART0->TASKS_STARTTX = 1;
    NRF_UART0->TASKS_STARTRX = 1;
    NRF_UART0->EVENTS_RXDRDY = 0;
}

void UART::put(uint8_t b)
{
    NRF_UART0->TXD = (uint8_t)b;
    // wait for TXD data to be sent.
    while (NRF_UART0->EVENTS_TXDRDY != 1) {}
    NRF_UART0->EVENTS_TXDRDY = 0;
}

void UART::get(uint8_t* pb)
{
    // wait for RXD data to be received
    while (NRF_UART0->EVENTS_RXDRDY != 1) {}
    NRF_UART0->EVENTS_RXDRDY = 0;
    *pb = (uint8_t)NRF_UART0->RXD;
}

bool UART::get_timeout(uint8_t* pb, uint32_t timeout_ms)
{
    bool status = true;
    // wait for RXD data to be received
    while (NRF_UART0->EVENTS_RXDRDY != 1) {
        if (timeout_ms-- >= 0) {
            // wait in 1ms chunk before checking for status.
            nrf_delay_us(1000);
        } else {
            status = false;
            break;
        }
    } 
    if (timeout_ms >= 0) {
        // clear the event and set *pb with received byte.
        NRF_UART0->EVENTS_RXDRDY = 0;
        *pb = (uint8_t)NRF_UART0->RXD;
    }
    return status;
}

}
