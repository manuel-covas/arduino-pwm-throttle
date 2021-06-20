#define PWM_OUTPUT_PIN         3  // Digital pin to output the PWM signal on. 
#define THROTTLE_PIN      PIN_A1  // Analog pin to read the throttle value from.
#define BRAKE_PIN         PIN_A2  // Analog pin to read the brake value from.

#define ADC_NEUTRAL_RANGE     10  // Neutral range above the neutral point.
#define ADC_MULTISAMPLE_COUNT 16  // ADC Multisample count when taking averaged readings.


/*
*  ADC Multisampling function
*/
int analogReadMultisample(uint8_t pin) {

    int sum = 0;

    for (uint8_t i = 0; i < ADC_MULTISAMPLE_COUNT; i++)
        sum += analogRead(pin);

    return (int) round(sum / ADC_MULTISAMPLE_COUNT);
}


/*
*  ADC Neutral points (read at boot)
*/
int throttle_neutral_point;
int brake_neutral_point;

void setup() {
    
    Serial.begin(9600);

    pinMode(PWM_OUTPUT_PIN,      OUTPUT);
    pinMode(THROTTLE_PIN, INPUT);
    pinMode(BRAKE_PIN,    INPUT);

    throttle_neutral_point = analogReadMultisample(THROTTLE_PIN);
    brake_neutral_point    = analogReadMultisample(BRAKE_PIN);
}


int throttle_reading;
int brake_reading;
int output_pwm_duty;

void loop() {
    
    output_pwm_duty = 0.5;
    throttle_reading = analogReadMultisample(THROTTLE_PIN);
    brake_reading    = analogReadMultisample(BRAKE_PIN);

    // Ignore throttle while braking.

    if (brake_reading > (brake_neutral_point + ADC_NEUTRAL_RANGE)) {
        
        output_pwm_duty = 128 - (brake_reading - brake_neutral_point) / 8;

    } else if (throttle_reading > (throttle_neutral_point + ADC_NEUTRAL_RANGE)) {
        
        output_pwm_duty = 128 + (throttle_reading - throttle_neutral_point) / 8;

    } else {

        output_pwm_duty = 128;
        
    }

    analogWrite(PWM_OUTPUT_PIN, output_pwm_duty);
    delay(10);
}