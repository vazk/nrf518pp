#include "uart.hpp"

namespace nrfpp 
{

UART::UART(const UART::Config& config)
{
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
