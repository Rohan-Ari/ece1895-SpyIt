#include <EEPROM.h>
#define EEPROM_ADDRESS 0
static int randomSeedInt = 0;

//pin mappings
const int joystickX = 5;
const int joystickY = 4;
const int yellowButton = 8; 
const int blueButton = 6;
const int missileSwitch = 5;
const int redLED = 0;
const int greenLED = 2; 
const int topBlue = 10;
const int bottomBlue = 11; 
const int leftYellow = 12;
const int rightYellow = 3;
const int speaker = 9;

//other variables
int commandArray[15];
bool wrongMove = false;
int time1;
int time2;
int joyX;
int joyY;
int pastSwitchState;
float commandSpeed = 1000;
float commandPause = 750;

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

  //Reading static random seed variable
  randomSeedInt = EEPROM.read(EEPROM_ADDRESS);
}

void loop() 
{
  
  //We'll have the blue button act as a start button
  while(digitalRead(blueButton) == LOW)
  {
    //Do nothing
    int dummyint = 0;
  }
  

  //Clear inputs so we aren't instantly failing the game, after a debounce delay
  delay(100);
  resetInputs();

  //Now we're starting, generate the list of commands
  //Initialize random seed
  randomSeed(randomSeedInt);

  for(int i = 0; i < 15; i++)
  {
    commandArray[i] = random(0, 7);
  }

  //Change random seed value
  randomSeedInt++;
  randomSeedInt = randomSeedInt % 100;
  EEPROM.write(EEPROM_ADDRESS, randomSeedInt);

  //Play a little Mission Impossible theme to get things started
  digitalWrite(topBlue, HIGH);
  digitalWrite(bottomBlue, HIGH);
  tone(speaker, 196, 300);
  delay(450);
  digitalWrite(topBlue, LOW);
  digitalWrite(bottomBlue, LOW);
  tone(speaker, 196, 450);
  delay(450);
  digitalWrite(topBlue, HIGH);
  tone(speaker, 233, 300);
  delay(300);
  digitalWrite(bottomBlue, HIGH);
  tone(speaker, 262, 300);
  delay(320);
  digitalWrite(leftYellow, HIGH);
  digitalWrite(rightYellow, HIGH);
  tone(speaker, 196, 300);
  delay(450);
  digitalWrite(leftYellow, LOW);
  digitalWrite(rightYellow, LOW);
  tone(speaker, 196, 450);
  delay(450);
  digitalWrite(leftYellow, HIGH);
  tone(speaker, 175, 300);
  delay(300);
  digitalWrite(rightYellow, HIGH);
  tone(speaker, 185, 300);
  delay(300);
  digitalWrite(topBlue, LOW);
  digitalWrite(bottomBlue, LOW);
  digitalWrite(leftYellow, LOW);
  digitalWrite(rightYellow, LOW);
  tone(speaker, 196, 300);
  delay(300);

  delay(1000);

  //Now the game starts
  for(int i = 0; i < 15; i++)
  {
    if(commandSpeed <= 500)
      commandSpeed = 500;
    else
      commandSpeed = commandSpeed * .92;

    if(commandPause <= 200)
      commandPause = 200;
    else
      commandPause = commandPause * .92;

    //Reading out all of the commands up to the new one
    for(int j = 0; j <= i; j++)
    {
      processCommand(commandArray[j], commandSpeed);
      delay(commandPause);
    }

    //Playing the State Farm jingle so they know they can start
    digitalWrite(greenLED, HIGH);
    tone(speaker, 131, 200);
    delay(200);
    tone(speaker, 165, 200);
    delay(200);
    tone(speaker, 196, 200);
    delay(200);
    tone(speaker, 247, 400);
    delay(410);
    tone(speaker, 262, 400);
    delay(400);
    digitalWrite(greenLED, LOW);

    //Now we have to error check the inputs (let less than 100 and greater than 900 be the joystick cutoffs)
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

        //Function to display score
        endGame(i);

        exit(0);
      }

      //If we're here, it means we got the correct input.
      //If it's not the last input, we'll play a quick tone
      if(j != i)
      {
        //Indicate correct input
        digitalWrite(greenLED, HIGH);
        tone(speaker, 261, 100);
        delay(100);
        digitalWrite(greenLED, LOW);
      }

      //Delay so inputs don't spill over
      delay(200);
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
    delay(1000);
  }

  //If we make it here, they've won the whole game
  endGame(15);

}

//Command to give the correct output for each command
void processCommand(int x, float commandSpeed)
{
  //for joystick directions, we light the corresponding directional light and do a low beep (293 hz)
  //for buttons, we light the two corresponding colored lights and do a high beep (440 hz)
  //for the missile switch, we light all of the lights and do two high beeps
  switch(x)
  {
    case 0:
      //joystick up
      digitalWrite(topBlue, HIGH);
      tone(speaker, 293, commandSpeed);
      delay(commandSpeed);
      digitalWrite(topBlue, LOW);
      break;

    case 1:
      //joystick down
      digitalWrite(bottomBlue, HIGH);
      tone(speaker, 293, commandSpeed);
      delay(commandSpeed);
      digitalWrite(bottomBlue, LOW);
      break;

    case 2:
      //joystick left
      digitalWrite(leftYellow, HIGH);
      tone(speaker, 293, commandSpeed);
      delay(commandSpeed);
      digitalWrite(leftYellow, LOW);
      break;

    case 3:
      //joystick right
      digitalWrite(rightYellow, HIGH);
      tone(speaker, 293, commandSpeed);
      delay(commandSpeed);
      digitalWrite(rightYellow, LOW);
      break;

    case 4:
      //blue button
      digitalWrite(topBlue, HIGH);
      digitalWrite(bottomBlue, HIGH);
      tone(speaker, 440, commandSpeed);
      delay(commandSpeed);
      digitalWrite(topBlue, LOW);
      digitalWrite(bottomBlue, LOW);
      break;

    case 5:
      //yellow button
      digitalWrite(leftYellow, HIGH);
      digitalWrite(rightYellow, HIGH);
      tone(speaker, 440, commandSpeed);
      delay(commandSpeed);
      digitalWrite(leftYellow, LOW);
      digitalWrite(rightYellow, LOW);
      break;

    case 6:
      //flip switch
      digitalWrite(topBlue, HIGH);
      digitalWrite(bottomBlue, HIGH);
      digitalWrite(leftYellow, HIGH);
      digitalWrite(rightYellow, HIGH);
      tone(speaker, 293, commandSpeed/2);
      tone(speaker, 440, commandSpeed/2);
      delay(commandSpeed);
      digitalWrite(topBlue, LOW);
      digitalWrite(bottomBlue, LOW);
      digitalWrite(leftYellow, LOW);
      digitalWrite(rightYellow, LOW);
      break;
  }
}

//The check input method
bool checkInput(int commandNum)
{
  switch(commandNum)
  {
    case 0:
    Serial.print("Checking command 0\n");
    //joy up
    time1 = millis();
    time2 = millis();

    //While in the time limit
    while(time2 - time1 <= 1500)
    {
      //Updating time, reading inputs
      time2 = millis();
      readInputs();
      //Correct input pressed, no incorrect inputs registered
      if(joyUp && !yellowPressed && !switchFlipped && !bluePressed && !joyDown && !joyLeft && !joyRight)
      {
        resetInputs();
        Serial.print("Command 0 passed\n");
        return true; 
      }

      //Incorrect inputs
      if(yellowPressed || switchFlipped || bluePressed || joyDown || joyLeft || joyRight)
      {
        if(bluePressed)
          Serial.print("blue button pressed\n");
        if(yellowPressed)
          Serial.print("yellow button pressed\n");
        if(switchFlipped)
          Serial.print("missile switch flipped\n");
        if(joyUp)
          Serial.print("joystick pressed up\n");
        if(joyDown)
          Serial.print("joystick pressed down\n");
        if(joyLeft)
          Serial.print("joystick pressed left\n");
        if(joyRight)
          Serial.print("joystick pressed right\n");
        
        resetInputs();
        Serial.print("Command 0 failed (incorrect input)\n");
        return false;
      }

      //Small delay
      delay(10);
    }

    //Ran out of time
    Serial.print("Command 0 failed (ran out of time)\n");
    return false;
    break;

    case 1:
    Serial.print("Checking command 1\n");
    //joy down
    time1 = millis();
    time2 = millis();

    //While in the time limit
    while(time2 - time1 <= 1500)
    {
      //Updating time, reading inputs
      time2 = millis();
      readInputs();
      //Correct input pressed, no incorrect inputs registered
      if(joyDown && !yellowPressed && !switchFlipped && !joyUp && !bluePressed && !joyLeft && !joyRight)
      {
        resetInputs();
        Serial.print("Command 1 passed\n");
        return true; 
      }

      //Incorrect inputs
      if(yellowPressed || switchFlipped || joyUp || bluePressed || joyLeft || joyRight)
      {
        if(bluePressed)
          Serial.print("blue button pressed\n");
        if(yellowPressed)
          Serial.print("yellow button pressed\n");
        if(switchFlipped)
          Serial.print("missile switch flipped\n");
        if(joyUp)
          Serial.print("joystick pressed up\n");
        if(joyDown)
          Serial.print("joystick pressed down\n");
        if(joyLeft)
          Serial.print("joystick pressed left\n");
        if(joyRight)
          Serial.print("joystick pressed right\n");
        
        resetInputs();
        Serial.print("Command 1 failed (incorrect input)\n");
        return false;
      }

      //Small delay
      delay(10);
    }

    //Ran out of time
    Serial.print("Command 1 failed (ran out of time)\n");
    return false;
    break;

    case 2:
    Serial.print("Checking command 2\n");
    //joy left
    time1 = millis();
    time2 = millis();

    //While in the time limit
    while(time2 - time1 <= 1500)
    {
      //Updating time, reading inputs
      time2 = millis();
      readInputs();
      //Correct input pressed, no incorrect inputs registered
      if(joyLeft && !yellowPressed && !switchFlipped && !joyUp && !joyDown && !bluePressed && !joyRight)
      {
        resetInputs();
        Serial.print("Command 2 passed\n");
        return true; 
      }

      //Incorrect inputs
      if(yellowPressed || switchFlipped || joyUp || joyDown || bluePressed || joyRight)
      {
        if(bluePressed)
          Serial.print("blue button pressed\n");
        if(yellowPressed)
          Serial.print("yellow button pressed\n");
        if(switchFlipped)
          Serial.print("missile switch flipped\n");
        if(joyUp)
          Serial.print("joystick pressed up\n");
        if(joyDown)
          Serial.print("joystick pressed down\n");
        if(joyLeft)
          Serial.print("joystick pressed left\n");
        if(joyRight)
          Serial.print("joystick pressed right\n");
        
        resetInputs();
        Serial.print("Command 2 failed (incorrect input)\n");
        return false;
      }

      //Small delay
      delay(10);
    }

    //Ran out of time
    Serial.print("Command 2 failed (ran out of time)\n");
    return false;
    break;

    case 3:
    Serial.print("Checking command 3\n");
    //joy right
    time1 = millis();
    time2 = millis();

    //While in the time limit
    while(time2 - time1 <= 1500)
    {
      //Updating time, reading inputs
      time2 = millis();
      readInputs();
      //Correct input pressed, no incorrect inputs registered
      if(joyRight && !yellowPressed && !switchFlipped && !joyUp && !joyDown && !joyLeft && !bluePressed)
      {
        resetInputs();
        Serial.print("Command 3 passed\n");
        return true; 
      }

      //Incorrect inputs
      if(yellowPressed || switchFlipped || joyUp || joyDown || joyLeft || bluePressed)
      {
        if(bluePressed)
          Serial.print("blue button pressed\n");
        if(yellowPressed)
          Serial.print("yellow button pressed\n");
        if(switchFlipped)
          Serial.print("missile switch flipped\n");
        if(joyUp)
          Serial.print("joystick pressed up\n");
        if(joyDown)
          Serial.print("joystick pressed down\n");
        if(joyLeft)
          Serial.print("joystick pressed left\n");
        if(joyRight)
          Serial.print("joystick pressed right\n");
        
        resetInputs();
        Serial.print("Command 3 failed (incorrect input)\n");
        return false;
      }

      //Small delay
      delay(10);
    }

    //Ran out of time
    Serial.print("Command 3 failed (ran out of time)\n");
    return false;
    break;

    case 4:
    Serial.print("Checking command 4\n");
    //blue button
    time1 = millis();
    time2 = millis();

    //While in the time limit
    while(time2 - time1 <= 1500)
    {
      //Updating time, reading inputs
      time2 = millis();
      readInputs();
      //Correct input pressed, no incorrect inputs registered
      if(bluePressed && !yellowPressed && !switchFlipped && !joyUp && !joyDown && !joyLeft && !joyRight)
      {
        resetInputs();
        Serial.print("Command 4 passed\n");
        return true; 
      }

      //Incorrect inputs
      if(yellowPressed || switchFlipped || joyUp || joyDown || joyLeft || joyRight)
      {
        if(bluePressed)
          Serial.print("blue button pressed\n");
        if(yellowPressed)
          Serial.print("yellow button pressed\n");
        if(switchFlipped)
          Serial.print("missile switch flipped\n");
        if(joyUp)
          Serial.print("joystick pressed up\n");
        if(joyDown)
          Serial.print("joystick pressed down\n");
        if(joyLeft)
          Serial.print("joystick pressed left\n");
        if(joyRight)
          Serial.print("joystick pressed right\n");
        
        resetInputs();
        Serial.print("Command 4 failed (incorrect input)\n");
        return false;
      }

      //Small delay
      delay(10);
    }

    //Ran out of time
    Serial.print("Command 4 failed (ran out of time)\n");
    return false;
    break;

    case 5:
    Serial.print("Checking command 5\n");
    //yellow button
    time1 = millis();
    time2 = millis();

    //While in the time limit
    while(time2 - time1 <= 1500)
    {
      //Updating time, reading inputs
      time2 = millis();
      readInputs();
      //Correct input pressed, no incorrect inputs registered
      if(yellowPressed && !bluePressed && !switchFlipped && !joyUp && !joyDown && !joyLeft && !joyRight)
      {
        resetInputs();
        Serial.print("Command 5 passed\n");
        return true; 
      }

      //Incorrect inputs
      if(bluePressed || switchFlipped || joyUp || joyDown || joyLeft || joyRight)
      {
        if(bluePressed)
          Serial.print("blue button pressed\n");
        if(yellowPressed)
          Serial.print("yellow button pressed\n");
        if(switchFlipped)
          Serial.print("missile switch flipped\n");
        if(joyUp)
          Serial.print("joystick pressed up\n");
        if(joyDown)
          Serial.print("joystick pressed down\n");
        if(joyLeft)
          Serial.print("joystick pressed left\n");
        if(joyRight)
          Serial.print("joystick pressed right\n");
        
        resetInputs();
        Serial.print("Command 5 failed (incorrect input)\n");
        return false;
      }

      //Small delay
      delay(10);
    }

    //Ran out of time
    Serial.print("Command 5 failed (ran out of time)\n");
    return false;
    break;

    case 6:
    Serial.print("Checking command 6\n");
    //flip switch
    time1 = millis();
    time2 = millis();

    //While in the time limit
    while(time2 - time1 <= 1500)
    {
      //Updating time, reading inputs
      time2 = millis();
      readInputs();
      //Correct input pressed, no incorrect inputs registered
      if(switchFlipped && !yellowPressed && !bluePressed && !joyUp && !joyDown && !joyLeft && !joyRight)
      {
        resetInputs();
        Serial.print("Command 6 passed\n");
        return true; 
      }

      //Incorrect inputs
      if(yellowPressed || bluePressed || joyUp || joyDown || joyLeft || joyRight)
      {
        if(bluePressed)
          Serial.print("blue button pressed\n");
        if(yellowPressed)
          Serial.print("yellow button pressed\n");
        if(switchFlipped)
          Serial.print("missile switch flipped\n");
        if(joyUp)
          Serial.print("joystick pressed up\n");
        if(joyDown)
          Serial.print("joystick pressed down\n");
        if(joyLeft)
          Serial.print("joystick pressed left\n");
        if(joyRight)
          Serial.print("joystick pressed right\n");
        
        resetInputs();
        Serial.print("Command 6 failed (incorrect input)\n");
        return false;
      }

      //Small delay
      delay(10);
    }

    //Ran out of time
    Serial.print("Command 6 failed (ran out of time)\n");
    return false;
    break;
  } 
}

//Read analog inputs
void readInputs()
{
  joyX = analogRead(joystickX);
  joyY = analogRead(joystickY);

  if(joyX < 100)
    joyLeft = true;
  else if(joyX > 900)
    joyRight = true;

  if(joyY < 100)
    joyUp = true;
  else if(joyY > 900)
    joyDown = true;
  
  bluePressed = (digitalRead(blueButton) == HIGH);
  yellowPressed = (digitalRead(yellowButton) == HIGH);
  switchFlipped = (digitalRead(missileSwitch) != pastSwitchState);
  pastSwitchState = digitalRead(missileSwitch);

  //Debounce delay
  delay(100);
}

//Reset inputs to prevent values from lingering
void resetInputs()
{
  bluePressed = yellowPressed = switchFlipped = joyUp = joyDown = joyLeft = joyRight = false;
}

//The end game function
void endGame(int score)
{
  int scaleArray[] = {131, 147, 165, 174, 196, 220, 247, 262, 294, 330, 349, 392, 440, 494, 523};
  int lightArray[] = {topBlue, rightYellow, bottomBlue, leftYellow};
  bool lightStates[] = {true, true, true, true};

  for(int i = 0; i < score; i++)
  {
    digitalWrite(lightArray[i%4], lightStates[i%4]);
    lightStates[i%4] = !lightStates[i%4];
    tone(speaker, scaleArray[i], 300);
    delay(340);
  }

  if(score == 14)
  {
    //Turn all the lights off
    digitalWrite(topBlue, LOW);
    digitalWrite(bottomBlue, LOW);
    digitalWrite(leftYellow, LOW);
    digitalWrite(rightYellow, LOW);

    delay(1000);

    digitalWrite(topBlue, HIGH);
    digitalWrite(bottomBlue, HIGH);
    digitalWrite(leftYellow, HIGH);
    digitalWrite(rightYellow, HIGH);
    //Play the new item theme from Metroid
    tone(speaker, 175, 250);
    delay(250);
    tone(speaker, 233, 250);
    delay(250);
    tone(speaker, 262, 250);
    delay(250);
    tone(speaker, 294, 250);
    delay(250);
    tone(speaker, 330, 250);
    delay(250);
    tone(speaker, 262, 250);
    delay(250);
    tone(speaker, 220, 250);
    delay(250);
    tone(speaker, 262, 250);
    delay(250);
    tone(speaker, 349, 250);
    delay(250);
    tone(speaker, 294, 250);
    delay(250);
    tone(speaker, 233, 250);
    delay(250);
    tone(speaker, 196, 250);
    delay(250);
    tone(speaker, 220, 1000);
    delay(1000);
  }
  
  delay(1000);

  //Turn all the lights off
  digitalWrite(topBlue, LOW);
  digitalWrite(bottomBlue, LOW);
  digitalWrite(leftYellow, LOW);
  digitalWrite(rightYellow, LOW);
}
