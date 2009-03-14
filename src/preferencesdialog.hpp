


#ifndef __PREFERENCES_DIALOG_HPP_
#define __PREFERENCES_DIALOG_HPP_

#include <string>
#include <map>

#include <gtkmm/dialog.h>
#include <gtkmm/treeiter.h>
#include <gtkmm/liststore.h>
#include <gtkmm/combobox.h>

namespace gnote {

class SyncServiceAddin;


class PreferencesDialog
	: public Gtk::Dialog
{
public:
	PreferencesDialog();
	

	Gtk::Widget *make_editing_pane();
	Gtk::Widget *make_hotkeys_pane();
	Gtk::Widget *make_sync_pane();
	Gtk::Widget *make_addins_pane();

private:
	Gtk::Label *make_tip_label(std::string label_text);
	Gtk::Button *make_font_button();
	Gtk::Label *make_label (const std::string & label_text/*, params object[] args*/);
	Gtk::CheckButton *make_check_button (const std::string & label_text);

	void open_template_button_clicked();
	void on_font_button_clicked();
	void update_font_button(const std::string & font_desc);
	void on_sync_addin_combo_changed();
	void on_advanced_sync_config_button();
	void on_reset_sync_addin_button();
	void on_save_sync_addin_button();

	class SyncStoreModel
		: public Gtk::TreeModelColumnRecord
	{
	public:
		SyncStoreModel()
			{
				add(m_col1);
			}

		Gtk::TreeModelColumn<std::string> m_col1;
	};

	SyncStoreModel syncAddinStoreRecord;
	Glib::RefPtr<Gtk::ListStore> syncAddinStore;
	std::map<std::string, Gtk::TreeIter> syncAddinIters;
	Gtk::ComboBox *syncAddinCombo;
	SyncServiceAddin *selectedSyncAddin;
	Gtk::VBox   *syncAddinPrefsContainer;
	Gtk::Widget *syncAddinPrefsWidget;
	Gtk::Button *resetSyncAddinButton;
	Gtk::Button *saveSyncAddinButton;
	//AddinManager addin_manager;
		
	Gtk::Button *font_button;
	Gtk::Label  *font_face;
	Gtk::Label  *font_size;

	//Mono.Addins.Gui.AddinTreeWidget addin_tree;
	
	Gtk::Button enable_addin_button;
	Gtk::Button disable_addin_button;
	Gtk::Button addin_prefs_button;
	Gtk::Button addin_info_button;

	Gtk::RadioButton promptOnConflictRadio;
	Gtk::RadioButton renameOnConflictRadio;
	Gtk::RadioButton overwriteOnConflictRadio;


	/// <summary>
	/// Keep track of the opened addin prefs dialogs so other windows
	/// can be interacted with (as opposed to opening these as modal
	/// dialogs).
	///
	/// Key = Mono.Addins.Addin.Id
	/// </summary>
	std::map<std::string, Gtk::Dialog> addin_prefs_dialogs;

	/// <summary>
	/// Used to keep track of open AddinInfoDialogs.
	/// Key = Mono.Addins.Addin.Id
	/// </summary>
	std::map<std::string, Gtk::Dialog> addin_info_dialogs;

};


}

#endif
