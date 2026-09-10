/*
  VitaShell
  Copyright (C) 2015-2018, TheFloW

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <psp2kern/kernel/modulemgr.h>
#include <psp2kern/io/fcntl.h>
#include <psp2kern/udcd.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <taihen.h>

static tai_hook_ref_t ksceIoOpenRef;
static tai_hook_ref_t ksceIoReadRef;

static SceUID hooks[3];

static int first = 1;

#undef TAI_CONTINUE
#define TAI_CONTINUE_OPEN(hook, ...) (((SceUID (*)(const char *, int, SceMode))(hook))(__VA_ARGS__))
#define TAI_CONTINUE_READ(hook, ...) (((int (*)(SceUID, void *, SceSize))(hook))(__VA_ARGS__))

static SceUID ksceIoOpenPatched(const char *file, int flags, SceMode mode) {
  first = 1;

  SceUID fd = TAI_CONTINUE_OPEN(ksceIoOpenRef, file, flags, mode);

  if (fd == 0x800F090D)
    return TAI_CONTINUE_OPEN(ksceIoOpenRef, file, flags & ~SCE_O_WRONLY, mode);

  return fd;
}

static int ksceIoReadPatched(SceUID fd, void *data, SceSize size) {
  int res = TAI_CONTINUE_READ(ksceIoReadRef, fd, data, size);

  if (first) {
    first = 0;

    // Manipulate boot sector to support exFAT[cite: 1, 4]
    if (memcmp((uint8_t *)data + 0x3, "EXFAT", 5) == 0) {
      // Sector size[cite: 1, 4]
      *(uint16_t *)((uint8_t *)data + 0xB) = 1 << *(uint8_t *)((uint8_t *)data + 0x6C);

      // Volume size[cite: 1, 4]
      *(uint32_t *)((uint8_t *)data + 0x20) = *(uint32_t *)((uint8_t *)data + 0x48);
    }
  }

  return res;
}

int _start(SceSize args, void *argp) __attribute__ ((weak, alias("module_start")));
int module_start(SceSize args, void *argp) {
  // Get tai module info[cite: 1, 4]
  tai_module_info_t info;
  info.size = sizeof(tai_module_info_t);
  if (taiGetModuleInfoForKernel(KERNEL_PID, "SceUsbstorVStorDriver", &info) < 0)
    return SCE_KERNEL_START_SUCCESS;

  // Remove image path limitation[cite: 1, 4]
  char zero[0x6E];
  memset(zero, 0, 0x6E);
  hooks[0] = taiInjectDataForKernel(KERNEL_PID, info.modid, 0, 0x1738, zero, 0x6E);

  // Add patches to support exFAT[cite: 1, 4]
  hooks[1] = taiHookFunctionImportForKernel(KERNEL_PID, &ksceIoOpenRef, "SceUsbstorVStorDriver",
                                            0x40FD29C7, 0x75192972, ksceIoOpenPatched);
  hooks[2] = taiHookFunctionImportForKernel(KERNEL_PID, &ksceIoReadRef, "SceUsbstorVStorDriver",
                                            0x40FD29C7, 0xE17EFC03, ksceIoReadPatched);

  ksceUdcdStopCurrentInternal(2);

  return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize args, void *argp) {
  if (hooks[2] >= 0)
    taiHookReleaseForKernel(hooks[2], ksceIoReadRef);
  if (hooks[1] >= 0)
    taiHookReleaseForKernel(hooks[1], ksceIoOpenRef);
  if (hooks[0] >= 0)
    taiInjectReleaseForKernel(hooks[0]);

  return SCE_KERNEL_STOP_SUCCESS;
}