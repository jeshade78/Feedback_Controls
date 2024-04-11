
volatile byte half_revolutions; //Variable for counting number of half revolutions detected by sensor
 unsigned int rpm; 
 unsigned long timeold; //Variable for storing previous time(ms)
 double hall_effect; //Variable for storing analog reading of hall effect sensor
 void setup()
 {
   Serial.begin(115200);
   attachInterrupt(0, magnet_detect, RISING);//Initialize the intterrupt pin (Arduino digital pin 2)
   pinMode(A6, INPUT);
   pinMode(3, OUTPUT);
   half_revolutions = 0;
   rpm = 0;
   timeold = 0;
   hall_effect = 0;
 }
 void loop()//Measure RPM
 {
   if (half_revolutions >= 20) { 
     rpm = 30*1000/(millis() - timeold)*half_revolutions;
     timeold = millis();
     half_revolutions = 0;
     //Serial.println(rpm,DEC);
   }
 }
 void magnet_detect()//This function is called whenever a magnet/interrupt is detected by the arduino
 {
    half_revolutions++;
    Serial.println("detect");
    hall_effect = analogRead(A6);
    if (hall_effect >= 655 || hall_effect <= 368)
    {
      digitalWrite(3, LOW);
    }
    else
    {
      digitalWrite(3, HIGH);
    }
 }
