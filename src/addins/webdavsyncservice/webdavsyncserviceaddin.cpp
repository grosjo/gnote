/*
 * gnote
 *
 * Copyright (C) 2012-2013,2017,2019 Aurimas Cernius
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


#include <glibmm/i18n.h>

#include "debug.hpp"
#include "ignote.hpp"
#include "preferences.hpp"
#include "webdavsyncserviceaddin.hpp"
#include "gnome_keyring/keyringexception.hpp"
#include "gnome_keyring/ring.hpp"
#include "sharp/string.hpp"
#include "synchronization/isyncmanager.hpp"


using gnome::keyring::KeyringException;
using gnome::keyring::Ring;
using gnote::Preferences;


namespace webdavsyncserviceaddin {

WebDavSyncServiceModule::WebDavSyncServiceModule()
{
  ADD_INTERFACE_IMPL(WebDavSyncServiceAddin);
}




const char *WebDavSyncServiceAddin::KEYRING_ITEM_NAME = "Tomboy sync WebDAV account";
std::map<Glib::ustring, Glib::ustring> WebDavSyncServiceAddin::s_request_attributes;

WebDavSyncServiceAddin * WebDavSyncServiceAddin::create()
{
  s_request_attributes["name"] = KEYRING_ITEM_NAME;
  return new WebDavSyncServiceAddin;
}

Gtk::Widget *WebDavSyncServiceAddin::create_preferences_control(EventHandler requiredPrefChanged)
{
  Gtk::Table *table = new Gtk::Table(3, 2, false);
  table->set_row_spacings(5);
  table->set_col_spacings(10);

  // Read settings out of gconf
  Glib::ustring url, username, password;
  get_config_settings(url, username, password);

  m_url_entry = new Gtk::Entry();
  m_url_entry->set_text(url);
  m_url_entry->signal_changed().connect(requiredPrefChanged);
  add_row(table, m_url_entry, _("_URL:"), 0);

  m_username_entry = new Gtk::Entry();
  m_username_entry->set_text(username);
  m_username_entry->signal_changed().connect(requiredPrefChanged);
  add_row(table, m_username_entry, _("User_name:"), 1);

  m_password_entry = new Gtk::Entry();
  m_password_entry->set_text(password);
  m_password_entry->set_visibility(false);
  m_password_entry->signal_changed().connect(requiredPrefChanged);
  add_row(table, m_password_entry, _("_Password:"), 2);

  table->set_hexpand(true);
  table->set_vexpand(false);
  table->show_all();
  return table;
}

bool WebDavSyncServiceAddin::is_configured()
{
  Glib::ustring url, username, password;
  return get_config_settings(url, username, password);
}


bool WebDavSyncServiceAddin::are_settings_valid()
{
  Glib::ustring url, username, password;
  return get_pref_widget_settings(url, username, password);
}


Glib::ustring WebDavSyncServiceAddin::name()
{
  const char *res = _("WebDAV");
  return res;
}


Glib::ustring WebDavSyncServiceAddin::id()
{
  return "wdfs";
}

Glib::ustring WebDavSyncServiceAddin::fuse_mount_directory_error()
{
  const char *res = _("There was an error connecting to the server.  This may be caused by using an incorrect user name and/or password.");
  return res;
}

std::vector<Glib::ustring> WebDavSyncServiceAddin::get_fuse_mount_exe_args(const Glib::ustring & mountPath, bool fromStoredValues)
{
  Glib::ustring url, username, password;
  if(fromStoredValues) {
    get_config_settings(url, username, password);
  }
  else {
    get_pref_widget_settings(url, username, password);
  }
  
  return get_fuse_mount_exe_args(mountPath, url, username, password, accept_ssl_cert());
}

Glib::ustring WebDavSyncServiceAddin::get_fuse_mount_exe_args_for_display(const Glib::ustring & mountPath, bool fromStoredValues)
{
  std::vector<Glib::ustring> args = get_fuse_mount_exe_args(mountPath, fromStoredValues);
  Glib::ustring result;
  for(auto iter : args) {
    result += iter + " ";
  }

  return result;
}

Glib::ustring WebDavSyncServiceAddin::fuse_mount_exe_name()
{
  return "wdfs";
}

bool WebDavSyncServiceAddin::verify_configuration()
{
  Glib::ustring url, username, password;

  if(!get_pref_widget_settings(url, username, password)) {
    // TODO: Figure out a way to send the error back to the client
    DBG_OUT("One of url, username, or password was empty");
    throw gnote::sync::GnoteSyncException(_("URL, username, or password field is empty."));
  }

  return true;
}

void WebDavSyncServiceAddin::save_configuration_values()
{
  Glib::ustring url, username, password;
  get_pref_widget_settings(url, username, password);

  save_config_settings(url, username, password);
}

void WebDavSyncServiceAddin::reset_configuration_values()
{
  save_config_settings("", "", "");

  // TODO: Unmount the FUSE mount!
}

std::vector<Glib::ustring> WebDavSyncServiceAddin::get_fuse_mount_exe_args(const Glib::ustring & mountPath, const Glib::ustring & url,
    const Glib::ustring & username, const Glib::ustring & password, bool acceptSsl)
{
  std::vector<Glib::ustring> args;
  args.reserve(12);
  args.push_back(url);
  args.push_back(mountPath);
  args.push_back("-o");
  args.push_back("username=" + username);
  args.push_back("-o");
  args.push_back("password=" + password);
  args.push_back("-o");
  args.push_back("fsname=gnotewdfs");
  if(acceptSsl) {
    args.push_back("-o");
    args.push_back("accept_sslcert");
  }
  args.push_back("-o");
  args.push_back("fsname=gnotewdfs");
  return args;
}

bool WebDavSyncServiceAddin::get_config_settings(Glib::ustring & url, Glib::ustring & username, Glib::ustring & password)
{
  // Retrieve configuration from the GNOME Keyring and GSettings
  url = "";
  username = "";
  password = "";

  try {
    password = sharp::string_trim(Ring::find_password(s_request_attributes));
    if(password != "") {
      Glib::RefPtr<Gio::Settings> settings = ignote().preferences()
        .get_schema_settings(Preferences::SCHEMA_SYNC_WDFS);
      username = sharp::string_trim(settings->get_string(Preferences::SYNC_FUSE_WDFS_USERNAME));
      url = sharp::string_trim(settings->get_string(Preferences::SYNC_FUSE_WDFS_URL));
    }
  }
  catch(KeyringException & ke) {
    DBG_OUT("Getting configuration from the GNOME keyring failed with the following message: %s", ke.what());
  }

  return url != "" && username != "" && password != "";
}

void WebDavSyncServiceAddin::save_config_settings(const Glib::ustring & url, const Glib::ustring & username, const Glib::ustring & password)
{
  // Save configuration into the GNOME Keyring and GSettings
  try {
    Glib::RefPtr<Gio::Settings> settings = ignote().preferences()
      .get_schema_settings(Preferences::SCHEMA_SYNC_WDFS);
    settings->set_string(Preferences::SYNC_FUSE_WDFS_USERNAME, username);
    settings->set_string(Preferences::SYNC_FUSE_WDFS_URL, url);

    if(password != "") {
      Ring::create_password(Ring::default_keyring(), KEYRING_ITEM_NAME, s_request_attributes, password);
    }
    else {
      Ring::clear_password(s_request_attributes);
    }
  }
  catch(KeyringException & ke) {
    DBG_OUT("Saving configuration to the GNOME keyring failed with the following message: %s", ke.what());
    // TODO: If the above fails (no keyring daemon), save all but password
    //       to GConf, and notify user.
    // Save configuration into GConf
    //Preferences.Set ("/apps/tomboy/sync_wdfs_url", url ?? string.Empty);
    //Preferences.Set ("/apps/tomboy/sync_wdfs_username", username ?? string.Empty);

    Glib::ustring msg = Glib::ustring::compose(
      // TRANSLATORS: %1 is the format placeholder for the error message.
      _("Saving configuration to the GNOME keyring failed with the following message:\n\n%1"),
      ke.what());
    throw gnote::sync::GnoteSyncException(msg.c_str());
  }
}

bool WebDavSyncServiceAddin::get_pref_widget_settings(Glib::ustring & url, Glib::ustring & username, Glib::ustring & password)
{
  url = sharp::string_trim(m_url_entry->get_text());
  username = sharp::string_trim(m_username_entry->get_text());
  password = sharp::string_trim(m_password_entry->get_text());

  return url != "" && username != "" && password != "";
}

bool WebDavSyncServiceAddin::accept_ssl_cert()
{
  try {
    return ignote().preferences().get_schema_settings(Preferences::SCHEMA_SYNC_WDFS)->get_boolean(
        Preferences::SYNC_FUSE_WDFS_ACCEPT_SSLCERT);
  }
  catch(...) {
    return false;
  }
}

void WebDavSyncServiceAddin::add_row(Gtk::Table *table, Gtk::Widget *widget, const Glib::ustring & labelText, uint row)
{
  Gtk::Label *l = new Gtk::Label(labelText);
  l->set_use_underline(true);
  l->property_xalign() = 0.0f;
  l->show();
  table->attach(*l, 0, 1, row, row + 1,
		Gtk::FILL,
		Gtk::EXPAND | Gtk::FILL,
		0, 0);

  widget->show();
  table->attach(*widget, 1, 2, row, row + 1,
		Gtk::EXPAND | Gtk::FILL,
		Gtk::EXPAND | Gtk::FILL,
		0, 0);

  l->set_mnemonic_widget(*widget);

  // TODO: Tooltips
}

}

