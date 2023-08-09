#ifndef HX711leg_h
#define HX711leg_h

#define PD_SCKleg_PORT         PORTB	                        // Power Down and Serial Clock Input Port
#define PD_SCKleg_DDR          DDRB                            // Power Down and Serial Clock DDR
#define PD_SCKleg_PIN          PB5                             // Power Down and Serial Clock Pin

#define PD_SCKleg_SET_OUTPUT   PD_SCKleg_DDR |= (1<<PD_SCKleg_PIN)

#define PD_SCKleg_SET_HIGH     PD_SCKleg_PORT |= (1<<PD_SCKleg_PIN)
#define PD_SCKleg_SET_LOW      PD_SCKleg_PORT &= ~(1<<PD_SCKleg_PIN)

#define DOUTleg_PORT           PORTB                           // Serial Data Output Port
#define DOUTleg_DDR            DDRB                            // Serial Data Output DDR
#define DOUTleg_INPUT          PINB                            // Serial Data Output Input
#define DOUTleg_PIN            PB4 		                    // Serial Data Output Pin
#define DOUTleg_READ           (DOUTleg_INPUT & (1<<DOUTleg_PIN))    // Serial Data Output Read Pin

#define DOUTleg_SET_HIGH       DOUTleg_PORT |= (1<<DOUTleg_PIN)
#define DOUTleg_SET_LOW        DOUTleg_PORT &= ~(1<<DOUTleg_PIN)
#define DOUTleg_SET_INPUT      DOUTleg_DDR &= ~(1<<DOUTleg_PIN); DOUTleg_SET_HIGH
#define DOUTleg_SET_OUTPUT     DOUTleg_DDR |= (1<<DOUTleg_PIN); DOUTleg_SET_LOW

#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0)

uint8_t GAIN;		                // amplification factor
int32_t OFFSET;	                // used for tare weight
float SCALE;	                    // used to return weight in grams, kg, ounces, whatever

// define clock and data pin, channel, and gain factor
// channel selection is made by passing the appropriate gain: 128 or 64 for channel A, 32 for channel B
// gain: 128 or 64 for channel A; channel B works with 32 gain factor only
void HX711leg_init(uint8_t gain);

// check if HX711leg is ready
// from the datasheet: When output data is not ready for retrieval, digital output pin DOUTleg is high. Serial clock
// input PD_SCKleg should be low. When DOUTleg goes to low, it indicates data is ready for retrieval.
bool HX711leg_is_ready();

// set the gain factor; takes effect only after a call to read()
// channel A can be set for a 128 or 64 gain; channel B has a fixed 32 gain
// depending on the parameter, the channel is also set to either A or B
void HX711leg_set_gain(uint8_t gain);

// waits for the chip to be ready and returns a reading
int32_t HX711leg_read();

// returns an average reading; times = how many times to read
int32_t HX711leg_read_average(uint8_t times);

// returns (read_average() - OFFSET), that is the current value without the tare weight; times = how many readings to do
double HX711leg_get_value(uint8_t times);

// returns get_value() divided by SCALE, that is the raw value divided by a value obtained via calibration
// times = how many readings to do
float HX711leg_get_units(uint8_t times);

// set the OFFSET value for tare weight; times = how many times to read the tare value
void HX711leg_tare(uint8_t times);

// set the SCALE value; this value is used to convert the raw data to "human readable" data (measure units)
void HX711leg_set_scale(float scale);

// get the current SCALE
float HX711leg_get_scale();

// set OFFSET, the value that's subtracted from the actual reading (tare weight)
void HX711leg_set_offset(int32_t offset);

// get the current OFFSET
int32_t HX711leg_get_offset();

// puts the chip into power down mode
void HX711leg_power_down();

// wakes up the chip after power down mode
void HX711leg_power_up();

// Sends/receives data. Modified from Arduino source
uint8_t shiftIn(void);

#endif /* HX711leg_h */