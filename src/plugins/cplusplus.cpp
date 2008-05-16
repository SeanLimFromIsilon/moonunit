/*
 * Copyright (c) 2008, Brian Koropoff
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

#ifdef HAVE_CONFIG_H
#    include <config.h>
#endif
#include <exception>
#include <typeinfo>
#ifdef HAVE_CXXABI_H
#    include <cxxabi.h>
#endif

#include <moonunit/test.h>
#include <moonunit/util.h>

#include "c-token.h"
#include "cplusplus.h"

extern "C" void
cplusplus_trampoline(MuThunk thunk)
{
    MuInterfaceToken* _token = Mu_Interface_CurrentToken();
    CToken* token = reinterpret_cast<CToken*>(_token);

    try
    {
        thunk();
    }
    catch (std::exception& e)
    {
        const std::type_info& info = typeid(e);
        const char* name = info.name();
#ifdef HAVE_CXXABI_H
        int status;

        name = abi::__cxa_demangle(name, 0, 0, &status);
        
        /* If demangling failed for some reason, give up */
        if (status)
            name = info.name();
#endif

        MuTestResult result;  

        result.status = MU_STATUS_EXCEPTION;
        result.expected = token->expected;
        result.stage = token->current_stage;
        result.reason = format("%s: %s", name, e.what());
        result.file = NULL;
        result.line = 0;
        result.backtrace = NULL;

        _token->result(_token, &result);
        free(const_cast<char*>(result.reason));
    }
    catch (...)
    {
        MuTestResult result;  

        result.status = MU_STATUS_EXCEPTION;
        result.expected = token->expected;
        result.stage = token->current_stage;
        result.reason = "Unknown exception";
        result.file = NULL;
        result.line = 0;
        result.backtrace = NULL;

        _token->result(_token, &result);
    }
}
