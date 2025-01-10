.global _start
.align 2

_start:
	b _printf
	b _terminate

_printf:
	mov x0, #1		// stdout
	adr x1, helloworld	// address of hello world string
	mov x2, #14 		// length of hello world string
	mov x16, #4		// write to stdout
	svc 0			// syscall

_terminate:
	mov x0, #0		// return 0
	mov x16, #1		// exit
	svc 0			// syscall:w::w:wq
  

helloworld: .ascii "Hello, World!\n"
