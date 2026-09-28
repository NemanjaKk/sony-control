#pragma once

#include "SemanticTypes.h"
#include <memory>
#include <map>
#include <chrono>
#include <vector>

namespace sony::protocol {

struct FeatureStatus {
    std::string availability{"unknown"};
    int64_t lastSuccessMs{0};
    std::string error;
};
inline int64_t stateTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
struct DeviceState {
    std::map<std::string, FeatureStatus> features;
    BatteryState battery;
    NoiseControlState noiseControl;
    EqualizerState equalizer;

    bool dsee{false};

    std::string firmware;
    std::string codec;

    int autoPowerOff{0};
    bool speakToChat{false};
    bool adaptiveVolume{false};

    // Devices connected to the headset, when it supports switching playback between them.
    // Empty when it doesn't, or when the list hasn't been read.
    std::vector<PlaybackDevice> playbackDevices;

    // Legacy State
    // Appended So Existing DeviceState Fields Retain Their Layout
    int connectionQuality{-1};  // -1 unknown, 0 sound quality, 1 stable connection
    int voiceGuidance{-1};      // -1 unknown, 0 off, 1 on
    int vpt{-1};                // -1 unknown, otherwise preset 0..4
    int soundPosition{-1};      // -1 unknown, otherwise raw position code
};

using DeviceStateSnapshot = std::shared_ptr<const DeviceState>;

} // namespace sony::protocol
