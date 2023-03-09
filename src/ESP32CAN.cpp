#include "ESP32CAN.h"

/*TODO: add details constructor and have current constructor just call it with defaults*/


/* Init method to start CAN with defaults. */
ESP32CAN_status_t ESP32CAN::CANInit(){
    return startCANBus();
}

/* Initilizer for loading default conifg but changing the bus spped. */
ESP32CAN_status_t ESP32CAN::CANInit(ESP32CAN_timing_t busSpeed){
    ESP32CAN_status_t ret = ESP32CAN_OK;

    if (setBusSpeed(busSpeed)){
        ret = startCANBus();
    }else ret = ESP32CAN_NOK;
    return ret;
}

/* Overload simple init method. Most programs will probably just use this. */
ESP32CAN_status_t ESP32CAN::CANInit(gpio_num_t tx_pin, gpio_num_t rx_pin, ESP32CAN_timing_t speed){
    return CANInit(tx_pin, rx_pin, speed, TWAI_MODE_NORMAL);
}

/*Overload detailed init method. Allows more control over TWAI (CAN) initialization parameters. 
  
  Mode paramter details from SDK:
    TWAI_MODE_NORMAL           < Normal operating mode where TWAI controller can send/receive/acknowledge messages 
    TWAI_MODE_NO_ACK           < Transmission does not require acknowledgment. Use this mode for self testing 
    TWAI_MODE_LISTEN_ONLY      < The TWAI controller will not influence the bus (No transmissions or acknowledgments) but can receive messages
 */

ESP32CAN_status_t ESP32CAN::CANInit(gpio_num_t tx_pin, gpio_num_t rx_pin, ESP32CAN_timing_t speed, twai_mode_t mode) {
 
    ESP32CAN_status_t ret = ESP32CAN_OK;

    if (setBusSpeed(speed)){
        g_config = TWAI_GENERAL_CONFIG_DEFAULT(tx_pin, rx_pin, mode);
        ret = startCANBus();
    }else ret = ESP32CAN_NOK;
    return ret;
}


ESP32CAN_status_t ESP32CAN::CANStop() {
    /* stop the TWAI driver */
    switch (twai_stop()) {
        case ESP_OK:
            debugPrintln("TWAI STOP: ok");
            break;
        case ESP_ERR_INVALID_STATE:
            debugPrintln("TWAI STOP: ESP_ERR_INVALID_STATE");
            return ESP32CAN_NOK;
            break;
        default:
            debugPrintln("TWAI STOP: unknow error");
            return ESP32CAN_NOK;
            break;
    }

    /* uninstall TWAI driver */
    switch (twai_driver_uninstall()) {
        case ESP_OK:
            debugPrintln("TWAI UNINSTALL: ok");
            break;
        case ESP_ERR_INVALID_STATE:
            debugPrintln("TWAI UNINSTALL: ESP_ERR_INVALID_STATE");
            return ESP32CAN_NOK;
            break;
        default:
            break;
    }

    return ESP32CAN_OK;
}



ESP32CAN_status_t ESP32CAN::CANWriteFrame(const twai_message_t* p_frame) {
    /* queue message for transmission */
    switch (twai_transmit(p_frame, pdMS_TO_TICKS(10))) {
        case ESP_OK:
            break;
        case ESP_ERR_INVALID_ARG:
            debugPrintln("TWAI TX: ESP_ERR_INVALID_ARG");
            return ESP32CAN_NOK;
            break;
        case ESP_ERR_TIMEOUT:
            debugPrintln("TWAI TX: ESP_ERR_TIMEOUT");
            return ESP32CAN_NOK;
            break;
        case ESP_FAIL:
            debugPrintln("TWAI TX: ESP_FAIL");
            return ESP32CAN_NOK;
            break;
        case ESP_ERR_INVALID_STATE:
            debugPrintln("TWAI TX: ESP_ERR_INVALID_STATE");
            return ESP32CAN_NOK;
            break;
        case ESP_ERR_NOT_SUPPORTED:
            debugPrintln("TWAI TX: ESP_ERR_NOT_SUPPORTED");
            return ESP32CAN_NOK;
            break;
        default:
            debugPrintln("TWAI TX: unknow error");
            return ESP32CAN_NOK;
            break;
    }

    return ESP32CAN_OK;
}

ESP32CAN_status_t ESP32CAN::CANReadFrame(twai_message_t* p_frame) {
    switch (twai_receive(p_frame, pdMS_TO_TICKS(10))) {
    case ESP_OK:
        break;
    case ESP_ERR_TIMEOUT:
        debugPrintln("TWAI RX: ESP_ERR_TIMEOUT");
        return ESP32CAN_NOK;
        break;
    case ESP_ERR_INVALID_ARG:
        debugPrintln("TWAI RX: ESP_ERR_INVALID_ARG");
        return ESP32CAN_NOK;
        break;
    case ESP_ERR_INVALID_STATE:
        debugPrintln("TWAI RX: ESP_ERR_INVALID_STATE");
        return ESP32CAN_NOK;
        break;
    default:
        debugPrintln("TWAI RX: unknow error");
        return ESP32CAN_NOK;
        break;
    }

    return ESP32CAN_OK;
}

/**< Number of messages queued for transmission or awaiting transmission completion */
uint32_t ESP32CAN::getTxQueueDepth(){
    loadCANStatus();
    return status_info.msgs_to_tx;
} 

/**< Number of messages in RX queue waiting to be read */
uint32_t ESP32CAN::getRxQueueDepth(){
    loadCANStatus();
    return status_info.msgs_to_rx;
} 

/**< Current value of Transmit Error Counter */
uint32_t ESP32CAN::getTxErrCount(){
    loadCANStatus();
    return status_info.tx_error_counter;
}

/**< Current value of Receive Error Counter */
uint32_t ESP32CAN::getRxErrCount(){
    loadCANStatus();
    return status_info.rx_error_counter;
}

/**< Number of messages that failed transmissions */
uint32_t ESP32CAN::getTxFailedCount(){
    loadCANStatus();
    return status_info.tx_failed_count;
} 

/**< Number of messages that were lost due to a full RX queue (or errata workaround if enabled) */
uint32_t ESP32CAN::getRxMissCount(){
    loadCANStatus();
    return status_info.rx_missed_count;
}     

/**< Number of messages that were lost due to a RX FIFO overrun */
uint32_t ESP32CAN::getRxOverrunCount(){
    loadCANStatus();
    return status_info.rx_overrun_count;
}       

/**< Number of instances arbitration was lost */
uint32_t ESP32CAN::getArbLostCount(){
    loadCANStatus();
    return status_info.arb_lost_count;
}

/**< Number of instances a bus error has occurred */
uint32_t ESP32CAN::getBusErrCount(){
    loadCANStatus();
    return status_info.bus_error_count;
}        


// int ESP32CAN::CANConfigFilter(const CAN_filter_t* p_filter)
// {
//     return CAN_config_filter(p_filter);
// }


ESP32CAN ESP32Can;


/*
    Private methods
*/


ESP32CAN_status_t ESP32CAN::startCANBus(){
    /* install TWAI driver */
    switch (twai_driver_install(&g_config, &t_config, &f_config)) {
        case ESP_OK:
            debugPrintln("TWAI INSTALL: ok");
            break;
        case ESP_ERR_INVALID_ARG:
            debugPrintln("TWAI INSTALL: ESP_ERR_INVALID_ARG");
            return ESP32CAN_NOK;
            break;
        case ESP_ERR_NO_MEM:
            debugPrintln("TWAI INSTALL: ESP_ERR_NO_MEM");
            return ESP32CAN_NOK;
            break;
        case ESP_ERR_INVALID_STATE:
            debugPrintln("TWAI INSTALL: ESP_ERR_INVALID_STATE");
            return ESP32CAN_NOK;
            break;
        default:
            debugPrintln("TWAI INSTALL: uknown error");
            return ESP32CAN_NOK;
            break;
    }

    /* start TWAI driver */
    switch (twai_start()) {
        case ESP_OK:
            debugPrintln("TWAI START: ok");
            break;
        case ESP_ERR_INVALID_STATE:
            debugPrintln("TWAI START: ESP_ERR_INVALID_STATE");
            return ESP32CAN_NOK;
            break;
        default:
            debugPrintln("TWAI START: uknown error");
            return ESP32CAN_NOK;
            break;
    }

    return ESP32CAN_OK;
}


ESP32CAN_status_t ESP32CAN::setBusSpeed( ESP32CAN_timing_t speed){
    ESP32CAN_status_t ret = ESP32CAN_OK;

    switch (speed) {
        case ESP32CAN_SPEED_100KBPS:
            t_config = TWAI_TIMING_CONFIG_100KBITS();
            break;
        case ESP32CAN_SPEED_125KBPS:
            t_config = TWAI_TIMING_CONFIG_125KBITS();
            break;
        case ESP32CAN_SPEED_250KBPS:
            t_config = TWAI_TIMING_CONFIG_250KBITS();
            break;
        case ESP32CAN_SPEED_500KBPS:
            t_config = TWAI_TIMING_CONFIG_500KBITS();
            break;
        case ESP32CAN_SPEED_800KBPS:
            t_config = TWAI_TIMING_CONFIG_800KBITS();
            break;
        case ESP32CAN_SPEED_1MBPS:
            t_config = TWAI_TIMING_CONFIG_1MBITS();
            break;
        default:
            debugPrintln("TWAI: undefined buad rate");
            ret = ESP32CAN_NOK;
            break;
    }
    return ret;
}

ESP32CAN_status_t ESP32CAN::loadCANStatus(){
    ESP32CAN_status_t ret = ESP32CAN_OK;

    if(twai_get_status_info(&status_info) != ESP_OK ) ret = ESP32CAN_NOK;
   
    return ret;
}
