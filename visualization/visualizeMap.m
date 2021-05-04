function visualizeMap(inputMap, colors)

fid = fopen(inputMap);

line = fgetl(fid);

components = textscan(line, '%s%d');
assert(strcmp( components{1}{1}, '#objects:'));

numberOfObjects = components{2};


if ~exist('colors', 'var')
    colors = hsv(numberOfObjects);
else
    colors = repmat(colors, numberOfObjects, 1);
end


obj = 1;
ellipsoid_cnt = 1;

hold on

while(true)
    
    if obj > numberOfObjects
        break;
    end
    
    line = fgetl(fid);
    if isempty(line)
        continue
    end
    
    components = textscan(line, '%s%d');
    assert(strcmp( components{1}{1}, '#ellipsoids:'));
    
    numberOfEllipsoids = components{2};
    
    for ellipsoids = 1:numberOfEllipsoids
        line = fgetl(fid);
        components = textscan(line, '%s%d');
        assert(strcmp( components{1}{1}, '@id:'));
        
        line = fgetl(fid);
        components = textscan(line, '%s%d');
        assert(strcmp( components{1}{1}, '@sourceID:'));
        
        line = fgetl(fid);
        components = textscan(line, '%s');
        assert(strcmp( components{1}{1}, '@mu:'));
        
        line = fgetl(fid);
        mu{ellipsoid_cnt} = sscanf(line, '%f');
        
        line = fgetl(fid);
        components = textscan(line, '%s');
        assert(strcmp( components{1}{1}, '@cov:'));
        
        line = fgetl(fid);
        cov{ellipsoid_cnt}(1,:) = sscanf(line, '%f');
        line = fgetl(fid);
        cov{ellipsoid_cnt}(2,:) = sscanf(line, '%f');
        line = fgetl(fid);
        cov{ellipsoid_cnt}(3,:) = sscanf(line, '%f');
        
        h = plot_gaussian_ellipsoid(mu{ellipsoid_cnt}, cov{ellipsoid_cnt}, colors(obj, :));
        
        ellipsoid_cnt = ellipsoid_cnt + 1;
    end
    obj = obj + 1;
end


fclose(fid);

end
