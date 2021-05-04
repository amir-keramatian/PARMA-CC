function visualizeMaps(varargin)
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here

colors = hsv(length(varargin));

figure, hold on

for map = 1:length(varargin)
    visualizeMap(varargin{map}, colors(map, :));
end

end

