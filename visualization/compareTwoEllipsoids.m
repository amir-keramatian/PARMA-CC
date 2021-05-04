function [ decision ] = compareTwoEllipsoids( mu_1, sigma_1, mu_2, sigma_2 )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here



%corresponding to myEllipsoid1
A = zeros(4);
A(1:3, 1:3) = inv(sigma_1);
A(4, 4)     = -1;

%corresponding to myEllipsoid2
offset = mu_2 - mu_1;

T = eye(4);
T(4,:) = [-offset, 1];


B = zeros(4);
B(1:3, 1:3) = inv(sigma_2);
B(4, 4)     = -1;

B = T * B * T';


tokhmiTakhayoli = inv(A) * B;
disp(tokhmiTakhayoli);

[eigenVectors, eigenValues] = eig(tokhmiTakhayoli);
eigenValues = diag(eigenValues);


toGetRidOf = find(eigenVectors(4,:)==0);

eigenValues(toGetRidOf) = [];
eigenVectors(:,toGetRidOf) = [];

flagAddmissible = ones(size(eigenVectors,2),1);
insideBoth = zeros(size(eigenVectors,2),1);
outsideBoth = zeros(size(eigenVectors,2),1);
onBoth = zeros(size(eigenVectors,2),1);


for i = 1:size(eigenVectors,2)
    
    normalizedEigenVector(:,i) = real(eigenVectors(1:3,i)./eigenVectors(4,i));
    
    
    if [normalizedEigenVector(:,i)' 1]*A*[normalizedEigenVector(:,i); 1] < 0 ...
            && [normalizedEigenVector(:,i)' 1]*B*[normalizedEigenVector(:,i); 1] < 0
        insideBoth(i) = 1;
    end
    
    if [normalizedEigenVector(:,i)' 1]*A*[normalizedEigenVector(:,i); 1] > 0 ...
            && [normalizedEigenVector(:,i)' 1]*B*[normalizedEigenVector(:,i); 1] > 0
        outsideBoth(i) = 1;
    end
    
    
    if abs([normalizedEigenVector(:,i)' 1]*A*[normalizedEigenVector(:,i); 1]) < 0.01 ...
            && abs([normalizedEigenVector(:,i)' 1]*B*[normalizedEigenVector(:,i); 1]) < 0.01
        onBoth(i) = 1;
    end
    
        %([normalizedEigenVector(:,i)' 1]*A*[normalizedEigenVector(:,i); 1] ) 
    
        %([normalizedEigenVector(:,i)' 1]*B*[normalizedEigenVector(:,i); 1] ) 
       
end

if sum(insideBoth(:)) ~= 0 || sum(onBoth(:)) ~= 0
    decision = 'matched';
else
    decision = 'apart';
end


%if isreal(eigenValues) == 0
%    decision= 'OverLap';
%elseif sum(eigenValues > 0) == 2
%    decision = 'Past';
%elseif abs(eigenValues(1) - eigenValues(2)) < .001
%    decision = 'Touch';
%else
%    decision = 'Outside';
%end


end

