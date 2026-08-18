


_throw_undef:
    udf #0
    

.global _test_return_jump
_test_return_jump
    mov, r0, #10
    b 1f
    _throw_undef
    _throw_undef
    _throw_undef
    _throw_undef

1:
    subs r0, r0, #1
    bne 2b
    bxeq lr
    _throw_undef
    _throw_undef
    _throw_undef
    _throw_undef

2: b 1f
    _throw_undef
    _throw_undef
    _throw_undef
    _throw_undef

.global _test_registers
_test_registers:
    push {r0-r5}
    mov r0  r6
    mov r1  r7
    mov r2  r8
    mov r3  r9
    mov r4  r10
    mov r5  r11

    add r0  r0  137
    add r1  r1  137
    add r2  r2  137
    add r3  r3  137
    add r4  r4  137
    add r5  r5  137

    be  r0  r6  _undef
    be  r1  r7  _undef
    be  r2  r8  _undef
    be  r3  r9  _undef
    be  r4  r10 _undef
    be  r5  r11 _undef

    add r6  r6  137
    add r7  r7  137
    add r8  r8  137
    add r9  r9  137
    add r10 r10 137
    add r11 r11 137

    bne r0  r6  _undef
    bne r1  r7  _undef
    bne r2  r8  _undef
    bne r3  r9  _undef
    bne r4  r10 _undef
    bne r5  r11 _undef

_undef:
    /* Do an undef */

