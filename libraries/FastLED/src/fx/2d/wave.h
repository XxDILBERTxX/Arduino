

#include <stdint.h>

#include "fl/warn.h"

#include "fl/ptr.h"
#include "fl/wave_simulation.h"
#include "fl/xymap.h"
#include "fx/fx.h"
#include "fx/fx2d.h"
#include "pixelset.h"
#include "colorutils.h"

namespace fl {

FASTLED_SMART_PTR(WaveFx);
FASTLED_SMART_PTR(WaveCrgbMap);
FASTLED_SMART_PTR(WaveCrgbMapDefault);
FASTLED_SMART_PTR(WaveCrgbGradientMap);

class WaveCrgbMap: public Referent {
  public:
    virtual ~WaveCrgbMap() = default;
    virtual void mapWaveToLEDs(const XYMap& xymap, WaveSimulation2D &waveSim, CRGB *leds) = 0;
};

// A great deafult for the wave rendering. It will draw black and then the
// amplitude of the wave will be more white.
class WaveCrgbMapDefault : public WaveCrgbMap {
  public:
    void mapWaveToLEDs(const XYMap& xymap, WaveSimulation2D &waveSim, CRGB *leds) override {
        const uint32_t width = waveSim.getWidth();
        const uint32_t height = waveSim.getHeight();
        for (uint32_t y = 0; y < height; y++) {
            for (uint32_t x = 0; x < width; x++) {
                uint32_t idx = xymap(x, y);
                uint8_t value8 = waveSim.getu8(x, y);
                leds[idx] = CRGB(value8, value8, value8);
            }
        }
    }
};

class WaveCrgbGradientMap : public WaveCrgbMap {
  public:
    WaveCrgbGradientMap(CRGBPalette16 palette) : mPalette(palette) {}

    void mapWaveToLEDs(const XYMap& xymap, WaveSimulation2D &waveSim, CRGB *leds) override {
        const uint32_t width = waveSim.getWidth();
        const uint32_t height = waveSim.getHeight();
        for (uint32_t y = 0; y < height; y++) {
            for (uint32_t x = 0; x < width; x++) {
                uint32_t idx = xymap(x, y);
                uint8_t value8 = waveSim.getu8(x, y);
                leds[idx] = ColorFromPalette(mPalette, value8);
            }
        }
    }

  private:
    CRGBPalette16 mPalette;
};

struct WaveFxArgs {
    SuperSample factor = SuperSample::SUPER_SAMPLE_2X;
    bool half_duplex = true;
    bool auto_updates = true;
    float speed = 0.16f;
    float dampening = 6.0f;
    WaveCrgbMapPtr crgbMap;
};


// Uses bilearn filtering to double the size of the grid.
class WaveFx : public Fx2d {
  public:
    using Args = WaveFxArgs;

    WaveFx(XYMap xymap, Args args = Args())
        : Fx2d(xymap), mWaveSim(xymap.getWidth(), xymap.getHeight(), args.factor,
                                args.speed, args.dampening) {
        // Initialize the wave simulation with the given parameters.
        if (args.crgbMap == nullptr) {
            // Use the default CRGB mapping function.
            mCrgbMap = WaveCrgbMapDefaultPtr::New();
        } else {
            // Set a custom CRGB mapping function.
            mCrgbMap = args.crgbMap;
        }
        setAutoUpdate(args.auto_updates);
    }

    void setSpeed(float speed) {
        // Set the speed of the wave simulation.
        mWaveSim.setSpeed(speed);
    }

    void setDampening(float dampening) {
        // Set the dampening of the wave simulation.
        mWaveSim.setDampening(dampening);
    }

    void setHalfDuplex(bool on) {
        // Set whether the wave simulation is half duplex.
        mWaveSim.setHalfDuplex(on);
    }

    void setSuperSample(SuperSample factor) {
        // Set the supersampling factor of the wave simulation.
        mWaveSim.setSuperSample(factor);
    }

    void setEasingMode(U8EasingFunction mode) {
        // Set the easing mode for the 8-bit value.
        mWaveSim.setEasingMode(mode);
    }

    void setf(size_t x, size_t y, float value) {
        // Set the value at the given coordinates in the wave simulation.
        mWaveSim.setf(x, y, value);
    }

    void addf(size_t x, size_t y, float value) {
        // Add a value at the given coordinates in the wave simulation.
        float sum = value + mWaveSim.getf(x, y);        
        mWaveSim.setf(x, y, MIN(1.0f, sum));
    }

    uint8_t getu8(size_t x, size_t y) const {
        // Get the 8-bit value at the given coordinates in the wave simulation.
        return mWaveSim.getu8(x, y);
    }

    // This will now own the crgbMap.
    void setCrgbMap(WaveCrgbMapPtr crgbMap) {
        // Set a custom CRGB mapping function.
        mCrgbMap.reset(crgbMap);
    }

    void draw(DrawContext context) override {
        // Update the wave simulation.
        if (mAutoUpdates) {
            mWaveSim.update();
        }
        // Map the wave values to the LEDs.
        mCrgbMap->mapWaveToLEDs(mXyMap, mWaveSim, context.leds);
    }

    void setAutoUpdate(bool autoUpdate) {
        // Set whether to automatically update the wave simulation.
        mAutoUpdates = autoUpdate;
    }

    void update() {
        // Called automatically in draw. Only invoke this if you want extra
        // simulation updates.
        // Update the wave simulation.
        mWaveSim.update();
    }

    fl::Str fxName() const override { return "WaveFx"; }

    WaveSimulation2D mWaveSim;
    WaveCrgbMapPtr mCrgbMap;
    bool mAutoUpdates = true;
};

} // namespace fl
