clear all

np=cell(0);
wtime=cell(0);





leg={'Columbia 150x150',   % i=1 is the Columbia 150x150 case
     'Columbia 300x300',   % i=2 is the Columbia 300x300 case
     'Lonestar 150x150',   % i=3 is the Lonestar 150x150 case
     'Lonestar 300x300',   % i=4 is the Lonestar 300x300 case
     };

if exist('OCTAVE_VERSION')
  colors = {'b',
	    'b',
	    'r',
	    'r'};

else
  colors = {'bs-',
	    'bs--',
	    'ro-',
	    'ro--'};
end


% Columbia 150x150 case
np{1} = [ 
     1
     2
     4
     8
     16
     24
     32
     48
     64
     ];

% Columbia 300x300 case
np{2} = [     
    1         
    2         
    4         
    8         
    16        
    24        
    32        
    48        
    54        
    60        
    64        
    72        
    80        
    96        
    128       
	];

% Lonestar 150x150
np{3} = [
    1      
    2      
    4      
    8      
    16     
    32     
    64     
    128    
    ];





% Lonestar 150x150
wtime{3} = [
1838.26
1478.61
813.171
418.096
235.213
163.417
128.016
130.745
    ];



 % Columbia 150x150 case, 1st and 2nd series average
 wtime{1} = [
     (1995    + 1986) / 2.0
     (1185    + 1202) / 2.0
     (515     + 521) / 2.0
     (214     + 220) / 2.0
     (101     + 103) / 2.0
     (80     + 81  ) / 2.0
     (72     + 71  ) / 2.0
     (70     + 63  ) / 2.0
     (65     + 65  ) / 2.0
     ];



 % Columbia 300x300 case
 wtime{2} = [
     25900
     14223
     6743
     3118
     1465
     953
     610
     376
     316
     303
     330
     345
     374
     489
     759
     ];



% Lonestar 300x300
np{4} = [
    1      
    2      
    4      
    8      
    16     
    32     
    64
    128
    ];


% Lonestar 300x300
wtime{4} = [
    16632.8
    10760.5
    6021.43
    3174.81
    1609.73
    942.426
    605.31
    686.786    
    ];
 


% Get union of CPU configurations


figure(1);
clf;
all_cpu=[];
hold on

for i=1:length(np)
  all_cpu = union(all_cpu, np{i});
  
  x = np{i};

  
if (i>2)
  y = 2.* wtime{i}(2) ./ wtime{i};
else
  y = wtime{i}(1) ./ wtime{i};
end

  plot (x, y, colors{i});
  
  %  y = wtime2{i}(1) ./ wtime2{i};
  %  plot (x, y, 'b-+');
  title('walltime - fixed size speedup');
  xlabel('N. CPUs');
end

plot (all_cpu, all_cpu, 'k-');

orient landscape


if exist('OCTAVE_VERSION')
  %Octave stuff;
  print('-dps', 'all_walltime.ps');
else
  legend(leg,0);
  axis square
  set(gca,'XTick',all_cpu);
  set(gca,'XLim',[all_cpu(1) all_cpu(length(all_cpu))]);
  set(gca,'YLim',[all_cpu(1) all_cpu(length(all_cpu))]);
  print('-dpdf', 'all_walltime.pdf');
end



