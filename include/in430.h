/* ============================================================================
 * Copyright (c) 2011, Peter A. Bigot (pabigot@users.sourceforge.net)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  *  Neither the name of the copyright owner nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ============================================================================
 */

/* Provide declarations for builtins (intrinsics) supported by the
 * MSP430 backend for GCC */

#ifndef __MSP430_INTRINSICS_H_
#define __MSP430_INTRINSICS_H_

#if !defined(__ASSEMBLER__)

/* Insert a nop instruction */
void __nop (void);

/* Insert a dint instruction.  This disables maskable interrupts, such
 * as those from peripheral devices (multiplier, UART, I/O ports). */
void __dint (void);

/* Insert an eint instruction.  This enables maskable interrupts, such
   as those from peripheral devices. */
void __eint (void);

/* Return the current value of the status register (SR) (r2) */
unsigned int __read_status_register (void);

/* Write a new value into the status register (SR) (r2) */
void __write_status_register (unsigned int sr);

/* Return the current value of the stack pointer (SP) (r1) */
void *__read_stack_pointer (void);

/* Write a new value into the stack pointer (SP) (r1) */
void __write_stack_pointer (void *sp);

/* Clear the given bits in the status register (SR) */
void __bic_status_register (unsigned int bits);

/* Set the given bits in the status register (SR) */
void __bis_status_register (unsigned int bits);

/* Within an interrupt handler, clear the given bits in the value of
 * SR that will be popped off the stack on return. */
void __bic_status_register_on_exit (unsigned int bits);

/* Within an interrupt handler, set the given bits in the value of SR
 * that will be popped off the stack on return */
void __bis_status_register_on_exit (unsigned int bits);

/* GCC built-in to obtain frame address.  Parameter indicates nesting
 * depth; for non-zero values, null is returned */
void *__builtin_frame_address (int zero);

/* GCC built-in to obtain return address.  Parameter indicates nesting
 * depth; for non-zero values, null is returned */
void *__builtin_return_address (int zero);

/* Generates code to spin in place for exactly the given number of
 * machine cycles.  (Note: Prior to mspgcc 20110809, this was limited
 * to roughly 198000 cycles.  Subsequent mspgcc releases support
 * delays up to 2^31 cycles.) */
void __delay_cycles (unsigned long int delay);

/* Legacy compatible names */
#define nop() __nop()
#define _NOP() __nop()
#define READ_SP __read_stack_pointer();
#define WRITE_SP(_x) __write_stack_pointer(_x)
#define READ_SR __read_status_register()
#define WRITE_SR(_x) __write_status_register(_x)
#define _BIS_SR(_b) __bis_status_register(_b)
#define _BIC_SR(_b) __bic_status_register(_b)
#define _BIS_SR_IRQ(_b) __bis_status_register_on_exit(_b)
#define _BIC_SR_IRQ(_b) __bic_status_register_on_exit(_b)

/* IAR compatible aliases */
#define __no_operation() __nop()
#define __bic_SR_register(_b) __bic_status_register(_b)
#define __bic_SR_register_on_exit(_b) __bic_status_register_on_exit(_b)
#define __bis_SR_register(_b) __bis_status_register(_b)
#define __bis_SR_register_on_exit(_b) __bis_status_register_on_exit(_b)
#define __enable_interrupt() __eint()
#define __disable_interrupt() __dint()

#endif /* ! __ASSEMBLER__ */

#endif /* __MSP430_INTRINSICS_H_ */
