# Adv.Kratos-26
Ongoing/Completed tasks:

Task 1: Creating a SSR circuit diagram on LT-Spice, showcasing the power distribution system of the rover along with its dual killswitch feature.

Task 2: Creating a system to read the values from a power logger (the INA-219) and transmit them to the base-station. 
Subtasks:
      -Establishing a UART communication line between the required MCUs (the ESP32 and the STM32 Nucleo-F446RE)
      -Interfacing and reading the values of the INA219 using the ESP32.
      -Wrapping the data and sending a compact message to the STM32 and therefore to the base-station.

