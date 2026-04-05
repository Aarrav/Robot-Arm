#include <Arduino.h>
#include <micro_ros_arduino.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/float32_multi_array.h>

rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rclc_executor_t executor;
rcl_subscription_t cmd_sub;
std_msgs__msg__Float32MultiArray cmd_msg;
static float data_buffer[3];

HardwareSerial* uart_port = &Serial7;
bool uart_open(struct uxrCustomTransport* t)  { uart_port->begin(115200); return true; }
bool uart_close(struct uxrCustomTransport* t) { uart_port->end(); return true; }
size_t uart_write(struct uxrCustomTransport* t, const uint8_t* buf, size_t len, uint8_t* err) {
  return uart_port->write(buf, len);
}
size_t uart_read(struct uxrCustomTransport* t, uint8_t* buf, size_t len, int timeout, uint8_t* err) {
  unsigned long start = millis();
  size_t n = 0;
  while (n < len && (millis() - start) < (unsigned long)timeout)
    if (uart_port->available()) buf[n++] = uart_port->read();
  return n;
}

void cmd_callback(const void* msg_in) {
  const std_msgs__msg__Float32MultiArray* msg =
    (const std_msgs__msg__Float32MultiArray*)msg_in;

  Serial.println("--- Command Received ---");
  Serial.print("Initial pos: "); Serial.println(msg->data.data[0]);
  Serial.print("Final pos:   "); Serial.println(msg->data.data[1]);
  Serial.print("Duration:    "); Serial.println(msg->data.data[2]);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  rmw_uros_set_custom_transport(true, NULL, uart_open, uart_close, uart_write, uart_read);
  while (RMW_RET_OK != rmw_uros_ping_agent(1000, 10))
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  digitalWrite(LED_BUILTIN, HIGH);

  allocator = rcl_get_default_allocator();
  rclc_support_init(&support, 0, NULL, &allocator);
  rclc_node_init_default(&node, "teensy_node", "", &support);

  cmd_msg.data.data     = data_buffer;
  cmd_msg.data.size     = 0;
  cmd_msg.data.capacity = 3;

  rclc_subscription_init_default(&cmd_sub, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32MultiArray),
    "/move_base");

  rclc_executor_init(&executor, &support.context, 1, &allocator);
  rclc_executor_add_subscription(&executor, &cmd_sub, &cmd_msg, &cmd_callback, ON_NEW_DATA);

  Serial.println("Ready. Waiting for /move_base commands...");
}

void loop() {
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
}