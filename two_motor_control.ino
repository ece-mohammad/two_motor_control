/****************************************************************************/
/* configurations */
/****************************************************************************/

/* switch state */
#define CLOSED LOW
#define OPEN HIGH

/* switch debounce time in milliseconds */
#define DEBOUNCE_TIME_MS 20

#define CONTROL_BUTTON_PIN 10

/* buzzer pin */
#define BUZZER_PIN 11

/* buzzer on time */
#define BUZZER_DURATION_MS 500

/* buzzer tone frequency (HZ) */
#define BUZZER_TONE_FREQ_HZ 1000

/* motor pins */
#define M1_FWD_PIN 2
#define M1_BWD_PIN 3
#define M2_FWD_PIN 4
#define M2_BWD_PIN 5

/* limit switches */
#define LIMIT_M1_FRONT 6
#define LIMIT_M2_FRONT 7
#define LIMIT_M1_BACK 8
#define LIMIT_M2_BACK 9

/****************************************************************************/
/* code */
/****************************************************************************/

/* system states enum */
enum class CurrentState {
  Idle,
  Start,
  Motor1Forward,
  Motor2Forward,
  Motor2Backward,
  Motor1Backward,
  End,
};

/* motor class */
class Motor {
private:
  int m_fwd_pin{};
  int m_bwd_pin{};
  int m_front_switch{};
  int m_back_switch{};

public:
  Motor(int fwd_pin, int bwd_pin, int front_switch, int back_switch)
      : m_fwd_pin(fwd_pin), m_bwd_pin(bwd_pin), m_front_switch(front_switch),
        m_back_switch(back_switch) {}

  void begin() {
    pinMode(m_fwd_pin, OUTPUT);
    pinMode(m_bwd_pin, OUTPUT);
    pinMode(m_front_switch, INPUT_PULLUP);
    pinMode(m_back_switch, INPUT_PULLUP);
  }

  void stop() {
    digitalWrite(m_fwd_pin, LOW);
    digitalWrite(m_bwd_pin, LOW);
  }

  void move_forward() {
    digitalWrite(m_fwd_pin, HIGH);
    digitalWrite(m_bwd_pin, LOW);
  }

  void move_backward() {
    digitalWrite(m_fwd_pin, LOW);
    digitalWrite(m_bwd_pin, HIGH);
  }

  bool at_front() {
    return (digitalRead(m_front_switch) == CLOSED &&
            digitalRead(m_back_switch) == OPEN);
  }

  bool at_back() {
    return (digitalRead(m_front_switch) == OPEN &&
            digitalRead(m_back_switch) == CLOSED);
  }
};

/* push button states */
enum class ButtonState {
  Released,
  Pressed,
  PreReleased,
  PrePressed,
};

/* push button callback */
using ButtonCallback = void (*)(ButtonState);

/* push button class */
template <int PRESSED_LEVEL, int RELEASED_LEVEL> class PushButton {
private:
  int m_pin{};
  long m_debounce_ms{};
  long m_last_update_time{};
  int m_last_level{RELEASED_LEVEL};
  ButtonCallback m_callback{};
  ButtonState m_current_state{ButtonState::Released};

public:
  PushButton(int pin, long debounce_ms, ButtonCallback callback = nullptr)
      : m_pin(pin), m_debounce_ms(debounce_ms), m_callback(callback) {}

  void begin() {
    if (PRESSED_LEVEL == HIGH) {
      pinMode(m_pin, INPUT);

    } else {
      pinMode(m_pin, INPUT_PULLUP);
    }
  }

  auto state() const -> ButtonState { return m_current_state; }

  void update() {
    long current_time = millis();

    if ((current_time - m_last_update_time) < m_debounce_ms) {
      return;
    }
    m_last_update_time = current_time;

    int current_level = digitalRead(m_pin);
    ButtonState new_state{m_current_state};

    switch (m_current_state) {
    case ButtonState::Released: {
      if (current_level == PRESSED_LEVEL && m_last_level == PRESSED_LEVEL) {
        new_state = ButtonState::PrePressed;
      }
    } break;

    case ButtonState::Pressed: {
      if (current_level == RELEASED_LEVEL && m_last_level == RELEASED_LEVEL) {
        new_state = ButtonState::PreReleased;
      }

    } break;

    case ButtonState::PreReleased: {
      if (current_level == RELEASED_LEVEL) {
        new_state = ButtonState::Released;
      }
    } break;

    case ButtonState::PrePressed: {
      if (current_level == PRESSED_LEVEL) {
        new_state = ButtonState::Pressed;
      }
    } break;

    default: {
      m_current_state = ButtonState::Released;
    } break;
    }

    m_last_level = current_level;
    if (new_state != m_current_state && m_callback) {
      m_callback(new_state);
    }
    m_current_state = new_state;
  }
};

/* buzzer class */
template <int FREQ> class Buzzer {
private:
  int m_pin{};

public:
  Buzzer(int pin) : m_pin(pin) {}

  void begin() { pinMode(m_pin, OUTPUT); }

  void play(int duration_ms) { tone(m_pin, FREQ, duration_ms); }
};

/* control button */
static void control_button_callback(ButtonState state);

/**
 * control button state, used by control button callback to notify system that
 * button was pressed 
 * */
bool buttonPressed{false};

static PushButton<LOW, HIGH> controlButton{
    CONTROL_BUTTON_PIN,
    DEBOUNCE_TIME_MS,
    control_button_callback,
};

/* motors */
static Motor Motor1{
    M1_FWD_PIN,
    M1_BWD_PIN,
    LIMIT_M1_FRONT,
    LIMIT_M1_BACK,
};
static Motor Motor2{
    M2_FWD_PIN,
    M2_BWD_PIN,
    LIMIT_M2_FRONT,
    LIMIT_M2_BACK,
};

/* buzzer */
static Buzzer<BUZZER_TONE_FREQ_HZ> NotificationBuzzer{BUZZER_PIN};

/* system state */
static CurrentState currentState{CurrentState::Idle};

/* state handlers */
static auto idle_handler() -> CurrentState;
static auto start_handler() -> CurrentState;
static auto motor1_forward_handler() -> CurrentState;
static auto motor2_forward_handler() -> CurrentState;
static auto motor2_backward_handler() -> CurrentState;
static auto motor1_backward_handler() -> CurrentState;
static auto end_handler() -> CurrentState;

void setup() {
  controlButton.begin();
  Motor1.begin();
  Motor2.begin();
  NotificationBuzzer.begin();

  Motor1.stop();
  Motor2.stop();
}

void loop() {
  controlButton.update();

  switch (currentState) {
  case CurrentState::Idle: {
    currentState = idle_handler();
  } break;

  case CurrentState::Start: {
    currentState = start_handler();
  } break;

  case CurrentState::Motor1Forward: {
    currentState = motor1_forward_handler();
  } break;

  case CurrentState::Motor2Forward: {
    currentState = motor2_forward_handler();
  } break;

  case CurrentState::Motor2Backward: {
    currentState = motor2_backward_handler();
  } break;

  case CurrentState::Motor1Backward: {
    currentState = motor1_backward_handler();
  } break;

  case CurrentState::End: {
    currentState = end_handler();
  } break;

  default: {
    currentState = CurrentState::Idle;
  } break;
  }
}

static void control_button_callback(ButtonState state) {
  if (state == ButtonState::Pressed) {
    buttonPressed = true;
  }
}

static auto idle_handler() -> CurrentState {
  if (buttonPressed) {
    buttonPressed = false;
    return CurrentState::Start;
  }
  return CurrentState::Idle;
}

static auto start_handler() -> CurrentState {
  if (Motor1.at_back() && Motor2.at_back()) {
    NotificationBuzzer.play(BUZZER_DURATION_MS);
    return CurrentState::Motor1Forward;

  } else if (Motor1.at_front() && Motor2.at_front()) {
    return CurrentState::Motor2Backward;

  } else {
    return CurrentState::Idle;
  }
}

static auto motor1_forward_handler() -> CurrentState {
  if (Motor1.at_front()) {
    Motor1.stop();
    return CurrentState::Motor2Forward;
  }

  if (Motor1.at_back()) {
    Motor1.move_forward();
  }
  return CurrentState::Motor1Forward;
}

static auto motor2_forward_handler() -> CurrentState {
  if (Motor2.at_front()) {
    Motor2.stop();
    return CurrentState::Idle;
  }

  if (Motor2.at_back()) {
    Motor2.move_forward();
  }
  return CurrentState::Motor2Forward;
}

static auto motor2_backward_handler() -> CurrentState {
  if (Motor2.at_back()) {
    Motor2.stop();
    return CurrentState::Motor1Backward;
  }

  if (Motor2.at_front()) {
    Motor2.move_backward();
  }
  return CurrentState::Motor2Backward;
}

static auto motor1_backward_handler() -> CurrentState {
  if (Motor1.at_back()) {
    Motor1.stop();
    return CurrentState::End;
  }

  if (Motor1.at_front()) {
    Motor1.move_backward();
  }
  return CurrentState::Motor1Backward;
}

static auto end_handler() -> CurrentState {
  NotificationBuzzer.play(BUZZER_DURATION_MS);
  return CurrentState::Idle;
}
