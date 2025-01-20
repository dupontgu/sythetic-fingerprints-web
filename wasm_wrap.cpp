#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <iostream>

#include "finge.h"

using namespace emscripten;

class Fingerprint {
 public:
  Fingerprint() : finge_() {}

  void generateFromSeed(const std::string& uuid) {
    // Convert UUID to 64-bit seed
    uint64_t seed = 0;
    for (char c : uuid) {
      seed = seed * 31 + c;
    }
    finge_.setSeed(seed);
    finge_.generateFromSeed();
  }

  val getFingerprint() {
    const Matrix& fp = finge_.getMasterFingerprint();
    return val(typed_memory_view(fp.rows() * fp.cols(), fp.data()));
  }

  int getWidth() {
    const Matrix& fp = finge_.getMasterFingerprint();
    return fp.cols();
  }

  int getHeight() {
    const Matrix& fp = finge_.getMasterFingerprint();
    return fp.rows();
  }

  val getDebugInfo() {
    val info = val::object();
    info.set("nCore", finge_.getNCores());
    info.set("nDelta", finge_.getNDeltas());
    info.set("shapeL", finge_.getShapeL());
    info.set("shapeR", finge_.getShapeR());
    info.set("shapeT", finge_.getShapeT());
    info.set("shapeB", finge_.getShapeB());
    info.set("shapeM", finge_.getShapeM());
    info.set("minF", finge_.getMinF());
    info.set("maxF", finge_.getMaxF());

    // Convert core points to arrays
    val corePoints = val::array();
    const auto& cores = finge_.getCorePoints();
    for (size_t i = 0; i < cores.size(); i++) {
      val point = val::object();
      point.set("x", cores[i].x);
      point.set("y", cores[i].y);
      corePoints.call<void>("push", point);
    }
    info.set("cores", corePoints);

    // Convert delta points to arrays
    val deltaPoints = val::array();
    const auto& deltas = finge_.getDeltaPoints();
    for (size_t i = 0; i < deltas.size(); i++) {
      val point = val::object();
      point.set("x", deltas[i].x);
      point.set("y", deltas[i].y);
      deltaPoints.call<void>("push", point);
    }
    info.set("deltas", deltaPoints);

    return info;
  }

 private:
  FinGe finge_;
};

EMSCRIPTEN_BINDINGS(finge_module) {
  class_<Fingerprint>("Fingerprint")
      .constructor<>()
      .function("generateFromSeed", &Fingerprint::generateFromSeed)
      .function("getFingerprint", &Fingerprint::getFingerprint)
      .function("getWidth", &Fingerprint::getWidth)
      .function("getHeight", &Fingerprint::getHeight)
      .function("getDebugInfo", &Fingerprint::getDebugInfo);
}