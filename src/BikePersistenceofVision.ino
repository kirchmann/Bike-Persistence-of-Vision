const byte ledPin = 13;
const byte magneticSensorPin = 2;
volatile uint16_t timePerRevolution;
volatile int currentSegment;
uint8_t nrOfSegments = 35; // how many segments to divide an entire revolution into

#include <FastLED.h>
#define LED_PIN 5
#define NUM_LEDS 50
#define BRIGHTNESS 64
#define LED_TYPE WS2811 // check upon arrival
#define COLOR_ORDER GRB // check upon arrival
CRGB leds[NUM_LEDS];

void initTimers() {
  cli(); // disable global interrupts
  // Clear timer registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR2A = 0;
  TCCR2B = 0;
  
  // Timer1: time for full rotation of wheel
  TCCR1B = (1 << CS12) | (0 << CS11) | (1 << CS10); //set TCNT1(divide the clock with 1024)
  EIMSK = 1<<INT0; //enable ISR 16 bit timer
  EICRA = 1<<ISC01 | 0<<ISC00; //Triggers on falling edge 

  // Timer2: 8 bit timer used to divide the entire cycle into
  TCCR2A =  (0 << WGM22) | (1 << WGM21) | (0 << WGM20); //set to CTC (Clear timer on compare match) compare to value stored in OCR2A
  TCCR2B = (1 << CS12) | (1 << CS11) | (1 << CS10); //divide with 128
  TIMSK2 = 1<<OCIE2A; //enable ISR2 8 bit timer
  OCR2A = 0b11111111;//has to be set after each cycle for timer1, this is just a random initial value
  TCNT1 = 0;
  sei();

  timePerRevolution = 0;
  currentSegment = 0;
}
void initFastLED(){
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
  pinMode(ledPin, OUTPUT);
}

void setup() {
  delay(600); // power-up safety delay
  initFastLED();
  pinMode(magneticSensorPin, INPUT);
  //attachInterrupt(digitalPinToInterrupt(magneticSensorPin), handleFullRotationDone, CHANGE );
  initTimers();
  Serial.begin(9600);
}

ISR(TIMER2_COMPA_vect){
  currentSegment++;
  currentSegment %= nrOfSegments;
  /*
  CTC sets it to 0 when comparer
  update pixels
  compares t0 OCR2A
  */
}

//void handleFullRotationDone() {
ISR(INT0_vect){
  // restart timer for entire rotation
  // calculate time it took for the rotation
  
  // set segment counter to  timePerRevolution /
  //print timePerSegment, timePerRevolution, currentSegment
  timePerRevolution = TCNT1;
  //set timer2 OCR2A interrupt value
  OCR2A = timePerRevolution/nrOfSegments; //divide one rotation into 33 parts
  TCNT1 = 0;
  currentSegment = 0; // restart what currentSegment we are on.
}

void loop() {
  //Serial.print("currentsegment: ");
  //Serial.println(currentSegment);
  Serial.print("timePerRevolution: ");
  Serial.println(timePerRevolution);
  
  delay(300);
  // do some pattern that is depending on currentSegment/nrOfSegments
}
