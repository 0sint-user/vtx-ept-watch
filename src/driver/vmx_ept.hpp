#pragma once

#include <ntddk.h>
#include <cstdint>

namespace vtx_drv {

struct alignas(16) InveptDescriptor {
    uint64_t ept_pointer;
    uint64_t reserved;
};

enum class InveptType : uint64_t {
    SingleContext = 1,
    Global = 2,
};

extern "C" {
uint8_t __stdcall vmx_invept(uint64_t type, const InveptDescriptor* desc);
}

// Updates a 4KB EPT PTE with W^X split permissions and invalidates the EPT context.
NTSTATUS set_ept_page_permissions(
    uint64_t* ept_pte,
    uint64_t eptp,
    bool read,
    bool write,
    bool execute);

} // namespace vtx_drv
