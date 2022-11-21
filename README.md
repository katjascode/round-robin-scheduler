# round-robin-scheduler

This program is written in C for Atmega328p to control 4 LEDs connected to the microcontroller ports B and D via the Round Robin scheduling algorithm.

4 tasks are initiated and added to the ready-to-go queue. After that, timer1 is initiated and the CTC timer interrupt is activated. The timer period and the
time quantum of the scheduler is set to 500 ms.
When a task gets its time quantum to execute, its dedicated LED turns on. As soon as Round Robin grants the next time quantum to another task, the LED of the predecessor task gets turned off. When the queue gets empty, all LEDs turn on.

The first task has the burst time of 2 time quantums, the second one - of 4 time quantums, the third one - of 4 quantums and the fourth one - of 3 time quantums.
