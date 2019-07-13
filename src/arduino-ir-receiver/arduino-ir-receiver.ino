/*
This file is part of a "The Pink Owl" project.

It is a free software under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE Version 3:
you can redistribute it and/or modify under the same terms.

Please see LICENSE file at the root of this project or visit: http://www.affero.org/oagf.html

@author Valentin Viennot
*/

// the setup function runs once when you press reset or power the board
void setup()
{
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop()
{
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(1000);                     // wait for a second
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
    delay(1000);                     // wait for a second
}