// Store in program memory (text), create linker symbol which matches externed C symbol name
    .text
    .globl iir_asm_filt
    .type iir_asm_filt, %function

iir_asm_filt:
	// ADC SFR base address passed in r0, DAC SFR base address in r1
	// No need to push to stack since we will never be returning

/*
	REGISTER USAGE
	r0	ADC SFR base address
	r1	DAC SFR base address
	r2	Working register (used to check ADC status)
	r3	Current filter value
	r4	Current ADC value
	r5	| X(k-2) | X(k-1) | History as packed halfwords
	r6	247 (Y(k-1) weight), need lo for normal mul in thumb mode
	r7	EOC test mask (thumb instructions do not support immediate test)

	r8
	r9
	r10 832 (normalization divisor)
	r11	Weights | 260 | 200 |
*/

/*
********************** Register Initialization ***************************
*/

	mov r3, #0 // Current Filter Result in r3
	mov r5, #0 // r5 is history, r5b = X(k-1), r5t = X(k-2)

	mov r6, #247 // Y(k-1) weight

	mov r7, #2 // Test pattern for EOC flag ()

	movw r10, #832

	movw r11, #200 //  X(k-1) weight (10/13 with scaling of 260)
	movt r11, #260 //  X(k-2)/X(k) weight (1 with scaling of 260)

/*
************************** Main Filter Loop ******************************
*/

	wait_for_conv:
	ldr r2, [r0] // Get ADC status register
	tst r2, r7 // Test EOC flag
	beq wait_for_conv // beq looks for zero flag set (no data yet)

	ldr r4, [r0, #0x4C] // Read data register into r4

	// Do filter computation
	mul r3, r3, r6 // 			r3 *= 247					r3 = 247*Y(k-1)
	smlabt r3, r4, r11, r3 // 	r3 += r4*260				r3 = 247*Y(k-1) + 260*X(k)
	smlad r3, r5, r11, r3 // 	r3 += r5t*260 + r5b*200		r3 = 247*Y(k-1) + 260*X(k) + 260*X(k-2) + 200*X(k-1)
	udiv r3, r3, r10 // 		r3 /= 832					r3 = 19/64*Y(k-1) + 5/16*X(k) + 5/16*X(k-2) + 25/104*X(k-1)

	usat r3, #12, r3 // Saturate result to 12 bit
	str r3, [r1, #8] // Write result to DAC_DHR12R1 to trigger conversion

	pkhbt r5, r4, r5, lsl #16; // Shift history halfwords (r5b -> r5t, r4b -> r5b)

	b wait_for_conv
