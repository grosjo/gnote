/*
 * gnote
 *
 * Copyright (C) 2017,2019-2020 Aurimas Cernius
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <UnitTest++/UnitTest++.h>

#include "test/testgnote.hpp"
#include "test/testnotemanager.hpp"


SUITE(NoteManager)
{
  struct Fixture
  {
    test::Gnote g;
    test::NoteManager manager;

    Fixture()
      : manager(make_notes_dir(), g)
    {
      g.notebook_manager(&manager.notebook_manager());
    }

    Glib::ustring make_notes_dir()
    {
      char notes_dir_tmpl[] = "/tmp/gnotetestnotesXXXXXX";
      char *notes_dir = g_mkdtemp(notes_dir_tmpl);
      return notes_dir;
    }
  };


  TEST_FIXTURE(Fixture, create_no_args)
  {
    auto note1 = manager.create();
    auto note2 = manager.create();

    CHECK_EQUAL("New Note 1", note1->get_title());
    CHECK_EQUAL("New Note 2", note2->get_title());
    CHECK_EQUAL(2, manager.get_notes().size());
  }

  TEST_FIXTURE(Fixture, create_with_title)
  {
    auto note = manager.create("test");
    CHECK_EQUAL("test", note->get_title());
    CHECK_EQUAL(1, manager.get_notes().size());
  }

  TEST_FIXTURE(Fixture, create_and_find)
  {
    manager.create();
    manager.create();
    gnote::NoteBase::Ptr test_note = manager.create("test note");
    CHECK(test_note != NULL);
    CHECK_EQUAL(3, manager.get_notes().size());
    CHECK(manager.find("test note") == test_note);
    CHECK(manager.find_by_uri(test_note->uri()) == test_note);
  }
}

