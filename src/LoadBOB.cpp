// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include "Archiv.h"
#include "ArchivItem_Bob.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "OpenMemoryStream.h"
#include "libsiedler2.h"
#include "prototypen.h"
#include "libendian/EndianIStreamAdapter.h"
#include <boost/filesystem/path.hpp>

/**
 *  lädt eine BOB-File in ein Archiv.
 *
 *  @param[in]  file    Dateiname der BOB-File
 *  @param[out] items   Archiv-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadBOB(const std::string& file, Archiv& items, const ArchivItem_Palette* palette)
{
    if(palette == nullptr)
        return ErrorCode::PALETTE_MISSING;

    MMStream mmapStream;
    if(int ec = openMemoryStream(file, mmapStream))
        return ec;
    libendian::EndianIStreamAdapter<false, MMStream&> bob(mmapStream);

    // Header einlesen
    uint32_t header;

    bob >> header;

    // ist es eine BOB-File? (Header 0xF601F501)
    if(!bob || header != 0x01F501F6)
        return ErrorCode::WRONG_HEADER;

    auto item = getAllocator().create<ArchivItem_Bob>(BOBTYPE_BOB);

    boost::filesystem::path filePath(file);
    if(filePath.has_filename())
        item->setName(filePath.filename().string()); //-V522

    if(int ec = item->load(bob.getStream(), palette))
        return ec;

    // Item alloziieren und zuweisen
    items.clear();
    items.push(std::move(item));

    return ErrorCode::NONE;
}
