#pragma once
#include "matrix.h"
#include "imgproc.h"
#include <vector>
#include <random>

#define FG_CACHE_DEG 36
#define FG_CACHE_FREQ 20
#define FG_GABOR_FILTER_SIZE 10

class FinGe {
public:
    FinGe();
    ~FinGe();
    void clearMemory();
    void setSeed(uint64_t seed);
    void generateFromSeed();
    void setShape(int left, int right, int top, int bottom, int middle);
    void setPoints(size_t nCore_, const std::vector<Point>& cores_,
                  size_t nDelta_, const std::vector<Point>& deltas_);
    const Matrix& getMasterFingerprint() const { return master_fng; }
    void setRidgeWidth(double min_width, double max_width);
    size_t getNCores() const { return nCore; }
    size_t getNDeltas() const { return nDelta; }
    int getShapeL() const { return sL; }
    int getShapeR() const { return sR; }
    int getShapeT() const { return sT; }
    int getShapeB() const { return sB; }
    int getShapeM() const { return sM; }
    double getMinF() const { return minF; }
    double getMaxF() const { return maxF; }
    const std::vector<Point>& getCorePoints() const { return cores; }
    const std::vector<Point>& getDeltaPoints() const { return deltas; }

private:
    void generateShape();
    void generateDensity();
    void generateOrientation();
    void generateRidge();
    
    double indexToVal(int index, double min, double max, int n) {
        double gap = (max - min) / n;
        return gap * (double(index) + 0.5) + min;
    }
    
    int valToIndex(double val, double min, double max, int n) {
        double gap = (max - min) / n;
        return floor((val - min) / gap);
    }

    double randRange(double min, double max) {
        return min + (max - min) * (double(rng()) / rng.max());
    }
    
    int randRangeInt(int min, int max) {
        return min + (rng() % (max - min + 1));
    }

    double minF, maxF;
    int sL, sR, sT, sB, sM;
    Size size;
    Matrix shape;
    Matrix density;
    Matrix orientation;
    Matrix master_fng;
    std::vector<Point> cores;
    std::vector<Point> deltas;
    size_t nCore;
    size_t nDelta;
    std::mt19937_64 rng;  // 64-bit Mersenne Twister
    uint64_t seed;
    std::vector<Matrix> gb_cache;
};