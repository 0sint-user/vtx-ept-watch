#include "ept_audit.hpp"
#include "ept_types.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>

int main() {
    // 1. Decode EPT_VIOLATION: instruction fetch (bit 2) on a non-executable page with valid GLA (bit 7)
    {
        const uint64_t qual = (1ULL << 2) | (1ULL << 3) | (1ULL << 4) | (1ULL << 7);
        const auto q = vtx_ept::decode_ept_violation(qual);
        assert(!q.data_read);
        assert(!q.data_write);
        assert(q.instruction_fetch);
        assert(q.was_readable);
        assert(q.was_writable);
        assert(!q.was_executable);
        assert(q.guest_linear_valid);
    }

    // 2. Leaf PTE marked RWX with WB memory type (6)
    {
        const uint64_t pte = 0x0000000102000000ULL | (6ULL << 3) | 0x7ULL;
        const auto a = vtx_ept::audit_ept_entry(pte, vtx_ept::EptLevel::Pt);
        assert(a.present);
        assert(a.is_leaf);
        assert(a.is_rwx);
        assert(a.valid_mem_type);
        assert(!a.warnings.empty());
    }

    // 3. Misconfigured write-only PTE (W=1, R=0)
    {
        const uint64_t pte = 0x0000000102000000ULL | (6ULL << 3) | 0x2ULL;
        const auto a = vtx_ept::audit_ept_entry(pte, vtx_ept::EptLevel::Pt);
        assert(a.invalid_perm_combo);
    }

    // 4. Misaligned 2MB PDE
    {
        const uint64_t pde = 0x0000000102001000ULL | (1ULL << 7) | (6ULL << 3) | 0x5ULL;
        const auto a = vtx_ept::audit_ept_entry(pde, vtx_ept::EptLevel::Pd);
        assert(a.is_leaf);
        assert(a.misaligned_large_page);
    }

    std::cout << "test_ept_audit passed\n";
    return 0;
}
