#include "awe_control_interface.h"
#include "awe_xcore_internal.h"
#include "xcore/hwtimer.h"

static inline int is_writeable(int reg_num) {
    return reg_num < NUM_REGISTERS && reg_num >= FIRST_WRITEABLE;
}

static int awe_control_interface(uint32_t *handle,
                                 uint32_t *offset,
                                 uint32_t *mask,
                                 uint8_t *bytes,
                                 uint32_t byte_count,
                                 uint8_t command) {
    switch(command) {
    case AWE_CTRL_SET_VALUE:
        return awe_ctrlSetValue((AWEInstance *)&g_AWEInstance, *handle, bytes, *offset, byte_count / 4);
    case AWE_CTRL_GET_VALUE:
        return awe_ctrlGetValue((AWEInstance *)&g_AWEInstance, *handle, bytes, *offset, byte_count / 4);
    case AWE_CTRL_SET_VALUE_MASK:
        return awe_ctrlSetValueMask((AWEInstance *)&g_AWEInstance, *handle, bytes, *offset, byte_count / 4, *mask);
    case AWE_CTRL_GET_VALUE_MASK:
        return awe_ctrlGetValueMask((AWEInstance *)&g_AWEInstance, *handle, bytes, *offset, byte_count / 4, *mask);
    case AWE_CTRL_SET_STATUS:
        return awe_ctrlSetStatus((AWEInstance *)&g_AWEInstance, *handle, ((uint8_t *)offset)[0]);
    case AWE_CTRL_GET_STATUS:
        return awe_ctrlGetStatus((AWEInstance *)&g_AWEInstance, *handle, (UINT32*)offset);
    case AWE_SET_PROFILING_STATUS:
        return awe_setProfilingStatus((AWEInstance *)&g_AWEInstance, offset[0]);
    case AWE_GET_AVERAGE_LAYOUT_CYCLES:
        return awe_getAverageLayoutCycles((AWEInstance *)&g_AWEInstance, offset[0], (UINT32*)offset);
    case AWE_LOAD_AWB:
        return awe_loadAWBfromArray((AWEInstance *)&g_AWEInstance, (UINT32*)bytes, byte_count, (UINT32*)offset);
    case AWE_GET_CHANNEL_COUNT:
        awe_layoutGetChannelCount((AWEInstance *)&g_AWEInstance, handle[0], (UINT32*)offset, (UINT32*)mask);
        return 0;
    case AWE_GET_INPUT_BLOCK_SIZE:
        return awe_layoutGetInputBlockSize((AWEInstance *)&g_AWEInstance, handle[0], (UINT32*)offset);
    case AWE_GET_INPUT_SAMPLE_RATE: 
        return awe_layoutGetInputSampleRate((AWEInstance *)&g_AWEInstance, handle[0], (float *)offset);
    case AWE_LAYOUT_IS_VALID:
        return awe_layoutIsValid((AWEInstance *)&g_AWEInstance);
    }
    return 1;
}

int awe_control_register_write(awe_reg_control_t *arc, int *regnum, uint8_t data) {
    int rnum = *regnum;
    int reg_param_bytes = arc->registers[REG_PARAM_BYTES0 / 4];
    if (rnum == REG_BYTE_STREAM) { // Byte stream auto increments internally
        arc->bytes[arc->bytes_ptr] = data;
        if (reg_param_bytes) {
            arc->bytes_ptr = (arc->bytes_ptr + 1) % reg_param_bytes;
        }
    } else if (rnum == REG_EXECUTE) {    // Forces execution
        int ret = awe_control_interface(&arc->registers[REG_PARAM_A_0 / 4],
                                        &arc->registers[REG_PARAM_B_0 / 4],
                                        &arc->registers[REG_PARAM_C_0 / 4],
                                        arc->bytes,
                                        reg_param_bytes,
                                        ((uint8_t *)arc->registers)[REG_COMMAND]);
        ((uint8_t *)arc->registers)[REG_EXECUTE] = ret; // Stores error
    } else {
        if (is_writeable(rnum)) {        // Store data in reg file
            ((uint8_t *)arc->registers)[rnum] = data;
        } else {
            return 0;
        }
        if (rnum < NUM_REGISTERS) {
            *regnum = rnum + 1;
        }
    }
    return 1;
}

uint8_t awe_control_register_read(awe_reg_control_t *arc, int *regnum) {
    uint8_t data;
    int rnum = *regnum;
    int reg_param_bytes = arc->registers[REG_PARAM_BYTES0 / 4];
    if (rnum == REG_BYTE_STREAM) { // Byte stream auto increments internally
        data = arc->bytes[arc->bytes_ptr];
        if (reg_param_bytes) {
            arc->bytes_ptr = (arc->bytes_ptr + 1) % reg_param_bytes;
        }
    } else {                         // General purpose register selected
        data = ((uint8_t *)arc->registers)[rnum];
        if (rnum < NUM_REGISTERS) {
            *regnum = rnum + 1;
        }
    }
    return data;
}

extern hwtimer_t __timers[];

void awe_control_register_init(awe_reg_control_t *arc) {
    memset(arc, 0, sizeof(*arc));
    arc->bytes_ptr = 0;
    ((uint8_t *)arc->registers)[REG_MAGIC_VID]     = AWE_I2C_VID_VALUE;
    ((uint8_t *)arc->registers)[REG_MAGIC_PID]     = AWE_I2C_PID_VALUE;
    ((uint8_t *)arc->registers)[REG_VERSION_MAJOR] = AWE_I2C_VERSION_MAJOR;
    ((uint8_t *)arc->registers)[REG_VERSION_MINOR] = AWE_I2C_VERSION_MINOR;

    // This is a workaround for xcore/parallel.h not playing nicely with combinable interfaces
    int id;
    asm volatile("get r11,id; add %0, r11, 0" : "=r" (id));
    __timers[id] = hwtimer_alloc();


    hwtimer_t x = hwtimer_alloc();
    int now = hwtimer_get_time(x);
    now += 1000000000;
    hwtimer_set_trigger_time(x, now);
    (void) hwtimer_get_time(x);
    while(1) {
        float val = 24.0;
        awe_ctrlSetValueMask((AWEInstance *)&g_AWEInstance, 0x07531008, &val, 0, 1, 0x100);
        now += 100000000;
        hwtimer_set_trigger_time(x, now);
        (void) hwtimer_get_time(x);
        val = -120.0;
        awe_ctrlSetValueMask((AWEInstance *)&g_AWEInstance, 0x07531008, &val, 0, 1, 0x100);
        now += 100000000;
        hwtimer_set_trigger_time(x, now);
        (void) hwtimer_get_time(x);
    }
}
