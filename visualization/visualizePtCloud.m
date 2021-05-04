function visualizePtCloud(ptCloudFile, idxFile)

data = dlmread(ptCloudFile);
idx = dlmread(idxFile);

uniques = unique(idx);
colors = hsv(length(uniques));


figure, hold on

for i = 1:length(uniques)
if(uniques(i) == -1)
continue;
end
    cluster_indexes = (idx==uniques(i));
    plot3(data(cluster_indexes,1), data(cluster_indexes,2), data(cluster_indexes,3), '.', 'color', colors(i,:))
end


end
