#include <xc.h>

// Configuration bits
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// Oscillator Selection
#define _XTAL_FREQ 16000000  // 16 MHz internal oscillator

void initSystem() {
    // Initialize the oscillator control
    OSCCON = 0x78;  // Set oscillator to 16MHz

    // Set up GPIO
    TRISA = 0b00000001;  // RA0 as input (AN0 for ADC), others as output
    ANSELA = 0b00000001; // RA0 as analog

    // Initialize PWM on RA2 (CCP1)
    TRISA2 = 0;          // Set RA2 as output
    APFCON0bits.CCP1SEL = 1;  // Select RA2 as the output pin for CCP1
}

void initADC() {
    ADCON0 = 0x01;  // Channel AN0, ADC On
    ADCON1 = 0xA0;  // FOSC/32, left justified (only read ADRESH for 8-bit result)
    ADRESL = 0;     // Clear ADRESL
    ADRESH = 0;     // Clear ADRESH
}

void initPWM() {
    PR2 = 0xFF;     // Set the PWM period
    CCP1CON = 0x0C; // PWM mode, no decimal (00)
    CCPR1L = 0x7F;  // Set duty cycle to 50%
    PIR1bits.TMR2IF = 0;    // Clear timer2 interrupt flag
    T2CON = 0x04;   // TMR2 on, prescale = 1
    while (!PIR1bits.TMR2IF);  // Wait for timer overflow (update of PWM)
}

unsigned int readADC() {
    __delay_us(5);       // Wait for the holding capacitor to charge
    ADCON0bits.GO_nDONE = 1;  // Start conversion
    while (ADCON0bits.GO_nDONE);  // Wait for conversion to complete
    return ((unsigned int)ADRESH) << 8 | ADRESL;  // Return 10-bit result
}

void updatePWM(unsigned int duty) {
    CCPR1L = duty >> 2;      // Update duty cycle (upper 8 bits of 10-bit value)
    CCP1CONbits.DC1B = duty & 0x03;  // Update lower 2 bits of duty cycle
}

unsigned int calculateDutyCycle(unsigned int sensorValue) {
    // based on the model
    unsigned int targetValue = 512;  // Example target value for the sensor
    int error = targetValue - sensorValue;
    unsigned int duty = 128 + error;  // Calculate new duty cycle
    if (duty > 255) duty = 255;  // Clamp the value between 0 and 255
    if (duty < 0) duty = 0;
    return duty;
}

int main(void) {
    initSystem();
    initADC();
    initPWM();

    while(1) {
        unsigned int sensorValue = readADC();  // Read the Hall sensor
        unsigned int dutyCycle = calculateDutyCycle(sensorValue);  // Compute the duty cycle
        updatePWM(dutyCycle);  // Update PWM to control the electromagnet
        __delay_ms(10);  // Delay for stability
    }

    return 0;
}
