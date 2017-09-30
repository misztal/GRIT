Point(1) = {-0.5, -0.25, 0, 1.0};
Point(2) = {0.5, -0.25, 0, 1.0};
Point(3) = {0.5, 0.25, 0, 1.0};
Point(4) = {-0.5, 0.25, 0, 1.0};
Point(5) = {-0.5, 0., 0, 1.0};
Point(6) = {0.5, 0., 0, 1.0};
Point(7) = {0.75, 0., 0, 1.0};
Point(8) = {-0.75, 0., 0, 1.0};
Line(1) = {3, 4};
Circle(2) = {4, 5, 8};
Circle(3) = {8, 5, 1};
Line(4) = {1, 2};
Circle(5) = {2, 6, 7};
Circle(6) = {7, 6, 3};
Line Loop(7) = {6, 1, 2, 3, 4, 5};
Plane Surface(8) = {7};
Physical Surface(9) = {8};
Point(9) = {-1, 0.3, 0, 1.0};
Point(10) = {1, 0.3, 0, 1.0};
Point(11) = {1, 0.4, 0, 1.0};
Point(12) = {-1, 0.4, 0, 1.0};
Line(10) = {9, 10};
Line(11) = {10, 11};
Line(12) = {11, 12};
Line(13) = {12, 9};
Characteristic Length {12, 9} = 0.1;
Characteristic Length {9, 10} = 0.1;
Characteristic Length {10, 11} = 0.1;
Characteristic Length {11, 12} = 0.1;
Line Loop(14) = {12, 13, 10, 11};
Plane Surface(15) = {14};
Physical Surface(16) = {15};
Point(13) = {-1, -0.3, 0, 1.0};
Point(14) = {-1, -0.4, 0, 1.0};
Point(15) = {1, -0.4, 0, 1.0};
Point(16) = {1, -0.3, 0, 1.0};
Line(17) = {13, 14};
Line(18) = {14, 15};
Line(19) = {15, 16};
Line(20) = {16, 13};
Line Loop(21) = {20, 17, 18, 19};
Plane Surface(22) = {21};
Characteristic Length {16, 13, 14, 15} = 0.1;
Physical Surface(23) = {22};
Point(17) = {-1.5, -0.5, 0, 1.0};
Point(18) = {1.5, -0.5, 0, 1.0};
Point(19) = {1.5, 0.5, 0, 1.0};
Point(20) = {-1.5, 0.5, 0, 1.0};
Line(24) = {19, 20};
Line(25) = {20, 17};
Line(26) = {17, 18};
Line(27) = {18, 19};
Line Loop(28) = {27, 24, 25, 26};
Plane Surface(29) = {7, 14, 21, 28};
Physical Surface(30) = {29};
Physical Surface(31) = {29};
Delete {
  Surface{29};
}
Plane Surface(32) = {7, 14, 21, 28};
Physical Surface(33) = {32};
Characteristic Length {4, 3, 1, 2} = 0.1;
Characteristic Length {4, 3} = 0.1;
Characteristic Length {4, 3, 2, 1} = 0.1;
Characteristic Length {3, 4, 1, 2} = 0.05;
Characteristic Length {4, 3, 1, 2} = 0.05;
Transfinite Line {1, 4} = 20 Using Progression 1;
Characteristic Length {3, 4, 1, 2} = 0.05;
Point(21) = {0.5, 0.25, 0, 1.0};
Point(22) = {0., 0.25, 0, 1.0};
Point(23) = {0., -0.25, 0, 1.0};
Split Line(1) {22};
Split Line(4) {23};
Characteristic Length {4, 22, 3, 1, 23, 2} = 0.1;
Characteristic Length {4, 22, 3, 1, 23, 2} = 0.1;