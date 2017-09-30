clf; clear; close all

I = imread('horseshoe.png');

[p, t] = segment_mesh(I);