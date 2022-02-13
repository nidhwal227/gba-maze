.GLOBL drawPlayer
drawPlayer:
stmfd   sp!, {r4-r11, lr}

mov r4, #0x7000000 //stores sprite memory adress

add r4, r4, r0, lsl #3 //increments adress based on sprite number
mov r5, r1
mov r6, r2
mov r7, #24
orr r6, r6, #0x2000
orr r5, r5, #0x4000
strh r6, [r4, #0] //stores y position
strh r5, [r4, #2]	//stores x position and size
strh r7, [r4, #4] //stores sprite number

ldmfd   sp!, {r4-r11, lr}
mov     pc, lr	
