#include <pic12fxxxx.h>

// Define configuration bits for PIC12F
__CONFIG(FOSC_INTRCIO & WDTE_OFF & PWRTE_ON & MCLRE_OFF & CP_OFF & CPD_OFF & BOREN_OFF & IESO_OFF & FCMEN_OFF);

// Define constants
#define HALL_SENSOR_PIN    RA0 // Hall effect sensor input pin
#define PWM_PIN            RC0 // PWM output pin for controlling the actuator
#define PWM_PERIOD         255 // PWM period for desired frequency (adjust as needed)
#define PWM_DUTY_CYCLE_MAX 200 // Maximum duty cycle for PWM (adjust as needed)
#define PWM_DUTY_CYCLE_MIN 100 // Minimum duty cycle for PWM (adjust as needed)

// Function prototypes
void initialize(void);
void delay_ms(unsigned int ms);
void init_PWM(void);

void main() {
    unsigned int hall_sensor_value;
    unsigned int duty_cycle;

    initialize();
    init_PWM();

    while(1) {
        // Read the value from the Hall effect sensor
        hall_sensor_value = HALL_SENSOR_PIN;

        // Adjust duty cycle based on sensor value
        duty_cycle = hall_sensor_value * (PWM_DUTY_CYCLE_MAX - PWM_DUTY_CYCLE_MIN) / 1023 + PWM_DUTY_CYCLE_MIN;
        CCPR1L = duty_cycle >> 2; // Set PWM duty cycle
        CCP1CON &= 0xCF; // Clear bits 4 and 5
        CCP1CON |= ((duty_cycle & 0x03) << 4); // Set bits 4 and 5

        //logic for stability and control

        // Delay for stability
        delay_ms(100);
    }
}

void initialize(void) {
    // Configure pins as input or output
    TRISA = 0b00000001; // RA0 as input (Hall sensor)
    TRISC = 0b00000000; // All pins on port C as output
}

void init_PWM(void) {
    PR2 = PWM_PERIOD; // Set PWM period
    CCP1CON = 0b00001100; // Configure CCP1 module for PWM mode
    CCP1CONbits.DC1B = 0; // Initialize duty cycle to 0
    T2CON = 0b00000111; // Turn on Timer2 with prescaler 1:16
}

void delay_ms(unsigned int ms) {
    unsigned int i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 165; j++); // MCU frequency
}
