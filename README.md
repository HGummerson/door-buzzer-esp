## SMS-Controlled Door Buzzer

A Wi-Fi enabled project that allows a physical door buzzer to be pressed remotely by sending a text message.
Built using an ESP32-S3 microcontroller, a servo motor, a Cloudflare Worker relay, and Twilio SMS.

# Overview

This project uses a cloud-based relay to securely allow the text and hardware to communicate.
When a specific SMS keyword is received, a servo motor physically moves to presses a door buzzer.

The system is designed to:

- Avoid port forwarding or exposing the ESP32 directly to the internet
- Work over the ESP-32 Wifi features
- Demonstrate hardware + cloud integration

# How It Works

1. A text message containing the keyword buzz is sent to a Twilio phone number.
2. Twilio forwards the message to a Cloudflare Worker webhook.
3. The Worker sets a short-lived flag in Cloudflare KV.
4. The ESP32 polls the Worker endpoint every few seconds.
5. When the ESP32 detects a pending “buzz” event:
  - It actuates a servo motor
  - The servo physically presses the door buzzer
  - The event is cleared

# System Architecture

<img width="237" height="762" alt="arch drawio" src="https://github.com/user-attachments/assets/b16c6b05-355f-4d4d-b04b-ed42443df363" />

<img width="493" height="495" alt="esp32-wiring" src="https://github.com/user-attachments/assets/12c5ebc2-e4f2-4ce1-9799-99f5c0f5b1aa" />

# Hardware Used

- ESP32-S3 Dev Board
- SG90 Micro Servo
- Breadboard + jumper wires
- USB power
- Physical door buzzer/intercom button

# Software & Services

- Arduino IDE
- ESP32Servo library
- Cloudflare Workers + KV
- Twilio SMS Webhooks
- HTTP polling over Wi-Fi

# Security Notes

- No ports are opened on the local network
- The ESP32 only makes outbound HTTPS requests
- A key is required to poll the relay
- SMS commands are one-shot and time-limited via KV TTL
