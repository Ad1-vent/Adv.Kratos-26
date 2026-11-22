# Adv.Kratos-26
Ongoing/Completed tasks:

**Task 1:** Creating a SSR circuit simulation on Proteus, showcasing
- a) the power distribution system of the rover
- b) the dual killswitch feature.
- c) a visual representation of the power logging system

`[complete]`

---

**Task 2:** Rover Power Monitoring, Display & Telemetry 

Tasks:
- Establishing a UART communication line between the required MCUs (the ESP32 and the STM32 Nucleo-F446RE)
- Interfacing and reading the values of the INA219 using the ESP32.
- Interfacing an ADAFruit OLED display (attached on the rover) and displaying the power values on the display (1 reading every 4 seconds)
- Wrapping the data and sending a float array to the STM32 and therefore to the base-station.

`[final testing pending]`

---

**Task 3:** Cuvette Motor to STM32 Interfacing

Tasks:
- Interfacing a stepper motor via the A4988 stepper motor driver to the STM32
- Created a custom program called `move_to_cuvette(int cuv_no)` that moves the motor to 5 custom positions it to 5 custom positions for 5 different cuvettes

`[complete]`
