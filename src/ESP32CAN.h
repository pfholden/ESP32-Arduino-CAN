#ifndef INC_ESP32CAN_H
#define INC_ESP32CAN_H

#include <Arduino.h>
#include "driver/gpio.h"
#include "driver/twai.h"

/* Defines ------------------------------------------------------------------- */
#define ESP32CAN_DEBUG     /* serial print debug info */

/* Macros -------------------------------------------------------------------- */
#ifdef ESP32CAN_DEBUG
#define debugPrint(x) 	    Serial.print(x)
#define debugPrintln(x)	    Serial.println(x)
#else
#define debugPrint(x)
#define debugPrintln(x)
#endif

/* typedef ------------------------------------------------------------------- */
typedef enum {
    ESP32CAN_NOK = 0,  /* not ok, something is wrong */
    ESP32CAN_OK  = 1   /* ok, all seems well */
} ESP32CAN_status_t;

typedef enum {
    ESP32CAN_SPEED_100KBPS = 100,
    ESP32CAN_SPEED_125KBPS = 125,
    ESP32CAN_SPEED_250KBPS = 250,
    ESP32CAN_SPEED_500KBPS = 500,
    ESP32CAN_SPEED_800KBPS = 800,
    ESP32CAN_SPEED_1MBPS   = 1000,
} ESP32CAN_timing_t;

/* Globals ------------------------------------------------------------------- */

/* Function Prototypes ------------------------------------------------------- */

/* Class --------------------------------------------------------------------- */
class ESP32CAN {
    public:
        ESP32CAN_status_t CANInit();
        ESP32CAN_status_t CANInit(ESP32CAN_timing_t busSpeed);
        ESP32CAN_status_t CANInit(gpio_num_t tx_pin, gpio_num_t rx_pin, ESP32CAN_timing_t speed);
        ESP32CAN_status_t CANInit(gpio_num_t tx_pin, gpio_num_t rx_pin, ESP32CAN_timing_t speed, twai_mode_t MODE);
        ESP32CAN_status_t CANStop();
        ESP32CAN_status_t CANWriteFrame(const twai_message_t* p_frame);
        ESP32CAN_status_t CANReadFrame(twai_message_t* p_frame);

        // int CANConfigFilter(const CAN_filter_t* p_filter);

    private:
        /* Default setup TX on GPIO5, RX on GPIO4, Normal Mode, accept all data frames (no filter configured), 
             Bus speed set to 250Kbps, RX & TX queues both set to 5. */
        twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_5, GPIO_NUM_4, TWAI_MODE_NORMAL);
        twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
        twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
        
        ESP32CAN_status_t setBusSpeed(ESP32CAN_timing_t speed);
        ESP32CAN_status_t startCANBus();

};

/* External Globals ---------------------------------------------------------- */
extern ESP32CAN ESP32Can;

#endif
