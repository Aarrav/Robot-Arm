%% Real-Time Serial Plotter with Time Axis (ms)
clear; clc; close all;

% -- Configuration --
port = "/dev/tty.usbmodem178375201";          % <--- Change this to your Teensy COM port
baudrate = 9600;      
sampleIntervalMs = 2;  % Your 10ms loop
windowSize = 4000;       % Number of points shown on screen

% Create the time vector (0, 10, 20, ..., up to window size)
timeLog = (0:windowSize-1) * sampleIntervalMs;
dataLog = zeros(1, windowSize); 

% -- Initialize Serial Port --
device = serialport(port, baudrate);
configureTerminator(device, "LF"); 
flush(device);

% -- Setup Figure --
fig = figure('Name', 'Teensy Motor Data', 'Color', [1 1 1]);
hPlot = plot(timeLog, dataLog, 'LineWidth', 1.5, 'Color', [0.850, 0.325, 0.098]);
grid on;
xlabel('Time (ms)');
ylabel('Velocity (deg/s)');
title('Velocity vs Time');
ylim([-80 80]); % Adjust as needed

% -- Animation Loop --
disp("Acquiring data... Close figure to stop.");

while ishandle(fig)
    strData = readline(device);
    val = str2double(strData);
    
    if ~isnan(val)
        % Update the data buffer (Last-In, First-Out)
        dataLog = [dataLog(2:end), val];
        
        % Update Y-Data (X-Data stays the same as it's a "scrolling" window)
        set(hPlot, 'YData', dataLog);
        
        % Optional: Dynamically adjust the X-axis labels to show "real" time
        % uncomment the next line if you want the X-axis to show total elapsed time
        % set(hPlot, 'XData', (totalSamples-windowSize:totalSamples-1)*10);
        
        drawnow limitrate
    end
end

clear device;
disp("Serial Connection Closed.");