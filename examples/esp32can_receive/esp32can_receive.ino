/* ESP32 Arduino CAN Receive Basic Demo
 *     This example will receive messages on the CAN bus
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
  Serial.println("ESP32-Arduino-CAN Receive Basic Demo");

  /* Initialize and start, use pin 5 as CAN_tx and pin 4 as CAN_rx, CAN bus is set to 125kbps */
  ESP32Can.initCAN(GPIO_NUM_5, GPIO_NUM_4, ESP32CAN_SPEED_125KBPS);
}

void loop() {
  uint32_t msg_id = 0;
  uint8_t numOfDataBytes = 0;
  uint8_t CAN_flags = 0;
  uint8_t data_bytes[TWAI_FRAME_MAX_DLC];
  
  if (ESP32CAN_OK == ESP32Can.readCANMsg(&msg_id, &CAN_flags, &numOfDataBytes, data_bytes )) {  /* only print when CAN message is received*/
    Serial.print(msg_id, HEX);               /* print the CAN ID*/
    Serial.print(" ");
    Serial.print(numOfDataBytes);              /* print number of bytes in data frame*/
    Serial.print(" ");
    
    for (int i=0; i<numOfDataBytes; i++) {     /* print the data frame*/
      Serial.print(data_bytes[i], HEX);
    }

    Serial.println();
  } else Serial.println("No message");
  
  delay(1000);
}

