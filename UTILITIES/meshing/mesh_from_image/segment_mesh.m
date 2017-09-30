function [p, t] = segment_mesh(I)

addpath('../distmesh')

I = double(rgb2gray(I));
I = imresize(I, 0.5);
I = padarray(I,[250 200],max(I(:)),'both');
[M, N] = size(I);

dd = chanvese(I,100,0.5,1);

if abs(max(dd(:))) < abs(min(dd(:))), dd=-dd; end

[xx,yy] = meshgrid(linspace(-1,1,N),linspace(-1,1,M));

% close all;
% figure(1)
% surf(xx,yy,dd);
% figure(2);
% contour(dd,[0 0],'r','Linewidth',2);

% figure(3);
fd = @(p) ddiff(drectangle(p,-1,1,-1,1),dmatrix(p,xx,yy,dd));
fh = @(p) 0.05+0.0005*dmatrix(p,xx,yy,dd);
[p,t] = distmesh2d(fd,fh,0.05,[-1,-1;1,1],[-1,-1;-1,1;1,-1;1,1]);



end