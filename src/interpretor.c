#include "common.h"
#include "display.h"
#include "input.h"
#include "instructions.h"
#include "decoder.h"
#include <stdio.h>

#define MEM_SIZE 4096

static uint8_t V[16], SP, DT, ST;
static uint16_t I, PC;
static uint16_t stack[16];
static uint8_t mem[MEM_SIZE] = {
    /* Hex sprite storage: */
    /* "0" */ 0xF0, 0x90, 0x90, 0x90, 0xF0,
    /* "1" */ 0x20, 0x60, 0x20, 0x20, 0x70,
    /* "2" */ 0xF0, 0x10, 0xF0, 0x80, 0xF0,
    /* "3" */ 0xF0, 0x10, 0xF0, 0x10, 0xF0,
    /* "4" */ 0x90, 0x90, 0xF0, 0x10, 0x10,
    /* "5" */ 0xF0, 0x80, 0xF0, 0x10, 0xF0,
    /* "6" */ 0xF0, 0x80, 0xF0, 0x90, 0xF0,
    /* "7" */ 0xF0, 0x10, 0x20, 0x40, 0x40,
    /* "8" */ 0xF0, 0x90, 0xF0, 0x90, 0xF0,
    /* "9" */ 0xF0, 0x90, 0xF0, 0x10, 0xF0,
    /* "A" */ 0xF0, 0x90, 0xF0, 0x90, 0x90,
    /* "B" */ 0xE0, 0x90, 0xE0, 0x90, 0xE0,
    /* "C" */ 0xF0, 0x80, 0x80, 0x80, 0xF0,
    /* "D" */ 0xE0, 0x90, 0x90, 0x90, 0xE0,
    /* "E" */ 0xF0, 0x80, 0xF0, 0x80, 0xF0,
    /* "F" */ 0xF0, 0x80, 0xF0, 0x80, 0x80,
};

static uint8_t *key_dest = NULL;
static bool halted = false;

void cpu_timer_tick() {
  if (DT > 0)
    --DT;
  if (ST > 0) {
    --ST;
    if (!ST)
      puts("BEEEP!!\n");
  }
}

void cpu_load_rom(const uint8_t *prog, const size_t len) {
  memcpy(&mem[0x200], prog, len);
  PC = 0x0200;
}

void cpu_mem_dump() {
  FILE *fp = fopen("mem.dump", "wb");
  fwrite(mem, sizeof(uint8_t), MEM_SIZE, fp);
  fflush(fp);
  fclose(fp);
}

static void reg_dump() {
  printf("registers: PC: %04X I: %04X SP: %02X DT: %02X ST: %02X\n", PC, I, SP,
         DT, ST);
  for (int i = 0; i < 16; ++i)
    printf("V%X: %02X ", i, V[i]);
  puts("\n");
}

static void stack_dump() {
  puts("stack:");
  for (int i = 0; i < 16; ++i)
    printf(" %X: %04X", i, stack[i]);
  puts("\n");
}

static void crash_dump() {
  reg_dump();
  stack_dump();
  cpu_mem_dump();
  exit(1);
}

static void check_stack() {
  if (SP > 0xF) {
    fprintf(stderr, "Stack overflow\n");
    crash_dump();
  }
}

bool cpu_tick() {
  if (PC > MEM_SIZE) {
    fprintf(stderr, "PC %X out of bounds!", PC);
    crash_dump();
  }

  if (key_dest) {
    int8_t next = key_next();
    if (next >= 0) {
      *key_dest = next;
      key_dest = NULL;
      halted = false;
    } else {
      return true;
    }
  }

  const uint16_t inst = (mem[PC] << 8) | mem[PC + 1];
  PC += 2;
  if (debugger) {
    reg_dump();
    stack_dump();
  }
  if (!decode(inst)) {
    fprintf(stderr, "Unable to decode instruction: %04X\n", inst);
    crash_dump();
  }

  return !halted;
}

// Instruction.h implementations:

void inst_cls() { display_clear(); }

void inst_ret() {
  --SP;
  check_stack();
  PC = stack[SP];
}

void inst_sys(uint16_t addr) { printf("SYS: %04X\n", addr); }

void inst_jp(uint16_t addr) { PC = addr; }

void inst_call(uint16_t addr) {
  stack[SP] = PC;
  ++SP;
  check_stack();
  PC = addr;
}
void inst_se_vx_byte(uint8_t vx, uint8_t byte) {
  PC += (V[vx] == byte) ? 2 : 0;
}
void inst_sne_vx_byte(uint8_t vx, uint8_t byte) {
  PC += (V[vx] != byte) ? 2 : 0;
}
void inst_se_vx_vy(uint8_t vx, uint8_t vy) { PC += (V[vx] == V[vy]) ? 2 : 0; }
void inst_ld_vx_byte(uint8_t vx, uint8_t byte) { V[vx] = byte; }
void inst_add_vx_byte(uint8_t vx, uint8_t byte) { V[vx] += byte; }
void inst_ld_vx_vy(uint8_t vx, uint8_t vy) { V[vx] = V[vy]; }
void inst_or_vx_vy(uint8_t vx, uint8_t vy) { V[vx] |= V[vy]; }
void inst_and_vx_vy(uint8_t vx, uint8_t vy) { V[vx] &= V[vy]; }
void inst_xor_vx_vy(uint8_t vx, uint8_t vy) { V[vx] ^= V[vy]; }
void inst_add_vx_vy(uint8_t vx, uint8_t vy) {
  const uint16_t sum = V[vx] + V[vy];
  V[vx] = sum & 0xFF;
  V[0xF] = (0x100 & sum) >> 8;
}

void inst_sub_vx_vy(uint8_t vx, uint8_t vy) {
  V[0xF] = V[vx] > V[vy] ? 1 : 0;
  V[vx] -= V[vy];
}

void inst_shr_vx(uint8_t vx) {
  V[0xF] = V[vx] & 0x1;
  V[vx] >>= 1;
}


void inst_subn_vx_vy(uint8_t vx, uint8_t vy) {
  V[0xF] = V[vy] > V[vx] ? 1 : 0;
  V[vx] = V[vy] - V[vx];
}
void inst_shl_vx(uint8_t vx) {
  V[0xF] = (V[vx] & 0x80) >> 7;
  V[vx] <<= 1;
}
void inst_sne_vx_vy(uint8_t vx, uint8_t vy) { PC += (V[vx] != V[vy]) ? 2 : 0; }
void inst_ld_i(uint16_t addr) { I = addr; }
void inst_jp_v0(uint16_t addr) { PC = addr + V[0]; }
void inst_rnd(uint8_t vx, uint8_t byte) {
  V[vx] = (rand() / ((RAND_MAX + 1u) / 256)) & byte;
}
void inst_drw(uint8_t vx, uint8_t vy, uint8_t n) {
  V[0xF] = display_sprite(V[vx], V[vy], &mem[I], n);
}
void inst_skp(uint8_t vx) {
  if (key_check(V[vx]))
    PC += 2;
}
void inst_sknp(uint8_t vx) {
  if (!key_check(V[vx]))
    PC += 2;
}
void inst_ld_vx_dt(uint8_t vx) { V[vx] = DT; }
void inst_ld_vx_k(uint8_t vx) {
  key_dest = &V[vx];
  halted = true;
}

void inst_ld_dt_vx(uint8_t vx) { DT = V[vx]; }
void inst_ld_st_vx(uint8_t vx) { ST = V[vx]; }
void inst_add_i_vx(uint8_t vx) { I += V[vx]; }
void inst_ld_f_vx(uint8_t vx) { I = 5 * V[vx]; }
void inst_ld_b_vx(uint8_t vx) {
  mem[I] = V[vx] / 100;
  mem[I + 1] = (V[vx] % 100) / 10;
  mem[I + 2] = V[vx] % 10;
}
void inst_ld_i_vx(uint8_t vx) { memcpy(&mem[I], V, vx + 1); }
void inst_ld_vx_i(uint8_t vx) { memcpy(V, &mem[I], vx + 1); }
