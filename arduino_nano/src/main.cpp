#include <Arduino.h>

String msg;

unsigned long BAUD_RATE = 9600;

uint8_t FORWARD_PIN1 = PD2;
uint8_t FORWARD_PIN2 = PD3;

uint8_t BACKWARD_PIN1 = PD4;
uint8_t BACKWARD_PIN2 = PD5;

void forward_start()
{
    digitalWrite(FORWARD_PIN1, HIGH);
    digitalWrite(FORWARD_PIN2, LOW);
}

void forward_stop()
{
    digitalWrite(FORWARD_PIN1, LOW);
    digitalWrite(FORWARD_PIN2, HIGH);
}

void backward_start()
{
    digitalWrite(BACKWARD_PIN1, HIGH);
    digitalWrite(BACKWARD_PIN2, LOW);
}

void backward_stop()
{
    digitalWrite(BACKWARD_PIN1, LOW);
    digitalWrite(BACKWARD_PIN2, HIGH);
}

void handshake(String msg)
{
    if (msg.equals("who are you?"))
    {
        Serial.print("focuser\n");
    }
}

void focuser_control(String msg)
{
    if (msg.equals("forward_start"))
    {
        forward_start();
        backward_stop();
    }
    else if (msg.equals("forward_stop"))
    {
        forward_stop();
        backward_stop();
    }
    else if (msg.equals("backward_start"))
    {
        backward_start();
        forward_stop();
    }
    else if (msg.equals("backward_stop"))
    {
        backward_stop();
        forward_stop();
    }
}

String receive()
{
    msg = Serial.readString();
    msg.trim();
    return msg;
}

void echo(String msg)
{
    Serial.println("I received: " + msg);
}

void setup()
{
    pinMode(FORWARD_PIN1, OUTPUT);
    pinMode(FORWARD_PIN2, OUTPUT);

    pinMode(BACKWARD_PIN1, OUTPUT);
    pinMode(BACKWARD_PIN2, OUTPUT);

    Serial.begin(BAUD_RATE);
    msg = "";

    forward_stop();
    backward_stop();
}

void loop()
{
    msg = receive();

    handshake(msg);
    // focuser_control(msg);
}
