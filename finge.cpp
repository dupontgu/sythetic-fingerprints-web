#include "finge.h"

#include <cmath>

FinGe::FinGe() : minF(1.0 / 12), maxF(1.0 / 3.5), nCore(0), nDelta(0) {}

FinGe::~FinGe() { clearMemory(); }

void FinGe::clearMemory() {
    shape = Matrix();
    density = Matrix();
    orientation = Matrix();
    master_fng = Matrix();

    cores.clear();
    deltas.clear();
    gb_cache.clear();

    size = Size(0, 0);
}

void FinGe::setSeed(uint64_t seed_) {
  seed = seed_;
  rng.seed(seed);
}

void FinGe::generateFromSeed() {
  clearMemory();
  nCore = randRangeInt(1, 2);
  nDelta = nCore;
  double baseWidth = randRange(80, 120);

  sL = baseWidth;  // Left side width
  sR = randRange(baseWidth * 0.5,
                 baseWidth * 2.0);
  sT = randRange(baseWidth * 1.4,
                 baseWidth * 2.4);  // Top curve slightly smaller
  sB = randRange(baseWidth * 0.5, baseWidth * 0.9);  // Bottom curve much flatter
  sM = randRange(std::min(sL, sR) * 0.8, std::max(sL, sR) * 0.8);  // Middle section longer
  setShape(sL, sR, sT, sB, sM);

  // Generate ridge width parameters
  double min_width = randRange(3.3, 4.7);
  double max_width = randRangeInt(min_width + 1, 13);
  setRidgeWidth(min_width, max_width);

  cores.clear();
  for (size_t i = 0; i < nCore; i++) {
    double x = randRange(size.width * 0.3, size.width * 0.7);
    double y = randRange(size.height * 0.2, size.height * 0.6);
    cores.push_back(Point(x, y));
  }

  deltas.clear();
  for (size_t i = 0; i < nDelta; i++) {
    double x = randRange(size.width * 0.1, size.width * 0.9);
    double y = randRange(cores[i].y * 1.1, size.height * 0.8);
    deltas.push_back(Point(x, y));
  }
  // Set points and trigger fingerprint generation
  setPoints(nCore, cores, nDelta, deltas);
}

void FinGe::setShape(int left, int right, int top, int bottom, int middle) {
  sL = left;
  sR = right;
  sT = top;
  sB = bottom;
  sM = middle;
  size = Size(sL + sR, sT + sM + sB);
  generateShape();
}

void FinGe::setRidgeWidth(double min_width, double max_width) {
  minF = 1.0 / max_width;
  maxF = 1.0 / min_width;
  if (size.width > 0 && size.height > 0) {
    generateDensity();
  }
}

void FinGe::generateShape() {
  shape = Matrix(size.height, size.width);

  imgproc::ellipse(shape, Point(sL, sT), Size(sL, sT), 0, 180, 270, 1, -1);
  imgproc::ellipse(shape, Point(sL - 1, sT), Size(sR, sT), 0, 270, 360, 1, -1);
  imgproc::ellipse(shape, Point(sL, sT + sM - 1), Size(sL, sB), 0, 90, 180, 1, -1);
  imgproc::ellipse(shape, Point(sL - 1, sT + sM - 1), Size(sR, sB), 0, 0, 90, 1, -1);
  imgproc::rectangle(shape, Rect(0, sT, sL + sR, sM), 1, -1);

  generateDensity();
}

void FinGe::generateDensity() {
  const std::vector<int> resolution = {5, 6, 7};
  density = Matrix(size.height, size.width);

  for (size_t i = 0; i < 3; i++) {
    Matrix zoom_img(resolution[i], resolution[i]);
    for (size_t y = 0; y < resolution[i]; y++) {
      for (size_t x = 0; x < resolution[i]; x++) {
        zoom_img.at(y, x) = randRange(minF, maxF);
      }
    }

    Matrix resized(size.height, size.width);
    double scale_x = double(size.width) / resolution[i];
    double scale_y = double(size.height) / resolution[i];

    for (size_t y = 0; y < size.height; y++) {
      for (size_t x = 0; x < size.width; x++) {
        double src_x = x / scale_x;
        double src_y = y / scale_y;

        int x0 = floor(src_x);
        int y0 = floor(src_y);

        double fx = src_x - x0;
        double fy = src_y - y0;

        double val = 0;
        for (int dy = -1; dy <= 2; dy++) {
          for (int dx = -1; dx <= 2; dx++) {
            int px = std::min(std::max(x0 + dx, 0), int(resolution[i] - 1));
            int py = std::min(std::max(y0 + dy, 0), int(resolution[i] - 1));
            double weight = imgproc::cubicInterpolate(fx - dx) *
                            imgproc::cubicInterpolate(fy - dy);
            val += zoom_img.at(py, px) * weight;
          }
        }
        resized.at(y, x) = val;
      }
    }

    for (size_t y = 0; y < size.height; y++) {
      for (size_t x = 0; x < size.width; x++) {
        density.at(y, x) += resized.at(y, x) / 3.0;
      }
    }
  }
}

void FinGe::setPoints(size_t nCore_, const std::vector<Point>& cores_,
                      size_t nDelta_, const std::vector<Point>& deltas_) {
  nCore = nCore_;
  nDelta = nDelta_;
  cores = cores_;
  deltas = deltas_;
  generateOrientation();
}

void FinGe::generateOrientation() {
  orientation = Matrix(size.height, size.width);
  for (size_t y = 0; y < size.height; y++) {
    for (size_t x = 0; x < size.width; x++) {
      double theta = 0;

      for (size_t k = 0; k < nCore; k++) {
        double dx = cores[k].x - x;
        double dy = cores[k].y - y;
        theta += atan2(dy, dx);
      }

      for (size_t k = 0; k < nDelta; k++) {
        double dx = deltas[k].x - x;
        double dy = deltas[k].y - y;
        theta -= atan2(dy, dx);
      }

      theta *= 0.5;
      orientation.at(y, x) = theta;
    }
  }
  generateRidge();
}

void FinGe::generateRidge() {
  gb_cache.resize(FG_CACHE_DEG * FG_CACHE_FREQ);
  for (int i = 0; i < FG_CACHE_DEG; i++) {
    for (int j = 0; j < FG_CACHE_FREQ; j++) {
      double theta = indexToVal(i, 0, M_PI * 2, FG_CACHE_DEG) + M_PI / 2.0;
      double F = indexToVal(j, minF, maxF, FG_CACHE_FREQ);
      double sigma = sqrt(-9.0 / (8.0 * F * F * log(0.001)));
      Size ksize(2 * FG_GABOR_FILTER_SIZE, 2 * FG_GABOR_FILTER_SIZE);
      Matrix kernel = imgproc::getGaborKernel(ksize, sigma, theta, 1.0 / F, 1, 0);
      gb_cache[i * FG_CACHE_FREQ + j] = std::move(kernel);
    }
    
  }

  Matrix finger(size.height, size.width);
  for (size_t y = 0; y < size.height; y++) {
    for (size_t x = 0; x < size.width; x++) {
      finger.at(y, x) = randRange(0, 1);
    }
  }

  for (size_t y = 0; y < size.height; y++) {
    for (size_t x = 0; x < size.width; x++) {
      finger.at(y, x) = (finger.at(y, x) < 0.001) ? 1.0 : 0.0;
    }
  }

  const int b = FG_GABOR_FILTER_SIZE;
  bool converged = false;

  while (!converged) {
    Matrix layer(size.height, size.width, 0.0);
    size_t prevNonZero = 0;
    size_t newNonZero = 0;

    for (size_t y = 0; y < size.height; y++) {
      for (size_t x = 0; x < size.width; x++) {
        if (shape.at(y, x) > 0) {
          if (finger.at(y, x) > 0) prevNonZero++;

          double theta = orientation.at(y, x);
          theta = (theta < 0) ? theta + M_PI * 2 : theta;
          double F = density.at(y, x);

          int thetaIndex = valToIndex(theta, 0, M_PI * 2, FG_CACHE_DEG);
          int fIndex = valToIndex(F, minF, maxF, FG_CACHE_FREQ);
          int index = thetaIndex * FG_CACHE_FREQ + fIndex;

          // Calculate valid ROI regions
          int fil_x = std::max(b - (int)x, 0);
          int fil_y = std::max(b - (int)y, 0);
          int fil_width = std::min(
              std::min(2 * b, (int)size.width + b - (int)x), b + (int)x);
          int fil_height = std::min(
              std::min(2 * b, (int)size.height + b - (int)y), b + (int)y);

          int fng_x = std::max((int)x - b, 0);
          int fng_y = std::max((int)y - b, 0);

          // Apply filter using ROI approach
          double sum = 0;
          for (int dy = 0; dy < fil_height; dy++) {
            for (int dx = 0; dx < fil_width; dx++) {
              sum += gb_cache[index].at(fil_y + dy, fil_x + dx) *
                     finger.at(fng_y + dy, fng_x + dx);
            }
          }

          double val = (sum > 0) ? 1.0 : 0.0;
          layer.at(y, x) = val;
          if (val > 0) newNonZero++;
        }
      }
    }

    double diff =
        std::abs(double(prevNonZero - newNonZero)) / (size.width * size.height);
    if (diff <= 0.001) {
      converged = true;
    }

    finger = layer;
  }


  // Generate final fingerprint
  master_fng = Matrix(size.height, size.width);
  for (size_t y = 0; y < size.height; y++) {
    for (size_t x = 0; x < size.width; x++) {
      if (shape.at(y, x) > 0) {
        master_fng.at(y, x) = finger.at(y, x) + 0.1;
      } else {
        master_fng.at(y, x) = 1.0;
      }
    }
  }
}