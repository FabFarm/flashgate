clear all
close all
clc

%needed vars for built in functions
global peak_old counter times

% get data
a = arduino('/dev/tty.usbmodem14101','UNO');

% set code
code = ["short"; "long"];

%% Acquire and display live data
% plotting basics
figure
subplot(2,1,1)
ax1 = gca;
h = animatedline;
h2 = animatedline;
h2.Color = 'red';
ax1.YGrid = 'on';
ax1.YLim = [-0.1 5];

subplot(2,1,2)
ax2 = gca;
h3 = animatedline;
h4 = animatedline;
h5 = animatedline;
h5.Color = 'red';
ax2.YGrid = 'on';
ax2.YLim = [-20 20];

stop = false;
startTime = datetime('now');

%% process data

%setup memory variables
v_old = zeros(10,1);
v_med_old = 0;
t_old = datetime('now') - startTime;
dvdt_old = zeros(4,1);

while ~stop
    % Read current voltage value
    v = readVoltage(a,'A3'); 
    
    %shift old measurements
    v_old(1:end-1) = v_old(2:end);
    v_old(end) = v;
    %selfmade medfilt
    v_med = mean(v_old(end-5:end));
   
    % Get current time
    t =  datetime('now') - startTime;
    
    % selfmade derivative
    dv = v_med-v_med_old;
    v_med_old = v_med;
    dt = seconds(t)-seconds(t_old);
    t_old = t;
    
    if dt == 0
        dvdt = 0;
    else
        dvdt = dv/dt;
    end
    
    %shift old measurements
    dvdt_old(1:end-1) = dvdt_old(2:end);
    dvdt_old(end) = dvdt;
    
    %selfmade medfilt
    dvdt_med = mean(dvdt_old(end-3:end));
    
    %findpeaks
    [peak_time,peak_height,flag_high] = fpeak(dvdt_med,datenum(t));
    [peak_low_time,peak_low,flag_low] = fpeak(-dvdt_med,datenum(t));
    
    %plot peaks and start code recognition
    if flag_high && ~isequal(peak_height,0)
       hold on;
       plot(ax2,peak_time,peak_height,'rv')
       %start measureing time between high and low peak
       tic
    end
    if flag_low && ~isequal(peak_low,0)
       hold on;
       plot(ax2,peak_low_time,-peak_low,'rv')
       
       %code recognition
       [Flag] = codeRecognition(toc,code);
       
       %code test
      if Flag
            h2.Color = 'green';
            h5.Color = 'green';
            % reset after succesfull code recognition
            times = [];
      else
            h2.Color = 'red';
            h5.Color = 'red';
      end  
    end
   
    
    % Add points to animation
    addpoints(h,datenum(t),v)
    addpoints(h2,datenum(t),v_med)
    addpoints(h3,datenum(t),v_med)
    addpoints(h4,datenum(t),dvdt)
    addpoints(h5,datenum(t),dvdt_med)
    
    % Update axes
    ax1.XLim = datenum([t-seconds(15) t]);
    ax2.XLim = datenum([t-seconds(15) t]);
    datetick('x','keeplimits')
    drawnow 
end