section .data
str0: db 'Hello, world!', 10
strlen0: equ $-str0
str5: db 'This should return error code 69!\n', 10
strlen5: equ $-str5
section .text
global _start
_start:
  mov edx, strlen0
  mov ecx, str0
  mov ebx, 1
  mov eax, 4
  int 0x80
  mov edx, strlen5
  mov ecx, str5
  mov ebx, 1
  mov eax, 4
  int 0x80
  mov eax, 1
  mov ebx, 69
  int 0x80
