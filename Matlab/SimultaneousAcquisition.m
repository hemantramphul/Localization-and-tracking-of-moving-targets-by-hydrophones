clc;
close all;
clear all;
a = arduino('com5','uno');
samples = 100;


figure;
h = plot(0,0,'o'); % initialize plot with dummy data
xlabel('X-axis');
ylabel('Y-axis');
title('Real-time Data');
xlim([0 samples]);
ylim([0 5]);

while true
    tic;
    b = zeros(samples, 3);
    for i = 1:samples
        v0 = readVoltage(a, 'A0');
        v1 = readVoltage(a, 'A1');
        v2 = readVoltage(a, 'A2');
        b(i,:) = [v0 v1 v2];
    end
    disp(b);
    set(h, 'XData', 1:samples, 'YData', b(:,1)); % update plot with new data
    drawnow; % refresh the plot
    toc;
end
