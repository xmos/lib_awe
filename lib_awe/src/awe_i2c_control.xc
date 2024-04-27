// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <xs1.h>

#include "i2c.h"
#include "awe_control_interface.h"

[[combinable]]
void i2c_register_file_implementation(server i2c_slave_callback_if i2c,
                                      awe_reg_control_t &arc) {
    int current_regnum = -1;       // Set to -1 if no current register has been selected.
    while (1) {
        select {
        case i2c.ack_read_request(void) -> i2c_slave_ack_t response:
            if (current_regnum == -1) {
                response = I2C_SLAVE_NACK;   // reject if no register selected
            } else {
                response = I2C_SLAVE_ACK;
            }
            break;
        case i2c.ack_write_request(void) -> i2c_slave_ack_t response:
            // Write requests are always accepted
            response = I2C_SLAVE_ACK;
            break;
        case i2c.master_sent_data(uint8_t data) -> i2c_slave_ack_t response:
            if (current_regnum != -1) {     // Register selected - write to it
                if (awe_control_register_write(arc, current_regnum, data)) {
                    response = I2C_SLAVE_ACK;
                } else {
                    response = I2C_SLAVE_NACK;            // Register not selected and invalid
                }
            } else if (data < NUM_REGISTERS) {        // Register not selected and valid
                current_regnum = data;
                response = I2C_SLAVE_ACK;
            } else {
                response = I2C_SLAVE_NACK;            // Register not selected and invalid
            }
            break;
        case i2c.master_requires_data() -> uint8_t data:
            if (current_regnum != -1) {               // Register selected
                data = awe_control_register_read(arc, current_regnum);
            } else {                                  // No register selected
                data = 0;
            }
            break;
        case i2c.stop_bit():
            current_regnum = -1;
            break;
        } // select
    }
}

void awe_i2c_main(port p_scl, port p_sda, int device_addr) {
    i2c_slave_callback_if i_i2c;
    awe_reg_control_t arc;
    awe_control_register_init(arc);              // Initialise RO register values

    [[combine]] par {
        i2c_register_file_implementation(i_i2c, arc);
        i2c_slave(i_i2c, p_scl, p_sda, device_addr);
    }
}
