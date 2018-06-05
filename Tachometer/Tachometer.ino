/*
   Thank for original source code from http://www.glennsweeney.com/tutorials/interrupt-driven-analog-conversion-with-an-atmega328p
*/
byte readFlag = 0;
int analogVal = 0;

float avg = 0;
float strenght = 0;
int pulse_round = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  adc_init();
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

}

unsigned long timer;
unsigned long first_measure_time;
unsigned long last_measure_time;
byte sensor_state = 0;
byte measure_state = 0;

void loop() {

  if (readFlag == 1) {
    readFlag = 0;
    strenght  = abs(float(analogVal - avg)) * 0.005 + 0.995 * strenght;
  }
  if (millis() - timer >= 1000) {
    timer += 1000;
    /*
      Serial.print(pulse_round * 60);
      Serial.print("\t");
      Serial.print(last_measure_time - first_measure_time);
      Serial.print("\t");
      Serial.println(strenght);
    */
    float period = (last_measure_time - first_measure_time) / 1000.0;
    float RPS = pulse_round / period;
    float RPM = RPS * 60;
    Serial.print(RPM);
    Serial.print("\t");
    Serial.print(period);
    Serial.print("\t");
    Serial.println(strenght);
    pulse_round = 0;
    measure_state = 0;
  }
}
ISR(ADC_vect) {

  readFlag = 1;
  analogVal = ADCL | (ADCH << 8);//alway read Low byte first!!
  avg = float(analogVal) * 0.005 + 0.995 * avg;
  if (analogVal > avg + 15 && sensor_state == 0) sensor_state = 1;
  
  else if (analogVal < avg - 15 && sensor_state == 1) {
    sensor_state = 0;
    if (millis() - timer < 1000) {
      if (measure_state == 0) {
        measure_state = 1;
        first_measure_time = millis();
        last_measure_time = millis() + 1;
        pulse_round = 0;
      }
      else {
        pulse_round++;
        last_measure_time = millis();
      }
    }
  }
}
