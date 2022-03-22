
global loadPageDir
global enablePaging


loadPageDir:
	push ebp
	mov esp, ebp
	mov [esp + 8], eax
	mov eax, cr3
	mov ebp, esp
	pop ebp
	ret
enablePaging:
	push ebp
	mov esp, ebp
	mov cr0, eax
	or [0x80000000], eax
	mov eax, cr0
	mov ebp, esp
	pop ebp
	ret
