/*
This file is part of a "The Pink Owl" project.

It is a free software under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE Version 3:
you can redistribute it and/or modify under the same terms.

Please see LICENSE file at the root of this project or visit: http://www.affero.org/oagf.html

@author Valentin Viennot
*/
#include <IRremote.h>

// // the loop function runs over and over again forever
// void loop()
// {
//     digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
//     delay(1000);                     // wait for a second
//     digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
//     delay(1000);                     // wait for a second
// }

String ir[3];
unsigned int irBuf[100];
unsigned int irBufLen;
unsigned int irBufType;
boolean repeat = false;

int recvPin = 11;
IRrecv irrecv(recvPin); //pin 11
IRsend irsend;          //pin 3 on UNO,  Pin 13 on Leo, Pin 9 on Mega

void setup(void)
{
    // pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    irrecv.enableIRIn();
    Serial.println("Start");
}

void loop(void)
{
    decode_results results;
    if (irrecv.decode(&results))
    {                       // Grab an IR code
        dumpCode(&results); // Output the results as source code
        delay(50);
        irrecv.resume(); // Prepare for the next value
    }

    if (repeat)
    {
        sendCode();
    }
}

void dumpCode(decode_results *results)
{
    int codeType = results->decode_type;
    int codeLen = results->bits;
    unsigned long codeValue = results->value;
    String data_status;
    data_status += F("{\"T\":\"");
    data_status += codeType;
    data_status += F("\",\"D\":[\"");
    data_status += codeValue;
    data_status += F("\",\"");
    for (int i = 1; i < results->rawlen; i++)
    {
        data_status += results->rawbuf[i] * USECPERTICK;
        if (i != results->rawlen - 1)
            data_status += ",";
    }
    data_status += F(",\",\"");
    data_status += results->rawlen;

    data_status += F("\"]}");
    Serial.println(F("Got IR Code"));
    Serial.println(data_status);
}

void sendCode()
{

    irsend.sendRaw(irBuf, irBufLen, 38);
    delay(50);
    irrecv.enableIRIn();
}

void stringToIntArry(String irRawString)
{
    String dataShort = "";
    int counter = 0;
    for (int i = 0; i <= irRawString.length(); i++)
    {
        dataShort += irRawString[i];
        if (irRawString[i] == ',')
        {
            dataShort = dataShort.substring(0, dataShort.length() - 1);
            irBuf[counter] = dataShort.toInt();
            counter = counter + 1;
            dataShort = "";
        }
    }
}