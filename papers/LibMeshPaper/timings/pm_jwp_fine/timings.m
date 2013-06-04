 np = [ 
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
 
 % 1st series
 wtime1 = [
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

wtime2 = wtime1;

%  % assembly + linear solve - 2nd series
%  apls = [
%      1781.6045
%      1086.6862
%      447.4486
%      175.0400
%      68.3882
%      51.3649
%      41.9875
%      31.8571
%      26.7878
% 	];
	
%  % assembly - 2nd series
%  assmbly = [
%      151.4155
%      76.4381
%      39.0462
%      19.9994
%      10.7010
%      7.4584
%      6.0946
%      4.4993
%      3.5799
% 	   ];
 
%  % linear solve - 2nd series
%  ls = apls - assmbly;

 
 x = np;
 
%-------------------------------------------
 % plot the wall time speedup for both series
 figure(1);
 clf
 plot (x, x, 'k-');
 hold on

 y = wtime1(1) ./ wtime1;
 
 plot (x, y, 'r-o');
 
 y = wtime2(1) ./ wtime2;
 
 plot (x, y, 'b-+');
 title('walltime - fixed size speedup (2 trials)');
 xlabel('N. CPUs');
 legend('linear speed-up', 'series 1', 'series 2',0);
 
if exist('OCTAVE_VERSION')
  %Octave stuff;
else
  axis square
  set(gca,'XTick',np);
  set(gca,'XLim',[np(1) np(length(np))]);
  set(gca,'YLim',[np(1) np(length(np))]);
end


%  %-----------------------------------
%  % plot the components for 2nd series 
%  figure(2);
% clf
%  plot (x, x, 'k-');
% max_y = max(x);
%  hold on
 
%  y = assmbly(1) ./ assmbly;
%  max_y = max(max(y), max_y);
 
%  plot (x, y, 'r-o');
 
%  y = ls(1) ./ ls;
%  max_y = max(max(y), max_y);
 
%  plot (x, y, 'b-+');

%  legend('linear speed-up', 'assembly', 'linear solver',0);
% xlabel('N. CPUs');
% title('fixed size speedup');

% if exist('OCTAVE_VERSION')
%   %Octave stuff;
% else
%   axis square
%   set(gca,'XTick',np);
%   set(gca,'XLim',[np(1) np(length(np))]);
%   set(gca,'YLim',[np(1) max_y]);
% end


