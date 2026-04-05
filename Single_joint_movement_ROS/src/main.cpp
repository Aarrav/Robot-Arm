#include <Arduino.h>
#include <micro_ros_arduino.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/float32_multi_array.h>

#include "motor_utils.h"
#include "microros_transport.h"
#include "trajectory.h"

// ── UART port for micro-ROS ──────────────────────────────────
HardwareSerial* uart_port = &Serial7;

// ── Trajectory state ─────────────────────────────────────────
static float         startPos    = 0.0f;
static float         endPos      = 0.0f;
static float         moveDurMs   = 3000.0f;
static unsigned long moveStartMs = 0;

// ── micro-ROS handles ────────────────────────────────────────
rclc_support_t   support;
rcl_allocator_t  allocator;
rcl_node_t       node;
rclc_executor_t  executor;
rcl_subscription_t cmd_sub;
std_msgs__msg__Float32MultiArray cmd_msg;
static float data_buffer[2];  // [final_pos, duration]

// ── ROS callback ─────────────────────────────────────────────
void cmd_callback(const void* msg_in) {
  const auto* msg = (const std_msgs__msg__Float32MultiArray*)msg_in;
  if (msg->data.size < 2) { Serial.println("Error: need [final_pos, duration]"); return; }

  endPos      = msg->data.data[0];
  moveDurMs   = msg->data.data[1] * 1000.0f;  // seconds → ms
  startPos    = getPositionDeg();               // start from current position
  moveStartMs = millis();

  Serial.println("--- Command Received ---");
  Serial.print("  Start:    "); Serial.println(startPos);
  Serial.print("  End:      "); Serial.println(endPos);
  Serial.print("  Duration: "); Serial.print(moveDurMs / 1000.0f); Serial.println("s");
}

// ── micro-ROS init ───────────────────────────────────────────
void initMicroROS() {
  rmw_uros_set_custom_transport(true, NULL, uart_open, uart_close, uart_write, uart_read);

  Serial.println("Waiting for micro-ROS agent...");
  while (RMW_RET_OK != rmw_uros_ping_agent(1000, 10))
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Agent connected!");

  allocator = rcl_get_default_allocator();
  rclc_support_init(&support, 0, NULL, &allocator);
  rclc_node_init_default(&node, "teensy_arm_node", "", &support);

  cmd_msg.data.data     = data_buffer;
  cmd_msg.data.size     = 0;
  cmd_msg.data.capacity = 2;

  rclc_subscription_init_default(&cmd_sub, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32MultiArray),
    "/move_base");

  rclc_executor_init(&executor, &support.context, 1, &allocator);
  rclc_executor_add_subscription(&executor, &cmd_sub, &cmd_msg, &cmd_callback, ON_NEW_DATA);
}

// ── Setup ────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  motorInit();
  initMicroROS();

  startPos    = getPositionDeg();
  moveStartMs = millis();
  Serial.println("Ready. Waiting for /move_base commands...");
}

// ── Loop ─────────────────────────────────────────────────────
void loop() {
  unsigned long nowUs = micros();
  static unsigned long lastControlUs = 0;
  static unsigned long lastRosUs     = 0;

  // 500us control loop
  if (nowUs - lastControlUs >= 500) {
    lastControlUs = nowUs;
    updateVelocity();
    float target = trajectoryGenerator(endPos, startPos, millis() - moveStartMs, (unsigned long)moveDurMs);
    moveToPosition(target);
    printDebugInfo(target);
  }

  // 10ms micro-ROS check
  if (nowUs - lastRosUs >= 10000) {
    lastRosUs = nowUs;
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1));
  }
}