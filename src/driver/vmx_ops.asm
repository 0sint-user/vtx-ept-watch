.code

; uint8_t __stdcall vmx_invept(uint64_t type, const InveptDescriptor* desc)
; rcx = type, rdx = desc
vmx_invept PROC
    invept rcx, oword ptr [rdx]
    setna al
    xor al, 1
    ret
vmx_invept ENDP

END
