# Automated Dual-Motor Control System

An Arduino-based system for controlling two motors with limit switches and push button. Provides reliable reciprocating motion with position feedback and safety features.

![System Diagram](https://via.placeholder.com/800x400.png?text=Connection+Diagram+Placeholder)

## Components

- **Microcontroller**: Arduino Uno/Nano
- **Motors**: 2x DC gear motors (12V)
- **Motor Drivers**: 2x H-bridge modules (e.g., L298N)
- **Limit Switches**: 4x Mechanical (NO type)
- **Push Button**: 1x Tactile switch
- **LED**: 1x 5mm (Direction indicator)
- **Power Supply**: 12V 2A DC

## Connection Diagram

- 2 (D2) → Motor1 Forward
- 3 (D3) → Motor1 Backward
- 4 (D4) → Motor2 Forward
- 5 (D5) → Motor2 Backward
- 6 (D6) ← Motor1 Front Limit Switch
- 7 (D7) ← Motor2 Front Limit Switch
- 8 (D8) ← Motor1 Back Limit Switch
- 9 (D9) ← Motor2 Back Limit Switch
- 10 (D10) ← Control Button (to ground)
- GND → Common ground for all components

## System Operation

### Initialization

1. System starts in safe mode (motors stopped)
2. Checks initial limit switch states

### Normal Operation

1. **Button Press**:
    - Only accepted when motors are at end positions
    - Toggles movement direction
    - 50ms debounce prevents false triggers

2. **Forward Direction**:
    - Motor2 moves first until front limit engaged
    - Motor1 then moves until front limit engaged

3. **Backward Direction**:
    - Motor1 moves first until back limit engaged
    - Motor2 then moves until back limit engaged

### Safety Features

- Automatic stop at limit switches
- Movement timeout (configurable through `#define`)
- Emergency stop on invalid switch states
- Mutual exclusion prevents motor conflicts
