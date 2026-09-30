#include "FakeHeadset.h"

#include "sony/protocol/ProtocolV2.h"
#include "sony/protocol/SonyProtocolSession.h"

#include <gtest/gtest.h>

#include <memory>

using sony::protocol::ProtocolV2;
using sony::protocol::SonyProtocolSession;
using sony::test::FakeHeadset;
using sony::test::kTestAddress;
using sony::test::Payload;

namespace {

// ProtocolV2 on a scripted headset, with the ULT equalizer layout on or off.
class EqualizerV2 : public ::testing::TestWithParam<bool> {
protected:
    void SetUp() override {
        auto transport = std::make_unique<FakeHeadset>();
        headset = transport.get();
        session = std::make_unique<SonyProtocolSession>(std::move(transport));
        session->connect(kTestAddress);
        protocol = std::make_unique<ProtocolV2>(*session, false, GetParam());
    }

    void TearDown() override { session->disconnect(); }

    Payload lastRequest() const { return headset->requests().back(); }

    FakeHeadset* headset{};
    std::unique_ptr<SonyProtocolSession> session;
    std::unique_ptr<ProtocolV2> protocol;
};

} // namespace

TEST_P(EqualizerV2, GetSendsTheInquiredTypeAndParsesTheReply) {
    if (GetParam()) {
        headset->reply({{0x57, 0x03, 0xa0, 0x01, 0x06, 0x0f, 0x06, 0x0a, 0x0f, 0x12, 0x14}});
    } else {
        headset->reply({{0x57, 0x00, 0xa0, 0x06, 0x0f, 0x06, 0x0a, 0x0f, 0x12, 0x14}});
    }

    const auto state = protocol->getEqualizer();

    EXPECT_EQ(lastRequest(), (Payload{0x56, static_cast<uint8_t>(GetParam() ? 0x03 : 0x00)}));
    EXPECT_EQ(state.preset, 0xa0);
    EXPECT_EQ(state.clearBass, 5);
    EXPECT_EQ(state.bands, (std::array<int, 5>{-4, 0, 5, 8, 10}));
}

TEST_P(EqualizerV2, SetPresetCarriesTheUltByteOnlyOnUltDevices) {
    headset->reply();
    protocol->setEqualizerPreset(0x14);

    if (GetParam()) {
        EXPECT_EQ(lastRequest(), (Payload{0x58, 0x03, 0x14, 0x01, 0x00}));   // capture: 58 03 14 01 00
    } else {
        EXPECT_EQ(lastRequest(), (Payload{0x58, 0x00, 0x14, 0x00}));
    }
}

TEST_P(EqualizerV2, SetCustomCarriesTheUltByteOnlyOnUltDevices) {
    headset->reply();
    protocol->setEqualizerCustom(5, {-4, 0, 5, 8, 10});

    if (GetParam()) {
        EXPECT_EQ(lastRequest(), (Payload{0x58, 0x03, 0xa0, 0x01, 0x06, 0x0f, 0x06, 0x0a, 0x0f, 0x12, 0x14}));
    } else {
        EXPECT_EQ(lastRequest(), (Payload{0x58, 0x00, 0xa0, 0x06, 0x0f, 0x06, 0x0a, 0x0f, 0x12, 0x14}));
    }
}

// googletest 1.8.1 predates INSTANTIATE_TEST_SUITE_P.
INSTANTIATE_TEST_CASE_P(Layouts, EqualizerV2, ::testing::Bool(),
                        [](const ::testing::TestParamInfo<bool>& info) {
                            return std::string(info.param ? "UltWear" : "Legacy");
                        });
