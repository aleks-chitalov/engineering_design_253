// Pins IN - W{number} means container is {number}% full. 
const int W25 = A1;
const int W50 = A2;
const int W75 = A3;
const int W90 = A4;

bool W25_state = false;
bool W50_state = false;
bool W75_state = false;
bool W90_state = false;

// Pn is potentionometer.
const int pnPin = A5;
const int buttonPin = 4;

// Pins OUT
const int pumpPin = 13;
const int buzzerPin = 3;

// State for button TRUE = Pressed IN, FALSE = Pressed OUT
bool buttonState = false;

// Keeps track of the last state of the button to detect changes
int lastButtonState = false;

// State of the pump and buzzer  True = ON, False = OFF
bool pumpState = false;
bool buzzerState = false;

// True if water has reached a level specified by a knob
bool emergencyPump = false;
bool lastEmergencyPump = false;

class Controll {
  public:
    bool checkWater(int waterLevelPin) {
      	int waterVal = analogRead(waterLevelPin);
        
        if (waterVal > 200) {
          return true;     
        }
        return false;
    }

    //Depending on the setting of the knob, function returns true if water has reached that level
    bool waterLimitReached(int waterLimit) {
      if (waterLimit <= 256 ) {
        return W25_state && W50_state;
      }
      if (waterLimit <= 512 ) {
        return W25_state && W50_state && W75_state;
      }
      if (waterLimit <= 768 ) {
        return W25_state && W50_state && W75_state && W90_state;
      }
      if (waterLimit <= 1024 ) {
        return false;
      }

    }

    void pumpControll(bool controllBool) {
        if (controllBool) {
        digitalWrite(pumpPin, HIGH);
      } else {
        digitalWrite(pumpPin, LOW);
      }
    }

    void buzzControll(bool controllBool) {
        if (controllBool) {
        digitalWrite(buzzerPin, HIGH);
      } else {
        digitalWrite(buzzerPin, LOW);
      }
    }

    void setPins() {
      pinMode(W25, INPUT);
      pinMode(W50, INPUT);
      pinMode(W75, INPUT);
      pinMode(W90, INPUT);
      pinMode(buttonPin, INPUT);
      pinMode(buzzerPin, OUTPUT);
      pinMode(pumpPin, OUTPUT);
    }
};

Controll controller;

void setup() {
  Serial.begin(9600);
  controller.setPins();
}

void loop() {
  buttonState = digitalRead(buttonPin);
  int pnValue = analogRead(pnPin);
  Serial.println(pnValue);

  // True if water has reached that percent of the contaier i.e. W25_state = True => Container is 25% full
  W25_state = controller.checkWater(W25);
  W50_state = controller.checkWater(W50);
  W75_state = controller.checkWater(W75);
  W90_state = controller.checkWater(W90);

  //Sets the buzzerState to TRUE container is 90% full.
  // We check for all previous levels as well, ensuring no false positives.
  buzzerState = W25_state && W50_state && W75_state && W90_state;
  emergencyPump = controller.waterLimitReached(pnValue);
  // Everytime the button is pressed, it changes the state of the pump
  if (buttonState != lastButtonState) {
    if (buttonState == false) {
      Serial.println("button press");
      pumpState = !pumpState;
    } 
  }

  // Everytime the water levels crosses the limit specified by the knob, the pump is enabled.
  if (emergencyPump != lastEmergencyPump) {
      pumpState = emergencyPump;
  }

  //Activates the pump and buzzer depending on their state
  controller.pumpControll(pumpState);
  controller.buzzControll(buzzerState);

  lastButtonState = buttonState;
  lastEmergencyPump = emergencyPump;
}
