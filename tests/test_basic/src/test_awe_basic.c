// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <stdio.h>
#include <stdlib.h>
#include <xcore/channel.h>
#include <xcore/chanend.h>
#include <xcore/select.h>
#include <xcore/parallel.h>
#include <xclib.h>
#include "awe_xcore.h"

int parse_cmd_line(int argc, char** argv, uint32_t msg_to_awe_buffer[]){
    int count = 0;

    if(argc <= 1){
        fprintf(stderr, "Too few args to command line: %d\n", argc);
        exit(1);
    }
    uint32_t header = strtoul(argv[++count], NULL, 16);
    fprintf(stderr, "header\t0x%8lx\n", header);
    msg_to_awe_buffer[count - 1] = header;
    int num_words = header >> 16;
    for(int i = 1; i < num_words; i++){
        uint32_t word = strtoul(argv[++count], NULL, 16);
        msg_to_awe_buffer[count - 1] = word;
        fprintf(stderr, "payload\t0x%8lx\n", word);
    }
    if(count != argc - 1) {
        fprintf(stderr, "Mismatch in number of commands and header in xcore test app\n");
        exit(1);
    }

    return count;
}

void send_ctrl_to_awe(chanend_t c_tuning_from_host, uint32_t msg_to_awe_buffer[], int num_words){
    chanend_out_word(c_tuning_from_host, num_words);
    for(int i = 0; i < num_words; i++) {
        chanend_out_word(c_tuning_from_host, msg_to_awe_buffer[i]);
        // printf("Sent %i: 0x%8lx\n", i, msg_to_awe_buffer[i]);
    }
    chanend_out_end_token(c_tuning_from_host);
    chanend_check_end_token(c_tuning_from_host);
}

DECLARE_JOB(awe_test, (uint32_t*, int, chanend_t, chanend_t, chanend_t));
void awe_test(uint32_t *msg_to_awe_buffer, int msg_len, chanend_t c_tuning_from_host, chanend_t c_tuning_to_host, chanend_t c_data){

    uint32_t msg_from_awe_buffer[16] = {0};
    int num_words_rx = 0;
    int rx_msg_len = 100000; // A big number which we will re-write

    send_ctrl_to_awe(c_tuning_from_host, msg_to_awe_buffer, msg_len);

    SELECT_RES(
        CASE_THEN(c_tuning_to_host, input_from_awe),
        DEFAULT_GUARD_THEN(num_words_rx >= rx_msg_len, completed)
        )
    {
    input_from_awe:
        chanend_check_end_token(c_tuning_to_host);
        chanend_out_word(c_tuning_to_host, 13);
        chanend_out_end_token(c_tuning_to_host);
        rx_msg_len = chanend_in_word(c_tuning_to_host); // get actual length
        msg_from_awe_buffer[0] = ((rx_msg_len << 2) << 24) | 0x00000001;
        for(int i = 0; i < rx_msg_len; i++) {
            msg_from_awe_buffer[i+1] = chanend_in_word(c_tuning_to_host);
            printf("%lx\n", msg_from_awe_buffer[i+1]);
            num_words_rx++;
        }
        chanend_in_word(c_tuning_to_host); // Consume finished? token
        chanend_out_end_token(c_tuning_to_host);
        chanend_check_end_token(c_tuning_to_host);
        continue;

    completed:
        _Exit(0);
        continue;
    }

}

DECLARE_JOB(awe_main_wrapper, (chanend_t, chanend_t, chanend_t));
#pragma stackfunction 5000
__attribute__((noinline))
void awe_main_wrapper(chanend_t c_tuning_from_host, chanend_t c_tuning_to_host, chanend_t c_data){
    awe_xcore_main(c_tuning_from_host, c_tuning_to_host, c_data);
}

int main(int argc, char** argv){
    channel_t c_tuning_from_host = chan_alloc();
    channel_t c_tuning_to_host = chan_alloc();
    channel_t c_data = chan_alloc();

    uint32_t msg_to_awe_buffer[AWE_HID_PACKET_BUFFER_SIZE] = {0};
    int num_words = parse_cmd_line(argc, argv, msg_to_awe_buffer);

    PAR_JOBS(
        PJOB(awe_main_wrapper, (c_tuning_from_host.end_a, c_tuning_to_host.end_a, c_data.end_a)),
        PJOB(awe_test, (msg_to_awe_buffer, num_words, c_tuning_from_host.end_b, c_tuning_to_host.end_b, c_data.end_b)));

}
