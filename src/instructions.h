#pragma once

#include "common.h"
#include <stdint.h>

/* Clear screen */
void inst_cls();
/* Return */
void inst_ret();
/* Sys call */
void inst_sys(uint16_t addr);
/* Jump to location nnn. */
void inst_jp(uint16_t addr);
/* Call subroutine at nnn. */
void inst_call(uint16_t addr);
/* Skip next instruction if Vx = kk. */
void inst_se_vx_byte(uint8_t vx, uint8_t byte);
/* Skip next instruction if Vx != kk. */
void inst_sne_vx_byte(uint8_t vx, uint8_t byte);
/* Skip next instruction if Vx = Vy. */
void inst_se_vx_vy(uint8_t vx, uint8_t vy);
/* Set Vx = kk. */
void inst_ld_vx_byte(uint8_t vx, uint8_t byte);
/* Set Vx = Vx + kk. */
void inst_add_vx_byte(uint8_t vx, uint8_t byte);
/* Set Vx = Vy. */
void inst_ld_vx_vy(uint8_t vx, uint8_t vy);
/* Set Vx = Vx OR Vy. */
void inst_or_vx_vy(uint8_t vx, uint8_t vy);
/* Set Vx = Vx AND Vy. */
void inst_and_vx_vy(uint8_t vx, uint8_t vy);
/* Set Vx = Vx XOR Vy. */
void inst_xor_vx_vy(uint8_t vx, uint8_t vy);
/* Set Vx = Vx + Vy, set VF = carry. */
void inst_add_vx_vy(uint8_t vx, uint8_t vy);
/* Set Vx = Vx - Vy, set VF = NOT borrow. */
void inst_sub_vx_vy(uint8_t vx, uint8_t vy);
/* Set Vx = Vx SHR 1. */
void inst_shr_vx(uint8_t vx);
/* Set Vx = Vy - Vx, set VF = NOT borrow. */
void inst_subn_vx_vy(uint8_t vx, uint8_t vy);
/* Set Vx = Vx SHL 1. */
void inst_shl_vx(uint8_t vx);
/* Skip next instruction if Vx != Vy. */
void inst_sne_vx_vy(uint8_t vx, uint8_t vy);
/* Set I = nnn. */
void inst_ld_i(uint16_t addr);
/* Jump to location nnn + V0. */
void inst_jp_v0(uint16_t addr);
/* Set Vx = random byte AND kk. */
void inst_rnd(uint8_t vx, uint8_t byte);
/* Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision. */
void inst_drw(uint8_t vx, uint8_t vy, uint8_t n);
/* Skip next instruction if key with the value of Vx is pressed. */
void inst_skp(uint8_t vx);
/* Skip next instruction if key with the value of Vx is not pressed. */
void inst_sknp(uint8_t vx);
/* Set Vx = delay timer value. */
void inst_ld_vx_dt(uint8_t vx);
/* Wait for a key press, store the value of the key in Vx. */
void inst_ld_vx_k(uint8_t vx);
/* Set delay timer = Vx. */
void inst_ld_dt_vx(uint8_t vx);
/* Set sound timer = Vx. */
void inst_ld_st_vx(uint8_t vx);
/* Set I = I + Vx. */
void inst_add_i_vx(uint8_t vx);
/* Set I = location of sprite for digit Vx. */
void inst_ld_f_vx(uint8_t vx);
/* Store BCD representation of Vx in memory locations I, I+1, and I+2 */
void inst_ld_b_vx(uint8_t vx);
/* Store registers V0 through Vx in memory starting at location I. */
void inst_ld_i_vx(uint8_t vx);
/* Read registers V0 through Vx from memory starting at location I. */
void inst_ld_vx_i(uint8_t vx);
