function [  ] = run(  )
%RUN Summary of this function goes here
%   Detailed explanation goes here

    addpath('distmesh');
    
    width = 20;
    fd = @(p) dcircle(p,0,0,width);
    [p, t] = distmesh2d(fd,@huniform,width/3,[-width,-width;width,width],[]);

    %close all;
    
    export('circle.txt', p, t);
    
    
    
    width = 40;
    fd = @(p) dcircle(p,0,0,width);
    [p, t] = distmesh2d(fd,@huniform,width/5,[-width,-width;width,width],[]);

    %close all;
    
    export('circle_big.txt', p, t);
    
    
    width = 20;
    fd = @(p) dcircle(p,0,0,width);
    [p, t] = distmesh2d(fd,@huniform,width/2.2,[-width,-width;width,width],[]);

    %close all;
    
    export('circle_small.txt', p, t);
end

