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

#include <moonunit/option.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void
Mu_Option_SetString(void* obj, MuOption* option, const char *name, const char* value)
{
    void* data;
    bool boolean;
    int integer;
    char* string;
    double fpoint;
    void* pointer;

    switch (option->type(obj, name))
    {
    case MU_BOOLEAN:
        boolean = !strcmp(value, "true");
        data = &boolean;
        break;
    case MU_INTEGER:
        integer = atoi(value);
        data = &integer;
        break;
    case MU_STRING:
        string = (char*) value;
        data = string;
        break;
    case MU_FLOAT:
        fpoint = atof(value);
        data = &fpoint;
        break;
    case MU_POINTER:
        pointer = (void*) value;
        data = pointer;
        break;
    default:
        data = NULL;
        break;
    }

    if (data)
        option->set(obj, name, data);
}

void
Mu_Option_Setv(void* obj, MuOption* option, const char *name, va_list ap)
{
    void* data;
    bool boolean;
    int integer;
    char* string;
    double fpoint;
    void* pointer;

    switch (option->type(obj, name))
    {
    case MU_BOOLEAN:
        boolean = va_arg(ap, int);
        data = &boolean;
        break;
    case MU_INTEGER:
        integer = va_arg(ap, int);
        data = &integer;
        break;
    case MU_STRING:
        string = va_arg(ap, char*);
        data = string;
        break;
    case MU_FLOAT:
        fpoint = va_arg(ap, double);
        data = &fpoint;
        break;
    case MU_POINTER:
        pointer = va_arg(ap, void*);
        data = pointer;
        break;
    default:
        data = NULL;
        break;
    }

    if (data)
        option->set(obj, name, data);
}

void
Mu_Option_Set(void* obj, MuOption* option, const char *name, ...)
{
    va_list ap;

    va_start(ap, name);

    Mu_Option_Setv(obj, option, name, ap);

    va_end(ap);
}


const void*
Mu_Option_Get(void* obj, MuOption* option, const char* name)
{
    return option->get(obj, name);
}

MuType
Mu_Option_Type(void* obj, MuOption* option, const char* name)
{
    return option->type(obj, name);
}
