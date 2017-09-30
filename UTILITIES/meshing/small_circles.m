function small_circles

close all;

addpath('distmesh');

c1 = @(p) dcircle(p,-0.75,0.70,0.15);
c2 = @(p) dcircle(p,-0.25,0.75,0.15);
c3 = @(p) dcircle(p,0.25,0.70,0.15);
c4 = @(p) dcircle(p,0.75,0.75,0.15);

circ = @(p) dunion(dunion(dunion(c1(p), c2(p)), c3(p)), c4(p));

fd = @(p) ddiff(drectangle(p,-1,1,-1,1), circ(p));
fh = @(p) 0.4+0.01*circ(p);
[p,t] = distmesh2d(fd,fh,0.05,[-1,-1;1,1],[-1,-1;-1,1;1,-1;1,1]); % 0.06

e = 1e-4;
idxs  = abs(circ(p)) < e;

hold on;
plot(p(idxs, 1), p(idxs, 2), 'xr')
hold off;

new_fix = p(idxs, :);

figure;
[p_,t_] = distmesh2d(circ,@huniform, 0.06, [-1,-1;1,1], new_fix); % 0.07

new_t = [t; t_ + size(p,1)];
new_p = [p; p_];

[p,t] = fixmesh(new_p, new_t, 0.003);

px = p(t(:, 1), 1) + p(t(:, 2), 1) + p(t(:, 3), 1);
py = p(t(:, 1), 2) + p(t(:, 2), 2) + p(t(:, 3), 2);

pmid = [px ./ 3, py ./ 3];

close all;

figure;
triplot(t,p(:,1), p(:,2));
axis equal;

hold on;
idxs = circ(pmid) < e;
plot(pmid(idxs, 1), pmid(idxs, 2), '.r')
hold off;

export('small_circles.txt', p, t, idxs);

end
