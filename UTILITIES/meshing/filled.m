addpath('distmesh');

fd = @(p) drectangle(p,-1,1,-1,1);
pfix = [-1,-1;-1,1;1,-1;1,1];
[p,t] = distmesh2d(fd,@huniform,0.25,[-1,-1;1,1],pfix);

export('filled.txt', p, t, ones(length(t),1));