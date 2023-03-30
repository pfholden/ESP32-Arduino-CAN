/* ESP32 Arduino CAN Send Basic Demo
 *     This example will send messages on the CAN bus
 *
 *     An external transceiver is required and should be connected 
 *     to the CAN_tx and CAN_rx gpio pins specified by CANInit. Be sure
 *     to use a 3.3V compatable transceiver such as the SN65HVD23x
 *
 */

#include <Arduino.h>
#include <ESP32CAN.h>


void setup() {
  Serial.begin(115200);
  Serial.println("ESP32-Arduino-CAN Send Basic Demo");

  /* initialize and start, use pin 5 as CAN_tx and pin 4 as CAN_rx, CAN bus is set to 125kbps */
  ESP32Can.initCAN(GPIO_NUM_5, GPIO_NUM_4, ESP32CAN_SPEED_125KBPS, TWAI_MODE_NO_ACK);
}

void loop() {
  uint32_t msg_id = 0;
  uint8_t dlc = 0;
  uint8_t data_bytes[TWAI_FRAME_MAX_DLC];
  
  
  dlc = 8;  /* send 8 bytes of data */
  msg_id = 0x123;    /* CAN id is 0x123 */

  /* assemble the 8 bytes of data */
  data_bytes[0] = 0xDE;
  data_bytes[1] = 0xAD;
  data_bytes[2] = 0xBE;
  data_bytes[3] = 0xEF;
  data_bytes[4] = 0xBA;
  data_bytes[5] = 0x5E;
  data_bytes[6] = 0xBA;
  data_bytes[7] = 0x11;


  ESP32Can.setCANMsg(msg_id, dlc, data_bytes); /* Configure standard CAN message. */

  ESP32Can.sendCANMsg(); /* send the CAN message */

  Serial.println("CAN Frame Sent");

  /* delay before sending another CAN message*/
  delay(1000);
}
