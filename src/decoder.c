#include <stdio.h>

#include "decoder.h"
#include "instructions.h"


bool decode(uint16_t instruction) {
  const uint8_t leading = instruction >> 12;
  const uint16_t addr = (instruction & 0x0FFF);
  const uint8_t vx = (instruction & 0x0F00) >> 8;
  const uint8_t vy = (instruction & 0x00F0) >> 4;
  const uint8_t n = addr & 0xF;
  const uint8_t kk = addr & 0xFF;

  if (debugger) {
    printf(
        "instruction: %04X leading: %X addr: %03X x: %X y: %X n: %X kk: %02X\n",
        instruction, leading, addr, vx, vy, n, kk);
  }

  switch (leading) {
  case 0x0:
    switch (instruction) {
    case 0x00E0: inst_cls(); break;
    case 0x00EE: inst_ret(); break;
    default: inst_sys(addr); break;
    }
    break;
  case 0x1: inst_jp(addr); break;
  case 0x2: inst_call(addr); break;
  case 0x3: inst_se_vx_byte(vx, kk); break;
  case 0x4: inst_sne_vx_byte(vx, kk); break;
  case 0x5: inst_se_vx_vy(vx, vy); break;
  case 0x6: inst_ld_vx_byte(vx, kk); break;
  case 0x7: inst_add_vx_byte(vx, kk); break;
  case 0x8:
    switch (n) {
    case 0x0: inst_ld_vx_vy(vx, vy); break;
    case 0x1: inst_or_vx_vy(vx, vy); break;
    case 0x2: inst_and_vx_vy(vx, vy); break;
    case 0x3: inst_xor_vx_vy(vx, vy); break;
    case 0x4: inst_add_vx_vy(vx, vy); break;
    case 0x5: inst_sub_vx_vy(vx, vy); break;
    case 0x6: inst_shr_vx(vx); break;
    case 0x7: inst_subn_vx_vy(vx, vy); break;
    case 0xE: inst_shl_vx(vx); break;
    default: return false;
    }
    break;
  case 0x9: inst_sne_vx_vy(vx, vy); break;
  case 0xA: inst_ld_i(addr); break;
  case 0xB: inst_jp_v0(addr); break;
  case 0xC: inst_rnd(vx, kk); break;
  case 0xD: inst_drw(vx, vy, n); break;
  case 0xE:
    switch (kk) {
    case 0x9E: inst_skp(vx); break;
    case 0xA1: inst_sknp(vx); break;
    default: return false;
    }
    break;
  case 0xF:
    switch (kk) {
    case 0x07: inst_ld_vx_dt(vx); break;
    case 0x0A: inst_ld_vx_k(vx); break;
    case 0x15: inst_ld_dt_vx(vx); break;
    case 0x18: inst_ld_st_vx(vx); break;
    case 0x1E: inst_add_i_vx(vx); break;
    case 0x29: inst_ld_f_vx(vx); break;
    case 0x33: inst_ld_b_vx(vx); break;
    case 0x55: inst_ld_i_vx(vx); break;
    case 0x65: inst_ld_vx_i(vx); break;
    default: return false;
    }
    break;
  default: return false;
  }

  return true;
}
