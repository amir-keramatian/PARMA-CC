function visualizePtCloud(ptCloudFile, idxFile, text)

data = dlmread(ptCloudFile);
idx = dlmread(idxFile);

uniques = unique(idx);
%colors = hsv(length(uniques));
colors = rand(length(uniques), 3);

figure, hold on

for i = 1:length(uniques)
    if(uniques(i) == -1)
        continue;
    end
    cluster_indexes = (idx==uniques(i));
    %plot3(data(cluster_indexes,1), data(cluster_indexes,2), data(cluster_indexes,3), '.', 'color', colors(i,:))
    pcshow(data(cluster_indexes, :), [colors(i,1) colors(i,2) colors(i,3)])
end

title(text)
end
