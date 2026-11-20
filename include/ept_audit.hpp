#pragma once

#include "ept_types.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace vtx_ept {

enum class EptLevel {
    Pml4 = 4,
    Pdpt = 3,
    Pd = 2,
    Pt = 1,
};

struct EptEntryAudit {
    bool present = false;
    bool is_leaf = false;
    bool read = false;
    bool write = false;
    bool execute = false;
    bool execute_user = false;
    bool is_rwx = false;
    bool is_execute_only = false;
    bool valid_mem_type = true;
    bool misaligned_large_page = false;
    bool invalid_perm_combo = false;
    uint8_t memory_type = 0;
    uint64_t physical_address = 0;
    std::vector<std::string> warnings;
};

EptEntryAudit audit_ept_entry(uint64_t raw_entry, EptLevel level);
const char* memory_type_to_string(uint8_t mem_type);

} // namespace vtx_ept
