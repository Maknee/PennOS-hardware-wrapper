section .text

; pit sigaction handler stub
[extern pit_sigaction_prev_eip]
[extern pit_sigaction_handler]
global pit_sigaction_handler_stub
pit_sigaction_handler_stub:
	;hacky af, basically store the previous eip, call the handler and then call ret to restore eip
	;cli
	;pushad
	;pushfd
	;call pit_sigaction_handler
	;mov [pit_sigaction_prev_eip], eax
	;popfd
	;popad
	;mov [esp - 8], eax
	;mov eax, [pit_sigaction_prev_eip]
	;push eax
	;mov eax, [esp - 8]
	;sti
	;ret
	call pit_sigaction_handler
	push eax
	ret

; sigint sigaction handler stub
[extern sigint_sigaction_prev_eip]
[extern sigint_sigaction_handler]
global sigint_sigaction_handler_stub
sigint_sigaction_handler_stub:
	;hacky af, basically store the previous eip, call the handler and then call ret to restore eip
	call sigint_sigaction_handler
	push eax
	ret

; sigtstp sigaction handler stub
[extern sigtstp_sigaction_prev_eip]
[extern sigtstp_sigaction_handler]
global sigtstp_sigaction_handler_stub
sigtstp_sigaction_handler_stub:
	;hacky af, basically store the previous eip, call the handler and then call ret to restore eip
	call sigtstp_sigaction_handler
	push eax
	ret


