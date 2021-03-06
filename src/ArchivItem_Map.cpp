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

#include "ArchivItem_Map.h"
#include "ArchivItem_Map_Header.h"
#include "ArchivItem_Raw.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "libsiedler2.h"
#include "libendian/EndianIStreamAdapter.h"
#include "libendian/EndianOStreamAdapter.h"
#include <iostream>

struct BlockHeader
{                     //-V802
    uint16_t id;      // Must be 0x2710
    uint32_t unknown; // Always 0
    uint16_t w, h;
    uint16_t multiplier; // Not sure, always 1
    uint32_t blockLength;
};

libsiedler2::ArchivItem_Map::ArchivItem_Map() : ArchivItem(BOBTYPE_MAP) {}

libsiedler2::ArchivItem_Map::~ArchivItem_Map() = default;

/**
 *  lädt die Mapdaten aus einer Datei.
 *
 *  @param[in] file Dateihandle der Datei
 *  @param[in] only_header Soll nur der Header gelesen werden?
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Map::load(std::istream& file, bool only_header)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    clear();

    {
        auto header = getAllocator().create<ArchivItem_Map_Header>(BOBTYPE_MAP_HEADER);
        assert(header);

        int ec = header->load(file); //-V522
        if(ec)
            return ec;

        push(std::move(header));
    }

    // nur der Header?
    if(only_header)
        return ErrorCode::NONE;

    const auto& header = static_cast<const ArchivItem_Map_Header&>(*get(0));
    const uint16_t w = header.getWidth();
    const uint16_t h = header.getHeight();

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);
    for(uint32_t i = 1; i < 15; ++i)
    {
        BlockHeader bHeader;
        fs >> bHeader.id >> bHeader.unknown >> bHeader.w >> bHeader.h >> bHeader.multiplier >> bHeader.blockLength;
        // Header id must match
        if(!fs || bHeader.id != 0x2710 || bHeader.unknown != 0)
        {
            return ErrorCode::WRONG_FORMAT;
        }
        // Multiplier of 0 means unused block and implies no data
        if(bHeader.multiplier == 0)
        {
            assert(bHeader.blockLength == 0);
            push(nullptr);
            continue;
        }
        // Blocksize must match extents
        if(bHeader.blockLength != static_cast<uint32_t>(w) * static_cast<uint32_t>(h))
        {
            assert(false);
            return ErrorCode::WRONG_FORMAT;
        }
        // If there is data, size must match
        if(bHeader.w != w || bHeader.h != h)
        {
            assert(false);
            return ErrorCode::WRONG_FORMAT;
        }

        auto layer = getAllocator().create<ArchivItem_Raw>(BOBTYPE_RAW);
        if(auto ec = layer->load(file, bHeader.blockLength)) //-V522
            return ec;
        if(i == 1 && header.hasExtraWord())
        {
            // Work around for map file bug: There are 2 extra bytes inbetween the header which would actually belong to the first block
            fs.setPositionRel(-2);
            std::vector<uint8_t>& data = layer->getData();
            // Replace last 2 bytes by 3rd last one
            data[data.size() - 1] = data[data.size() - 2] = data[data.size() - 3];
        }

        push(std::move(layer));
    }

    extraInfo.clear();

    while(true)
    {
        ExtraAnimalInfo info;
        fs >> info.id;
        if(info.id == 0xFF)
            break;
        fs >> info.x >> info.y;
        extraInfo.push_back(info);
    }

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

/**
 *  schreibt die Mapdaten in eine Datei.
 *
 *  @param[in] file Dateihandle der Datei
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int libsiedler2::ArchivItem_Map::write(std::ostream& file) const
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;

    const auto* header = dynamic_cast<const ArchivItem_Map_Header*>(get(0));

    if(!header)
        return ErrorCode::WRONG_ARCHIV;
    int ec = header->write(file);
    if(ec)
        return ec;

    BlockHeader bHeader;
    bHeader.id = 0x2710;
    bHeader.unknown = 0;
    bHeader.w = header->getWidth();
    bHeader.h = header->getHeight();
    // For unused
    bHeader.multiplier = 0;
    bHeader.blockLength = 0;

    libendian::EndianOStreamAdapter<false, std::ostream&> fs(file);
    for(uint32_t i = 1; i < std::max(15u, uint32_t(size())); ++i)
    {
        const auto* layer = dynamic_cast<const ArchivItem_Raw*>(get(i));
        fs << bHeader.id << bHeader.unknown << bHeader.w << bHeader.h;
        if(layer)
        {
            fs << uint16_t(1);
            assert(layer->getData().size() == size_t(header->getWidth()) * size_t(header->getHeight()));
            // Size is written in layer
            ec = layer->write(file, true);
            if(ec)
                return ec;
        } else
            fs << bHeader.multiplier << bHeader.blockLength;
    }

    for(auto it : extraInfo)
    {
        fs << it.id << it.x << it.y;
    }
    fs << uint8_t(0xFF);

    return (!file) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}
