
	.data
global_variable:	.word 0
string5:	 .asciiz 	"\n"
string1:	 .asciiz 	"\nb : "
string2:	 .asciiz 	"\nc : "
string3:	 .asciiz 	"\nd : "
string4:	 .asciiz 	"\nglobal_variable : "
string0:	 .asciiz 	"a : "

	.text 			# The .text assembler directive indicates
	.globl f1		# The following is the code (as oppose to data)
f1:				# .globl makes main know to the 
				# outside of the program.
# Prologue begins 
	sw $ra, 0($sp)		# Save the return address
	sw $fp, -4($sp)		# Save the frame pointer
	sub $fp, $sp, 8		# Update the frame pointer

	sub $sp, $sp, 8		# Make space for the locals
# Prologue ends

	li $v0, 2
	move $v1, $v0
	j epilogue_f1

# Epilogue Begins
epilogue_f1:
	add $sp, $sp, 8
	lw $fp, -4($sp)  
	lw $ra, 0($sp)   
	jr        $31		# Jump back to the called procedure
# Epilogue Ends


	.text 			# The .text assembler directive indicates
	.globl f2		# The following is the code (as oppose to data)
f2:				# .globl makes main know to the 
				# outside of the program.
# Prologue begins 
	sw $ra, 0($sp)		# Save the return address
	sw $fp, -4($sp)		# Save the frame pointer
	sub $fp, $sp, 8		# Update the frame pointer

	sub $sp, $sp, 8		# Make space for the locals
# Prologue ends

	li $v0, 2000
	move $v1, $v0
	j epilogue_f2

# Epilogue Begins
epilogue_f2:
	add $sp, $sp, 8
	lw $fp, -4($sp)  
	lw $ra, 0($sp)   
	jr        $31		# Jump back to the called procedure
# Epilogue Ends


	.text 			# The .text assembler directive indicates
	.globl f3		# The following is the code (as oppose to data)
f3:				# .globl makes main know to the 
				# outside of the program.
# Prologue begins 
	sw $ra, 0($sp)		# Save the return address
	sw $fp, -4($sp)		# Save the frame pointer
	sub $fp, $sp, 8		# Update the frame pointer

	sub $sp, $sp, 8		# Make space for the locals
# Prologue ends

	li $v0, 3
	move $v1, $v0
	j epilogue_f3

# Epilogue Begins
epilogue_f3:
	add $sp, $sp, 8
	lw $fp, -4($sp)  
	lw $ra, 0($sp)   
	jr        $31		# Jump back to the called procedure
# Epilogue Ends


	.text 			# The .text assembler directive indicates
	.globl main		# The following is the code (as oppose to data)
main:				# .globl makes main know to the 
				# outside of the program.
# Prologue begins 
	sw $ra, 0($sp)		# Save the return address
	sw $fp, -4($sp)		# Save the frame pointer
	sub $fp, $sp, 8		# Update the frame pointer

	sub $sp, $sp, 28		# Make space for the locals
# Prologue ends

	jal f1
	move $v0, $v1
	sw $v0, 0($fp)
	jal f2
	move $v0, $v1
	sw $v0, -4($fp)
	jal f3
	move $v0, $v1
	sw $v0, -8($fp)
	li.d $f2, 20.22
	s.d $f2, -12($fp)
	lw $v0, 0($fp)
	lw $t0, -4($fp)
	add $t1, $v0, $t0
	sw $t1, global_variable
	addi $sp, $sp, -4
	sw $v0, 0($sp)
	addi $sp, $sp, -4
	sw $a0, 0($sp)
	addi $sp, $sp, -8
	s.d $f12, 0($sp)
	la $a0, string0
	li $v0, 4
	syscall 
	l.d $f12, 0($sp)
	addi $sp, $sp, 8
	lw $a0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, 0($fp)
	addi $sp, $sp, -4
	sw $v0, 0($sp)
	addi $sp, $sp, -4
	sw $a0, 0($sp)
	addi $sp, $sp, -8
	s.d $f12, 0($sp)
	move $a0, $v0
	li $v0, 1
	syscall 
	l.d $f12, 0($sp)
	addi $sp, $sp, 8
	lw $a0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, 0($sp)
	addi $sp, $sp, 4
	addi $sp, $sp, -4
	sw $v0, 0($sp)
	addi $sp, $sp, -4
	sw $a0, 0($sp)
	addi $sp, $sp, -8
	s.d $f12, 0($sp)
	la $a0, string1
	li $v0, 4
	syscall 
	l.d $f12, 0($sp)
	addi $sp, $sp, 8
	lw $a0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, -4($fp)
	addi $sp, $sp, -4
	sw $v0, 0($sp)
	addi $sp, $sp, -4
	sw $a0, 0($sp)
	addi $sp, $sp, -8
	s.d $f12, 0($sp)
	move $a0, $v0
	li $v0, 1
	syscall 
	l.d $f12, 0($sp)
	addi $sp, $sp, 8
	lw $a0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, 0($sp)
	addi $sp, $sp, 4
	addi $sp, $sp, -4
	sw $v0, 0($sp)
	addi $sp, $sp, -4
	sw $a0, 0($sp)
	addi $sp, $sp, -8
	s.d $f12, 0($sp)
	la $a0, string2
	li $v0, 4
	syscall 
	l.d $f12, 0($sp)
	addi $sp, $sp, 8
	lw $a0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, -8($fp)
	addi $sp, $sp, -4
	sw $v0, 0($sp)
	addi $sp, $sp, -4
	sw $a0, 0($sp)
	addi $sp, $sp, -8
	s.d $f12, 0($sp)
	move $a0, $v0
	li $v0, 1
	syscall 
	l.d $f12, 0($sp)
	addi $sp, $sp, 8
	lw $a0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, 0($sp)
	addi $sp, $sp, 4
	addi $sp, $sp, -4
	sw $v0, 0($sp)
	addi $sp, $sp, -4
	sw $a0, 0($sp)
	addi $sp, $sp, -8
	s.d $f12, 0($sp)
	la $a0, string3
	li $v0, 4
	syscall 
	l.d $f12, 0($sp)
	addi $sp, $sp, 8
	lw $a0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, 0($sp)
	addi $sp, $sp, 4
	l.d $f2, -12($fp)
	addi $sp, $sp, -4
	sw $v0, 0($sp)
	addi $sp, $sp, -4
	sw $a0, 0($sp)
	addi $sp, $sp, -8
	s.d $f12, 0($sp)
	mov.d $f12, $f2
	li $v0, 3
	syscall 
	l.d $f12, 0($sp)
	addi $sp, $sp, 8
	lw $a0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, 0($sp)
	addi $sp, $sp, 4
	addi $sp, $sp, -4
	sw $v0, 0($sp)
	addi $sp, $sp, -4
	sw $a0, 0($sp)
	addi $sp, $sp, -8
	s.d $f12, 0($sp)
	la $a0, string4
	li $v0, 4
	syscall 
	l.d $f12, 0($sp)
	addi $sp, $sp, 8
	lw $a0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, global_variable
	addi $sp, $sp, -4
	sw $v0, 0($sp)
	addi $sp, $sp, -4
	sw $a0, 0($sp)
	addi $sp, $sp, -8
	s.d $f12, 0($sp)
	move $a0, $v0
	li $v0, 1
	syscall 
	l.d $f12, 0($sp)
	addi $sp, $sp, 8
	lw $a0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, 0($sp)
	addi $sp, $sp, 4
	addi $sp, $sp, -4
	sw $v0, 0($sp)
	addi $sp, $sp, -4
	sw $a0, 0($sp)
	addi $sp, $sp, -8
	s.d $f12, 0($sp)
	la $a0, string5
	li $v0, 4
	syscall 
	l.d $f12, 0($sp)
	addi $sp, $sp, 8
	lw $a0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, 0($sp)
	addi $sp, $sp, 4
	lw $v0, 0($fp)
	li $t0, 1
	seq $t1, $v0, $t0
	beq $t1, $zero, label4 
	li $v0, 0
	move $v1, $v0
	j epilogue_main
	j label5

label4:    	
	lw $v0, -4($fp)
	li $t0, 1
	seq $t2, $v0, $t0
	beq $t2, $zero, label2 
	li $v0, 1
	move $v1, $v0
	j epilogue_main
	j label3

label2:    	
	lw $v0, -8($fp)
	li $t0, 1
	seq $t3, $v0, $t0
	beq $t3, $zero, label0 
	li $v0, 2
	move $v1, $v0
	j epilogue_main
	j label1

label0:    	
	li $v0, 3
	move $v1, $v0
	j epilogue_main

label1:    	

label3:    	

label5:    	
	li $v0, 0
	move $v1, $v0
	j epilogue_main

# Epilogue Begins
epilogue_main:
	add $sp, $sp, 28
	lw $fp, -4($sp)  
	lw $ra, 0($sp)   
	jr        $31		# Jump back to the called procedure
# Epilogue Ends

