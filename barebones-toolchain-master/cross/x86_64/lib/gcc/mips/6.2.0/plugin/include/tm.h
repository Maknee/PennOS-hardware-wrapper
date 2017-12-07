#ifndef GCC_TM_H
#define GCC_TM_H
#define TARGET_CPU_DEFAULT ((MASK_EXPLICIT_RELOCS) | MASK_RELAX_PIC_CALLS)
#ifndef LIBC_GLIBC
#define LIBC_GLIBC 1
#endif
#ifndef LIBC_UCLIBC
#define LIBC_UCLIBC 2
#endif
#ifndef LIBC_BIONIC
#define LIBC_BIONIC 3
#endif
#ifndef LIBC_MUSL
#define LIBC_MUSL 4
#endif
#ifdef IN_GCC
#include "config/elfos.h"
#include "config/initfini-array.h"
#include "config/mips/elf.h"
#include "config/mips/mips.h"
#include "config/newlib-stdint.h"
#include "config/vxworks-dummy.h"
#include "insn-constants.h"
#include "options.h"
#endif
#if defined IN_GCC && !defined GENERATOR_FILE && !defined USED_FOR_TARGET
#include "insn-flags.h"
#endif
#if defined IN_GCC && !defined GENERATOR_FILE
#include "insn-modes.h"
#endif
#if defined IN_GCC && defined GENERATOR_FILE && !defined BITS_PER_UNIT
#include "machmode.h"
#endif
#include "defaults.h"
#endif /* GCC_TM_H */
