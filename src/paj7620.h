//*****************************************************************************
// file        : paj7620.h
// paj7620 device package
//
// Change Logs:
// Date                 Author      Note
// 2020/07/05           Cheney      First draft version
//
//*****************************************************************************
#ifndef __PAJ7620_H__
#define __PAJ7620_H__

//*****************************************************************************
//
//! \addtogroup paj7620
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include <rtthread.h>
#include <rtdevice.h>

struct paj7620_device
{
    struct rt_i2c_bus_device *i2c;
    rt_mutex_t lock;
};
typedef struct paj7620_device *paj7620_device_t;

typedef enum
{
    PAJ7620_GESTURE_UP,
    PAJ7620_GESTURE_DOWN,
    PAJ7620_GESTURE_LEFT,
    PAJ7620_GESTURE_RIGHT,
    PAJ7620_GESTURE_FORWARD,
    PAJ7620_GESTURE_BACKWARD,
    PAJ7620_GESTURE_CLOCKWISE,
    PAJ7620_GESTURE_ANTICLOCKWISE,
    PAJ7620_GESTURE_WAVE,
    PAJ7620_GESTURE_NONE
} paj7620_gesture_t;

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
paj7620_device_t paj7620_init(const char *i2c_bus_name);
void paj7620_deinit(paj7620_device_t dev);
rt_err_t paj7620_get_gesture(paj7620_device_t dev, paj7620_gesture_t *gest);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************

#ifdef __cplusplus
}
#endif

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
#endif //  __PAJ7620_H__