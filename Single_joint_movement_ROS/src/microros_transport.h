#pragma once
#include <Arduino.h>
#include <uxr/client/transport.h>

extern HardwareSerial* uart_port;

inline bool uart_open(struct uxrCustomTransport* t) {
  uart_port->begin(115200); return true;
}
inline bool uart_close(struct uxrCustomTransport* t) {
  uart_port->end(); return true;
}
inline size_t uart_write(struct uxrCustomTransport* t, const uint8_t* buf, size_t len, uint8_t* err) {
  return uart_port->write(buf, len);
}
inline size_t uart_read(struct uxrCustomTransport* t, uint8_t* buf, size_t len, int timeout, uint8_t* err) {
  unsigned long start = millis();
  size_t n = 0;
  while (n < len && (millis() - start) < (unsigned long)timeout)
    if (uart_port->available()) buf[n++] = uart_port->read();
  return n;
}