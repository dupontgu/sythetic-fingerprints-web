#include "matrix.h"

Matrix::Matrix() : rows_(0), cols_(0), data_(0, 0.0) {}

Matrix::Matrix(size_t rows, size_t cols)
    : rows_(rows), cols_(cols), data_(rows * cols, 0.0) {}

Matrix::Matrix(size_t rows, size_t cols, double val)
    : rows_(rows), cols_(cols), data_(rows * cols, val) {}

double& Matrix::at(size_t row, size_t col) { return data_[row * cols_ + col]; }

const double& Matrix::at(size_t row, size_t col) const {
  return data_[row * cols_ + col];
}

size_t Matrix::rows() const { return rows_; }
size_t Matrix::cols() const { return cols_; }

double* Matrix::data() { return data_.data(); }
const double* Matrix::data() const { return data_.data(); }

void Matrix::resize(size_t rows, size_t cols) {
  rows_ = rows;
  cols_ = cols;
  data_.resize(rows * cols, 0.0);
}

Matrix Matrix::clone() const {
  Matrix result(rows_, cols_);
  result.data_ = data_;
  return result;
}

Point::Point() : x(0), y(0) {}
Point::Point(double x_, double y_) : x(x_), y(y_) {}

Size::Size() : width(0), height(0) {}
Size::Size(size_t w, size_t h) : width(w), height(h) {}

Rect::Rect() : x(0), y(0), width(0), height(0) {}
Rect::Rect(int x_, int y_, size_t w, size_t h)
    : x(x_), y(y_), width(w), height(h) {}