#include "ept_audit.hpp"

namespace vtx_ept {
namespace {

bool is_valid_ept_mem_type(uint8_t mt) {
    return mt == 0 || mt == 1 || mt == 4 || mt == 5 || mt == 6;
}

} // namespace

EptEntryAudit audit_ept_entry(uint64_t raw_entry, EptLevel level) {
    EptEntryAudit out;
    out.read = (raw_entry & ept_bits::kRead) != 0;
    out.write = (raw_entry & ept_bits::kWrite) != 0;
    out.execute = (raw_entry & ept_bits::kExecute) != 0;
    out.execute_user = (raw_entry & ept_bits::kExecuteUser) != 0;
    out.present = out.read || out.write || out.execute || out.execute_user;

    if (!out.present) {
        return out;
    }

    const bool large_bit = (raw_entry & ept_bits::kLargePage) != 0;
    out.is_leaf = (level == EptLevel::Pt) ||
                  ((level == EptLevel::Pd || level == EptLevel::Pdpt) && large_bit);

    out.physical_address = raw_entry & ept_bits::kPfnMask;
    out.memory_type = static_cast<uint8_t>((raw_entry & ept_bits::kMemTypeMask) >> 3);

    // Intel SDM: write-only (W=1, R=0) is an EPT misconfiguration
    if (out.write && !out.read) {
        out.invalid_perm_combo = true;
        out.warnings.push_back("EPT misconfiguration: write=1 with read=0");
    }

    out.is_rwx = out.read && out.write && out.execute;
    if (out.is_leaf && out.is_rwx) {
        out.warnings.push_back("Leaf page is RWX (consider W^X split)");
    }

    out.is_execute_only = !out.read && !out.write && out.execute;

    if (out.is_leaf) {
        out.valid_mem_type = is_valid_ept_mem_type(out.memory_type);
        if (!out.valid_mem_type) {
            out.warnings.push_back("Invalid EPT memory type in leaf entry");
        }

        if (level == EptLevel::Pd && large_bit) {
            // 2MB page requires bits [20:12] of physical address to be 0
            if ((out.physical_address & 0x1FFFFFULL) != 0) {
                out.misaligned_large_page = true;
                out.warnings.push_back("2MB EPT PDE physical address is not 2MB-aligned");
            }
        } else if (level == EptLevel::Pdpt && large_bit) {
            // 1GB page requires bits [29:12] of physical address to be 0
            if ((out.physical_address & 0x3FFFFFFFULL) != 0) {
                out.misaligned_large_page = true;
                out.warnings.push_back("1GB EPT PDPTE physical address is not 1GB-aligned");
            }
        }
    }

    return out;
}

const char* memory_type_to_string(uint8_t mem_type) {
    switch (mem_type) {
    case 0:
        return "UC";
    case 1:
        return "WC";
    case 4:
        return "WT";
    case 5:
        return "WP";
    case 6:
        return "WB";
    default:
        return "RESERVED";
    }
}

} // namespace vtx_ept
