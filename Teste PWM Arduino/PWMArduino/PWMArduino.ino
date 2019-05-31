int pwmPin= 9;      // Saida PWM
int PWM_Value = 127;

void setup()

{

  pinMode(pwmPin, OUTPUT);   // sets the pin as output

}

void loop()

{
  analogWrite(pwmPin, 127);  // analogWrite values from 0 to 255
  delay(1000);
  analogWrite(pwmPin, 180);  // analogWrite values from 0 to 255
  delay(1000);
  analogWrite(pwmPin, 255);  // analogWrite values from 0 to 255
  delay(1000);
  analogWrite(pwmPin, 100);  // analogWrite values from 0 to 255
  delay(1000);

}
