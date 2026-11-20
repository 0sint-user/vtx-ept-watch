# vtx-ept-watch

Intel VT-x EPT (Extended Page Tables) boundary auditor and introspection helper.

Contains two parts:
1. `vtx-ept-audit`: portable C++20 CLI and library for decoding `EPTP`, `EPT_PML4E`/`PDPTE`/`PDE`/`PTE` entries, and `EPT_VIOLATION` exit qualifications, plus flagging `RWX` pages, misaligned large pages, and reserved-bit violations.
2. `src/driver/`: Windows x64 WDK C++20 / MASM64 reference helpers for configuring EPT read/write/execute splits and issuing `INVEPT` inside an isolated nested-virtualization testbed.

## Build (portable auditor + tests)

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

## Usage

Decode an `EPT_VIOLATION` exit qualification:

```sh
./build/vtx-ept-audit --qual 0x00000182
```

Inspect a 64-bit EPT leaf entry (PTE or 2MB PDE):

```sh
./build/vtx-ept-audit --pte 0x0000000102000437
```

## Notes

- Tested against Intel SDM Vol. 3C (Chapter 28/29 EPT and VMX exit qualification layouts).
- Driver files in `src/driver/` target Windows 11 x64 WDK and are meant for offline nested VMs (Hyper-V / VMware / QEMU).
