#define DEBOUNCE_TIME_MS 300

#define PRESSED LOW
#define NOT_PRESSED HIGH

#define CLOSED LOW
#define OPEN HIGH

#define BACKWARD false
#define FORWARD true

// أرجل التحكم بالموتور
int m1_fwd_pin = 2;
int m1_bwd_pin = 3;
int m2_fwd_pin = 4;
int m2_bwd_pin = 5;

// limit switches
int limit_m1_front = 6; // لموتور 1 الاتجاه الأول
int limit_m2_front = 7; // لموتور 2 الاتجاه الأول
int limit_m1_back = 8;  // لموتور 1 الاتجاه العكسي
int limit_m2_back = 9;  // لموتور 2 الاتجاه العكسي

// زر التشغيل
int controlButton = 10;

// حالة الزر
bool lastButton = NOT_PRESSED;

// movement direction
bool direction = FORWARD;

void setup() {
  pinMode(m1_fwd_pin, OUTPUT);
  pinMode(m1_bwd_pin, OUTPUT);
  pinMode(m2_fwd_pin, OUTPUT);
  pinMode(m2_bwd_pin, OUTPUT);

  pinMode(controlButton, INPUT_PULLUP);
  pinMode(limit_m1_front, INPUT_PULLUP);
  pinMode(limit_m2_front, INPUT_PULLUP);
  pinMode(limit_m1_back, INPUT_PULLUP);
  pinMode(limit_m2_back, INPUT_PULLUP);

  // make sure motors are stopped
  stopMotor1();
  stopMotor2();
}

void loop() {
  // read button state
  bool currentButton = (digitalRead(controlButton) == PRESSED);

  // عند الضغط وتغيير الحالة
  if (currentButton && !lastButton) {
    if ((digitalRead(limit_m1_front) == CLOSED &&
         digitalRead(limit_m2_front) == CLOSED) ||
        (digitalRead(limit_m1_back) == CLOSED &&
         digitalRead(limit_m2_back) == CLOSED)) {
      direction = !direction;
      delay(DEBOUNCE_TIME_MS);
    }
  }

  // update prev button state to current state
  lastButton = currentButton;

  if (direction == BACKWARD) {

    // تحرك للخلف
    if (digitalRead(limit_m1_back) == CLOSED &&
        digitalRead(limit_m2_back) == CLOSED) {
      moveMotor1Backward();
    }
    if ((digitalRead(limit_m1_front) == CLOSED)) {
      stopMotor1();
    }

    if ((digitalRead(limit_m2_back) == CLOSED) &&
        (digitalRead(limit_m1_front) == CLOSED)) {
      moveMotor2Backward();
    }

    if (digitalRead(limit_m2_front) == CLOSED) {
      stopMotor2();
    }

  } else {

    // تحرك للأمام
    if (digitalRead(limit_m2_front) == CLOSED &&
        digitalRead(limit_m1_front) == CLOSED) {
      moveMotor2Forward();
    }

    if (digitalRead(limit_m2_back) == CLOSED) {
      stopMotor2();
    }

    if ((digitalRead(limit_m1_front) == CLOSED) &&
        (digitalRead(limit_m2_back) == CLOSED)) {
      moveMotor1Forward();
    }

    if (digitalRead(limit_m1_back) == CLOSED) {
      stopMotor1();
    }
  }
}

// دوال الموتور
void moveMotor1Forward() {
  digitalWrite(m1_fwd_pin, HIGH);
  digitalWrite(m1_bwd_pin, LOW);
}
void moveMotor1Backward() {
  digitalWrite(m1_fwd_pin, LOW);
  digitalWrite(m1_bwd_pin, HIGH);
}
void stopMotor1() {
  digitalWrite(m1_fwd_pin, LOW);
  digitalWrite(m1_bwd_pin, LOW);
}
void moveMotor2Forward() {
  digitalWrite(m2_fwd_pin, HIGH);
  digitalWrite(m2_bwd_pin, LOW);
}
void moveMotor2Backward() {
  digitalWrite(m2_fwd_pin, LOW);
  digitalWrite(m2_bwd_pin, HIGH);
}
void stopMotor2() {
  digitalWrite(m2_fwd_pin, LOW);
  digitalWrite(m2_bwd_pin, LOW);
}
