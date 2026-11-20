#include "vmx_ept.hpp"

namespace vtx_drv {
namespace {

constexpr uint64_t kEptRead = 1ULL << 0;
constexpr uint64_t kEptWrite = 1ULL << 1;
constexpr uint64_t kEptExecute = 1ULL << 2;
constexpr uint64_t kPermMask = kEptRead | kEptWrite | kEptExecute;

} // namespace

NTSTATUS set_ept_page_permissions(
    uint64_t* ept_pte,
    uint64_t eptp,
    bool read,
    bool write,
    bool execute)
{
    if (!ept_pte || eptp == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    // Disallow write-only EPT entries (triggers EPT misconfiguration VM-exit).
    if (write && !read) {
        return STATUS_INVALID_PARAMETER;
    }

    uint64_t entry = *ept_pte;
    entry &= ~kPermMask;
    if (read) {
        entry |= kEptRead;
    }
    if (write) {
        entry |= kEptWrite;
    }
    if (execute) {
        entry |= kEptExecute;
    }

    // Atomic 64-bit store on x64-aligned PTE before issuing INVEPT.
    InterlockedExchange64(reinterpret_cast<volatile LONG64*>(ept_pte), static_cast<LONG64>(entry));

    InveptDescriptor desc{};
    desc.ept_pointer = eptp;
    desc.reserved = 0;

    const uint8_t ok = vmx_invept(static_cast<uint64_t>(InveptType::SingleContext), &desc);
    return ok ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}

} // namespace vtx_drv
