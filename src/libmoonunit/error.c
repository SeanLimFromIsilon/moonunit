/*
 * Copyright (c) 2007, Brian Koropoff
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Moonunit project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY BRIAN KOROPOFF ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL BRIAN KOROPOFF BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <moonunit/error.h>
#include <moonunit/private/util.h>

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

static MuError error_mem =
{
    .code = MU_ERROR_MEMORY,
    .message = "Out of memory"
};

void
mu_error_raise(MuError** err, MuStatusCode code, const char* format, ...)
{
    va_list ap;

    if (!err)
        return;

    if (*err)
    {
        fprintf(stderr, "Error overwritten by raise!  Aborting...\n");
        abort();
    }

    *err = xmalloc(sizeof(MuError));

    if (!err)
    {
        *err = &error_mem;
        return;
    }

    (*err)->code = code;

    va_start(ap, format);

    (*err)->message = formatv(format, ap);

    va_end(ap);

    if (!(*err)->message)
    {
        free(*err);
        *err = &error_mem;
        return;
    }

    return;
}

void
mu_error_handle(MuError** err)
{
    if (!err)
        return;

    if (*err != &error_mem)
    {
        free((*err)->message);
        free(*err);
    }

    *err = NULL;
}

void
mu_error_reraise(MuError** err, MuError* src)
{
    if (err)
    {
        *err = src;
    }
    else
    {
        mu_error_handle(&src);
    }
}

bool
mu_error_equal(MuError* err, int code)
{
    return err && err->code == code;
}
