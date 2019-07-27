function [RecFlag] = codeRecognition(time,code)
global times

if isempty(times)
    times = zeros(length(code),1);
end

% shift array and save new peak
times(1:end-1,:) = times(2:end,:);
times(end,1) = time;

% set short/long definition
shortmin = min(times(:,1))*0.75;
shortmax = min(times(:,1))*1.25;

longmin = max(times(:,1))*0.75;
longmax = max(times(:,1))*1.25;

% define signal

% initialize compare array
compare = code;
% fill compare array 
for i = 1:length(times)
    if times(i,1) >= shortmin && times(i,1) <= shortmax && times(i,1) > 0
        compare(i,1) = "short";
    elseif times(i,1) >= longmin && times(i,1) <= longmax
        compare(i,1) = "long";
    else
        compare(i,1) = "misread";
    end
end

if compare == code
    RecFlag = true;
else
    RecFlag = false;
end

end