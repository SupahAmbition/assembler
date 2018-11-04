		add		1		0		1 		Hello there obiwan kenobi
tag2	nand	1		0		1
		lw		1		0		100
		sw		1		0		200
branch 	beq		0		0		100
hello 	jalr	1		1
		noop
		halt
