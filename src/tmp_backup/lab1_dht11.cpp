#include <Arduino.h>

/**
 * Lab 1: DHT11 Sensor reading using AVR Registers.
 * Pin: 2 (Digital Pin 2 on Arduino Uno corresponds to PORTD/PIND/DDRD pin 2).
 */

#define DHT_PIN 2

// Function prototypes
void dht_start();
uint8_t dht_check_response();
uint8_t dht_read_byte();

void setup() {
    Serial.begin(9600);
}

void loop() {
    uint8_t data[5];
    
    dht_start();
    if (dht_check_response()) {
        for (int i = 0; i < 5; i++) {
            data[i] = dht_read_byte();
        }

        // Checksum validation: byte0 + byte1 + byte2 + byte3 == byte4
        if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) == data[4]) {
            Serial.print("Humidity: ");
            Serial.print(data[0]);
            Serial.print(".");
            Serial.print(data[1]);
            Serial.print("%  Temperature: ");
            Serial.print(data[2]);
            Serial.print(".");
            Serial.print(data[3]);
            Serial.println("C");
        } else {
            Serial.println("Checksum Error!");
        }
    } else {
        Serial.println("DHT11 Not Responding!");
    }

    delay(2000); // DHT11 needs at least 1-2 seconds between readings
}

void dht_start() {
    DDRD |= (1 << DDD2);   // Set PD2 (Pin 2) as Output
    PORTD &= ~(1 << PORTD2); // Set PD2 to LOW
    delay(18);             // Start signal: keep LOW for at least 18ms
    PORTD |= (1 << PORTD2);  // Set PD2 to HIGH
    delayMicroseconds(30); // Wait for DHT response
    DDRD &= ~(1 << DDD2);  // Set PD2 as Input
}

uint8_t dht_check_response() {
    delayMicroseconds(40);
    if (!(PIND & (1 << PIND2))) { // Check if PD2 is LOW
        delayMicroseconds(80);
        if (PIND & (1 << PIND2)) { // Check if PD2 is HIGH
            delayMicroseconds(50);
            return 1;
        }
    }
    return 0;
}

uint8_t dht_read_byte() {
    uint8_t result = 0;
    for (int i = 0; i < 8; i++) {
        while (!(PIND & (1 << PIND2))); // Wait for pin to go HIGH
        delayMicroseconds(30);        // Wait 30us
        if (PIND & (1 << PIND2)) {    // If still HIGH after 30us, it's a '1'
            result |= (1 << (7 - i));
        }
        while (PIND & (1 << PIND2));    // Wait for pin to go LOW
    }
    return result;
}
