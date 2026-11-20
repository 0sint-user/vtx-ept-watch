#pragma once

#include <cstdint>

namespace vtx_ept {

// Intel SDM Vol. 3C: EPT memory types
enum class EptMemoryType : uint8_t {
    Uncacheable = 0,
    WriteCombining = 1,
    WriteThrough = 4,
    WriteProtected = 5,
    WriteBack = 6,
};

// Selected Intel SDM VMCS field encodings
namespace vmcs {
constexpr uint32_t kEptPointer = 0x0000201A;
constexpr uint32_t kGuestPhysicalAddress = 0x00002400;
constexpr uint32_t kExitReason = 0x00004402;
constexpr uint32_t kExitQualification = 0x00006400;
constexpr uint32_t kGuestLinearAddress = 0x0000640A;
constexpr uint32_t kGuestRip = 0x0000681E;
} // namespace vmcs

namespace ept_bits {
constexpr uint64_t kRead = 1ULL << 0;
constexpr uint64_t kWrite = 1ULL << 1;
constexpr uint64_t kExecute = 1ULL << 2;
constexpr uint64_t kMemTypeMask = 0x7ULL << 3;
constexpr uint64_t kIgnorePat = 1ULL << 6;
constexpr uint64_t kLargePage = 1ULL << 7;
constexpr uint64_t kAccessed = 1ULL << 8;
constexpr uint64_t kDirty = 1ULL << 9;
constexpr uint64_t kExecuteUser = 1ULL << 10;
constexpr uint64_t kPfnMask = 0x000FFFFFFFFFF000ULL;
constexpr uint64_t kSuppressVe = 1ULL << 63;
} // namespace ept_bits

// EPT_VIOLATION exit qualification bits (Intel SDM Vol. 3C, Table 28-7)
struct EptViolationQual {
    bool data_read = false;
    bool data_write = false;
    bool instruction_fetch = false;
    bool was_readable = false;
    bool was_writable = false;
    bool was_executable = false;
    bool was_user_executable = false;
    bool guest_linear_valid = false;
    bool translated_by_page_walk = false;
    bool nmi_unblocking = false;
};

inline EptViolationQual decode_ept_violation(uint64_t qual) {
    EptViolationQual q;
    q.data_read = (qual & (1ULL << 0)) != 0;
    q.data_write = (qual & (1ULL << 1)) != 0;
    q.instruction_fetch = (qual & (1ULL << 2)) != 0;
    q.was_readable = (qual & (1ULL << 3)) != 0;
    q.was_writable = (qual & (1ULL << 4)) != 0;
    q.was_executable = (qual & (1ULL << 5)) != 0;
    q.was_user_executable = (qual & (1ULL << 6)) != 0;
    q.guest_linear_valid = (qual & (1ULL << 7)) != 0;
    q.translated_by_page_walk = (qual & (1ULL << 8)) != 0;
    q.nmi_unblocking = (qual & (1ULL << 12)) != 0;
    return q;
}

} // namespace vtx_ept
