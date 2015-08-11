// $Id: main.h 9359 2014-04-25 15:37:22Z FloSoft $
//
// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.
#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#pragma once

#ifdef HAVE_CONFIG_H
#   include "../config.h"
#endif // HAVE_CONFIG_H

#ifdef _WIN32
#   define _CRTDBG_MAP_ALLOC
#   if defined _DEBUG && defined _MSC_VER
#       include <crtdbg.h>
#   endif // _DEBUG
#endif // !_WIN32

#include <cstdio>

namespace libsiedler2{
    inline size_t getFileLength(FILE* f)
    {
        fseek(f, 0, SEEK_END);
        size_t length = ftell(f);
        fseek(f, 0, SEEK_SET);
        return length;
    }

    template<typename T>
    struct Deleter {
        void operator()(T *p)
        {
            delete p;
        }
    };
} // namespace libsiedler2

namespace boost{
    // support boost::scoped_ptr
    inline void checked_delete(FILE* x)
    {
        fclose(x);
    }
} // namespace boost

#endif // MAIN_H_INCLUDED
