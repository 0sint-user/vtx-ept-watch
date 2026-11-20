#include "ept_audit.hpp"
#include "ept_types.hpp"

#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

namespace {

bool parse_hex_u64(const char* s, uint64_t& out) {
    if (!s || *s == '\0') {
        return false;
    }
    char* end = nullptr;
    out = std::strtoull(s, &end, 16);
    return end != s && *end == '\0';
}

void print_qual(uint64_t raw) {
    const auto q = vtx_ept::decode_ept_violation(raw);
    std::cout << "EPT_VIOLATION qualification: 0x" << std::hex << std::uppercase << raw << std::dec << "\n";
    std::cout << "  Access: "
              << (q.data_read ? "READ " : "")
              << (q.data_write ? "WRITE " : "")
              << (q.instruction_fetch ? "EXEC " : "") << "\n";
    std::cout << "  Entry perms: R=" << q.was_readable
              << " W=" << q.was_writable
              << " X=" << q.was_executable
              << " Xu=" << q.was_user_executable << "\n";
    std::cout << "  Guest linear valid: " << (q.guest_linear_valid ? "yes" : "no")
              << " | Page-walk translation: " << (q.translated_by_page_walk ? "yes" : "no") << "\n";
}

void print_entry(uint64_t raw, vtx_ept::EptLevel level) {
    const auto a = vtx_ept::audit_ept_entry(raw, level);
    std::cout << "EPT entry (level " << static_cast<int>(level) << "): 0x"
              << std::hex << std::uppercase << std::setw(16) << std::setfill('0') << raw << std::dec << "\n";
    if (!a.present) {
        std::cout << "  Not present\n";
        return;
    }
    std::cout << "  Leaf: " << (a.is_leaf ? "yes" : "no")
              << " | Perms: " << (a.read ? "R" : "-") << (a.write ? "W" : "-") << (a.execute ? "X" : "-")
              << " | Phys: 0x" << std::hex << a.physical_address << std::dec;
    if (a.is_leaf) {
        std::cout << " | MemType: " << vtx_ept::memory_type_to_string(a.memory_type);
    }
    std::cout << "\n";
    for (const auto& w : a.warnings) {
        std::cout << "  [WARN] " << w << "\n";
    }
}

} // namespace

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage:\n"
                  << "  vtx-ept-audit --qual <hex>\n"
                  << "  vtx-ept-audit --pte <hex>\n"
                  << "  vtx-ept-audit --pde <hex>\n";
        return 1;
    }

    const std::string mode = argv[1];
    uint64_t val = 0;
    if (!parse_hex_u64(argv[2], val)) {
        std::cerr << "Error: invalid hex value: " << argv[2] << "\n";
        return 1;
    }

    if (mode == "--qual") {
        print_qual(val);
        return 0;
    }
    if (mode == "--pte") {
        print_entry(val, vtx_ept::EptLevel::Pt);
        return 0;
    }
    if (mode == "--pde") {
        print_entry(val, vtx_ept::EptLevel::Pd);
        return 0;
    }

    std::cerr << "Error: unknown option: " << mode << "\n";
    return 1;
}
