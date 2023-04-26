#include "org_plr.h"

double pointToLineSegmentDistance(const Point& point, const LineSegment& segment) {
    double numerator = std::abs((segment.end.y - segment.start.y) * point.x - (segment.end.x - segment.start.x) * point.y + segment.end.x * segment.start.y - segment.end.y * segment.start.x);
    double denominator = std::sqrt(std::pow(segment.end.y - segment.start.y, 2) + std::pow(segment.end.x - segment.start.x, 2));
    return numerator / denominator;
}

std::vector<LineSegment> piecewiseLinearRegression(const std::vector<Point>& data, double maxError) {
    std::vector<LineSegment> segments;
    int n = data.size();
    int start = 0;

    while (start < n - 1) {
        int end = start + 1;
        LineSegment segment;
        segment.start = data[start];
        segment.end = data[end];
        segment.slope = (segment.end.y - segment.start.y) / (segment.end.x - segment.start.x);
        segment.intercept = segment.start.y - segment.slope * segment.start.x;

        double maxDist = 0;
        int maxDistIndex = start;

        for (int i = start + 1; i <= end; ++i) {
            double dist = pointToLineSegmentDistance(data[i], segment);
            if (dist > maxDist) {
                maxDist = dist;
                maxDistIndex = i;
            }
        }

        if (maxDist <= maxError) {
            while (end < n - 1) {
                end++;
                segment.end = data[end];
                segment.slope = (segment.end.y - segment.start.y) / (segment.end.x - segment.start.x);
                segment.intercept = segment.start.y - segment.slope * segment.start.x;

                maxDist = 0;
                for (int i = start + 1; i <= end; ++i) {
                    double dist = pointToLineSegmentDistance(data[i], segment);
                    if (dist > maxDist) {
                        maxDist = dist;
                        maxDistIndex = i;
                    }
                }

                if (maxDist > maxError) {
                    end--;
                    segment.end = data[end];
                    segment.slope = (segment.end.y - segment.start.y) / (segment.end.x - segment.start.x);
                    segment.intercept = segment.start.y - segment.slope * segment.start.x;
                    break;
                }
            }
        } else {
            end = maxDistIndex;
            segment.end = data[end];
            segment.slope = (segment.end.y - segment.start.y) / (segment.end.x - segment.start.x);
            segment.intercept = segment.start.y - segment.slope * segment.start.x;
        }

        segments.push_back(segment);
        start = end;
    }

    return segments;
}

double squaredError(const Point& point, const LineSegment& segment) {
    double predictedY = segment.slope * point.x + segment.intercept;
    return std::pow(predictedY - point.y, 2);
}

LineSegment fitLineSegment(const std::vector<Point>& data, int startIndex, int endIndex) {
    double xSum = 0, ySum = 0, xySum = 0, xSquaredSum = 0;
    int n = endIndex - startIndex + 1;
    
    for (int i = startIndex; i <= endIndex; i++) {
        xSum += data[i].x;
        ySum += data[i].y;
        xySum += data[i].x * data[i].y;
        xSquaredSum += data[i].x * data[i].x;
    }

    double xMean = xSum / n;
    double yMean = ySum / n;

    double slope = (n * xySum - xSum * ySum) / (n * xSquaredSum - xSum * xSum);
    double intercept = yMean - slope * xMean;

    return {data[startIndex], data[endIndex], slope, intercept};
}
std::vector<LineSegment> bottomUpPiecewiseLinearRegression(const std::vector<Point>& data, double maxError) {
    int n = data.size();
    std::vector<std::vector<double>> costMatrix(n, std::vector<double>(n, 0));
    std::vector<std::vector<int>> segmentMatrix(n, std::vector<int>(n, 0));
    std::vector<int> indices(n + 1, 0);
    std::vector<double> error(n + 1, 0);

    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            LineSegment segment = fitLineSegment(data, i, j);
            double errorSum = 0;
            for (int k = i; k <= j; k++) {
                errorSum += squaredError(data[k], segment);
            }
            costMatrix[i][j] = errorSum;
            if (errorSum <= maxError) {
                segmentMatrix[i][j] = 1;
            }
        }
    }

    error[0] = 0;
    for (int i = 1; i <= n; i++) {
        error[i] = std::numeric_limits<double>::max();
        for (int j = 0; j < i; j++) {
            if (segmentMatrix[j][i - 1] == 1 && error[j] + costMatrix[j][i - 1] < error[i]) {
                error[i] = error[j] + costMatrix[j][i - 1];
               
                indices[i] = j;
            }
        }
    }

    std::vector<LineSegment> segments;
    int index = n;
    while (index > 0) {
        int start = indices[index];
        int end = index - 1;
        LineSegment segment = fitLineSegment(data, start, end);
        segments.push_back(segment);
        index = start;
    }
    std::reverse(segments.begin(), segments.end());

    return segments;
}

std::vector<LineSegment> slidingWindowPiecewiseLinearRegression(const std::vector<Point>& data, double maxError) {
    int n = data.size();
    std::vector<LineSegment> segments;

    int start = 0, end = 0;
    while (start < n - 1) {
        end = start + 1;
        LineSegment segment = fitLineSegment(data, start, end);
        double error = 0;

        while (end < n - 1) {
            end++;
            LineSegment newSegment = fitLineSegment(data, start, end);
            error = 0;

            for (int i = start; i <= end; i++) {
                error += squaredError(data[i], newSegment);
            }

            if (error > maxError) {
                end--;
                segment = fitLineSegment(data, start, end);
                break;
            } else {
                segment = newSegment;
            }
        }

        segments.push_back(segment);
        start = end;
    }

    return segments;
}