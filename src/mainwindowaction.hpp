/*
 * gnote
 *
 * Copyright (C) 2015 Aurimas Cernius
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

#ifndef _MAIN_WINDOW_ACTION_HPP_
#define _MAIN_WINDOW_ACTION_HPP_

#include <giomm/simpleaction.h>

#include "base/macros.hpp"


namespace gnote {

class MainWindowAction
  : public Gio::SimpleAction
{
public:
  typedef Glib::RefPtr<MainWindowAction> Ptr;

  static Ptr create(const Glib::ustring & name);
  static Ptr create(const Glib::ustring & name, bool state);
  static Ptr create(const Glib::ustring & name, int state);
  static Ptr create(const Glib::ustring & name, const Glib::ustring & state);

  void set_state(const Glib::VariantBase & value)
    {
      Gio::SimpleAction::set_state(value);
    }
protected:
  MainWindowAction(const Glib::ustring & name);
  MainWindowAction(const Glib::ustring & name, bool state);
  MainWindowAction(const Glib::ustring & name, int state);
  MainWindowAction(const Glib::ustring & name, const Glib::ustring & state);
};

}

#endif
