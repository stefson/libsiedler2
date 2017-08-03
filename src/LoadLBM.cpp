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

#include "libSiedler2Defines.h" // IWYU pragma: keep
#include "ArchivItem_Bitmap.h"
#include "ArchivItem_Palette.h"
#include "ArchivInfo.h"
#include "prototypen.h"
#include "libsiedler2.h"
#include "IAllocator.h"
#include "ErrorCodes.h"
#include "fileFormatHelpers.h"
#include "OpenMemoryStream.h"
#include "libendian/src/EndianIStreamAdapter.h"
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>

/**
 *  lädt eine LBM-File in ein ArchivInfo.
 *
 *  @param[in]  file    Dateiname der LBM-File
 *  @param[out] items   ArchivInfo-Struktur, welche gefüllt wird
 *
 *  @return Null bei Erfolg, ein Wert ungleich Null bei Fehler
 */
int libsiedler2::loader::LoadLBM(const std::string& file, ArchivInfo& items)
{
    MMStream mmapStream;
    if(int ec = openMemoryStream(file, mmapStream))
        return ec;
    libendian::EndianIStreamAdapter<true, MMStream& > lbm(mmapStream);

    char header[4], pbm[4];
    uint32_t length;
    // Header einlesen
    lbm >> header >> length >> pbm;

    // ist es eine LBM-File? (Header "FORM")
    if(!lbm || !isChunk(header, "FORM") || !isChunk(pbm, "PBM "))
        return ErrorCode::WRONG_HEADER;

    boost::interprocess::unique_ptr< baseArchivItem_Bitmap, Deleter<baseArchivItem_Bitmap> > bitmap(dynamic_cast<baseArchivItem_Bitmap*>(getAllocator().create(BOBTYPE_BITMAP_RAW)));

    uint16_t width = 0, height = 0;
    uint16_t compression;
    char chunk[4];
    // Chunks einlesen
    while(lbm.read(chunk, 4))
    {
        if(isChunk(chunk, "BMHD"))
        {
            uint32_t unknown;
            uint8_t numPlanes, mask;
            // Länge einlesen
            lbm >> length;

            // Bei ungerader Zahl aufrunden
            if(length & 1)
                ++length;

            lbm >> width >> height >> unknown >> numPlanes >> mask >> compression;

            // Nur 256 Farben und nicht mehr!
            if(numPlanes != 8 || mask != 0)
                return ErrorCode::WRONG_FORMAT;

            // Keine bekannte Kompressionsart?
            if(compression != 0 && compression != 256)
                return ErrorCode::WRONG_FORMAT;

            length -= 12;

            // Rest überspringen
            lbm.ignore(length);
        } else if(isChunk(chunk, "CMAP"))
        {
            // Länge einlesen
            lbm >> length;

            // Bei ungerader Zahl aufrunden
            if(length & 1)
                ++length;

            // Ist Länge wirklich so groß wie Farbtabelle?
            if(length != 256 * 3)
                return ErrorCode::WRONG_FORMAT;

            // Daten von Item auswerten
            ArchivItem_Palette* palette = dynamic_cast<ArchivItem_Palette*>(getAllocator().create(BOBTYPE_PALETTE));
            bitmap->setPalette(palette);
            if(int ec = palette->load(lbm.getStream(), false))
                return ec;
        } else if(isChunk(chunk, "BODY"))
        {
            // Länge einlesen
            lbm >> length;

            // Bei ungerader Zahl aufrunden
            if(length & 1)
                ++length;

            // haben wir eine Palette erhalten?
            if(bitmap->getPalette() == NULL)
                return ErrorCode::PALETTE_MISSING;

            bitmap->tex_alloc(width, height, FORMAT_PALETTED);

            if(compression == 0) // unkomprimiert
            {
                if(length != static_cast<uint32_t>(width * height))
                    return ErrorCode::WRONG_FORMAT;
                for(int y = 0; y < height; ++y)
                {
                    for(int x = 0; x < width; ++x)
                    {
                        uint8_t color;
                        lbm >> color;
                        bitmap->setPixel(x, y, color);
                    }
                }
            } else // komprimiert (RLE?)
            {
                // Welcher Pixel ist dran?
                uint16_t x = 0, y = 0;

                // Solange einlesen, bis Block zuende bzw. Datei zuende ist
                while(length > 0 && !lbm.eof())
                {
                    // Typ lesen
                    int8_t ctype;
                    lbm >> ctype;
                    --length;
                    if(length == 0)
                        continue;

                    if(ctype > 0) // unkomprimierte Pixel
                    {
                        int16_t count = 1 + static_cast<int16_t>(ctype);

                        for(int16_t i = 0; i < count; ++i)
                        {
                            // Farbe auslesen
                            uint8_t color;
                            lbm >> color;
                            --length;

                            bitmap->setPixel(x++, y, color);
                            if(x >= width)
                            {
                                ++y;
                                x = 0;
                            }
                        }
                    } else // komprimierte Pixel
                    {
                        int16_t count = 1 - static_cast<int16_t>(ctype);

                        // Farbe auslesen
                        uint8_t color;
                        lbm >> color;
                        --length;

                        for(uint16_t i = 0; i < count; ++i)
                        {
                            bitmap->setPixel(x++, y, color);
                            if(x >= width)
                            {
                                ++y;
                                x = 0;
                            }
                        }
                    }
                }
            }
            items.push(bitmap.release());
        } else
        {
            // Länge einlesen
            lbm >> length;

            // Bei ungerader Zahl aufrunden
            if(length & 1)
                ++length;

            // Rest überspringen
            lbm.ignore(length);
        }
    }

    if(items.empty() || !lbm.eof())
        return ErrorCode::WRONG_FORMAT;

    return ErrorCode::NONE;
}
