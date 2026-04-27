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

// ── Base joint state ─────────────────────────────────────────
static float         baseStartPos  = 0.0f;
static float         baseEndPos    = 0.0f;
static float         baseDurMs     = 3000.0f;
static unsigned long baseStartMs   = 0;

// ── Shoulder joint state ─────────────────────────────────────
static float         shoulderStartPos = 0.0f;
static float         shoulderEndPos   = 0.0f;
static float         shoulderDurMs    = 3000.0f;
static unsigned long shoulderStartMs  = 0;

// ── micro-ROS handles ────────────────────────────────────────
rclc_support_t   support;
rcl_allocator_t  allocator;
rcl_node_t       node;
rclc_executor_t  executor;

rcl_subscription_t base_sub;
std_msgs__msg__Float32MultiArray base_msg;
static float base_data_buffer[2];

rcl_subscription_t shoulder_sub;
std_msgs__msg__Float32MultiArray shoulder_msg;
static float shoulder_data_buffer[2];

// ── Callbacks ────────────────────────────────────────────────
void base_callback(const void* msg_in) {
  const auto* msg = (const std_msgs__msg__Float32MultiArray*)msg_in;
  if (msg->data.size < 2) {
    Serial.println("Base error: need [final_pos, duration]");
    return;
  }
  baseEndPos   = msg->data.data[0];
  baseDurMs    = msg->data.data[1] * 1000.0f;
  baseStartPos = getBasePositionDeg();
  baseStartMs  = millis();

  Serial.println("--- Base Command ---");
  Serial.print("  Start:    "); Serial.println(baseStartPos);
  Serial.print("  End:      "); Serial.println(baseEndPos);
  Serial.print("  Duration: "); Serial.print(baseDurMs / 1000.0f);
  Serial.println("s");
}

void shoulder_callback(const void* msg_in) {
  const auto* msg = (const std_msgs__msg__Float32MultiArray*)msg_in;
  if (msg->data.size < 2) {
    Serial.println("Shoulder error: need [final_pos, duration]");
    return;
  }
  shoulderEndPos   = msg->data.data[0];
  shoulderDurMs    = msg->data.data[1] * 1000.0f;
  shoulderStartPos = getShoulderPositionDeg();
  shoulderStartMs  = millis();

  Serial.println("--- Shoulder Command ---");
  Serial.print("  Start:    "); Serial.println(shoulderStartPos);
  Serial.print("  End:      "); Serial.println(shoulderEndPos);
  Serial.print("  Duration: "); Serial.print(shoulderDurMs / 1000.0f);
  Serial.println("s");
}

// ── micro-ROS init ───────────────────────────────────────────
void initMicroROS() {
  rmw_uros_set_custom_transport(true, NULL,
    uart_open, uart_close, uart_write, uart_read);

  Serial.println("Waiting for micro-ROS agent...");
  while (RMW_RET_OK != rmw_uros_ping_agent(1000, 10))
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Agent connected!");

  allocator = rcl_get_default_allocator();
  rclc_support_init(&support, 0, NULL, &allocator);
  rclc_node_init_default(&node, "teensy_arm_node", "", &support);

  // Base subscriber
  base_msg.data.data     = base_data_buffer;
  base_msg.data.size     = 0;
  base_msg.data.capacity = 2;
  rclc_subscription_init_default(&base_sub, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32MultiArray),
    "/base/move");

  // Shoulder subscriber
  shoulder_msg.data.data     = shoulder_data_buffer;
  shoulder_msg.data.size     = 0;
  shoulder_msg.data.capacity = 2;
  rclc_subscription_init_default(&shoulder_sub, &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32MultiArray),
    "/shoulder/move");

  // Executor — must init BEFORE adding subscriptions
  rclc_executor_init(&executor, &support.context, 2, &allocator);
  rclc_executor_add_subscription(&executor, &base_sub,
    &base_msg, &base_callback, ON_NEW_DATA);
  rclc_executor_add_subscription(&executor, &shoulder_sub,
    &shoulder_msg, &shoulder_callback, ON_NEW_DATA);
}

// ── Setup ────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  motorInit();
  initMicroROS();

  // Initialise trajectory start positions
  baseStartPos     = getBasePositionDeg();
  shoulderStartPos = getShoulderPositionDeg();
  baseStartMs      = millis();
  shoulderStartMs  = millis();

  Serial.println("Ready. Waiting for /base/move and /shoulder/move commands...");
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

    float baseTarget = trajectoryGenerator(
      baseEndPos, baseStartPos,
      millis() - baseStartMs, (unsigned long)baseDurMs);

    float shoulderTarget = trajectoryGenerator(
      shoulderEndPos, shoulderStartPos,
      millis() - shoulderStartMs, (unsigned long)shoulderDurMs);

    moveBaseToPosition(baseTarget);
    moveShoulderToPosition(shoulderTarget);
    printDebugInfo(baseTarget, shoulderTarget);
  }

  // 10ms micro-ROS check
  if (nowUs - lastRosUs >= 10000) {
    lastRosUs = nowUs;
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(1));
  }
}