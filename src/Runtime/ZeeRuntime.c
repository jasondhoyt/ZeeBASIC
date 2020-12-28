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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ZeeBasic/Runtime/ZeeRuntime.h"

void zrt_init(int argc, char* argv[])
{
	/* TODO */
}

zrt_String* zrt_str_empty()
{
	zrt_String* str = malloc(sizeof(zrt_String));
	if (!str) abort();

	str->capacity = 16;
	str->length = 0;
	str->data = malloc(sizeof(char) * str->capacity);
	if (!str->data) abort();
	
	return str;
}

zrt_String* zrt_str_new(const char* text)
{
	zrt_String* str = malloc(sizeof(zrt_String));
	if (!str) abort();

	zrt_Int cap = 16;
	zrt_Int len = strlen(text);
	while (cap < len)
	{
		cap *= 2;
	}

	str->capacity = cap;
	str->length = len;
	str->data = malloc(sizeof(char) * str->capacity);
	if (!str->data) abort();
	memcpy(str->data, text, len);

	return str;
}

zrt_String* zrt_str_new_from_int(zrt_Int value)
{
	char buf[16];
	snprintf(buf, sizeof(buf), "%lld", value);
	return zrt_str_new(buf);
}

zrt_String* zrt_str_concat(zrt_String* lhs, zrt_String* rhs)
{
	zrt_String* str = malloc(sizeof(zrt_String));
	if (!str) abort();

	zrt_Int cap = 16;
	zrt_Int len = lhs->length + rhs->length;
	while (cap < len)
	{
		cap *= 2;
	}

	str->capacity = cap;
	str->length = len;
	str->data = malloc(sizeof(char) * str->capacity);
	if (!str->data) abort();
	memcpy(str->data, lhs->data, lhs->length);
	memcpy(str->data + lhs->length, rhs->data, rhs->length);

	return str;
}

void zrt_str_copy(zrt_String* dst, zrt_String* src)
{
	if (dst->capacity < src->length)
	{
		while (dst->capacity < src->length)
		{
			dst->capacity *= 2;
		}

		free(dst->data);
		dst->data = malloc(sizeof(char) * dst->capacity);
		if (!dst->data) abort();
	}

	memcpy(dst->data, src->data, src->length);
	dst->length = src->length;
}

void zrt_str_del(zrt_String* str)
{
	free(str->data);
	free(str);
}

void zrt_println_bool(zrt_Bool arg)
{
	if (!arg)
	{
		printf("false\n");
	}
	else
	{
		printf("true\n");
	}
}

void zrt_println_int(zrt_Int arg)
{
	printf("%lld\n", arg);
}

void zrt_println_real(zrt_Real arg)
{
	printf("%f\n", arg);
}

void zrt_println_str(zrt_String* arg)
{
	fwrite(arg->data, sizeof(char), arg->length, stdout);
	fputc('\n', stdout);
}
