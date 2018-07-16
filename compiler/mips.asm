.data
$global:	.space	4
ss:	.space	4
str_test:	.space	200
number:	.space	120
ans:	.space	120
ra:	.space	4
rb:	.space	4
rc:	.space	4
rd:	.space	4
$str_1: .asciiz "variable and const defination passed!"
$str_2: .asciiz "c is "
$str_3: .asciiz " !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
$str_4: .asciiz "please input the mode before test(1~3)"
$str_5: .asciiz "Easy"
$str_6: .asciiz "Middle"
$str_7: .asciiz "Hard"
$str_8: .asciiz "The character a in str_test is encountered "
$str_9: .asciiz "The sum of input numbers is "
$str_10: .asciiz "The cov of input number is "
$str_11: .asciiz "quick sort failed!"
$str_12: .asciiz "quick sort failed!"
$str_13: .asciiz "quick sort success!"
.text
la $gp, $global
addi $gp, $gp, 4
subi $sp, $sp, 4
move $fp, $sp
j	main
defi:
	subi $sp, $sp, 4
	sw $ra, ($sp)
	subi $sp, $sp, 492
	lw $t0, -8($fp)
	move $s0, $t0
	lw $t2, -8($fp)
	subi $t0, $fp, 24
	li $t1, 5
	sll $t1, $t1, 2
	sub $t0, $t0, $t1
	sw $t2, ($t0)
	li $t0, 100
	move $s0, $t0
	li $t0, 43
	move $s1, $t0
	la $a0, $str_1
	li $v0, 4
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
defi_test:
	subi $sp, $sp, 4
	sw $ra, ($sp)
	subi $sp, $sp, 0
	li $t0, 66
	sw $t0, -12($sp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal defi
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
max:
	subi $sp, $sp, 4
	sw $ra, ($sp)
	subi $sp, $sp, 20
	lw $t1, -8($fp)
	lw $t2, -12($fp)
	ble $t1, $t2, label_1
	lw $v1, -8($fp)
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
	j label_2
label_1:
label_2:
	lw $t1, -12($fp)
	lw $t2, -8($fp)
	ble $t1, $t2, label_3
	lw $v1, -12($fp)
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
	j label_4
label_3:
label_4:
	li $t1, 0
	lw $t2, -12($fp)
	sub $t0, $t1, $t2
	sw $t0, -16($fp)
	lw $t1, -8($fp)
	lw $t2, -16($fp)
	sub $t0, $t1, $t2
	sw $t0, -20($fp)
	lw $t1, -20($fp)
	li $t2, 2
	div $t0, $t1, $t2
	sw $t0, -24($fp)
	lw $v1, -24($fp)
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
fun_def_test:
	subi $sp, $sp, 4
	sw $ra, ($sp)
	subi $sp, $sp, 16
	li $t0, 5
	sw $t0, -12($sp)
	li $t0, 5
	sw $t0, -16($sp)
	sw $s0, -8($fp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal max
	sw $v1, -12($fp)
	lw $s0, -8($fp)
	lw $t0, -12($fp)
	sw $t0, -12($sp)
	li $t0, 5
	sw $t0, -16($sp)
	sw $s0, -8($fp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal max
	sw $v1, -16($fp)
	lw $s0, -8($fp)
	lw $t0, -16($fp)
	sw $t0, -12($sp)
	li $t0, 5
	sw $t0, -16($sp)
	sw $s0, -8($fp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal max
	sw $v1, -20($fp)
	lw $s0, -8($fp)
	lw $t0, -20($fp)
	move $s0, $t0
	la $a0, $str_2
	li $v0, 4
	syscall
	li $v0, 1
	move $a0, $s0
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
calcu:
	subi $sp, $sp, 4
	sw $ra, ($sp)
	subi $sp, $sp, 36
	li $t0, 0
	move $s0, $t0
	li $t0, 0
	move $s1, $t0
	addi $t0, $gp, 204
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -20($fp)
	lw $t2, -20($fp)
	add $t0, $s1, $t2
	sw $t0, -24($fp)
	lw $t0, -24($fp)
	move $s1, $t0
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -28($fp)
	lw $t0, -28($fp)
	move $s0, $t0
label_5:
	lw $t2, -8($fp)
	bge $s0, $t2, label_6
	addi $t0, $gp, 204
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -32($fp)
	lw $t2, -32($fp)
	add $t0, $s1, $t2
	sw $t0, -36($fp)
	lw $t0, -36($fp)
	move $s1, $t0
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -40($fp)
	lw $t0, -40($fp)
	move $s0, $t0
	li $v0, 1
	move $a0, $s1
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	j label_5
label_6:
	move $v1, $s1
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
calcucov:
	subi $sp, $sp, 4
	sw $ra, ($sp)
	subi $sp, $sp, 76
	li $t0, 0
	move $s0, $t0
	li $t0, 0
	move $s1, $t0
	addi $t0, $gp, 204
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -28($fp)
	lw $t1, -28($fp)
	lw $t2, -12($fp)
	sub $t0, $t1, $t2
	sw $t0, -32($fp)
	addi $t0, $gp, 204
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -36($fp)
	lw $t1, -36($fp)
	lw $t2, -12($fp)
	sub $t0, $t1, $t2
	sw $t0, -40($fp)
	lw $t1, -32($fp)
	lw $t2, -40($fp)
	mul $t0, $t1, $t2
	sw $t0, -44($fp)
	lw $t0, -44($fp)
	move $s2, $t0
	add $t0, $s1, $s2
	sw $t0, -48($fp)
	lw $t0, -48($fp)
	move $s1, $t0
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -52($fp)
	lw $t0, -52($fp)
	move $s0, $t0
label_7:
	lw $t2, -8($fp)
	bge $s0, $t2, label_8
	addi $t0, $gp, 204
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -56($fp)
	lw $t1, -56($fp)
	lw $t2, -12($fp)
	sub $t0, $t1, $t2
	sw $t0, -60($fp)
	addi $t0, $gp, 204
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -64($fp)
	lw $t1, -64($fp)
	lw $t2, -12($fp)
	sub $t0, $t1, $t2
	sw $t0, -68($fp)
	lw $t1, -60($fp)
	lw $t2, -68($fp)
	mul $t0, $t1, $t2
	sw $t0, -72($fp)
	lw $t0, -72($fp)
	move $s2, $t0
	add $t0, $s1, $s2
	sw $t0, -76($fp)
	lw $t0, -76($fp)
	move $s1, $t0
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -80($fp)
	lw $t0, -80($fp)
	move $s0, $t0
	j label_7
label_8:
	move $v1, $s1
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
partion:
	subi $sp, $sp, 4
	sw $ra, ($sp)
	subi $sp, $sp, 92
	addi $t0, $gp, 204
	lw $t1, -12($fp)
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -32($fp)
	lw $t0, -32($fp)
	move $s3, $t0
	lw $t1, -8($fp)
	li $t2, 1
	sub $t0, $t1, $t2
	sw $t0, -36($fp)
	lw $t0, -36($fp)
	move $s1, $t0
	lw $t0, -8($fp)
	move $s0, $t0
	addi $t0, $gp, 204
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -40($fp)
	lw $t1, -40($fp)
	bgt $t1, $s3, label_9
	li $t2, 1
	add $t0, $s1, $t2
	sw $t0, -44($fp)
	lw $t0, -44($fp)
	move $s1, $t0
	addi $t0, $gp, 204
	sll $t1, $s1, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -48($fp)
	lw $t0, -48($fp)
	move $s2, $t0
	addi $t0, $gp, 204
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -52($fp)
	lw $t2, -52($fp)
	addi $t0, $gp, 204
	sll $t1, $s1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	addi $t0, $gp, 204
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	sw $s2, ($t0)
	j label_10
label_9:
label_10:
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -56($fp)
	lw $t0, -56($fp)
	move $s0, $t0
label_11:
	lw $t2, -12($fp)
	bge $s0, $t2, label_12
	addi $t0, $gp, 204
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -60($fp)
	lw $t1, -60($fp)
	bgt $t1, $s3, label_13
	li $t2, 1
	add $t0, $s1, $t2
	sw $t0, -64($fp)
	lw $t0, -64($fp)
	move $s1, $t0
	addi $t0, $gp, 204
	sll $t1, $s1, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -68($fp)
	lw $t0, -68($fp)
	move $s2, $t0
	addi $t0, $gp, 204
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -72($fp)
	lw $t2, -72($fp)
	addi $t0, $gp, 204
	sll $t1, $s1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	addi $t0, $gp, 204
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	sw $s2, ($t0)
	j label_14
label_13:
label_14:
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -76($fp)
	lw $t0, -76($fp)
	move $s0, $t0
	j label_11
label_12:
	li $t2, 1
	add $t0, $s1, $t2
	sw $t0, -80($fp)
	addi $t0, $gp, 204
	lw $t1, -80($fp)
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -84($fp)
	lw $t0, -84($fp)
	move $s2, $t0
	li $t2, 1
	add $t0, $s1, $t2
	sw $t0, -88($fp)
	addi $t0, $gp, 204
	lw $t1, -12($fp)
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -92($fp)
	lw $t2, -92($fp)
	addi $t0, $gp, 204
	lw $t1, -88($fp)
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	addi $t0, $gp, 204
	lw $t1, -12($fp)
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $s2, ($t0)
	li $t2, 1
	add $t0, $s1, $t2
	sw $t0, -96($fp)
	lw $v1, -96($fp)
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
quick_sort:
	subi $sp, $sp, 4
	sw $ra, ($sp)
	subi $sp, $sp, 24
	lw $t1, -8($fp)
	lw $t2, -12($fp)
	bge $t1, $t2, label_15
	lw $t0, -8($fp)
	sw $t0, -12($sp)
	lw $t0, -12($fp)
	sw $t0, -16($sp)
	sw $s0, -16($fp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal partion
	sw $v1, -20($fp)
	lw $s0, -16($fp)
	lw $t0, -20($fp)
	move $s0, $t0
	li $t2, 1
	sub $t0, $s0, $t2
	sw $t0, -24($fp)
	lw $t0, -8($fp)
	sw $t0, -12($sp)
	lw $t0, -24($fp)
	sw $t0, -16($sp)
	sw $s0, -16($fp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal quick_sort
	lw $s0, -16($fp)
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -28($fp)
	lw $t0, -28($fp)
	sw $t0, -12($sp)
	lw $t0, -12($fp)
	sw $t0, -16($sp)
	sw $s0, -16($fp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal quick_sort
	lw $s0, -16($fp)
	j label_16
label_15:
label_16:
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
reverse:
	subi $sp, $sp, 4
	sw $ra, ($sp)
	subi $sp, $sp, 56
	li $t0, 0
	move $s0, $t0
	addi $t0, $gp, 4
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -20($fp)
	lw $t0, -20($fp)
	move $s1, $t0
	lw $t1, -8($fp)
	sub $t0, $t1, $s0
	sw $t0, -24($fp)
	addi $t0, $gp, 4
	lw $t1, -24($fp)
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -28($fp)
	lw $t2, -28($fp)
	addi $t0, $gp, 4
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	lw $t1, -8($fp)
	sub $t0, $t1, $s0
	sw $t0, -32($fp)
	addi $t0, $gp, 4
	lw $t1, -32($fp)
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $s1, ($t0)
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -36($fp)
	lw $t0, -36($fp)
	move $s0, $t0
label_17:
	lw $t1, -8($fp)
	li $t2, 2
	div $t0, $t1, $t2
	sw $t0, -40($fp)
	lw $t2, -40($fp)
	bge $s0, $t2, label_18
	addi $t0, $gp, 4
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -44($fp)
	lw $t0, -44($fp)
	move $s1, $t0
	lw $t1, -8($fp)
	sub $t0, $t1, $s0
	sw $t0, -48($fp)
	addi $t0, $gp, 4
	lw $t1, -48($fp)
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -52($fp)
	lw $t2, -52($fp)
	addi $t0, $gp, 4
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	lw $t1, -8($fp)
	sub $t0, $t1, $s0
	sw $t0, -56($fp)
	addi $t0, $gp, 4
	lw $t1, -56($fp)
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $s1, ($t0)
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -60($fp)
	lw $t0, -60($fp)
	move $s0, $t0
	j label_17
label_18:
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
charcount:
	subi $sp, $sp, 4
	sw $ra, ($sp)
	subi $sp, $sp, 72
	li $t0, 0
	move $s1, $t0
	li $t0, 0
	move $s0, $t0
	addi $t0, $gp, 4
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -16($fp)
	lw $t1, -16($fp)
	li $t2, 83
	bne $t1, $t2, label_20
	addi $t0, $gp, 4
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -20($fp)
	lw $t1, -20($fp)
	li $t2, 97
	bne $t1, $t2, label_21
	li $t2, 1
	add $t0, $s1, $t2
	sw $t0, -24($fp)
	lw $t0, -24($fp)
	move $s1, $t0
	j label_22
label_21:
label_22:
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -28($fp)
	lw $t0, -28($fp)
	move $s0, $t0
label_23:
	li $t2, 10
	bgt $s0, $t2, label_24
	addi $t0, $gp, 4
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -32($fp)
	lw $t1, -32($fp)
	li $t2, 97
	bne $t1, $t2, label_25
	li $t2, 1
	add $t0, $s1, $t2
	sw $t0, -36($fp)
	lw $t0, -36($fp)
	move $s1, $t0
	j label_26
label_25:
label_26:
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -40($fp)
	lw $t0, -40($fp)
	move $s0, $t0
	j label_23
label_24:
	j label_19
label_20:
	lw $t1, -16($fp)
	li $t2, 69
	bne $t1, $t2, label_27
	move $v1, $s1
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
	j label_19
label_27:
label_19:
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -44($fp)
	lw $t0, -44($fp)
	move $s0, $t0
label_28:
	li $t1, 1
	li $t2, 0
	beq $t1, $t2, label_29
	addi $t0, $gp, 4
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -48($fp)
	lw $t1, -48($fp)
	li $t2, 83
	bne $t1, $t2, label_31
	addi $t0, $gp, 4
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -52($fp)
	lw $t1, -52($fp)
	li $t2, 97
	bne $t1, $t2, label_32
	li $t2, 1
	add $t0, $s1, $t2
	sw $t0, -56($fp)
	lw $t0, -56($fp)
	move $s1, $t0
	j label_33
label_32:
label_33:
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -60($fp)
	lw $t0, -60($fp)
	move $s0, $t0
label_34:
	li $t2, 10
	bgt $s0, $t2, label_35
	addi $t0, $gp, 4
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -64($fp)
	lw $t1, -64($fp)
	li $t2, 97
	bne $t1, $t2, label_36
	li $t2, 1
	add $t0, $s1, $t2
	sw $t0, -68($fp)
	lw $t0, -68($fp)
	move $s1, $t0
	j label_37
label_36:
label_37:
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -72($fp)
	lw $t0, -72($fp)
	move $s0, $t0
	j label_34
label_35:
	j label_30
label_31:
	lw $t1, -48($fp)
	li $t2, 69
	bne $t1, $t2, label_38
	move $v1, $s1
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
	j label_30
label_38:
label_30:
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -76($fp)
	lw $t0, -76($fp)
	move $s0, $t0
	j label_28
label_29:
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
other_test1:
	subi $sp, $sp, 4
	sw $ra, ($sp)
	subi $sp, $sp, 12
	la $a0, $str_3
	li $v0, 4
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	la $a0, $str_4
	li $v0, 4
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	li $v0, 12
	syscall
	move $s0, $v0
	li $t2, 49
	bne $s0, $t2, label_40
	la $a0, $str_5
	li $v0, 4
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	li $t2, 5
	addi $t0, $gp, 204
	li $t1, 0
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 7
	addi $t0, $gp, 204
	li $t1, 1
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 9
	addi $t0, $gp, 204
	li $t1, 2
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 4
	addi $t0, $gp, 204
	li $t1, 3
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1
	addi $t0, $gp, 204
	li $t1, 4
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 3
	addi $t0, $gp, 204
	li $t1, 5
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 8
	addi $t0, $gp, 204
	li $t1, 6
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 10
	addi $t0, $gp, 204
	li $t1, 7
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 2
	addi $t0, $gp, 204
	li $t1, 8
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 6
	addi $t0, $gp, 204
	li $t1, 9
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1
	addi $t0, $gp, 324
	li $t1, 0
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 2
	addi $t0, $gp, 324
	li $t1, 1
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 3
	addi $t0, $gp, 324
	li $t1, 2
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 4
	addi $t0, $gp, 324
	li $t1, 3
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 5
	addi $t0, $gp, 324
	li $t1, 4
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 6
	addi $t0, $gp, 324
	li $t1, 5
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 7
	addi $t0, $gp, 324
	li $t1, 6
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 8
	addi $t0, $gp, 324
	li $t1, 7
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 9
	addi $t0, $gp, 324
	li $t1, 8
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 10
	addi $t0, $gp, 324
	li $t1, 9
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	j label_39
label_40:
	li $t2, 50
	bne $s0, $t2, label_41
	la $a0, $str_6
	li $v0, 4
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	lw $t1, 444($gp)
	li $t2, 5
	add $t0, $t1, $t2
	sw $t0, -12($fp)
	lw $t0, -12($fp)
	sw $t0, 444($gp)
	li $t2, 45
	addi $t0, $gp, 204
	li $t1, 0
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 19
	addi $t0, $gp, 204
	li $t1, 1
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 23
	addi $t0, $gp, 204
	li $t1, 2
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 78
	addi $t0, $gp, 204
	li $t1, 3
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 83
	addi $t0, $gp, 204
	li $t1, 4
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 55
	addi $t0, $gp, 204
	li $t1, 5
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 69
	addi $t0, $gp, 204
	li $t1, 6
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 37
	addi $t0, $gp, 204
	li $t1, 7
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 95
	addi $t0, $gp, 204
	li $t1, 8
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 77
	addi $t0, $gp, 204
	li $t1, 9
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 145
	addi $t0, $gp, 204
	li $t1, 10
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 189
	addi $t0, $gp, 204
	li $t1, 11
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 156
	addi $t0, $gp, 204
	li $t1, 12
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 123
	addi $t0, $gp, 204
	li $t1, 13
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 100
	addi $t0, $gp, 204
	li $t1, 14
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 19
	addi $t0, $gp, 324
	li $t1, 0
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 23
	addi $t0, $gp, 324
	li $t1, 1
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 37
	addi $t0, $gp, 324
	li $t1, 2
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 45
	addi $t0, $gp, 324
	li $t1, 3
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 55
	addi $t0, $gp, 324
	li $t1, 4
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 69
	addi $t0, $gp, 324
	li $t1, 5
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 77
	addi $t0, $gp, 324
	li $t1, 6
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 78
	addi $t0, $gp, 324
	li $t1, 7
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 83
	addi $t0, $gp, 324
	li $t1, 8
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 95
	addi $t0, $gp, 324
	li $t1, 9
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 100
	addi $t0, $gp, 324
	li $t1, 10
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 123
	addi $t0, $gp, 324
	li $t1, 11
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 145
	addi $t0, $gp, 324
	li $t1, 12
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 156
	addi $t0, $gp, 324
	li $t1, 13
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 189
	addi $t0, $gp, 324
	li $t1, 14
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	j label_39
label_41:
	li $t2, 51
	bne $s0, $t2, label_42
	la $a0, $str_7
	li $v0, 4
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	lw $t1, 444($gp)
	li $t2, 10
	add $t0, $t1, $t2
	sw $t0, -16($fp)
	lw $t0, -16($fp)
	sw $t0, 444($gp)
	li $t2, 120
	addi $t0, $gp, 204
	li $t1, 0
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 854
	addi $t0, $gp, 204
	li $t1, 1
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 165
	addi $t0, $gp, 204
	li $t1, 2
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 785
	addi $t0, $gp, 204
	li $t1, 3
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 666
	addi $t0, $gp, 204
	li $t1, 4
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 475
	addi $t0, $gp, 204
	li $t1, 5
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 324
	addi $t0, $gp, 204
	li $t1, 6
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 218
	addi $t0, $gp, 204
	li $t1, 7
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 165
	addi $t0, $gp, 204
	li $t1, 8
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 994
	addi $t0, $gp, 204
	li $t1, 9
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1230
	addi $t0, $gp, 204
	li $t1, 10
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1542
	addi $t0, $gp, 204
	li $t1, 11
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1462
	addi $t0, $gp, 204
	li $t1, 12
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1864
	addi $t0, $gp, 204
	li $t1, 13
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1023
	addi $t0, $gp, 204
	li $t1, 14
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1000
	addi $t0, $gp, 204
	li $t1, 15
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1765
	addi $t0, $gp, 204
	li $t1, 16
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1952
	addi $t0, $gp, 204
	li $t1, 17
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1949
	addi $t0, $gp, 204
	li $t1, 18
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1764
	addi $t0, $gp, 204
	li $t1, 19
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 120
	addi $t0, $gp, 324
	li $t1, 0
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 165
	addi $t0, $gp, 324
	li $t1, 1
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 165
	addi $t0, $gp, 324
	li $t1, 2
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 218
	addi $t0, $gp, 324
	li $t1, 3
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 324
	addi $t0, $gp, 324
	li $t1, 4
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 475
	addi $t0, $gp, 324
	li $t1, 5
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 666
	addi $t0, $gp, 324
	li $t1, 6
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 785
	addi $t0, $gp, 324
	li $t1, 7
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 854
	addi $t0, $gp, 324
	li $t1, 8
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 994
	addi $t0, $gp, 324
	li $t1, 9
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1000
	addi $t0, $gp, 324
	li $t1, 10
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1023
	addi $t0, $gp, 324
	li $t1, 11
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1230
	addi $t0, $gp, 324
	li $t1, 12
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1462
	addi $t0, $gp, 324
	li $t1, 13
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1542
	addi $t0, $gp, 324
	li $t1, 14
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1764
	addi $t0, $gp, 324
	li $t1, 15
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1765
	addi $t0, $gp, 324
	li $t1, 16
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1864
	addi $t0, $gp, 324
	li $t1, 17
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1949
	addi $t0, $gp, 324
	li $t1, 18
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 1952
	addi $t0, $gp, 324
	li $t1, 19
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	j label_39
label_42:
label_39:
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
other_test2:
	subi $sp, $sp, 4
	sw $ra, ($sp)
	subi $sp, $sp, 76
	li $t2, 83
	addi $t0, $gp, 4
	li $t1, 0
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 97
	addi $t0, $gp, 4
	li $t1, 1
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 97
	addi $t0, $gp, 4
	li $t1, 2
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 97
	addi $t0, $gp, 4
	li $t1, 3
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 97
	addi $t0, $gp, 4
	li $t1, 4
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 97
	addi $t0, $gp, 4
	li $t1, 5
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 98
	addi $t0, $gp, 4
	li $t1, 6
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 98
	addi $t0, $gp, 4
	li $t1, 7
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 97
	addi $t0, $gp, 4
	li $t1, 8
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 98
	addi $t0, $gp, 4
	li $t1, 9
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 98
	addi $t0, $gp, 4
	li $t1, 10
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 69
	addi $t0, $gp, 4
	li $t1, 11
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t2, 69
	addi $t0, $gp, 4
	li $t1, 12
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	li $t0, 12
	sw $t0, -12($sp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal reverse
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal charcount
	sw $v1, -16($fp)
	la $a0, $str_8
	li $v0, 4
	syscall
	li $v0, 1
	lw $a0, -16($fp)
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	li $t0, 10
	sw $t0, 444($gp)
	li $t0, 10
	sw $t0, 448($gp)
	li $t0, 10
	sw $t0, 452($gp)
	li $t0, 10
	sw $t0, 456($gp)
	li $t2, 0
	addi $t0, $gp, 0
	li $t1, 0
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	sw $t2, ($t0)
	lw $t1, 448($gp)
	lw $t2, 452($gp)
	mul $t0, $t1, $t2
	sw $t0, -20($fp)
	lw $t1, 444($gp)
	lw $t2, -20($fp)
	add $t0, $t1, $t2
	sw $t0, -24($fp)
	lw $t1, -24($fp)
	lw $t2, 456($gp)
	add $t0, $t1, $t2
	sw $t0, -28($fp)
	lw $t1, 444($gp)
	lw $t2, 456($gp)
	add $t0, $t1, $t2
	sw $t0, -32($fp)
	lw $t1, -20($fp)
	lw $t2, -32($fp)
	add $t0, $t1, $t2
	sw $t0, -36($fp)
	lw $t1, -36($fp)
	li $t2, 88
	add $t0, $t1, $t2
	sw $t0, -40($fp)
	lw $t1, -28($fp)
	lw $t2, -40($fp)
	div $t0, $t1, $t2
	sw $t0, -44($fp)
	lw $t0, 444($gp)
	sw $t0, -12($sp)
	lw $t0, 448($gp)
	sw $t0, -16($sp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal max
	sw $v1, -48($fp)
	lw $t1, -44($fp)
	lw $t2, -48($fp)
	add $t0, $t1, $t2
	sw $t0, -52($fp)
	addi $t0, $gp, 0
	li $t1, 0
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -56($fp)
	addi $t0, $gp, 4
	lw $t1, -56($fp)
	sll $t1, $t1, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -60($fp)
	lw $t1, -52($fp)
	lw $t2, -60($fp)
	sub $t0, $t1, $t2
	sw $t0, -64($fp)
	lw $t1, -64($fp)
	li $t2, 72
	add $t0, $t1, $t2
	sw $t0, -68($fp)
	lw $t1, -68($fp)
	li $t2, -70
	add $t0, $t1, $t2
	sw $t0, -72($fp)
	lw $t0, -72($fp)
	sw $t0, 444($gp)
	lw $t1, 444($gp)
	li $t2, -10
	add $t0, $t1, $t2
	sw $t0, -76($fp)
	lw $t0, -76($fp)
	sw $t0, 444($gp)
label_43:
	lw $t1, 444($gp)
	li $t2, 20
	blt $t1, $t2, label_44
	lw $t1, 444($gp)
	li $t2, -10
	add $t0, $t1, $t2
	sw $t0, -80($fp)
	lw $t0, -80($fp)
	sw $t0, 444($gp)
	j label_43
label_44:
	lw $t1, 444($gp)
	li $t2, 20
	bge $t1, $t2, label_45
	li $t0, 10
	sw $t0, 444($gp)
	j label_46
label_45:
label_46:
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	jr $ra
main:
	subi $sp, $sp, 4
	sw $ra, ($sp)
	subi $sp, $sp, 60
	sw $s3, -20($fp)
	sw $s1, -16($fp)
	sw $s0, -8($fp)
	sw $s2, -12($fp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal other_test2
	lw $s3, -20($fp)
	lw $s1, -16($fp)
	lw $s0, -8($fp)
	lw $s2, -12($fp)
	sw $s3, -20($fp)
	sw $s1, -16($fp)
	sw $s0, -8($fp)
	sw $s2, -12($fp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal other_test1
	lw $s3, -20($fp)
	lw $s1, -16($fp)
	lw $s0, -8($fp)
	lw $s2, -12($fp)
	li $v0, 1
	lw $a0, 444($gp)
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	lw $t0, 444($gp)
	sw $t0, -12($sp)
	sw $s3, -20($fp)
	sw $s1, -16($fp)
	sw $s0, -8($fp)
	sw $s2, -12($fp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal calcu
	sw $v1, -24($fp)
	lw $s3, -20($fp)
	lw $s1, -16($fp)
	lw $s0, -8($fp)
	lw $s2, -12($fp)
	lw $t0, -24($fp)
	move $s2, $t0
	lw $t2, 444($gp)
	div $t0, $s2, $t2
	sw $t0, -28($fp)
	lw $t0, -28($fp)
	move $s3, $t0
	lw $t0, 444($gp)
	sw $t0, -12($sp)
	sw $s3, -16($sp)
	sw $s3, -20($fp)
	sw $s1, -16($fp)
	sw $s0, -8($fp)
	sw $s2, -12($fp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal calcucov
	sw $v1, -32($fp)
	lw $s3, -20($fp)
	lw $s1, -16($fp)
	lw $s0, -8($fp)
	lw $s2, -12($fp)
	lw $t0, -32($fp)
	move $s1, $t0
	lw $t2, 444($gp)
	div $t0, $s1, $t2
	sw $t0, -36($fp)
	lw $t0, -36($fp)
	move $s1, $t0
	la $a0, $str_9
	li $v0, 4
	syscall
	li $v0, 1
	move $a0, $s2
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	la $a0, $str_10
	li $v0, 4
	syscall
	li $v0, 1
	move $a0, $s1
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	lw $t1, 444($gp)
	li $t2, 1
	sub $t0, $t1, $t2
	sw $t0, -40($fp)
	li $t0, 0
	sw $t0, -12($sp)
	lw $t0, -40($fp)
	sw $t0, -16($sp)
	sw $s3, -20($fp)
	sw $s1, -16($fp)
	sw $s0, -8($fp)
	sw $s2, -12($fp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal quick_sort
	lw $s3, -20($fp)
	lw $s1, -16($fp)
	lw $s0, -8($fp)
	lw $s2, -12($fp)
	li $t0, 0
	move $s0, $t0
	addi $t0, $gp, 204
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -44($fp)
	addi $t0, $gp, 324
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -48($fp)
	lw $t1, -44($fp)
	lw $t2, -48($fp)
	beq $t1, $t2, label_47
	la $a0, $str_11
	li $v0, 4
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	li $v0, 10
	syscall
	j label_48
label_47:
label_48:
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -52($fp)
	lw $t0, -52($fp)
	move $s0, $t0
label_49:
	lw $t2, 444($gp)
	bge $s0, $t2, label_50
	addi $t0, $gp, 204
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -56($fp)
	addi $t0, $gp, 324
	sll $t1, $s0, 2
	add $t0, $t0, $t1
	lw $t1, ($t0)
	sw $t1, -60($fp)
	lw $t1, -56($fp)
	lw $t2, -60($fp)
	beq $t1, $t2, label_51
	la $a0, $str_12
	li $v0, 4
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	li $v0, 10
	syscall
	j label_52
label_51:
label_52:
	li $t2, 1
	add $t0, $s0, $t2
	sw $t0, -64($fp)
	lw $t0, -64($fp)
	move $s0, $t0
	j label_49
label_50:
	la $a0, $str_13
	li $v0, 4
	syscall
	li $v0, 11
	li $a0, 10
	syscall
	sw $s3, -20($fp)
	sw $s1, -16($fp)
	sw $s0, -8($fp)
	sw $s2, -12($fp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal fun_def_test
	lw $s3, -20($fp)
	lw $s1, -16($fp)
	lw $s0, -8($fp)
	lw $s2, -12($fp)
	sw $s3, -20($fp)
	sw $s1, -16($fp)
	sw $s0, -8($fp)
	sw $s2, -12($fp)
	subi $sp, $sp, 4
	sw $fp, ($sp)
	move $fp, $sp
	jal defi_test
	lw $s3, -20($fp)
	lw $s1, -16($fp)
	lw $s0, -8($fp)
	lw $s2, -12($fp)
	lw $ra, -4($fp)
	addi $sp, $fp, 4
	lw $fp, ($fp)
	li $v0, 10
	syscall
	li $v0, 10
	syscall
