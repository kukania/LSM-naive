#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

struct Point {
    double x;
    double y;
};

struct LineSegment {
    Point start;
    Point end;
    double slope;
    double intercept;
};


std::vector<LineSegment> piecewiseLinearRegression(const std::vector<Point>& data, double maxError);

std::vector<LineSegment> bottomUpPiecewiseLinearRegression(const std::vector<Point>& data, double maxError);

std::vector<LineSegment> slidingWindowPiecewiseLinearRegression(const std::vector<Point>& data, double maxError);