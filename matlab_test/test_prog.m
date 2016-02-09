clc;
clear;
close all;
more off;

r = 1

z(1,1,:) = [0,0,0,1,1,0,0,0];
z(1,2,:) = [1,0,0,1,1,0,0,0];
z(1,3,:) = [2,0,0,1,1,0,0,0];

z(2,1,:) = [1,0,0,1,1,0,0,0];
z(2,2,:) = [2,0,0,1,1,0,0,0];
z(2,3,:) = [3,0,0,1,1,0,0,0];

z(3,1,:) = [2,0,0,1,1,0,0,0];
z(3,2,:) = [3,0,0,1,1,0,0,0];
z(3,3,:) = [4,0,0,1,1,0,0,0];

z

tmp_a = exp(-3 * 1 / 2); %exp(-||p-q||^2 / 2*sigma^2), sigma^2 = 1/3, ||p-q||^2 = 1
tmp_b = exp(-3 * 2 / 2); % ||p-q||^2 = 2

Wr = [tmp_b, tmp_a, tmp_b; tmp_a, 0, tmp_a; tmp_b, tmp_a, tmp_b];
beta = 1/sum(sum(Wr));

mu = sum(sum(z))./9;
mu = mu(:)

Crp = zeros(8,8);
for i = 1:3
    for j = 1:3
        Zq = z(i,j,:);
        Zq = Zq(:) - mu(:);
        Crp = Crp .+ Wr(i,j) .* (Zq * transpose(Zq));
    end
end

Crp = Crp .* beta

colormap("jet");

imshow(Crp, [0 50]);
