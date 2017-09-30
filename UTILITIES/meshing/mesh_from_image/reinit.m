function phi = reinit(phi)

c = contours(phi,[0,0]);

idx = 1;
count = 1;
pt = [];
while idx < size(c,2)
    n = c(2,idx);
    pt = [pt, c(:,idx+1:idx+n)];
    count = count+1;
    idx = idx+n+1;
end

phi0 = zeros(size(phi));
phi0(sub2ind(size(phi), round(pt(2,:)),round(pt(1,:)))) = 1;
phi = double(bwdist(phi0)).*sign(phi);


end