//*****************************************************************************
// file        : paj7620_samples.c
// paj7620 sample
// Copyright (c) 2011-2020 HSAE co. Ltd. All rights reserved
//
// Change Logs:
// Date                 Author      Note
// 2020/07/08           Cheney      First draft version
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup paj7620
//! @{
//
//*****************************************************************************
#include "paj7620.h"

#ifdef PAJ7620_USING_SAMPLES
/**< test thread handle */
static rt_thread_t tid1 = RT_NULL;

/**< test device handle */
static paj7620_device_t test_dev = RT_NULL;

char *gesture_string[] =
{
    "up",
    "down",
    "left",
    "right",
    "forward",
    "backward",
    "clockwise",
    "anticlockwise",
    "wave",
};

/**
 * @brief paj7620 gesture detection thread
 *
 * @param parameter input paramters
 */
static void paj7620_entry(void *parameter)
{
    paj7620_gesture_t gesture;

    while (1)
    {
        if (paj7620_get_gesture(test_dev, &gesture) == RT_EOK)
        {
            if (gesture < PAJ7620_GESTURE_NONE)
            {
                rt_kprintf("Detected gesture: %s\r\n", gesture_string[gesture]);
            }
        }

        rt_thread_mdelay(50);
    }
}

/**
 * @brief paj7620 msh command
 *
 * @param argc argument counter
 * @param argv argument value
 */
void paj7620(int argc, char *argv[])
{
    if (argc > 1)
    {
        if (!rt_strcmp(argv[1], "probe"))
        {
            if (argc > 2)
            {
                /* initialize the sensor when first probe */
                if (!test_dev || rt_strcmp(test_dev->i2c->parent.parent.name, argv[2]))
                {
                    /* deinit the old device */
                    if (test_dev)
                    {
                        paj7620_deinit(test_dev);
                    }

                    test_dev = paj7620_init(argv[2]);
                }
            }
            else
            {
                rt_kprintf("paj7620 probe <dev_name> - probe sensor by given name\n");
            }
        }
        else if (!rt_strcmp(argv[1], "open"))
        {
            if (test_dev)
            {
                tid1 = rt_thread_create("paj7620", paj7620_entry, RT_NULL, 1024, 20, 1);

                if (tid1 != RT_NULL)
                {
                    rt_thread_startup(tid1);
                }
            }
        }
        else if (!rt_strcmp(argv[1], "close"))
        {
            if (tid1 != RT_NULL)
            {
                rt_thread_delete(tid1);
                tid1 = RT_NULL;
            }
        }
        else
        {
            rt_kprintf("Usage:\n");
            rt_kprintf("paj7620 probe <dev_name>   - probe paj7620 by given name\n");
            rt_kprintf("paj7620 open               - open paj7620 gesture detection\n");
            rt_kprintf("paj7620 close              - close paj7620 gesture detection\n");
        }
    }
}
MSH_CMD_EXPORT(paj7620, paj7620 gesture function);
#endif

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************