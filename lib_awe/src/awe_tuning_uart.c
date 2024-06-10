#include <xcore/channel.h>
#include <xcore/select.h>
#include <print.h>

#include "ProxyIDs.h"
#define DEFINE_ERROR_STRINGS 0 // Prints error code as str but costs memory. Define to 0 for enum.
#include "Errors.h"
#include "awe_xcore_internal.h"
#if defined(AWE_TUNING_OVER_UART)
#include "AWECoreUtils.h"

#include "uart.h"
#include <xcore/parallel.h>


#define UART_BAUD           112500
#define UART_DATA_BITS      8
#define UART_PARITY         UART_PARITY_NONE
#define UART_STOP_BITS      1
#define UART_TX_PORT        XS1_PORT_1A
#define UART_RX_PORT        XS1_PORT_1C

// This sequence id (set by incoming msg from host) needs to be eechoed back by the response
volatile char seq = 0x30;

DECLARE_JOB(uart_stub, (chanend_t));
void uart_stub(chanend_t c_tuning_to_host){
    uart_tx_t uart;
    hwtimer_t tmr = hwtimer_alloc();
    uart_tx_blocking_init(&uart, UART_TX_PORT, UART_BAUD, UART_DATA_BITS, UART_PARITY, UART_STOP_BITS, tmr);

    while(1){
        unsigned int packet_buffer[64];
        int len = get_pkt(c_tuning_to_host, packet_buffer, 64);
        // printintln(len);

        uart_tx(&uart, 0x02);
        uart_tx(&uart, seq);
        // if(++seq == 0x40){
        //     seq = 0x30;
        // }

        for(int w = 1; w < len + 1; w++){
            unsigned int word = packet_buffer[w];
            for(int i = 0; i < 5; i++){
                uart_tx(&uart, (char)(0x80 | ((word >> (7 * i)) & 0x7f)) );
            } 
        }
        uart_tx(&uart, 0x03);
        // delay_milliseconds(1000);

        // printintln(packet_buffer[0]>>16);
        for(int w = 1; w < len + 1; w++){
            unsigned int word = packet_buffer[w];
            // printhexln(word);
        }
    }
}

HIL_UART_RX_CALLBACK_ATTR void rx_error_callback(uart_callback_code_t callback_code, void *app_data){
    return;

    switch(callback_code){
        case UART_START_BIT_ERROR:
            printstrln("UART_START_BIT_ERROR");
            break;
        case UART_PARITY_ERROR:
            printstrln("UART_PARITY_ERROR");
            break;
        case UART_FRAMING_ERROR:
            printstrln("UART_FRAMING_ERROR");
            // test_abort = 1;
            break;
        case UART_OVERRUN_ERROR:
            printstrln("UART_OVERRUN_ERROR");
            break;
        case UART_UNDERRUN_ERROR:
            printstrln("UART_UNDERRUN_ERROR");
            break;
        default:
            printstr("Unexpected callback code: ");
            printintln(callback_code);
    }
}

HIL_UART_RX_CALLBACK_ATTR void rx_complete_callback(void *app_data){
    uart_rx_t *uart = (uart_rx_t *)app_data;
    uart_callback_code_t callback_info = uart->cb_code;

    switch(callback_info){
        case UART_RX_COMPLETE:
            // bytes_received += 1;
            break;

        default:
            printstr("Unexpected callback code: ");
            printintln(callback_info);
    }
}

DECLARE_JOB(uart_rx_wrapper, (chanend_t));
void uart_rx_wrapper(chanend_t c_tuning_from_host){
    uart_rx_t uart;
    hwtimer_t tmr = hwtimer_alloc();

    hwtimer_delay(tmr, XS1_TIMER_KHZ * 500);

    uart_rx_blocking_init(  &uart, UART_RX_PORT, UART_BAUD, UART_DATA_BITS, UART_PARITY, UART_STOP_BITS, tmr,
                            rx_error_callback, &uart);

    unsigned int packetBuffer[32] = {0};
    unsigned int cmd = 0;
    unsigned byte_idx = 0;
    unsigned cmd_idx = 0;
    while(1){
        char rx_from_uart = uart_rx(&uart);
        switch(byte_idx){
            case 0:
                // will equal 0x02
                byte_idx++;
                cmd = 0;
            break;
            case 1:
                // sequence num
                seq = rx_from_uart;
                byte_idx++;
            break;
            // Handle 5 data bytes
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                rx_from_uart &= 0x7f;
                cmd |= rx_from_uart << ((byte_idx - 2) * 7);

                byte_idx++;
            break;
            // Handle terminator or next cmd
            case 7:
                packetBuffer[cmd_idx++] = cmd;
                // End of packet
                if(rx_from_uart == 0x03){
                    send_pkt(c_tuning_from_host, cmd_idx - 1, packetBuffer);
                    for(int i = 0; i < cmd_idx - 1; i++){
                        // printhexln(packetBuffer[i]);
                    }
                    // printhexln(cmd_idx - 1);
                    byte_idx = 0;
                    cmd_idx = 0;
                } else {
                    // More data - save cmd and start on next starting from second byte as we have processed first here
                    byte_idx = 3;
                    rx_from_uart &= 0x7f;
                    cmd = 0;
                    cmd |= rx_from_uart << ((byte_idx - 2) * 7);
                }
            break;
        }

        // int status = tuningDecodeByteUART(packetBuffer, rx_from_uart);
        // switch(status){
        //     case COMPLETE_NEW_PACKET:
        //         printhexln(packetBuffer[0]);
        //     break;
        //     case INCOMPLETE_PACKET:
        //         printchar('.');
        //     break;
        //     case COMPLETE_REPEATED_PACKET:
        //         printchar('+');
        //     break;
        // }

    }

}


void awe_uart(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host) {
    PAR_JOBS (
         // PJOB(INTERRUPT_PERMITTED(test), ()),
         PJOB(uart_stub, (c_tuning_to_host)),
         PJOB(uart_rx_wrapper, (c_tuning_from_host))
     );
}

#endif