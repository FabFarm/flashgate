
% function to find peaks and isolate them

function [peak_time,peak_height,Flag] = fpeak(peak,time)
% needs the use of global variables, make sure in main file!
global peak_old counter

Flag = false;

% min distance between peak
peak_distance = 10; %could be a function input option

%setup for later peak comparision
if isempty(peak_old)
    peak_old = zeros(peak_distance,2);
end
if isempty(counter)
    counter = 1;
end

% set min peak hight, can be added as function input option
% may depend on day/night time
minPeakHeight = 7;

if peak > minPeakHeight  && peak > max(peak_old(:,1))
    % shift the stored peak values
    peak_old(1:end-1,:) = peak_old(2:end,:);
    % save new peak and time
    peak_old(end,1) = peak;
    peak_old(end,2) = time;
    counter = 1;
elseif peak >= 0 %needed to avoid confusion with finding low peaks
    counter = counter+1;
end

if counter == 10 %after not finding a peak in 10 steps, plot peak
    Flag = true;
    peak_time = peak_old(end,2);
    peak_height = peak_old(end,1);
    
    %reset for new search
    peak_old = [];
else
    peak_time = 0;
    peak_height = 0;
end

end
