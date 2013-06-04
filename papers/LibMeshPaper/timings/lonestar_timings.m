% These are the timings obtained on lonestar
% 150x150 case has 99 assemble/ 72 solve
% 300x300 case has 92 assemble/ 67 solve (steady state detected quicker)

clear all


% N CPU,    avg. time/call,   % of total     
assembly_150x150 = [
    1          3.850125          19.85       
    2          1.997381          13.06       
    4          1.061510          12.67       
    8          0.543466          12.77       
    16         0.261840          11.35       
    32         0.153834           9.78       
    64         0.078315           6.69       
    128        0.051551           4.46       
    ];


% N CPU,    avg. time/call,   % of total      
assembly_300x300 = [
    1                      15.895476        8.63
    2                      7.536021         6.37
    4                      4.122012         6.26
    8                      2.068845         5.97
    16                     1.135907         6.47
    32                     0.554049         5.55
    64                     0.323891         5.19
%    128                    inf              inf
    ];





% N CPU,    avg. time/call,   % of total
solve_150x150 = [
    1          19.195939       71.98    
    2          17.146597       81.56    
    4          9.462256        82.14    
    8          4.743822        81.09    
    16         2.586162        81.81    
    32         1.756749        81.25    
    64         1.341291        83.30    
    128        1.273052        80.13    
];


% N CPU,         avg. time/call,   % of total
solve_300x300 = [
    1             222.159680        87.87
    2             147.874366         90.98
    4             82.477886          91.15
    8              43.412377         91.20
    16             21.667418         89.94
    32             12.246108         89.77
    64             7.373607          86.05
%    128            inf              inf
];



% N CPU,    "alive" time,   "active" time 
alive_active_150x150 = [
    1          1838.26        1920.1      
    2          1478.61        1513.63     
    4          813.171        829.378     
    8          418.096        421.226     
    16         235.213        230.763     
    32         163.417        155.676     
    64         128.016        115.931     
    128        130.745        114.384     
];


% N CPU,    "alive" time,   "active" time 
alive_active_300x300 = [
    1                   16632.8          16940.2
    2                   10760.5          10890.2
    4                   6021.43          6062.56
    8                   3174.81          3189.23          
    16                  1609.73          1614.06
    32                  942.426          927.587
    64                  605.31           574.119
%    128                 inf              inf
];





figure(1);
clf
hold on
leg = cell(0);

plot_assembly=true;
if (plot_assembly)
  % Plot the assembly times
  plot(assembly_150x150(:,1), assembly_150x150(1,2) ./ assembly_150x150(:,2), 'bs-'); % 150x150
  plot(assembly_300x300(:,1), assembly_300x300(1,2) ./ assembly_300x300(:,2), 'rs-'); % 300x300
  leg{length(leg)+1} = 'assembly time (150)';
  leg{length(leg)+1} = 'assembly time (300)';
end

plot_solve=true;
if (plot_solve)
  % Plot the solve times
  plot(solve_150x150(:,1), solve_150x150(1,2) ./ solve_150x150(:,2), 'bo-'); % 150x150
  plot(solve_300x300(:,1), solve_300x300(1,2) ./ solve_300x300(:,2), 'ro-'); % 300x300
  leg{length(leg)+1} = 'solve time (150)';
  leg{length(leg)+1} = 'solve time (300)';
end

% Note: alive time is like wall time.
plot_alive=true;
if (plot_alive)
  % Plot the alive times
  plot(alive_active_150x150(:,1), alive_active_150x150(1,2) ./ alive_active_150x150(:,2), 'b^-'); % 150x150
  plot(alive_active_300x300(:,1), alive_active_300x300(1,2) ./ alive_active_300x300(:,2), 'r^-'); % 300x300
  leg{length(leg)+1} = 'wall time (150)';
  leg{length(leg)+1} = 'wall time (300)';
end

%plot_active=true;
plot_active=false;
if (plot_active)
  % Plot the active times
  plot(alive_active_150x150(:,1), alive_active_150x150(1,3) ./ alive_active_150x150(:,3), 'bx-'); % 150x150
  plot(alive_active_300x300(:,1), alive_active_300x300(1,3) ./ alive_active_300x300(:,3), 'rx-'); % 300x300
  leg{length(leg)+1} = 'active time (150)';
  leg{length(leg)+1} = 'active time (300)';
end

% The CPU line is the union of all possible CPU configurations
cpu = union(assembly_150x150(:,1), assembly_300x300(:,1));

% cpu_tick= intersect(assembly_150x150(:,1), assembly_300x300(:,1));

% Plot y=x ideal speedup line
plot(cpu, cpu, 'b--');


xlabel('N. CPUs');
ylabel('normalized speedup');
legend(leg,0);
set(gca,'XTick',cpu);
set(gca,'XLim', [cpu(1) cpu(length(cpu))]);
set(gca,'YLim', [cpu(1) cpu(length(cpu))]);
axis square


% % Print commands
% orient landscape
% print('-dpdf', 'lonestar_speedup.pdf');
