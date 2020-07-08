//*****************************************************************************
// file        : paj7620.c
// paj7620 device package
//
// Change Logs:
// Date                 Author      Note
// 2020/07/05           Cheney      First draft version
// 2020/07/07           Cheney      Fix the bug and update the sample macro.
// 2020/07/08           Cheney      Move out the sample code to a separate file.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup paj7620
//! @{
//
//*****************************************************************************
#include "paj7620.h"

#define DBG_SECTION_NAME "paj7620"
#include <rtdbg.h>

#ifdef PKG_USING_PAJ7620

/**< paj7620 device address */
#define PAJ7620_ID                  0x73

/**< bank select register */
#define PAJ_BANK_SEL                0xef

/**< band0 register group */
#define PAJ_SUSPEND_CMD             0x03
#define PAJ_SET_INT_FLAG1           0x41
#define PAJ_SET_INT_FLAG2           0x42
#define PAJ_GET_INT_FLAG1           0x43
#define PAJ_GET_INT_FLAG2           0x44
#define PAJ_GET_STATE               0x45
#define PAJ_SET_HIGH_THRESHOLD      0x69
#define PAJ_SET_LOW_THRESHOLD       0x6A
#define PAJ_GET_APPROACH_STATE      0x6B
#define PAJ_GET_GESTURE_DATA        0x6C
#define PAJ_GET_OBJECT_BRIGHTNESS   0xB0
#define PAJ_GET_OBJECT_SIZE_1       0xB1
#define PAJ_GET_OBJECT_SIZE_2       0xB2

/**< band1 register group */
#define PAJ_SET_PS_GAIN             0x44
#define PAJ_SET_IDLE_S1_STEP_0      0x67
#define PAJ_SET_IDLE_S1_STEP_1      0x68
#define PAJ_SET_IDLE_S2_STEP_0      0x69
#define PAJ_SET_IDLE_S2_STEP_1      0x6A
#define PAJ_SET_OP_TO_S1_STEP_0     0x6B
#define PAJ_SET_OP_TO_S1_STEP_1     0x6C
#define PAJ_SET_S1_TO_S2_STEP_0     0x6D
#define PAJ_SET_S1_TO_S2_STEP_1     0x6E
#define PAJ_OPERATION_ENABLE        0x72

#define PAJ7620_VAL(val, maskbit)   (val << maskbit)

/**< gesture interrupt flag */
#define GES_RIGHT_FLAG              PAJ7620_VAL(1, 0)
#define GES_LEFT_FLAG               PAJ7620_VAL(1, 1)
#define GES_UP_FLAG                 PAJ7620_VAL(1, 2)
#define GES_DOWN_FLAG               PAJ7620_VAL(1, 3)
#define GES_FORWARD_FLAG            PAJ7620_VAL(1, 4)
#define GES_BACKWARD_FLAG           PAJ7620_VAL(1, 5)
#define GES_CLOCKWISE_FLAG          PAJ7620_VAL(1, 6)
#define GES_COUNT_CLOCKWISE_FLAG    PAJ7620_VAL(1, 7)
#define GES_WAVE_FLAG               PAJ7620_VAL(1, 0)

typedef enum
{
    PAJ7620_BANK0,
    PAJ7620_BANK1
} paj7620_bank_t;

static rt_uint8_t paj7620_init_regs[][2] =
{
    {0xEF, 0x00},
    {0x32, 0x29},
    {0x33, 0x01},
    {0x34, 0x00},
    {0x35, 0x01},
    {0x36, 0x00},
    {0x37, 0x07},
    {0x38, 0x17},
    {0x39, 0x06},
    {0x3A, 0x12},
    {0x3F, 0x00},
    {0x40, 0x02},
    {0x41, 0xFF},
    {0x42, 0x01},
    {0x46, 0x2D},
    {0x47, 0x0F},
    {0x48, 0x3C},
    {0x49, 0x00},
    {0x4A, 0x1E},
    {0x4B, 0x00},
    {0x4C, 0x20},
    {0x4D, 0x00},
    {0x4E, 0x1A},
    {0x4F, 0x14},
    {0x50, 0x00},
    {0x51, 0x10},
    {0x52, 0x00},
    {0x5C, 0x02},
    {0x5D, 0x00},
    {0x5E, 0x10},
    {0x5F, 0x3F},
    {0x60, 0x27},
    {0x61, 0x28},
    {0x62, 0x00},
    {0x63, 0x03},
    {0x64, 0xF7},
    {0x65, 0x03},
    {0x66, 0xD9},
    {0x67, 0x03},
    {0x68, 0x01},
    {0x69, 0xC8},
    {0x6A, 0x40},
    {0x6D, 0x04},
    {0x6E, 0x00},
    {0x6F, 0x00},
    {0x70, 0x80},
    {0x71, 0x00},
    {0x72, 0x00},
    {0x73, 0x00},
    {0x74, 0xF0},
    {0x75, 0x00},
    {0x80, 0x42},
    {0x81, 0x44},
    {0x82, 0x04},
    {0x83, 0x20},
    {0x84, 0x20},
    {0x85, 0x00},
    {0x86, 0x10},
    {0x87, 0x00},
    {0x88, 0x05},
    {0x89, 0x18},
    {0x8A, 0x10},
    {0x8B, 0x01},
    {0x8C, 0x37},
    {0x8D, 0x00},
    {0x8E, 0xF0},
    {0x8F, 0x81},
    {0x90, 0x06},
    {0x91, 0x06},
    {0x92, 0x1E},
    {0x93, 0x0D},
    {0x94, 0x0A},
    {0x95, 0x0A},
    {0x96, 0x0C},
    {0x97, 0x05},
    {0x98, 0x0A},
    {0x99, 0x41},
    {0x9A, 0x14},
    {0x9B, 0x0A},
    {0x9C, 0x3F},
    {0x9D, 0x33},
    {0x9E, 0xAE},
    {0x9F, 0xF9},
    {0xA0, 0x48},
    {0xA1, 0x13},
    {0xA2, 0x10},
    {0xA3, 0x08},
    {0xA4, 0x30},
    {0xA5, 0x19},
    {0xA6, 0x10},
    {0xA7, 0x08},
    {0xA8, 0x24},
    {0xA9, 0x04},
    {0xAA, 0x1E},
    {0xAB, 0x1E},
    {0xCC, 0x19},
    {0xCD, 0x0B},
    {0xCE, 0x13},
    {0xCF, 0x64},
    {0xD0, 0x21},
    {0xD1, 0x0F},
    {0xD2, 0x88},
    {0xE0, 0x01},
    {0xE1, 0x04},
    {0xE2, 0x41},
    {0xE3, 0xD6},
    {0xE4, 0x00},
    {0xE5, 0x0C},
    {0xE6, 0x0A},
    {0xE7, 0x00},
    {0xE8, 0x00},
    {0xE9, 0x00},
    {0xEE, 0x07},
    {0xEF, 0x01},
    {0x00, 0x1E},
    {0x01, 0x1E},
    {0x02, 0x0F},
    {0x03, 0x10},
    {0x04, 0x02},
    {0x05, 0x00},
    {0x06, 0xB0},
    {0x07, 0x04},
    {0x08, 0x0D},
    {0x09, 0x0E},
    {0x0A, 0x9C},
    {0x0B, 0x04},
    {0x0C, 0x05},
    {0x0D, 0x0F},
    {0x0E, 0x02},
    {0x0F, 0x12},
    {0x10, 0x02},
    {0x11, 0x02},
    {0x12, 0x00},
    {0x13, 0x01},
    {0x14, 0x05},
    {0x15, 0x07},
    {0x16, 0x05},
    {0x17, 0x07},
    {0x18, 0x01},
    {0x19, 0x04},
    {0x1A, 0x05},
    {0x1B, 0x0C},
    {0x1C, 0x2A},
    {0x1D, 0x01},
    {0x1E, 0x00},
    {0x21, 0x00},
    {0x22, 0x00},
    {0x23, 0x00},
    {0x25, 0x01},
    {0x26, 0x00},
    {0x27, 0x39},
    {0x28, 0x7F},
    {0x29, 0x08},
    {0x30, 0x03},
    {0x31, 0x00},
    {0x32, 0x1A},
    {0x33, 0x1A},
    {0x34, 0x07},
    {0x35, 0x07},
    {0x36, 0x01},
    {0x37, 0xFF},
    {0x38, 0x36},
    {0x39, 0x07},
    {0x3A, 0x00},
    {0x3E, 0xFF},
    {0x3F, 0x00},
    {0x40, 0x77},
    {0x41, 0x40},
    {0x42, 0x00},
    {0x43, 0x30},
    {0x44, 0xA0},
    {0x45, 0x5C},
    {0x46, 0x00},
    {0x47, 0x00},
    {0x48, 0x58},
    {0x4A, 0x1E},
    {0x4B, 0x1E},
    {0x4C, 0x00},
    {0x4D, 0x00},
    {0x4E, 0xA0},
    {0x4F, 0x80},
    {0x50, 0x00},
    {0x51, 0x00},
    {0x52, 0x00},
    {0x53, 0x00},
    {0x54, 0x00},
    {0x57, 0x80},
    {0x59, 0x10},
    {0x5A, 0x08},
    {0x5B, 0x94},
    {0x5C, 0xE8},
    {0x5D, 0x08},
    {0x5E, 0x3D},
    {0x5F, 0x99},
    {0x60, 0x45},
    {0x61, 0x40},
    {0x63, 0x2D},
    {0x64, 0x02},
    {0x65, 0x96},
    {0x66, 0x00},
    {0x67, 0x97},
    {0x68, 0x01},
    {0x69, 0xCD},
    {0x6A, 0x01},
    {0x6B, 0xB0},
    {0x6C, 0x04},
    {0x6D, 0x2C},
    {0x6E, 0x01},
    {0x6F, 0x32},
    {0x71, 0x00},
    {0x72, 0x01},
    {0x73, 0x35},
    {0x74, 0x00},
    {0x75, 0x33},
    {0x76, 0x31},
    {0x77, 0x01},
    {0x7C, 0x84},
    {0x7D, 0x03},
    {0x7E, 0x01},
};

/**
 * @brief read paj7620 register value
 *
 * @param dev device handle
 * @param addr register address
 * @param data the read data
 *
 * @return operation result
 */
static rt_err_t paj7620_read_reg(paj7620_device_t dev, rt_uint8_t addr, rt_uint8_t *data)
{
    struct rt_i2c_msg msgs[2];

    msgs[0].addr = PAJ7620_ID;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = &addr;
    msgs[0].len = 1;

    msgs[1].addr = PAJ7620_ID;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf = data;
    msgs[1].len = 1;

    if (rt_i2c_transfer(dev->i2c, msgs, 2) == 0)
    {
        return RT_ERROR;
    }
    else
    {
        return RT_EOK;
    }
}

/**
 * @brief write data value to paj7620 register
 *
 * @param dev device handle
 * @param addr register address
 * @param data register value to be written to paj7620
 *
 * @return operation result
 */
static rt_err_t paj7620_write_reg(paj7620_device_t dev, rt_uint8_t addr, uint8_t data)
{
    rt_uint8_t buf[2];

    buf[0] = addr;
    buf[1] = data;

    if (rt_i2c_master_send(dev->i2c, PAJ7620_ID, 0, buf, 2) == 2)
    {
        return RT_EOK;
    }
    else
    {
        return RT_ERROR;
    }
}

/**
 * @brief select paj7620 register bank
 *
 * @param dev device handle
 * @param bank bank to select
 *
 * @return operation result
 */
static rt_err_t paj7620_select_bank(paj7620_device_t dev, paj7620_bank_t bank)
{
    RT_ASSERT((bank == PAJ7620_BANK0) || (bank == PAJ7620_BANK1));

    switch (bank)
    {
    case PAJ7620_BANK0:
        if (paj7620_write_reg(dev, PAJ_BANK_SEL, PAJ7620_BANK0) == RT_EOK)
        {
            return RT_EOK;
        }
        break;

    case PAJ7620_BANK1:
        if (paj7620_write_reg(dev, PAJ_BANK_SEL, PAJ7620_BANK1) == RT_EOK)
        {
            return RT_EOK;
        }
        break;

    default:
        break;
    }

    return RT_ERROR;
}

/**
 * @brief get gesture
 *
 * @param dev device handle
 * @param gest the gesture state read from register
 *
 * @return operation result
 */
rt_err_t paj7620_get_gesture(paj7620_device_t dev, paj7620_gesture_t *gest)
{
    rt_uint8_t gesture;

    if (paj7620_read_reg(dev, PAJ_GET_INT_FLAG1, &gesture) == RT_EOK)
    {
        switch (gesture)
        {
        case GES_RIGHT_FLAG:
            rt_thread_mdelay(1);

            if (paj7620_read_reg(dev, PAJ_GET_INT_FLAG1, &gesture) == RT_EOK)
            {
                if (gesture == GES_FORWARD_FLAG)
                {
                    *gest = PAJ7620_GESTURE_FORWARD;
                }
                else if (gesture == GES_BACKWARD_FLAG)
                {
                    *gest = PAJ7620_GESTURE_BACKWARD;
                }
                else
                {
                    *gest = PAJ7620_GESTURE_RIGHT;
                }
            }
            else
            {
                return RT_ERROR;
            }
            break;

        case GES_LEFT_FLAG:
            rt_thread_mdelay(1);

            if (paj7620_read_reg(dev, PAJ_GET_INT_FLAG1, &gesture) == RT_EOK)
            {
                if (gesture == GES_FORWARD_FLAG)
                {
                    *gest = PAJ7620_GESTURE_FORWARD;
                }
                else if (gesture == GES_BACKWARD_FLAG)
                {
                    *gest = PAJ7620_GESTURE_BACKWARD;
                }
                else
                {
                    *gest = PAJ7620_GESTURE_LEFT;
                }
            }
            else
            {
                return RT_ERROR;
            }
            break;

        case GES_UP_FLAG:
            rt_thread_mdelay(1);

            if (paj7620_read_reg(dev, PAJ_GET_INT_FLAG1, &gesture) == RT_EOK)
            {
                if (gesture == GES_FORWARD_FLAG)
                {
                    *gest = PAJ7620_GESTURE_FORWARD;
                }
                else if (gesture == GES_BACKWARD_FLAG)
                {
                    *gest = PAJ7620_GESTURE_BACKWARD;
                }
                else
                {
                    *gest = PAJ7620_GESTURE_UP;
                }
            }
            else
            {
                return RT_ERROR;
            }
            break;

        case GES_DOWN_FLAG:
            rt_thread_mdelay(1);

            if (paj7620_read_reg(dev, PAJ_GET_INT_FLAG1, &gesture) == RT_EOK)
            {
                if (gesture == GES_FORWARD_FLAG)
                {
                    *gest = PAJ7620_GESTURE_FORWARD;
                }
                else if (gesture == GES_BACKWARD_FLAG)
                {
                    *gest = PAJ7620_GESTURE_BACKWARD;
                }
                else
                {
                    *gest = PAJ7620_GESTURE_DOWN;
                }
            }
            else
            {
                return RT_ERROR;
            }
            break;

        case GES_FORWARD_FLAG:
            *gest = PAJ7620_GESTURE_FORWARD;
            break;

        case GES_BACKWARD_FLAG:
            *gest = PAJ7620_GESTURE_BACKWARD;
            break;

        case GES_CLOCKWISE_FLAG:
            *gest = PAJ7620_GESTURE_CLOCKWISE;
            break;

        case GES_COUNT_CLOCKWISE_FLAG:
            *gest = PAJ7620_GESTURE_ANTICLOCKWISE;
            break;

        default:
            if (paj7620_read_reg(dev, PAJ_GET_INT_FLAG2, &gesture) == RT_EOK)
            {
                if (gesture == GES_WAVE_FLAG)
                {
                    *gest = PAJ7620_GESTURE_WAVE;
                }
                else
                {
                    *gest = PAJ7620_GESTURE_NONE;
                }
            }
            else
            {
                return RT_ERROR;
            }
            break;
        }
    }
    else
    {
        return RT_ERROR;
    }

    return RT_EOK;
}

/**
 * @brief wakeup paj7620
 * 
 * @param dev device handle
 * 
 * @return operation result 
 */
static rt_err_t paj7620_wakeup(paj7620_device_t dev)
{
    rt_uint8_t data0, data1;

    if (paj7620_select_bank(dev, PAJ7620_BANK0) != RT_EOK)
    {
        return RT_ERROR;
    }

    rt_thread_mdelay(1);

    if (paj7620_select_bank(dev, PAJ7620_BANK0) != RT_EOK)
    {
        return RT_ERROR;
    }

    if (paj7620_read_reg(dev, 0, &data0) != RT_EOK)
    {
        return RT_ERROR;
    }

    if (paj7620_read_reg(dev, 1, &data1) != RT_EOK)
    {
        return RT_ERROR;
    }

    if (data0 != 0x20 || data1 != 0x76)
    {
        LOG_E("paj7620 check failed!");
        return RT_ERROR;
    }

    if (data0 == 0x20)
    {
        LOG_I("paj7620 wakeup");
    }

    return RT_EOK;
}

/**
 * @brief import the default register setting to paj7620
 * 
 * @param dev device handle
 * 
 * @return operation result 
 */
static rt_err_t paj7620_register_init(paj7620_device_t dev)
{
    rt_uint8_t i;

    for (i = 0; i < sizeof(paj7620_init_regs) / sizeof(paj7620_init_regs[0]); i++)
    {
        if (paj7620_write_reg(dev, paj7620_init_regs[i][0], paj7620_init_regs[i][1]) != RT_EOK)
        {
            return RT_ERROR;
        }
    }

    return RT_EOK;
}

/**
 * @brief initialize the paj7620
 *
 * @param i2c_bus_name the name of i2c device
 * @return paj7620 device handle
 */
paj7620_device_t paj7620_init(const char *i2c_bus_name)
{
    paj7620_device_t dev;

    RT_ASSERT(i2c_bus_name);

    dev = rt_calloc(1, sizeof(struct paj7620_device));

    if (dev == RT_NULL)
    {
        LOG_E("Can't allocate memory for paj7620 device on '%s' ", i2c_bus_name);
        return RT_NULL;
    }

    dev->i2c = rt_i2c_bus_device_find(i2c_bus_name);

    if (dev->i2c == RT_NULL)
    {
        LOG_E("Can't find paj7620 device on '%s' ", i2c_bus_name);
        rt_free(dev);
        return RT_NULL;
    }

    dev->lock = rt_mutex_create("mutex_paj7620", RT_IPC_FLAG_FIFO);

    if (dev->lock == RT_NULL)
    {
        LOG_E("Can't create mutex for paj7620 device on '%s' ", i2c_bus_name);
        rt_free(dev);
        return RT_NULL;
    }

    if (paj7620_wakeup(dev) == RT_ERROR)
    {
        return RT_NULL;
    }

    if (paj7620_register_init(dev) == RT_ERROR)
    {
        return RT_NULL;
    }

    if (paj7620_select_bank(dev, PAJ7620_BANK0) == RT_ERROR)
    {
        return RT_NULL;
    }

    LOG_I("paj7620 finished the initialization");

    return dev;
}

/**
 * @brief deinitialize the paj7620
 *
 * @param dev device handle
 */
void paj7620_deinit(paj7620_device_t dev)
{
    RT_ASSERT(dev);

    rt_mutex_delete(dev->lock);
    rt_free(dev);
}

#endif

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************