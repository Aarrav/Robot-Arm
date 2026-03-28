/*
#include <Arduino.h>
#include <micro_ros_arduino.h>
#include <rcl/rcl.h> // core ROS 2 client library
#include <rclc/rclc.h> // rclc convenience functions for easier use of rcl
#include <rclc/executor.h> // rclc executor for callbacks - timers, subscribers, etc.
#include <std_msgs/msg/float32.h>

void error_loop(int blinks) {
  while(1) {
    for(int i = 0; i < blinks; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(300);
      digitalWrite(LED_BUILTIN, LOW);
      delay(300);
    }
    delay(1500);
  }
}

// Runs a function (fn). If the result is not RCL_RET_OK (success), it calls error_loop(code)
#define RCCHECK(fn, code) { rcl_ret_t rc = fn; if(rc != RCL_RET_OK){ error_loop(code); }} 

//It runs the function but doesn't stop the program if it fails.
#define RCSOFTCHECK(fn) { rcl_ret_t rc = fn; (void)rc; }

HardwareSerial* uart_port = &Serial7;

bool uart_open(struct uxrCustomTransport* transport) {
  uart_port->begin(115200);
  return true;
}
bool uart_close(struct uxrCustomTransport* transport) {
  uart_port->end();
  return true;
}
size_t uart_write(struct uxrCustomTransport* transport, const uint8_t* buf, size_t len, uint8_t* err) {
  return uart_port->write(buf, len);
}
size_t uart_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err) {
  unsigned long start = millis();
  size_t received = 0;
  while (received < len && (millis() - start) < (unsigned long)timeout) {
    if (uart_port->available()) {
      buf[received++] = uart_port->read();
    }
  }
  return received;
}

rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rclc_executor_t executor;
rcl_publisher_t sensor_publisher;
std_msgs__msg__Float32 sensor_msg;
rcl_timer_t timer;
rcl_subscription_t cmd_subscriber;
std_msgs__msg__Float32 cmd_msg;

void timer_callback(rcl_timer_t* timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    sensor_msg.data = (float)analogRead(A0) * (3.3f / 1023.0f);
    RCSOFTCHECK(rcl_publish(&sensor_publisher, &sensor_msg, NULL));
  }
}

void cmd_callback(const void* msg_in) {
  const std_msgs__msg__Float32* msg = (const std_msgs__msg__Float32*)msg_in;
  Serial.print("Received: ");
  Serial.println(msg->data);
  Serial.flush();  // force it out immediately
  digitalWrite(LED_BUILTIN, msg->data > 0 ? HIGH : LOW);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  digitalWrite(LED_BUILTIN, LOW);

  rmw_uros_set_custom_transport(true, NULL, uart_open, uart_close, uart_write, uart_read); //handshake (ROS middlware)

  while (RMW_RET_OK != rmw_uros_ping_agent(1000, 10)) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(100);
  }
  digitalWrite(LED_BUILTIN, HIGH); // agent found
  delay(2000);

  allocator = rcl_get_default_allocator();
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator), 1);
  RCCHECK(rclc_node_init_default(&node, "teensy_arm_node", "", &support), 2);
  RCCHECK(rclc_publisher_init_default(
    &sensor_publisher, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
    "arm/sensor"), 3);
  RCCHECK(rclc_subscription_init_default(
    &cmd_subscriber, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
    "arm/cmd"), 4);
  RCCHECK(rclc_timer_init_default(
    &timer, &support, RCL_MS_TO_NS(100), timer_callback), 5);
  RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator), 6);
  RCCHECK(rclc_executor_add_timer(&executor, &timer), 7);
  RCCHECK(rclc_executor_add_subscription(
    &executor, &cmd_subscriber, &cmd_msg, &cmd_callback, ON_NEW_DATA), 8);

  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  if (RMW_RET_OK == rmw_uros_ping_agent(100, 1)) {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    //digitalWrite(LED_BUILTIN, HIGH);
    
  } else {
    // Lost connection — restart
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    // Reinitialize
    rcl_publisher_fini(&sensor_publisher, &node);
    rcl_subscription_fini(&cmd_subscriber, &node);
    rcl_timer_fini(&timer);
    rcl_node_fini(&node);
    rclc_support_fini(&support);

    // Wait for agent to come back
    while (RMW_RET_OK != rmw_uros_ping_agent(1000, 10)) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(100);
    }

    // Reinit everything
    allocator = rcl_get_default_allocator();
    RCCHECK(rclc_support_init(&support, 0, NULL, &allocator), 1);
    RCCHECK(rclc_node_init_default(&node, "teensy_arm_node", "", &support), 2);
    RCCHECK(rclc_publisher_init_default(
      &sensor_publisher, &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
      "arm/sensor"), 3);
    RCCHECK(rclc_subscription_init_default(
      &cmd_subscriber, &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
      "arm/cmd"), 4);
    RCCHECK(rclc_timer_init_default(
      &timer, &support, RCL_MS_TO_NS(100), timer_callback), 5);
    RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator), 6);
    RCCHECK(rclc_executor_add_timer(&executor, &timer), 7);
    RCCHECK(rclc_executor_add_subscription(
      &executor, &cmd_subscriber, &cmd_msg, &cmd_callback, ON_NEW_DATA), 8);

    digitalWrite(LED_BUILTIN, HIGH);
  }
}
*/

#include <Arduino.h>
#include <micro_ros_arduino.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/float32.h>

// ── Handles ──────────────────────────────────────────────────
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rclc_executor_t executor;

rcl_publisher_t sensor_pub;
std_msgs__msg__Float32 sensor_msg;
rcl_timer_t timer;

rcl_subscription_t cmd_sub;
std_msgs__msg__Float32 cmd_msg;

// ── UART transport ───────────────────────────────────────────
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

// ── Callbacks ────────────────────────────────────────────────
void timer_callback(rcl_timer_t* timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);
  sensor_msg.data = (float)analogRead(A0) * (3.3f / 1023.0f);
  rcl_publish(&sensor_pub, &sensor_msg, NULL);
}

void cmd_callback(const void* msg_in) {
  float val = ((std_msgs__msg__Float32*)msg_in)->data;
  Serial.print("Received: ");
  Serial.println(val);
  digitalWrite(LED_BUILTIN, val > 0 ? HIGH : LOW);
}

// ── Setup ────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  // Connect to agent
  rmw_uros_set_custom_transport(true, NULL, uart_open, uart_close, uart_write, uart_read);
  while (RMW_RET_OK != rmw_uros_ping_agent(1000, 10))
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  digitalWrite(LED_BUILTIN, HIGH);

  // Init micro-ROS
  allocator = rcl_get_default_allocator();
  rclc_support_init(&support, 0, NULL, &allocator);
  rclc_node_init_default(&node, "teensy_node", "", &support);

  // Publisher
  rclc_publisher_init_default(&sensor_pub, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32), "arm/sensor");

  // Subscriber
  rclc_subscription_init_default(&cmd_sub, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32), "arm/cmd");

  // Timer — publish every 100ms
  rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(100), timer_callback);

  // Executor — 2 handles: 1 timer + 1 subscriber
  rclc_executor_init(&executor, &support.context, 2, &allocator);
  rclc_executor_add_timer(&executor, &timer);
  rclc_executor_add_subscription(&executor, &cmd_sub, &cmd_msg, &cmd_callback, ON_NEW_DATA);
}

// ── Loop ─────────────────────────────────────────────────────
void loop() {
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));
}