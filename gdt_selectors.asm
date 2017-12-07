%ifndef GDT_SEGMENTS
%define GDT_SEGMENTS
section .text

%define NULL_SEGMENT 0x0
%define CODE_SEGMENT 0x8
%define DATA_SEGMENT 0x10

global set_selectors_to_gdt
set_selectors_to_gdt:
	jmp CODE_SEGMENT:load_cs

load_cs:
	mov ax, DATA_SEGMENT
	mov ds, ax
	mov fs, ax
	mov es, ax
	mov ss, ax
	mov gs, ax
	ret
%endif



