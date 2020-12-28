/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2020, Jason Hoyt
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 */

#pragma once

#include <stdint.h>

typedef uint8_t zrt_Bool;
typedef int64_t zrt_Int;
typedef double zrt_Real;

void zrt_init(int argc, char* argv[]);

typedef struct zrt_String {
	zrt_Int length;
	zrt_Int capacity;
	char* data;
} zrt_String;

zrt_String* zrt_str_empty();
zrt_String* zrt_str_new(const char* text);
zrt_String* zrt_str_new_from_int(zrt_Int value);
zrt_String* zrt_str_concat(zrt_String* lhs, zrt_String* rhs);
void zrt_str_copy(zrt_String* dst, zrt_String* src);
void zrt_str_del(zrt_String* str);

void zrt_println_bool(zrt_Bool arg);
void zrt_println_int(zrt_Int arg);
void zrt_println_real(zrt_Real arg);
void zrt_println_str(zrt_String* arg);
