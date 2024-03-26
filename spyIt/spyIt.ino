//pin mappings
const int joystickX = 5;
const int joystickY = 4;
const int yellowButton = 7; 
const int blueButton = 6;
const int missileSwitch = 5;
const int redLED = 0;
const int greenLED = 1; 
const int topBlue = 10;
const int bottomBlue = 11; 
const int leftYellow = 12;
const int rightYellow = 3;
const int speaker = 9;
const int indicator = 4;

//other variables
int commandArray[15];
bool wrongMove = false;
int time1;
int time2;
int joyX;
int joyY;

//States of inputs (volatile because they're changed by interrupts)
volatile bool bluePressed = false;
volatile bool yellowPressed = false;
volatile bool switchFlipped = false;
volatile bool joyUp = false;
volatile bool joyDown = false;
volatile bool joyLeft = false;
volatile bool joyRight = false;

void setup() 
{ 
  //Setting up pins
  //Inputs
  pinMode(joystickX, INPUT);
  pinMode(joystickY, INPUT);
  pinMode(yellowButton, INPUT);
  pinMode(blueButton, INPUT);
  pinMode(missileSwitch, INPUT);

  //Pass/fail LEDs
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  //Command indicator LEDs
  pinMode(topBlue, OUTPUT);
  pinMode(bottomBlue, OUTPUT);
  pinMode(leftYellow, OUTPUT);
  pinMode(rightYellow, OUTPUT);

  //Speaker
  pinMode(speaker, OUTPUT);

  //Flag
  pinMode(indicator, OUTPUT);
}

void loop() 
{
  //We'll have inputs act as interrupts
  attachInterrupt(digitalPinToInterrupt(yellowButton), yellowButtonPressed, CHANGE);
  attachInterrupt(digitalPinToInterrupt(blueButton), blueButtonPressed, CHANGE);
  attachInterrupt(digitalPinToInterrupt(missileSwitch), switchFlip, CHANGE);

  //Disabling interrupts until setup is done
  noInterrupts();

  /*
  //We'll have the missile switch act as a start button
  while(digitalRead(missileSwitch) == LOW)
  {
    //Do nothing
    int dummyint = 0;
  }
  */

  //Clear inputs so we aren't instantly failing the game
  resetInputs();

  //Now we're starting, generate the list of commands
  //Initialize random seed
  randomSeed(analogRead(5));

  for(int i = 0; i < 15; i++)
  {
    commandArray[i] = random(0, 7);
  }

  //the command delay value
  int commandDelay = 1000;

  //Now the game starts
  for(int i = 0; i < 15; i++)
  {
    //Turning interrupts off
    noInterrupts();

    //Decreasing the delay at certain intervals
    if(i == 5)
    {
      commandDelay = 750;
    }
    else if(i == 10)
    {
      commandDelay = 500;
    }

    //Reading out all of the commands up to the new one
    for(int j = 0; j <= i; j++)
    {
      processCommand(commandArray[j]);
      delay(commandDelay);
    }

    //Turning interrupts back on
    interrupts();

    delay(1000);
    
    //Now we have to error check the inputs (let less than 200 and greater than 824 be the joystick cutoffs)
    for(int j = 0; j <=i; j++)
    {
      if(checkInput(commandArray[j]) == false)
      {
        //If an incorrect input is put in
        digitalWrite(redLED, HIGH);
        for(int k = 0; k < 4; k++)
        {
          tone(speaker, 440, 300);
          delay(300);
          tone(speaker, 349, 300);
          delay(300);
        }

        delay(2000);
        digitalWrite(redLED, LOW);

        exit(0);
      }
    }

    //If we make it here, all of the inputs were entered correctly and we can give the success notification
    digitalWrite(greenLED, HIGH);
    tone(speaker, 261, 100);
    delay(100);
    tone(speaker, 329, 100);
    delay(100);
    tone(speaker, 523, 100);
    delay(100);
    digitalWrite(greenLED, LOW);

    //A small pause, then we'll head back to the beginning of the loop and keep going
    delay(1500);
  }

  //If we make it here, they've won the whole game and we should prolly do something cool

}

//Command to give the correct output for each command
void processCommand(int x)
{
  //for joystick directions, we light the corresponding directional light and do a low beep (293 hz)
  //for buttons, we light the two corresponding colored lights and do a high beep (440 hz)
  //for the missile switch, we light all of the lights and do two high beeps
  switch(x)
  {
    case 0:
      //joystick up
      digitalWrite(topBlue, HIGH);
      tone(speaker, 293, 1000);
      delay(1000);
      digitalWrite(topBlue, LOW);
      break;

    case 1:
      //joystick down
      digitalWrite(bottomBlue, HIGH);
      tone(speaker, 293, 1000);
      delay(1000);
      digitalWrite(bottomBlue, LOW);
      break;

    case 2:
      //joystick left
      digitalWrite(leftYellow, HIGH);
      tone(speaker, 293, 1000);
      delay(1000);
      digitalWrite(leftYellow, LOW);
      break;

    case 3:
      //joystick right
      digitalWrite(rightYellow, HIGH);
      tone(speaker, 293, 1000);
      delay(1000);
      digitalWrite(rightYellow, LOW);
      break;

    case 4:
      //blue button
      digitalWrite(topBlue, HIGH);
      digitalWrite(bottomBlue, HIGH);
      tone(speaker, 440, 1000);
      delay(1000);
      digitalWrite(topBlue, LOW);
      digitalWrite(bottomBlue, LOW);
      break;

    case 5:
      //yellow button
      digitalWrite(leftYellow, HIGH);
      digitalWrite(rightYellow, HIGH);
      tone(speaker, 440, 1000);
      delay(1000);
      digitalWrite(leftYellow, LOW);
      digitalWrite(rightYellow, LOW);
      break;

    case 6:
      //flip switch
      digitalWrite(topBlue, HIGH);
      digitalWrite(bottomBlue, HIGH);
      digitalWrite(leftYellow, HIGH);
      digitalWrite(rightYellow, HIGH);
      tone(speaker, 440, 750);
      delay(750);
      digitalWrite(topBlue, LOW);
      digitalWrite(bottomBlue, LOW);
      digitalWrite(leftYellow, LOW);
      digitalWrite(rightYellow, LOW);
      delay(250);
      digitalWrite(topBlue, HIGH);
      digitalWrite(bottomBlue, HIGH);
      digitalWrite(leftYellow, HIGH);
      digitalWrite(rightYellow, HIGH);
      tone(speaker, 440, 750);
      delay(750);
      digitalWrite(topBlue, LOW);
      digitalWrite(bottomBlue, LOW);
      digitalWrite(leftYellow, LOW);
      digitalWrite(rightYellow, LOW);
      break;
  }
}

//interrupt functions
void yellowButtonPressed()
{
  yellowPressed = true;
  //Small debounce delay
  delay(150);
}

void blueButtonPressed()
{
  bluePressed = true;
  //Small debounce delay
  delay(150);
}

void switchFlip()
{
  switchFlipped = true;
  //Small debounce delay
  delay(150);
}

//The check input method
bool checkInput(int commandNum)
{
  switch(commandNum)
  {
    case 0:
    //joy up
    time1 = millis();
    time2 = millis();

    //While in the time limit
    while(time2 - time1 <= 1500)
    {
      //Updating time, reading analog inputs (digital inputs are interrupts)
      time2 = millis();
      readAnalogInputs();
      //Correct input pressed, no incorrect inputs registered
      if(joyUp && !yellowPressed && !switchFlipped && !bluePressed && !joyDown && !joyLeft && !joyRight)
      {
        resetInputs();
        return true; 
      }

      //Incorrect inputs
      if(yellowPressed || switchFlipped || bluePressed || joyDown || joyLeft || joyRight)
      {
        resetInputs();
        return false;
      }

      //Small delay
      delay(10);
    }

    //Ran out of time
    return false;
    break;

    case 1:
    //joy down
    time1 = millis();
    time2 = millis();

    //While in the time limit
    while(time2 - time1 <= 1500)
    {
      //Updating time, reading analog inputs (digital inputs are interrupts)
      time2 = millis();
      readAnalogInputs();
      //Correct input pressed, no incorrect inputs registered
      if(joyDown && !yellowPressed && !switchFlipped && !joyUp && !bluePressed && !joyLeft && !joyRight)
      {
        resetInputs();
        return true; 
      }

      //Incorrect inputs
      if(yellowPressed || switchFlipped || joyUp || bluePressed || joyLeft || joyRight)
      {
        resetInputs();
        return false;
      }

      //Small delay
      delay(10);
    }

    //Ran out of time
    return false;
    break;

    case 2:
    //joy left
    time1 = millis();
    time2 = millis();

    //While in the time limit
    while(time2 - time1 <= 1500)
    {
      //Updating time, reading analog inputs (digital inputs are interrupts)
      time2 = millis();
      readAnalogInputs();
      //Correct input pressed, no incorrect inputs registered
      if(joyLeft && !yellowPressed && !switchFlipped && !joyUp && !joyDown && !bluePressed && !joyRight)
      {
        resetInputs();
        return true; 
      }

      //Incorrect inputs
      if(yellowPressed || switchFlipped || joyUp || joyDown || bluePressed || joyRight)
      {
        resetInputs();
        return false;
      }

      //Small delay
      delay(10);
    }

    //Ran out of time
    return false;
    break;

    case 3:
    //joy right
    time1 = millis();
    time2 = millis();

    //While in the time limit
    while(time2 - time1 <= 1500)
    {
      //Updating time, reading analog inputs (digital inputs are interrupts)
      time2 = millis();
      readAnalogInputs();
      //Correct input pressed, no incorrect inputs registered
      if(joyRight && !yellowPressed && !switchFlipped && !joyUp && !joyDown && !joyLeft && !bluePressed)
      {
        resetInputs();
        return true; 
      }

      //Incorrect inputs
      if(yellowPressed || switchFlipped || joyUp || joyDown || joyLeft || bluePressed)
      {
        resetInputs();
        return false;
      }

      //Small delay
      delay(10);
    }

    //Ran out of time
    return false;
    break;

    case 4:
    //blue button
    time1 = millis();
    time2 = millis();

    //While in the time limit
    while(time2 - time1 <= 1500)
    {
      //Updating time, reading analog inputs (digital inputs are interrupts)
      time2 = millis();
      readAnalogInputs();
      //Correct input pressed, no incorrect inputs registered
      if(bluePressed && !yellowPressed && !switchFlipped && !joyUp && !joyDown && !joyLeft && !joyRight)
      {
        resetInputs();
        return true; 
      }

      //Incorrect inputs
      if(yellowPressed || switchFlipped || joyUp || joyDown || joyLeft || joyRight)
      {
        resetInputs();
        return false;
      }

      //Small delay
      delay(10);
    }

    //Ran out of time
    return false;
    break;

    case 5:
    //yellow button
    time1 = millis();
    time2 = millis();

    //While in the time limit
    while(time2 - time1 <= 1500)
    {
      //Updating time, reading analog inputs (digital inputs are interrupts)
      time2 = millis();
      readAnalogInputs();
      //Correct input pressed, no incorrect inputs registered
      if(yellowPressed && !bluePressed && !switchFlipped && !joyUp && !joyDown && !joyLeft && !joyRight)
      {
        resetInputs();
        return true; 
      }

      //Incorrect inputs
      if(bluePressed || switchFlipped || joyUp || joyDown || joyLeft || joyRight)
      {
        resetInputs();
        return false;
      }

      //Small delay
      delay(10);
    }

    //Ran out of time
    return false;
    break;

    case 6:
    //flip switch
    time1 = millis();
    time2 = millis();

    //While in the time limit
    while(time2 - time1 <= 1500)
    {
      //Updating time, reading analog inputs (digital inputs are interrupts)
      time2 = millis();
      readAnalogInputs();
      //Correct input pressed, no incorrect inputs registered
      if(switchFlipped && !yellowPressed && !bluePressed && !joyUp && !joyDown && !joyLeft && !joyRight)
      {
        resetInputs();
        return true; 
      }

      //Incorrect inputs
      if(yellowPressed || bluePressed || joyUp || joyDown || joyLeft || joyRight)
      {
        resetInputs();
        return false;
      }

      //Small delay
      delay(10);
    }

    //Ran out of time
    return false;
    break;
  } 
}

//Read analog inputs
void readAnalogInputs()
{
  joyX = analogRead(joystickX);
  joyY = analogRead(joystickY);

  if(joyX < 200)
    joyLeft = true;
  else if(joyX > 824)
    joyRight = true;

  if(joyY < 200)
    joyDown = true;
  else if(joyY > 824)
    joyUp = true;
}

//Reset inputs to prevent values from lingering
void resetInputs()
{
  bluePressed = yellowPressed = switchFlipped = joyUp = joyDown = joyLeft = joyRight = false;
}

//Indicator for debugging
void triggerIndicator()
{
  for(int i = 0; i < 4; i++)
  {
    digitalWrite(indicator, HIGH);
    delay(500);
    digitalWrite(indicator, LOW);
    delay(500);
  }
}