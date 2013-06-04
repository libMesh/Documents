% Here we plot some of the data from timings.txt,
% obtained on CFDLab workstation cluster.

% The time taken for matrix assembly (time in seconds)
% The times are for 99 calls to assemble()
% N CPU,  avg. time/call,   percent of total runtime
assembly = [
    1          6.626211    30.26
    2          3.327086    24.06        
    3          2.215123    21.84
    4          1.640559    20.76
    5          1.311823    15.49
    6          1.100951    16.16
    7          0.942001    18.56
    8          0.835295    19.89
    % 16         0.631934    16.69 % 2 jobs/CPU
    ];




% The time taken for solving linear systems (time in seconds)
% N CPU,  avg. time/call,   percent of total runtime.  The times
% are for 72 total calls to solve().
solve = [
    1   18.765639   62.32        
    2   13.313324   70.02        
    3   10.130532   72.66
    4   8.011031    73.74
    5   9.365534    80.41
    6   7.440152    79.44
    7   5.325127    76.30
    8   4.303636    74.54
    % 16  4.059456    77.98 % 2 jobs/CPU
    ];


% This is the "active" time reported by libmesh
% for solving the full problem. Times in seconds.
% Actually, the "alive" time is the best metric to use...
% That is the wall time!
active_time = [
    1    2167.94
    2    1368.91
    3    1003.88
    4    782.244 
    5    838.592
    6    674.367
    7    502.483
    8    415.693
    % 16   374.801 % 2 jobs/CPU
    ];

% Wall time
alive_time = [
    1    2095.47
    2    1358.59
    3    1029.45
    4    828.885 
    5    901.814
    6    752.006
    7    563.695
    8    469.445
    % 16 481.509   % 2 jobs/CPU
];    

figure(1);
clf
hold on

% Plot the assembly times
plot(assembly(:,1), assembly(1,2) ./ assembly(:,2), 'bs-');

% Plot the solve times
plot(solve(:,1), solve(1,2) ./ solve(:,2), 'bo-');

% Plot the active times
% plot(active_time(:,1), active_time(1,2) ./ active_time(:,2), 'b^-');

% Plot the alive (wall) times
plot(alive_time(:,1), alive_time(1,2) ./ alive_time(:,2), 'b^-');

% Plot y=x ideal speedup line
plot(active_time(:,1), active_time(:,1), 'b--');

xlabel('N. CPUs');
ylabel('normalized time');
legend('assembly time', 'solve time', 'wall time', '"ideal speed-up"',0);
orient landscape
print('-dpdf', 'cfdlab_speedup.pdf');


figure(2);
clf
hold on

% The bars have to overlap in the right order...
bar(solve(:,1), ones(1,length(solve(:,1))), 'k'); % "other" -- th
bar(solve(:,1), ...
    (solve(:,3)+assembly(:,3)) ./ 100, ...
    'r'); % "solve" time

bar(assembly(:,1), assembly(:,3) ./ 100, 'b'); % assembly time
set(gca,'XTick',linspace(1,8,8));
xlabel('N. CPUs');
ylabel('Percent of total time / 100');
legend('other', 'solve', 'assembly', 0);
orient landscape
print('-dpdf', 'cfdlab_breakdowns.pdf');
