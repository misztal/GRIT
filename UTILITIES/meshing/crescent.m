function crescent

addpath('distmesh');

shape = @(p) ddiff(dcircle(p,0,0,0.5),dcircle(p,0,0.30,0.25));

fd = @(p) ddiff(drectangle(p,-1,1,-1,1),shape(p));
fh = @(p) 0.05+0.001*shape(p);
morefixed = [ -0.189984,0.462500   ;  0.189984,0.462500 ]; % intersection points for circles
[p,t] = distmesh2d(fd, fh, 0.1, [-1,-1;1,1], [-1,-1;-1,1;1,-1;1,1    ;   morefixed  ]);

idxs = abs(shape(p))<1e-3;

hold on;
plot(p(idxs, 1), p(idxs, 2), 'xr')
hold off;

new_fix = p(idxs, :);

figure;
[p_,t_] = distmesh2d(shape,@huniform, 0.1, [-1,-1;1,1], new_fix);

new_t = [t; t_ + size(p,1)];
new_p = [p; p_];

[p,t] = fixmesh(new_p, new_t, 0.001);

px = p(t(:, 1), 1) + p(t(:, 2), 1) + p(t(:, 3), 1);
py = p(t(:, 1), 2) + p(t(:, 2), 2) + p(t(:, 3), 2);

pmid = [px ./ 3, py ./ 3];

close all;

figure;
triplot(t,p(:,1), p(:,2));
axis equal;

hold on;
idxs = shape(pmid) < 1e-3; % idxs = fdInner([p]) < 1e-3;
plot(pmid(idxs, 1), pmid(idxs, 2), '.r') %plot(p(idxs, 1), p(idxs, 2), '.r')
%plot(pmid(:, 1), pmid(:, 2), '.r')
hold off;

export('crescent.txt', p, t, idxs);

end
