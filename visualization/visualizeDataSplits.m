sortedData = dlmread('2');
number_of_splits = 20;
figure, hold on,

for i=1:number_of_splits
    starting = 1 + (i-1)*length(sortedData)/number_of_splits;
    ending = (i)*length(sortedData)/number_of_splits;
    
    pcshow(sortedData(starting:ending, :), [rand rand rand])
end

title('color-coded splits')