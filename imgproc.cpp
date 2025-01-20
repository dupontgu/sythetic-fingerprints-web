#include "imgproc.h"

#include <algorithm>

namespace imgproc {

void ellipse(Matrix& img, Point center, Size axes, double angle,
             double startAngle, double endAngle, double color, int thickness) {
  startAngle = startAngle * M_PI / 180.0;
  endAngle = endAngle * M_PI / 180.0;
  angle = angle * M_PI / 180.0;

  int min_x = std::max(0, static_cast<int>(center.x - axes.width));
  int max_x = std::min(static_cast<int>(img.cols()),
                       static_cast<int>(center.x + axes.width + 1));
  int min_y = std::max(0, static_cast<int>(center.y - axes.height));
  int max_y = std::min(static_cast<int>(img.rows()),
                       static_cast<int>(center.y + axes.height + 1));

  for (int y = min_y; y < max_y; y++) {
    for (int x = min_x; x < max_x; x++) {
      if (y < 0 || y >= img.rows() || x < 0 || x >= img.cols()) {
        continue;
      }

      double dx = x - center.x;
      double dy = y - center.y;

      double cos_angle = cos(-angle);
      double sin_angle = sin(-angle);
      double rx = dx * cos_angle - dy * sin_angle;
      double ry = dx * sin_angle + dy * cos_angle;

      rx /= axes.width;
      ry /= axes.height;

      double point_angle = atan2(ry, rx);
      if (point_angle < 0) point_angle += 2 * M_PI;

      double start = startAngle;
      double end = endAngle;
      if (end < start) end += 2 * M_PI;
      if (point_angle < start) point_angle += 2 * M_PI;

      if ((rx * rx + ry * ry) <= 1.0 && point_angle >= start &&
          point_angle <= end) {
        img.at(y, x) = color;
      }
    }
  }
}

void rectangle(Matrix& img, Rect rect, double color, int thickness) {
  int x2 = rect.x + rect.width - 1;  // Fix: subtract 1 for proper bounds
  int y2 = rect.y + rect.height - 1;

  for (int y = rect.y; y <= y2; ++y) {
    for (int x = rect.x; x <= x2; ++x) {
      if (x >= 0 && x < static_cast<int>(img.cols()) && y >= 0 &&
          y < static_cast<int>(img.rows())) {
        if (thickness < 0 ||           // Fill
            x == rect.x || x == x2 ||  // Vertical edges
            y == rect.y || y == y2) {  // Horizontal edges
          img.at(y, x) = color;
        }
      }
    }
  }
}

Matrix getGaborKernel(Size ksize, double sigma, double theta, double lambda,
                      double gamma, double psi) {
  if (ksize.width <= 0 || ksize.height <= 0) {
    return Matrix(1, 1, 0.0);
  }

  Matrix kernel(ksize.height, ksize.width);
  double sigma_x = sigma;
  double sigma_y = sigma / gamma;

  int half_width = ksize.width / 2;
  int half_height = ksize.height / 2;

  double cos_theta = cos(theta);
  double sin_theta = sin(theta);

  for (int y = -half_height; y <= half_height; y++) {
    for (int x = -half_width; x <= half_width; x++) {
      double x_theta = x * cos_theta + y * sin_theta;
      double y_theta = -x * sin_theta + y * cos_theta;

      double exp_val =
          exp(-0.5 * (pow(x_theta / sigma_x, 2) + pow(y_theta / sigma_y, 2)));
      double cos_val = cos(2 * M_PI * x_theta / lambda + psi);

      int matrix_y = y + half_height;
      int matrix_x = x + half_width;

      if (matrix_y >= 0 && matrix_y < ksize.height && matrix_x >= 0 &&
          matrix_x < ksize.width) {
        kernel.at(matrix_y, matrix_x) = exp_val * cos_val;
      }
    }
  }

  return kernel;
}

double cubicInterpolate(double x) {
  x = std::abs(x);
  if (x >= 2.0) return 0.0;
  if (x >= 1.0) return ((-0.5 * x + 2.5) * x - 4.0) * x + 2.0;
  return ((1.5 * x - 2.5) * x) * x + 1.0;
}

}  // namespace imgproc