section .text

;https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/x86/sys/ucontext.h.html

struc ucontext_t
	.uc_link resd 1
	;.stack_t resd 3 ;three uint32_t
	
	.ss_sp resd 1
	.ss_size resd 1
	.ss_flags resd 1

	.eax resd 1
	.ebx resd 1
	.ecx resd 1
	.edx resd 1
	.edi resd 1
	.esi resd 1
	.ebp resd 1
	
	.esp resd 1
	.ss resd 1
	.ds resd 1
	
	.eip resd 1
	.eflags resd 1
	.cs resd 1

	;uc_sigmask
	.uc_sigmask resd 1
endstruc

;extern void getcontext(ucontext_t*);
;https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/i386/getcontext.S.html

;getcontext does not push ANYTHING on the ucontext's stack, it only saves the current cpu state into the ucontext structure

global getcontext
getcontext:
	
	;get parameter (ucontext_t* ucp)
	mov eax, [esp + 4]
	
	;eax not preserved
	;save general purpose registers
	;mov [eax + ucontext_t.eax], 0
	mov [eax + ucontext_t.ebx], ebx
	mov [eax + ucontext_t.ecx], ecx
	mov [eax + ucontext_t.edx], edx
	mov [eax + ucontext_t.edi], edi
	mov [eax + ucontext_t.esi], esi
	mov [eax + ucontext_t.ebp], ebp

	;clear ecx (used for other variables)
	xor ecx, ecx

	;save stack
	mov [eax + ucontext_t.esp], esp
	mov cx, ss
	mov [eax + ucontext_t.ss], ecx
	mov cx, ds
	mov [eax + ucontext_t.ds], ecx

	;store return address as eip
	mov ecx, [esp]
	mov [eax + ucontext_t.eip], ecx

	;store eflags
	pushf
	mov ecx, [esp]
	mov [eax + ucontext_t.eflags], ecx
	popf
	
	;clear ecx (used for other variables)
	xor ecx, ecx

	mov cx, cs
	mov [eax + ucontext_t.cs], ecx
	
	;success
	mov eax, 0

	ret

;int setcontext(const ucontext_t *ucp)

;https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/i386/setcontext.S.html

;top of stack

;-------------
;function to execute (from ucontext's eip, which is set by makecontext)
;------------- (assume everything down here is set from make context
;ucontext_termination (return address)
;-------------
;parameters to func
;-------------
;termination function
;-------------
;uc_link
;-------------

;bottom of stack

;https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/i386/setcontext.S.html

global setcontext
setcontext:

	cli

	;get parameter (ucontext_t* ucp)
	mov eax, [esp + 4]

	;clear ecx (used for other variables)
	xor ecx, ecx

	;update stack
	mov ecx, [eax + ucontext_t.ss]
	mov ss, cx
	mov ecx, [eax + ucontext_t.ds]
	mov ds, cx

	;set stack to ucontext's stack
	mov esp, [eax + ucontext_t.esp]

	;push return address context's eip
	mov ecx, [eax + ucontext_t.eip]
	push ecx
	
	mov ecx, [eax + ucontext_t.cs]
	;mov cs, cx ;doing so results in a general protection fault?
	
	;restore eflags
	mov ecx, [eax + ucontext_t.eflags]
	push ecx
	popf
	
	;eax not preserved
	;save general purpose registers
	;mov [eax + ucontext_t.eax], 0
	mov ebx, [eax + ucontext_t.ebx]
	mov ecx, [eax + ucontext_t.ecx]
	mov edx, [eax + ucontext_t.edx]
	mov edi, [eax + ucontext_t.edi]
	mov esi, [eax + ucontext_t.esi]
	mov ebp, [eax + ucontext_t.ebp]

	sti

	;return to the ucontext's eip we pushed earlier
	ret

;void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...);
;https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/i386/makecontext.S.html

;top of stack

;-------------
;ucontext_termination (return address)
;-------------
;parameters to func
;-------------
;termination function
;-------------
;uc_link
;-------------

;bottom of stack

global makecontext
makecontext:

	;get parameter (ucontext_t* ucp)
	mov eax, [esp + 4]
	
	;get parameter (void (*function)())
	mov ecx, [esp + 8]

	;store the function of what we want to run
	mov [eax + ucontext_t.eip], ecx

	;get parameter (argc)
	mov edx, [esp + 12]

	;store paramaters in ucontext's ebx
	mov [eax + ucontext_t.ebx], edx

	;compute start of stack
	;get stack pointer
	mov ecx, [eax + ucontext_t.ss_sp]
	;add stack size (now at start of stack)
	add ecx, [eax + ucontext_t.ss_size]

	;allocate on stack parameters
	;negative of parameters (stack grows upwards)
	;substact 4 to get one less than start of stack (we are at one pass end)
	neg edx
	lea ecx, [ecx + edx * 4 - 4]
	neg edx

	;align stack (get more space)
	and ecx, 0xFFFFFFF0
	sub ecx, 4

	;update ucontext's stack pointer
	mov [eax + ucontext_t.esp], ecx

	;put next ucontext pointer on stack (uc_link)
	mov eax, [eax + ucontext_t.uc_link]
	mov [ecx + edx * 4 + 4], eax

	;swap (ecx = argc, edx = ss_sp)
	xchg ecx, edx

	;get each parameter and populate stack
	;jump if ecx == 0
	jecxz .done_args

.more_args:
	
	;get from rightmost to leftmost
	mov eax, [esp + ecx * 4 + 12]
	mov [edx + ecx * 4], eax
	
	dec ecx
	jnz .more_args

.done_args:
	
	;push on stack address when ucontext terminations (aka returns)
	lea ecx, [ucontext_termination]
	mov [edx], ecx

	ret

;after ucontext terminates (assume esp is the ucontext's esp)
ucontext_termination:
	
	;pop off arguments to function (ebx has number of arguments)
	lea esp, [esp + ebx * 4]

	;check if uc_link is null
	mov eax, [esp]
	test eax, eax
	je .ucontext_abort
	
	;not null, call set context on uc_link
	push eax
	call setcontext

	;if uc_link returns, then abort as well

.ucontext_abort:
[extern abort]
[extern vga_printf]
	push ucontext_abort_msg
	call vga_printf
	call abort

ucontext_abort_msg: db "ucontext aborted!", 10h, 0


;int swapcontext(ucontext_t *oucp, const ucontext_t *ucp);

;https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/i386/swapcontext.S.html

;global swapcontext
;swapcontext:

