void setup()
{
  //Setup the local serial interface
  Serial.begin(9600);

  //Setup the wireless interface
  Serial0.begin(115200);
  
  // Initialize the reflectance sensors module
  reflectanceSensors.init();

  // Wait for the user button to be pressed and released
  Serial.println("ready to calibrate");
  Serial0.println("ready to calibrate");
  button.waitForButton();
  Serial.println("calibrating");
  Serial0.println("calibrating");

  // Turn on LED to indicate we are in calibration mode
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // Wait 1 second and then begin automatic sensor calibration
  // by rotating in place to sweep the sensors over the line

  // You should update this section to automate the calibration process
  // Edit here------------------
  
  delay(1000);
  int i;
  for(i = 0; i < 500; i++)
  {
    reflectanceSensors.calibrate();
    delay(20);
  }

  // Finish editing here--------
  Serial.println("calibration complete");
  Serial0.println("calibration complete");

  // Turn off LED to indicate we are through with calibration
  digitalWrite(13, LOW);

  
  // Wait for the user button to be pressed and released
  Serial.println("ready to begin");
  Serial0.println("ready to begin");
  button.waitForButton();
  Serial.println("start");
  Serial0.println("start");

  // Delay for one second
  delay(1000);
}


void loop()
{
  selectState();
  delay(50);
}
