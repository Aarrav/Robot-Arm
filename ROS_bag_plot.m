%% Extracting Hardware Feedback from /arm/joint_states
clear; clc;

% 1. Load the bag
bag = ros2bag('logged_data'); 

% 2. Select the feedback topic
% According to your Teensy code, this is where the real data is
sel = select(bag, 'Topic', '/arm/joint_states');

if sel.NumMessages == 0
    error('Topic /arm/joint_states is empty. Check if micro-ROS was publishing.');
end

% 3. Read all messages
msgs = readMessages(sel);

% 4. Extract Time (using the MessageList table to avoid 'Unrecognized method' error)
msgTable = sel.MessageList;
timeSecs = seconds(msgTable.Time - msgTable.Time(1));

% 5. Extract Data (Teensy order: [base_pos, base_vel, shld_pos, shld_vel])
basePos   = cellfun(@(m) double(m.data(1)), msgs);
baseVel   = cellfun(@(m) double(m.data(2)), msgs);
shldPos   = cellfun(@(m) double(m.data(3)), msgs);
shldVel   = cellfun(@(m) double(m.data(4)), msgs);

%% 6. Plotting Feedback
figure('Color', 'w', 'Position', [100 100 800 600]);

% Plot Position
subplot(2,1,1);
plot(timeSecs, basePos, 'b', 'LineWidth', 1.5, 'DisplayName', 'Base');
hold on;
plot(timeSecs, shldPos, 'r', 'LineWidth', 1.5, 'DisplayName', 'Shoulder');
grid on; ylabel('Position (Degrees)');
title('Actual Arm Positions (Hardware Feedback)');
legend;

% Plot Velocity
subplot(2,1,2);
plot(timeSecs, baseVel, 'b--', 'DisplayName', 'Base Vel');
hold on;
plot(timeSecs, shldVel, 'r--', 'DisplayName', 'Shoulder Vel');
grid on; xlabel('Time (seconds)'); ylabel('Velocity (deg/s)');
title('Actual Arm Velocities');
legend;