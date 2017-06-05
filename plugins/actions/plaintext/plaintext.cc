/*
 *	subtitleeditor -- a tool to create or edit subtitle
 *
 *	http://home.gna.org/subtitleeditor/
 *	https://gna.org/projects/subtitleeditor/
 *
 *	Copyright @ 2005-2009, kitone
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <extension/action.h>
#include <utility.h>
#include <gui/dialogfilechooser.h>
#include <filereader.h>
#include <subtitleformatsystem.h>

/*
 *
 */
class PlainTextPlugin : public Action
{
public:

	PlainTextPlugin()
	{
		activate();
		update_ui();
	}

	~PlainTextPlugin()
	{
		deactivate();
	}

	/*
	 */
	void activate()
	{
		se_debug(SE_DEBUG_PLUGINS);

		// actions
		action_group = Gtk::ActionGroup::create("PlainTextPlugin");

		action_group->add(
				Gtk::Action::create("plain-text-import", _("_Import Plain Text"),
						_("Create a new document from any text file.")),
					sigc::mem_fun(*this, &PlainTextPlugin::on_import_transcript));

		action_group->add(
				Gtk::Action::create("plain-text-export", _("_Export Plain Text"), _("Export just a text in a file")),
					sigc::mem_fun(*this, &PlainTextPlugin::on_export_transcript));

		// ui
		Glib::RefPtr<Gtk::UIManager> ui = get_ui_manager();

		ui_id = ui->new_merge_id();

		ui->insert_action_group(action_group);

		ui->add_ui(ui_id, "/menubar/menu-file/menu-import/placeholder", "plain-text-import", "plain-text-import");
		ui->add_ui(ui_id, "/menubar/menu-file/menu-export/placeholder", "plain-text-export", "plain-text-export");
	}

	/*
	 */
	void deactivate()
	{
		se_debug(SE_DEBUG_PLUGINS);

		Glib::RefPtr<Gtk::UIManager> ui = get_ui_manager();

		ui->remove_ui(ui_id);
		ui->remove_action_group(action_group);
	}

	/*
	 */
	void update_ui()
	{
		se_debug(SE_DEBUG_PLUGINS);

		bool visible = (get_current_document() != NULL);

		action_group->get_action("plain-text-export")->set_sensitive(visible);
	}

protected:

	/*
	 */
	void on_import_transcript()
	{
		se_debug(SE_DEBUG_PLUGINS);

		DialogImportText::unique_ptr ui = DialogImportText::create();

		if(ui->run() == Gtk::RESPONSE_OK)
		{
			Glib::ustring uri = ui->get_uri();
			Glib::ustring filename = ui->get_filename();
			Glib::ustring charset = ui->get_encoding();

			try
			{
				Glib::ustring untitled = DocumentSystem::getInstance().create_untitled_name();
				Glib::ustring	format = get_config().get_value_string("document", "format");
				Glib::ustring untitled_fullname = Glib::build_filename(ui->get_current_folder(), untitled);

				Document *doc = new Document();
				SubtitleFormatSystem::instance().open_from_uri(doc, uri, charset, "Plain Text Format");
				doc->setName(untitled);
				doc->setFilename(untitled_fullname);
				doc->setFormat(format); // override the plain text format with the preferred format setting
				DocumentSystem::getInstance().append(doc);
				
			}
			catch(const std::exception &ex)
			{
				dialog_error(
						build_message(_("Could not import from file \"%s\"."), uri.c_str()), 
						ex.what());
			}
		}
	}

	/*
	 */
	void on_export_transcript()
	{
	 se_debug(SE_DEBUG_PLUGINS);

		DialogExportText::unique_ptr ui = DialogExportText::create();

		if(ui->run() == Gtk::RESPONSE_OK)
		{
			Glib::ustring uri = ui->get_uri();
			Glib::ustring charset = ui->get_encoding();
			Glib::ustring newline = ui->get_newline();

			try
			{
				Document *doc = get_current_document();
				SubtitleFormatSystem::instance().save_to_uri( doc, uri, "Plain Text Format", charset, newline );
			}
			catch(const std::exception &ex)
			{
				dialog_error(
						build_message(_("Could not export to the file \"%s\"."), uri.c_str()), 
						ex.what());
			}
		}
	}

	
protected:
	Gtk::UIManager::ui_merge_id ui_id;
	Glib::RefPtr<Gtk::ActionGroup> action_group;
};

REGISTER_EXTENSION(PlainTextPlugin)
