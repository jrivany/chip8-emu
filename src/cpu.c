#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "display.h"
#include "cpu.h"
#include "input.h"

#define LOG
#define MEM_SIZE 4096

static uint8_t V[16], SP, DT, ST;
static uint16_t I, PC;
static uint16_t stack[16];
static uint8_t mem[MEM_SIZE] = {
  // "0"
  0xF0, 0x90, 0x90, 0x90, 0xF0,
  // "1"
  0x20, 0x60, 0x20, 0x20, 0x70,
  // "2"
  0xF0, 0x10, 0xF0, 0x80, 0xF0,
  // "3"
  0xF0, 0x10, 0xF0, 0x10, 0xF0,
  // "4"
  0x90, 0x90, 0xF0, 0x10, 0x10,
  // "5"
  0xF0, 0x80, 0xF0, 0x10, 0xF0,
  // "6"
  0xF0, 0x80, 0xF0, 0x90, 0xF0,
  // "7"
  0xF0, 0x10, 0x20, 0x40, 0x40,
  // "8"
  0xF0, 0x90, 0xF0, 0x90, 0xF0,
  // "9"
  0xF0, 0x90, 0xF0, 0x10, 0xF0,
  // "A"
  0xF0, 0x90, 0xF0, 0x90, 0x90,
  // "B"
  0xE0, 0x90, 0xE0, 0x90, 0xE0,
  // "C"
  0xF0, 0x80, 0x80, 0x80, 0xF0,
  // "D"
  0xE0, 0x90, 0x90, 0x90, 0xE0,
  // "E"
  0xF0, 0x80, 0xF0, 0x80, 0xF0,
  // "F"
  0xF0, 0x80, 0xF0, 0x80, 0x80,
};

static uint8_t *key_dest;

static void reg_dump() {
  printf("registers: PC: %04X I: %04X SP: %02X DT: %02X ST: %02X\n", PC, I, SP, DT, ST);
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

void cpu_mem_dump() {
  FILE *fp = fopen("mem.dump", "wb");
  fwrite(mem, sizeof(uint8_t), MEM_SIZE, fp);
  fflush(fp);
  fclose(fp);
}

static void check_stack() {
  if (SP > 0xF) {
    fprintf(stderr, "Stack overflow\n");
    cpu_mem_dump();
    exit(1);
  }
}

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

bool cpu_tick() {
  if (PC > MEM_SIZE) {
    fprintf(stderr, "PC %X out of bounds!", PC);
    cpu_mem_dump();
    return false;
  }

  // if (PC == 0x450)
  //   return false;

  if (key_dest) {
    int8_t next = key_next();
    if (next >= 0) {
      *key_dest = next;
      key_dest = NULL;
    } else {
      return true;
    }
  }

  const uint16_t instruction = (mem[PC] << 8) | mem[PC + 1];
  const uint8_t leading = instruction >> 12;
  const uint16_t addr = (instruction  & 0x0FFF);
  const uint8_t x = (instruction & 0x0F00) >> 8;
  const uint8_t y = (instruction & 0x00F0) >> 4;
  const uint8_t n = addr & 0xF;
  const uint8_t kk = addr & 0xFF;
  const uint16_t sum =  V[x] + V[y];
  #ifdef LOG
  printf("instruction: %04X leading: %X addr: %03X x: %X y: %X n: %X kk: %02X\n",
          instruction, leading, addr, x, y, n, kk);
  #endif
  PC += 2;
  reg_dump();
  stack_dump();
  switch (leading) {
    case 0x0:
      switch (instruction) {
        case 0x00E0:
          display_clear();
          break; // clear screen
        case 0x00EE: // return
          --SP;
          check_stack();
          PC = stack[SP];
          break;
        default: break; // call - not needed
      }
      break;
    case 0x1: // Jump to location nnn.
      PC = addr;
      break;
    case 0x2: // Call subroutine at nnn.
      stack[SP] = PC;
      ++SP;
      check_stack();
      PC = addr;
      break;
    case 0x3: // Skip next instruction if Vx = kk.
      PC += (V[x] == kk) ? 2 : 0;
      break;
    case 0x4: // Skip next instruction if Vx != kk.
      PC += (V[x] != kk) ? 2 : 0;
      break;
    case 0x5: // Skip next instruction if Vx = Vy.
      PC += (V[x] == V[y]) ? 2 : 0;
      break;
    case 0x6: // Set Vx = kk.
      V[x] = kk;
      break;
    case 0x7: // Set Vx = Vx + kk.
      V[x] += kk;
      break;
    case 0x8:
      switch (n) {
      case 0x0: // Set Vx = Vy.
        V[x] = V[y];
        break;
      case 0x1: // Set Vx = Vx OR Vy.
        V[x] |= V[y];
        break;
      case 0x2: // Set Vx = Vx AND Vy.
        V[x] &= V[y];
        break;
      case 0x3: // Set Vx = Vx XOR Vy.
        V[x] ^= V[y];
        break;
      case 0x4: // Set Vx = Vx + Vy, set VF = carry.
        V[x] = sum & 0xFF;
        V[0xF] = (0x100 & sum) >> 8;
        break;
      case 0x5: // Set Vx = Vx - Vy, set VF = NOT borrow.
        V[0xF] = V[x] > V[y] ? 1 : 0;
        V[x] -= V[y];
        break;
      case 0x6: // Set Vx = Vx SHR 1.
        V[0xF] = V[x] & 0x1;
        V[x] >>= 1;
        break;
      case 0x7: // Set Vx = Vy - Vx, set VF = NOT borrow.
        V[0xF] = V[y] > V[x] ? 1 : 0;
        V[x] = V[y] - V[x];
        break;
      case 0xE: // Set Vx = Vx SHL 1.
        V[0xF] = (V[x] & 0x80) >> 7;
        V[x] <<= 1;
        break;
      default:
        break;
      }
      break;
    case 0x9: // Skip next instruction if Vx != Vy.
      PC += (V[x] != V[y]) ? 2 : 0;
      break;
    case 0xA: // Set I = nnn.
      I = addr;
      break;
    case 0xB: // Jump to location nnn + V0.
      PC = addr + V[0];
      break;
    case 0xC: // Set Vx = random byte AND kk.
      V[x] = (rand()/((RAND_MAX + 1u)/256)) & kk;
      break;
    case 0xD: // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
      V[0xF] = display_sprite(V[x], V[y], &mem[I], n);
      // return false;
      break;
    case 0xE:
      switch (kk) {
      case 0x9E: // Skip next instruction if key with the value of Vx is pressed.
        if (key_check(V[x]))
          PC += 2;
        break;
      case 0xA1: // Skip next instruction if key with the value of Vx is not pressed.
        if (!key_check(V[x]))
          PC += 2;
        break;
      default:
        break;
      }
      break;
    case 0xF:
      switch (kk) {
      case 0x07: // Set Vx = delay timer value.
        V[x] = DT;
        break;
      case 0x0A: // Wait for a key press, store the value of the key in Vx.
        key_dest = &V[x];
        break;
      case 0x15: // Set delay timer = Vx.
        DT = V[x];
        break;
      case 0x18: // Set sound timer = Vx.
        ST = V[x];
        break;
      case 0x1E: // Set I = I + Vx.
        I += V[x];
        break;
      case 0x29: // Set I = location of sprite for digit Vx.
        I = 5 * V[x];
        break;
      case 0x33: // Store BCD representation of Vx in memory locations I, I+1, and I+2.
        mem[I] = V[x] / 100;
        mem[I + 1] = (V[x] % 100)/ 10;
        mem[I + 2] = V[x] % 10;
        break;
      case 0x55: // Store registers V0 through Vx in memory starting at location I.
        memcpy(&mem[I], V, x + 1);
        break;
      case 0x65: // Read registers V0 through Vx from memory starting at location I.
        memcpy(V, &mem[I],  x + 1);
        break;
      default:
        break;
      }
      break;
    default: break;
  }

  return true;
}

