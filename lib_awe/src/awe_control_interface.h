#ifndef _awe_control_interface_h_
#define _awe_control_interface_h_

#include <stdint.h>

#define MAX_BYTE_STREAM      10
#define NUM_REGISTERS      0x18

typedef struct {
    uint32_t registers[(NUM_REGISTERS+3)/4];
    uint32_t bytes_ptr;
    uint8_t bytes[MAX_BYTE_STREAM];
} awe_reg_control_t;

#ifdef __XC__
#define DEREF &
#else
#include <xcore/port.h>
#define DEREF *
#endif

/** Function that implements control given a register file.
 */
void awe_control_register_init(awe_reg_control_t DEREF arc);
int awe_control_register_write(awe_reg_control_t DEREF arc, int DEREF regnum, uint8_t data);
uint8_t awe_control_register_read(awe_reg_control_t DEREF arc, int DEREF regnum);

/** Function that implements an I2C client that calls the above function for control.
 */
#ifdef __XC__
void awe_i2c_main(port p_scl, port p_sda, int device_addr);
#else
void awe_i2c_main(port_t p_scl, port_t p_sda, int device_addr);
#endif

/** I2C register map
 */

#define REG_MAGIC_VID      0x00
#define REG_MAGIC_PID      0x01
#define REG_VERSION_MAJOR  0x02
#define REG_VERSION_MINOR  0x03
#define                    FIRST_WRITEABLE    0x04
#define REG_BYTE_STREAM    0x04
#define REG_EXECUTE        0x05
#define REG_UNUSED         0x06
#define REG_COMMAND        0x07
#define REG_PARAM_BYTES0   0x08
#define REG_PARAM_BYTES1   0x09
#define REG_PARAM_BYTES2   0x0A
#define REG_PARAM_BYTES3   0x0B
#define REG_PARAM_A_0      0x0C
#define REG_PARAM_A_1      0x0D
#define REG_PARAM_A_2      0x0E
#define REG_PARAM_A_3      0x0F
#define REG_PARAM_B_0      0x10
#define REG_PARAM_B_1      0x11
#define REG_PARAM_B_2      0x12
#define REG_PARAM_B_3      0x13
#define REG_PARAM_C_0      0x14
#define REG_PARAM_C_1      0x15
#define REG_PARAM_C_2      0x16
#define REG_PARAM_C_3      0x17

/** Defined values for read-only I2C registers
 */
#ifndef AWE_I2C_VID_VALUE
#define AWE_I2C_VID_VALUE 0xDE
#endif
#ifndef AWE_I2C_PID_VALUE
#define AWE_I2C_PID_VALUE 0xEF
#endif
#ifndef AWE_I2C_VERISON_MAJOR
#define AWE_I2C_VERSION_MAJOR 1
#endif
#ifndef AWE_I2C_VERISON_MINOR
#define AWE_I2C_VERSION_MINOR 1
#endif

/** Defined values for Control Command bytes
 */

#define AWE_LOAD_AWB                    0x01         // write transfer
#define AWE_CTRL_SET_VALUE              0x02         // write transfer
#define AWE_CTRL_SET_VALUE_MASK         0x22         // write transfer
#define AWE_CTRL_GET_VALUE              0x41         // read transfer
#define AWE_CTRL_GET_VALUE_MASK         0x61         // read transfer
#define AWE_CTRL_SET_STATUS             0x81         // no transfer
#define AWE_SET_PROFILING_STATUS        0x82         // no transfer
#define AWE_CTRL_GET_STATUS             0x84         // no transfer
#define AWE_GET_AVERAGE_LAYOUT_CYCLES   0x88         // no transfer
#define AWE_GET_CHANNEL_COUNT           0x90         // no transfer
#define AWE_GET_INPUT_SAMPLE_RATE       0xA0         // no transfer
#define AWE_GET_INPUT_BLOCK_SIZE        0xA1         // no transfer
#define AWE_LAYOUT_IS_VALID             0xC0         // no transfer


#endif
