# DOS-Realtime-PoC
## Description
Basic and rudimentary implementation of Real Time mechanisms for DOS. Two versions:
- basic.c - two tasks running at scheduled intervals;
- fsm.c - added simple finite state machine (FSM) - in this example, Task 1 switches to RUNNING state when its state is altered by Task 2.

## Compatibility
Tested with Open Watcom C 1.9. Due to the bug in this compiler version, it may not work on 8086/8088 machines.

## TODO
- Add control of states using signals.
- Create a real life demo example (e.g. streetlights).
- Rebuild and test with Open Watcom 2.0.

## License
GNU GPL v.3

## Contact
https://linkedin.com/in/lukaszolczyk
