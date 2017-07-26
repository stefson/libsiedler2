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

#include "config.h"
#include "libsiedler2/src/ArchivInfo.h"
#include "libsiedler2/src/ArchivItem_Raw.h"
#include "libsiedler2/src/enumTypes.h"
#include "libsiedler2/src/IAllocator.h"
#include "libsiedler2/src/libsiedler2.h"
#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>
#include <stdexcept>

BOOST_AUTO_TEST_SUITE(Archiv)

BOOST_AUTO_TEST_CASE(Push)
{
    libsiedler2::ArchivInfo archiv;
    BOOST_REQUIRE(archiv.empty());
    libsiedler2::ArchivItem_Raw* rawItem = new libsiedler2::ArchivItem_Raw;
    rawItem->getData().push_back(42);
    archiv.pushC(*rawItem);
    BOOST_REQUIRE(!archiv.empty());
    BOOST_REQUIRE_EQUAL(archiv.size(), 1u);
    libsiedler2::ArchivItem_Raw* rawItemOut = dynamic_cast<libsiedler2::ArchivItem_Raw*>(archiv[0]);
    BOOST_REQUIRE(rawItemOut);
    // Should have copied
    BOOST_REQUIRE_NE(rawItem, rawItemOut);
    BOOST_REQUIRE_EQUAL(rawItemOut->getData()[0], 42u);
    // Invalid indizes return NULL
    BOOST_REQUIRE(!archiv[1]);
    // Add with passing ownership
    archiv.push(rawItem);
    BOOST_REQUIRE_EQUAL(archiv[1], rawItem);
    archiv.clear();
    BOOST_REQUIRE(archiv.empty());
}

BOOST_AUTO_TEST_CASE(Set)
{
    libsiedler2::ArchivInfo archiv;
    BOOST_REQUIRE(archiv.empty());
    archiv.push(new libsiedler2::ArchivItem_Raw);
    archiv.alloc(2);
    BOOST_REQUIRE_EQUAL(archiv.size(), 2u);
    // Archiv should have been cleared
    BOOST_REQUIRE(!archiv[0]);
    libsiedler2::ArchivItem_Raw* rawItem = new libsiedler2::ArchivItem_Raw;
    rawItem->getData().push_back(42);
    archiv.setC(0, *rawItem);
    libsiedler2::ArchivItem_Raw* rawItemOut = dynamic_cast<libsiedler2::ArchivItem_Raw*>(archiv[0]);
    BOOST_REQUIRE(rawItemOut);
    // Should have copied
    BOOST_REQUIRE_NE(rawItem, rawItemOut);
    BOOST_REQUIRE_EQUAL(rawItemOut->getData()[0], 42u);
    // Invalid indizes return NULL
    BOOST_REQUIRE(!archiv[1]);
    archiv.alloc_inc(3);
    BOOST_REQUIRE_EQUAL(archiv.size(), 5u);
    BOOST_REQUIRE_EQUAL(archiv[0], rawItemOut);
    // Throw out of range exception
    BOOST_REQUIRE_THROW(archiv.set(5, rawItem), std::range_error);
    // Add with passing ownership
    archiv.set(4, rawItem);
    BOOST_REQUIRE_EQUAL(archiv[4], rawItem);
    archiv.clearItem(4);
    BOOST_REQUIRE(!archiv[4]);
}

BOOST_AUTO_TEST_CASE(Find)
{
    libsiedler2::ArchivInfo archiv;
    libsiedler2::ArchivItem_Raw* rawItem1 = new libsiedler2::ArchivItem_Raw;
    rawItem1->setName("Foo1");
    libsiedler2::ArchivItem_Raw* rawItem2 = new libsiedler2::ArchivItem_Raw;
    rawItem2->setName("Foo2");
    libsiedler2::ArchivItem_Raw* rawItem3 = new libsiedler2::ArchivItem_Raw;
    rawItem3->setName("Foo3");
    archiv.push(rawItem1);
    archiv.push(rawItem2);
    archiv.push(NULL);
    archiv.push(rawItem3);
    BOOST_REQUIRE_EQUAL(archiv.find(rawItem1->getName()), rawItem1);
    BOOST_REQUIRE_EQUAL(archiv.find(rawItem2->getName()), rawItem2);
    BOOST_REQUIRE_EQUAL(archiv.find(rawItem3->getName()), rawItem3);
    BOOST_REQUIRE(!archiv.find("NonExistant"));
}

BOOST_AUTO_TEST_CASE(CreateAllTypesAndCopy)
{
    libsiedler2::ArchivInfo archiv;
    for(int i = 1; i < libsiedler2::NUM_BOB_TYPES; i++)
    {
        libsiedler2::BobType bobType = libsiedler2::BobType(i);
        if(bobType == libsiedler2::BOBTYPE_UNSET || bobType == libsiedler2::BOBTYPE_SOUND)
            continue;
        libsiedler2::ArchivItem* item = libsiedler2::getAllocator().create(bobType);
        BOOST_REQUIRE(item);
        item->setName("Item" + boost::lexical_cast<std::string>(i));
        archiv.push(item);
    }
    for(int i = libsiedler2::SOUNDTYPE_WAVE; i < libsiedler2::SOUNDTYPE_OTHER; i++)
    {
        libsiedler2::ArchivItem* item = libsiedler2::getAllocator().create(libsiedler2::BOBTYPE_SOUND, libsiedler2::SoundType(i));
        BOOST_REQUIRE(item);
        item->setName("Sound" + boost::lexical_cast<std::string>(i));
        archiv.push(item);
    }
    // Copy ctor
    libsiedler2::ArchivInfo archiv2(archiv), archiv3;
    BOOST_REQUIRE_EQUAL(archiv.size(), archiv2.size());
    // Avoid ctor init
    archiv3.push(new libsiedler2::baseArchivItem_Raw);
    // Assign
    archiv3 = archiv;
    BOOST_REQUIRE_EQUAL(archiv.size(), archiv3.size());
    // All should be equal
    for(unsigned i = 0; i < archiv.size(); i++)
    {
        BOOST_REQUIRE(archiv[i]);
        BOOST_REQUIRE(archiv2[i]);
        BOOST_REQUIRE(archiv3[i]);
        // Items should have been copied
        BOOST_REQUIRE_NE(archiv[i], archiv2[i]);
        BOOST_REQUIRE_NE(archiv[i], archiv3[i]);
        BOOST_REQUIRE_EQUAL(archiv[i]->getBobType(), archiv2[i]->getBobType());
        BOOST_REQUIRE_EQUAL(archiv[i]->getBobType(), archiv3[i]->getBobType());
        BOOST_REQUIRE_EQUAL(archiv[i]->getName(), archiv2[i]->getName());
        BOOST_REQUIRE_EQUAL(archiv[i]->getName(), archiv3[i]->getName());
    }
}

BOOST_AUTO_TEST_SUITE_END()