function small_rectangle

addpath('distmesh');
close all;

rect = @(p) drectangle(p,-0.2,0.2,-0.1,0.1);

fd = @(p) ddiff(drectangle(p,-1,1,-1,1),rect(p));
[p,t] = distmesh2d(fd,@huniform,0.1,[-1,-1;1,1], ...
                   [-1,-1;-1,1;1,-1;1,1; ...               % Outer corners
                    -0.2,-0.1;-0.2,0.1;0.2,-0.1;0.2,0.1]); % Inner corners

idxs = abs(rect(p)) < 1e-3;

hold on;
plot(p(idxs, 1), p(idxs, 2), 'xr')
hold off;

new_fix = p(idxs, :);

figure;
[p_,t_] = distmesh2d(rect,@huniform, 0.12, [-1,-1;1,1], new_fix);

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
idxs = rect(pmid) < 1e-3;
plot(pmid(idxs, 1), pmid(idxs, 2), '.r')
hold off;

export('small_rectangle.txt', p, t, idxs);

end
