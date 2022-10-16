#include "pico/stdlib.h"

/*
pin #   	*2C33 pin	*RAM pins	signal description
-----   	---------	---------	------------------
1		50		5 (green)	(O) -write
2		64		C (cyan)	(O) VCC (+5VDC)
3		49		6 (blue)	(O) -scan media
4		32		1 (brown)	(O) VEE (ground)
5		52		3 (orange)	(O) write data
6		37		B (pink)	(I) motor on/battery good
7		47		8 (grey)	(I) -writable media
8		-		-       	(I) motor power (note 1)
9		51		4 (yellow)	(I) read  data
A		45		A (black)	(I) -media set
B		46		9 (white)	(I) -ready
C		48		7 (violet)	(O) -stop motor
*/



//const uint LED_PIN = 25;

// Debug
const uint DEBUG_01_PIN = 0;
const uint DEBUG_02_PIN = 1;

// Output
const uint READY_PIN = 2; // IN, inverted
const uint MEDIA_SET_PIN = 3; // IN, inverted
const uint MOTOR_ON_PIN = 4; // IN
const uint READ_DATA_PIN = 5; // IN
const uint WRITABLE_MEDIA_PIN = 6; // IN, inverted

// Input
const uint WRITE_PIN = 7; // OUT, inverted
const uint SCAN_MEDIA_PIN = 8; // OUT, inverted
const uint WRITE_DATA_PIN = 9; // OUT
const uint STOP_MOTOR = 10; // OUT, inverted