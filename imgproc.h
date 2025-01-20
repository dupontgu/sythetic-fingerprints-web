#pragma once
#include <cmath>

#include "matrix.h"

namespace imgproc {
void ellipse(Matrix& img, Point center, Size axes, double angle,
             double startAngle, double endAngle, double color,
             int thickness = 1);

void rectangle(Matrix& img, Rect rect, double color, int thickness = 1);

Matrix getGaborKernel(Size ksize, double sigma, double theta, double lambda,
                      double gamma, double psi);

double cubicInterpolate(double x);
}  // namespace imgproc