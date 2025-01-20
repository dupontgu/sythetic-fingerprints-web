#pragma once
#include <cstddef>
#include <vector>

class Matrix {
 public:
  Matrix();
  Matrix(size_t rows, size_t cols);
  Matrix(size_t rows, size_t cols, double val);

  double& at(size_t row, size_t col);
  const double& at(size_t row, size_t col) const;

  size_t rows() const;
  size_t cols() const;
  double* data();
  const double* data() const;
  void resize(size_t rows, size_t cols);
  Matrix clone() const;

 private:
  size_t rows_;
  size_t cols_;
  std::vector<double> data_;
};

struct Point {
  double x;
  double y;
  Point();
  Point(double x_, double y_);
};

struct Size {
  size_t width;
  size_t height;
  Size();
  Size(size_t w, size_t h);
};

struct Rect {
  int x;
  int y;
  size_t width;
  size_t height;
  Rect();
  Rect(int x_, int y_, size_t w, size_t h);
};