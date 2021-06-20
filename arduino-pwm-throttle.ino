#define PWM_PIN      3
#define THROTTLE_PIN PIN_A1
#define BRAKE_PIN    PIN_A2

#define ADC_NEUTRAL_RANGE 10


// Multisampling to provide averaged readings

#define ADC_MULTISAMPLE_COUNT 16

int analogReadMultisample(uint8_t pin) {

    int sum = 0;

    for (uint8_t i = 0; i < ADC_MULTISAMPLE_COUNT; i++)
        sum += analogRead(pin);

    return (int) round(sum / ADC_MULTISAMPLE_COUNT);
}


// sprintf wraper

#include <stdarg.h>
void serial_printf(char *fmt, ... ){
        char buf[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(buf, 128, fmt, args);
        va_end (args);
        Serial.print(buf);
}


struct {
    int throttle;
    int brake;
} adc_neutral_points;


void setup() {
    
    Serial.begin(9600);

    pinMode(PWM_PIN,      OUTPUT);
    pinMode(THROTTLE_PIN, INPUT);
    pinMode(BRAKE_PIN,    INPUT);

    adc_neutral_points.throttle = analogReadMultisample(THROTTLE_PIN);
    adc_neutral_points.brake    = analogReadMultisample(BRAKE_PIN);
}


int throttle_reading;
int brake_reading;
int output_pwm_duty;

void loop() {
    
    output_pwm_duty = 0.5;
    throttle_reading = analogReadMultisample(THROTTLE_PIN);
    brake_reading    = analogReadMultisample(BRAKE_PIN);

    if (brake_reading > (adc_neutral_points.brake + 10)) {

        // Ignore throttle if braking.
        output_pwm_duty = 128 - (brake_reading - adc_neutral_points.brake) / 8;

    } else if (throttle_reading > (adc_neutral_points.throttle + 10)) {
        
        output_pwm_duty = 128 + (throttle_reading - adc_neutral_points.throttle) / 8;

    } else {
        output_pwm_duty = 128;
    }

    analogWrite(PWM_PIN, output_pwm_duty);
    delay(10);
}