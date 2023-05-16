clc;
close all;
clear all;
a = arduino('com5','uno');
samples = 10;

b = zeros(samples, 3);
while true
    tic;
    for i = 1:samples
        v0 = readVoltage(a, 'A0');
        v1 = readVoltage(a, 'A1');
        v2 = readVoltage(a, 'A2');
        b(i,:) = [v0 v1 v2];
    end
    disp(b);
    toc;
end
