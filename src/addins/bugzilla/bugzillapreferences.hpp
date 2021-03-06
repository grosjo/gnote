/*
 * gnote
 *
 * Copyright (C) 2013,2017,2019 Aurimas Cernius
 * Copyright (C) 2009 Hubert Figuiere
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



#ifndef __BUGZILLA_PREFERENCES_HPP_
#define __BUGZILLA_PREFERENCES_HPP_

#include <gdkmm/pixbuf.h>
#include <gtkmm/button.h>
#include <gtkmm/grid.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/treeview.h>

namespace sharp {

  class FileInfo;

}

namespace gnote {
  class Preferences;
}


namespace bugzilla {


class BugzillaPreferences
  : public Gtk::Grid
{
public:
  BugzillaPreferences(gnote::IGnote &, gnote::Preferences &, gnote::NoteManager &);

protected:
  virtual void on_realize() override;

private:
  void update_icon_store();
  Glib::ustring parse_host(const sharp::FileInfo &);
  void selection_changed();
  void add_clicked();
  bool copy_to_bugzilla_icons_dir(const Glib::ustring & file_path,
                                  const Glib::ustring & host,
                                  Glib::ustring & err_msg);
  void resize_if_needed(const Glib::ustring & path);
  void remove_clicked();

  class Columns
    : public Gtk::TreeModelColumnRecord
  {
  public:
    Columns()
      { add(icon); add(host); add(file_path); }
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
    Gtk::TreeModelColumn<Glib::ustring>              host;
    Gtk::TreeModelColumn<Glib::ustring>              file_path;
  };

  Columns        m_columns;
  Gtk::TreeView *icon_tree;
  Glib::RefPtr<Gtk::ListStore> icon_store;

  Gtk::Button *add_button;
  Gtk::Button *remove_button;

  Glib::ustring last_opened_dir;

  void _init_static();
  static bool          s_static_inited;
  static Glib::ustring s_image_dir;
};

}

#endif
