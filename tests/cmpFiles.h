// Copyright (c) 2017 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#pragma once

#ifndef cmpFiles_h__
#define cmpFiles_h__

#include <boost/test/unit_test.hpp>
#include <string>

namespace libsiedler2 {
class Archiv;
class ArchivItem_Palette;
} // namespace libsiedler2

boost::test_tools::predicate_result testLoad(int expectedResult, const std::string& file, libsiedler2::Archiv& items,
                                             const libsiedler2::ArchivItem_Palette* palette = nullptr);
boost::test_tools::predicate_result testWrite(int expectedResult, const std::string& file, libsiedler2::Archiv& items,
                                              const libsiedler2::ArchivItem_Palette* palette = nullptr);
boost::test_tools::predicate_result testFilesEqual(const std::string& fileToCheck, const std::string& expectedFile);

#endif // cmpFiles_h__
