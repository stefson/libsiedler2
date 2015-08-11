// $Id: ArchivInfo.cpp 9359 2014-04-25 15:37:22Z FloSoft $
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

///////////////////////////////////////////////////////////////////////////////
// Header
#include "main.h"
#include "ArchivInfo.h"
#include "types.h"

///////////////////////////////////////////////////////////////////////////////
// Makros / Defines
#if defined _WIN32 && defined _DEBUG && defined _MSC_VER
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace libsiedler2{
///////////////////////////////////////////////////////////////////////////////
/** @class ArchivInfo
 *
 *  Klasse für Archivdateien.
 *
 *  @author FloSoft
 */


///////////////////////////////////////////////////////////////////////////////
/** @var ArchivInfo::data
 *
 *  die Elemente.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Konstruktor von @p ArchivInfo.
 *
 *  @author FloSoft
 */
ArchivInfo::ArchivInfo(void)
{
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierkonstruktor von @p ArchivInfo.
 *
 *  @param[in] info Quellitem
 *
 *  @author FloSoft
 */
ArchivInfo::ArchivInfo(const ArchivInfo& info) : data(info.data)
{
    for(std::vector<ArchivItem*>::iterator it = data.begin(); it != data.end(); ++it)
        *it = getAllocator().clone(**it);
}

ArchivInfo& ArchivInfo::operator=(const ArchivInfo& info){
    if(this == &info)
        return *this;
    clear();
    data.reserve(info.size());
    for(std::vector<ArchivItem*>::const_iterator it = info.data.begin(); it != info.data.end(); ++it)
        data.push_back(getAllocator().clone(**it));
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Destruktor von @p ArchivInfo, räumt automatisch auf.
 *
 *  @author FloSoft
 */
ArchivInfo::~ArchivInfo(void)
{
    clear();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  erstellt den Datensatz in einer bestimmten Größe.
 *
 *  @param[in] count Elementanzahl auf den der Datensatz angelegt werden soll
 *
 *  @author FloSoft
 */
void ArchivInfo::alloc(size_t count)
{
    clear();

    data.resize(count);
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  gibt die angelegten Daten wieder frei.
 *
 *  @author FloSoft
 */
void ArchivInfo::clear(void)
{
    
    for(std::vector<ArchivItem*>::iterator it = data.begin(); it != data.end(); ++it)
        delete *it;
    data.clear();
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  Setzt den Inhalt eines ArchivItems auf das des Übergebenen.
 *
 *  @param[in] index Index des zu setzenden Eintrags
 *  @param[in] item  Item mit dem zu setzenden Inhalt
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  kopiert den Inhalt eines ArchivItems auf das des Übergebenen.
 *
 *  @param[in] index Index des zu setzenden Eintrags
 *  @param[in] item  Item mit dem zu kopierenden Inhalt
 *
 *  @author FloSoft
 */
void ArchivInfo::setC(size_t index, const ArchivItem& item)
{
    set(index, getAllocator().clone(item));
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  fügt ein Element hinten an.
 *
 *  @param[in] item Item mit dem anzufügenden Inhalt
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  fügt ein Element hinten an und kopiert die Daten von @p item.
 *
 *  @param[in] item Item mit dem zu kopierenden Inhalt
 *
 *  @author FloSoft
 */
void ArchivInfo::pushC(const ArchivItem& item)
{
    data.push_back(getAllocator().clone(item));
}

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den Inhalt eines ArchivItems am entsprechenden Index.
 *
 *  @param[in] index Index des zu setzenden Eintrags
 *
 *  @return liefert NULL bei Fehler, ansonsten das entsprechende Item
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den Inhalt eines ArchivItems am entsprechenden Index.
 *
 *  @param[in] index Index des zu setzenden Eintrags
 *
 *  @return liefert NULL bei Fehler, ansonsten das entsprechende Item
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert den Pointer eines ArchivItems am entsprechenden Index.
 *
 *  @param[in] index Index des zu setzenden Eintrags
 *
 *  @return liefert NULL bei Fehler, ansonsten das entsprechende Item
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  liefert die Größe des Archivs.
 *
 *  @return liefert die Größe des Archivs.
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Index-Operator von @p ArchivInfo.
 *
 *  @param[in] index Index des zu liefernden Eintrags
 *
 *  @return Bei Erfolg ArchivItem, ansonsten NULL
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Zuweisungsoperator von @p ArchivInfo.
 *
 *  @param[in] info Quellitem
 *
 *  @return Referenz auf this
 *
 *  @author FloSoft
 */

///////////////////////////////////////////////////////////////////////////////
/**
 *  Kopierfunktion von @p ArchivInfo.
 *
 *  @param[in] to     Zielposition
 *  @param[in] from   Quellposition
 *  @param[in] count  Anzahl
 *  @param[in] source Quelle
 *
 *  @author FloSoft
 */
void ArchivInfo::copy(size_t to, size_t from, size_t count, const ArchivInfo& source)
{
    if(to + count > size())
        data.resize(to + count);

    for(size_t f = from; f < from + count; ++to, ++f)
        setC(to, *source.get(f));
}

}
