clc;
close all;
clear all;
a = arduino('com5','uno');
samples = 100;

figure;
hold on;
h1 = plot(NaN, 'r');
h2 = plot(NaN, 'g');
h3 = plot(NaN, 'b');
legend('Mic 1', 'Mic 2', 'Mic 3');

while true
    tic;
    b = zeros(samples, 3);
    for i = 1:samples
        v0 = readVoltage(a, 'A0');
        v1 = readVoltage(a, 'A1');
        v2 = readVoltage(a, 'A2');
        b(i,:) = [v0 v1 v2];
    end
    
    set(h1, 'YData', b(:,1));
    set(h2, 'YData', b(:,2));
    set(h3, 'YData', b(:,3));
    
    drawnow;
    toc;
end
