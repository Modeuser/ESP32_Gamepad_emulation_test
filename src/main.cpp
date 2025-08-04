#include <Arduino.h>
#include <BleConnectionStatus.h>

#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>

int ledPin = 5; // LED connected to digital pin 13

XboxGamepadDevice *gamepad;
BleCompositeHID compositeHID("Seal_Cat_ProV2.0", "PPP", 100);

// Force resistance sensor 1
#define FORCE_PIN1 36
#define OUT3V3_1 25

// Force resistance sensor 2
#define FORCE_PIN2 39
#define OUT3V3_2 26

// Force resistance sensor 3
#define FORCE_PIN3 34
#define OUT3V3_3 27

// Force resistance sensor 3
#define FORCE_PIN4 35
#define OUT3V3_4 14

void OnVibrateEvent(XboxGamepadOutputReportData data)
{
    if(data.weakMotorMagnitude > 0 || data.strongMotorMagnitude > 0){
        digitalWrite(ledPin, LOW);
    } else {
        digitalWrite(ledPin, HIGH);
    }
    Serial.println("Vibration event. Weak motor: " + String(data.weakMotorMagnitude) + " Strong motor: " + String(data.strongMotorMagnitude));
}

void setup(){

  pinMode(ledPin, OUTPUT); // sets the digital pin as output

  // Force resistance sensor 1 3V3 out and analog reader
  pinMode (OUT3V3_1, OUTPUT);
  digitalWrite(OUT3V3_1, HIGH);

  // Force resistance sensor 2 'line24'
  pinMode (OUT3V3_2, OUTPUT);
  digitalWrite (OUT3V3_2, HIGH);

  // Force resistance sensor 3 'line24'
  pinMode (OUT3V3_3, OUTPUT);
  digitalWrite (OUT3V3_3, HIGH);

  // Force resistance sensor 3 'line24'
  pinMode (OUT3V3_4, OUTPUT);
  digitalWrite (OUT3V3_4, HIGH);

  XboxSeriesXControllerDeviceConfiguration* config =
  new XboxSeriesXControllerDeviceConfiguration();

  BLEHostConfiguration hostConfig = config->getIdealHostConfiguration();
  // Fake out the apple vision pro by emitting appropriate vendor and product IDs
  Serial.println("Using VID source: " + String(hostConfig.getVidSource(), HEX));
  Serial.println("Using VID: " + String(hostConfig.getVid(), HEX));
  Serial.println("Using PID: " + String(hostConfig.getPid(), HEX));
  Serial.println("Using GUID version: " + String(hostConfig.getGuidVersion(), HEX));
  Serial.println("Using serial number: " + String(hostConfig.getSerialNumber()));

  // Initialize gamepad
  gamepad = new XboxGamepadDevice(config);

  // Set up vibration event handler
  FunctionSlot<XboxGamepadOutputReportData> vibrationSlot(OnVibrateEvent);
  gamepad->onVibrate.attach(vibrationSlot);

  compositeHID.addDevice(gamepad);

  Serial.println("Start pretending to be xbox controller");
  compositeHID.begin(hostConfig);
  Serial.begin(9600);
}

// void testThumbsticks(){
//   int startTime = millis();
//   int reportCount = 0;
//   while(millis() - startTime < 8000){
//       reportCount++;
//       int16_t x = cos((float)millis() / 1000.0f) * XBOX_STICK_MAX;
//       int16_t y = sin((float)millis() / 1000.0f) * XBOX_STICK_MAX;

//       gamepad->setLeftThumb(x, y);
//       gamepad->setRightThumb(x, y);
//       gamepad->sendGamepadReport();
      
//       if(reportCount % 8 == 0)
//           Serial.println("Setting left thumb to " + String(x) + ", " + String(y));
          
//       delay(8);
//   }
// }

void loop() {

  int ADC1_value = analogRead(FORCE_PIN1);
  int ADC2_value = analogRead(FORCE_PIN2);
  int ADC3_value = analogRead(FORCE_PIN3);
  int ADC4_value = analogRead(FORCE_PIN4);

  // Run input tests
  // if(compositeHID.isConnected()){
  //   testThumbsticks();
  // }

  //serial monitor debugging check
  Serial.println("Force reading = ");
  Serial.println(ADC1_value);
  Serial.println(ADC2_value);
  Serial.println(ADC3_value);
  Serial.println(ADC4_value);

  // int to track which button to report
  int reportState = 0;

  //ADC1 state checker
  if (ADC1_value<100)
  {
    gamepad->release(XBOX_BUTTON_X);
    gamepad->release(XBOX_BUTTON_Y);
    //Serial.println(ADC1_value);
  } else if (ADC1_value<300)
  {
    gamepad->release(XBOX_BUTTON_Y);
    gamepad->press(XBOX_BUTTON_X);
  } else
  {
    gamepad->release(XBOX_BUTTON_X);
    gamepad->press(XBOX_BUTTON_Y);
  }

  //ADC2 state checker
  if (ADC2_value<400)
  {
    gamepad->release(XBOX_BUTTON_RB);
    gamepad->release(XBOX_BUTTON_LB);
    //Serial.println(ADC2_value);
  } else if (ADC2_value<1000)
  {
    gamepad->release(XBOX_BUTTON_LB);
    gamepad->press(XBOX_BUTTON_RB);
  } else
  {
    gamepad->release(XBOX_BUTTON_RB);
    gamepad->press(XBOX_BUTTON_LB);
  }

  //ADC3 state checker
  if (ADC3_value<400)
  {
    gamepad->release(XBOX_BUTTON_RS);
    //Serial.println(ADC3_value);
  } else
  {
    gamepad->press(XBOX_BUTTON_RS);
  }

  //ADC4 state checker
  if (ADC4_value<400)
  {
    gamepad->release(XBOX_BUTTON_LS);
    //Serial.println(ADC4_value);
  } else
  {
    gamepad->press(XBOX_BUTTON_LS);
  }
  // set joystick default
  gamepad->setLeftThumb(10,0);
  gamepad->setRightThumb(10,0);
  // send game pad report
  gamepad->sendGamepadReport();
  //delay controls update frequency
  delay(100);
}


  // if (ADC1_value < 500)
  //     Serial.println("no1pressure");
  //   else if (ADC1_value < 1400){
  //     Serial.println("pressure1level1");
  //     gamepad->press(XBOX_BUTTON_X);
  //     gamepad->sendGamepadReport();
  //     delay(50);
  //     gamepad->release(XBOX_BUTTON_X);
  //     gamepad->sendGamepadReport();
  //     delay(50);
  //   } else { // if reading is between 2000-4096
  //     Serial.println("pressure1level2");
  //     gamepad->press(XBOX_BUTTON_Y);
  //     gamepad->sendGamepadReport();
  //     delay(50);
  //     gamepad->release(XBOX_BUTTON_Y);
  //     gamepad->sendGamepadReport();
  //     delay(50);
  //   }

  // if (ADC2_value < 500)
  //     Serial.println("no1pressure");
  //   else if (ADC2_value < 1400){
  //     Serial.println("pressure1level1");
  //     gamepad->press(XBOX_BUTTON_RB);
  //     gamepad->sendGamepadReport();
  //     delay(50);
  //     gamepad->release(XBOX_BUTTON_RB);
  //     gamepad->sendGamepadReport();
  //     delay(50);
  //   } else { // if reading is between 2000-4096
  //     Serial.println("pressure1level2");
  //     gamepad->press(XBOX_BUTTON_LB);
  //     gamepad->sendGamepadReport();
  //     delay(50);
  //     gamepad->release(XBOX_BUTTON_LB);
  //     gamepad->sendGamepadReport();
  //     delay(50);
  //   }

  // if (ADC3_value < 500)
  //     Serial.println("no1pressure");
  //   else if (ADC3_value < 1400){
  //     Serial.println("pressure1level1");
  //     gamepad->press(XBOX_BUTTON_LS);
  //     gamepad->sendGamepadReport();
  //     delay(50);
  //     gamepad->release(XBOX_BUTTON_LS);
  //     gamepad->sendGamepadReport();
  //     delay(50);
  //   } else { // if reading is between 2000-4096
  //     Serial.println("pressure1level2");
  //     gamepad->press(XBOX_BUTTON_RS);
  //     gamepad->sendGamepadReport();
  //     delay(50);
  //     gamepad->release(XBOX_BUTTON_RS);
  //     gamepad->sendGamepadReport();
  //     delay(50);
  //   }
  // new button output system
  // we're going to try to select the highest one

    // // int to track which button to report
    // int reportState = 0;

    // //if the ADC1 has the highest value among the 3
    // if (ADC1_value>ADC2_value && ADC1_value>ADC3_value&&500<ADC1_value)
    // {
    //   reportState = (ADC1_value<1400) ? 1 : 2;
    // } else if (ADC2_value>ADC1_value&&ADC2_value>ADC3_value&&500<ADC2_value)
    // {
    //   reportState = (ADC2_value<1400) ? 3 : 4;
    // } else if (ADC3_value>ADC1_value&&ADC3_value>ADC2_value&&500<ADC3_value)
    // {
    //   reportState = (ADC3_value<1400) ? 5 : 6;
    // } else
    // {
    //   if (ADC1_value<500&&ADC2_value<500&&ADC3_value<500)
    //   {
    //     reportState = 0;
    //   }
    // }
  
    // // a switch case statement that sets the gamepad state based on report state
    // switch(reportState)
    // {
    //   case 0:
    //     // no input case
    //     gamepad->resetInputs();
    //     break;
    //   case 1:
    //     //ADC1 low
    //     gamepad->resetInputs();
    //     gamepad->press(XBOX_BUTTON_X);
    //     break;
    //   case 2:
    //     //ADC1 high
    //     gamepad->resetInputs();
    //     gamepad->press(XBOX_BUTTON_Y);
    //     break;
    //   case 3:
    //     //ADC2 low
    //     gamepad->resetInputs();
    //     gamepad->press(XBOX_BUTTON_RB);
    //     break;
    //   case 4:
    //     //ADC2 high
    //     gamepad->resetInputs();
    //     gamepad->press(XBOX_BUTTON_LB);
    //     break;
    //   case 5:
    //     //ADC3 low
    //     gamepad->resetInputs();
    //     gamepad->press(XBOX_BUTTON_LS);
    //     break;
    //   case 6:
    //     //ADC3 high
    //     gamepad->resetInputs();
    //     gamepad->press(XBOX_BUTTON_RS);
    //     break;
    //   default:
    //     // shouldn't reach
    //     printf("sum ting wong");
    // }
  
    // // send game pad report
    // gamepad->sendGamepadReport();