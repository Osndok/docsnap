/* This file is part of camorama
 *
 * AUTHORS
 *     Sven Herzberg  <herzi@gnome-de.org>
 *
 * Copyright (C) 2003  Greg Jones
 * Copyright (C) 2006  Sven Herzberg <herzi@gnome-de.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "camorama-window.h"

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <glib/gi18n.h>
#include <libgnomeui/gnome-file-entry.h>
#include "callbacks.h"
#include "camorama-globals.h"
#include "camorama-stock-items.h"
#include "glib-helpers.h"
#include "support.h"

//XXX: static GQuark menu_item_filter_type = 0;
//XXX: add_filter_clicked(GtkMenuItem* menuitem, CamoramaFilterChain* chain);

struct weak_target {
	GtkTreeModel* model;
	GList       * list;
};

static void
reference_path(GtkTreePath* path, struct weak_target* target) {
	target->list = g_list_prepend(target->list, gtk_tree_row_reference_new(target->model, path));
}

//XXX: delete_filter(GtkTreeRowReference* ref, GtkTreeModel* model)
//XXX: delete_filter_clicked(GtkTreeSelection* sel, GtkMenuItem* menuitem)
//XXX: menu_position_func(GtkMenu* menu, gint* x, gint* y, gboolean *push_in, gpointer user_data);
//XXX: show_popup(cam* cam, GtkTreeView* treeview, GdkEventButton* ev);
//XXX: treeview_popup_menu_cb(cam* cam, GtkTreeView* treeview);
//XXX: treeview_clicked_cb(cam* cam, GdkEventButton* ev, GtkTreeView* treeview);
//XXX: tray_clicked_callback (GtkStatusIcon* status, guint button, guint activate_time, cam * cam);

void
load_interface(cam* cam)
{
    GdkPixbuf *logo = NULL;

	GtkWidget *mainWindow=glade_xml_get_widget(cam->xml, "main_window");
	GdkScreen *mainScreen=gtk_widget_get_screen(mainWindow);
	GtkIconTheme *mainIconTheme=gtk_icon_theme_get_for_screen(mainScreen);

    logo = gtk_icon_theme_load_icon(mainIconTheme, CAMORAMA_STOCK_WEBCAM, 24, 0, NULL);
    gtk_window_set_default_icon(logo);

    logo = gtk_icon_theme_load_icon(mainIconTheme, "camorama", 48, 0, NULL);

//XXX: gtk_window_resize (GTK_WINDOW(glade_xml_get_widget(cam->xml, "main_window")), 320, 240);

glade_xml_signal_connect_data (cam->xml, "on_status_show",
                                   G_CALLBACK (on_status_show),
                                   (gpointer) cam);

    //XXX: cam->tray_icon = gtk_status_icon_new_from_stock (CAMORAMA_STOCK_WEBCAM);
    //XXX: update_tooltip (cam);

    /* add the status to the plug * /
    g_object_set_data (G_OBJECT (cam->tray_icon), "available",
                       GINT_TO_POINTER (1));
    g_object_set_data (G_OBJECT (cam->tray_icon), "embedded",
                       GINT_TO_POINTER (0));
    */

    //XXX: g_signal_connect (cam->tray_icon, "popup-menu", G_CALLBACK (tray_clicked_callback), cam);

    /* connect the signals in the interface 
     * glade_xml_signal_autoconnect(xml);
     * this won't work, can't pass data to callbacks.  have to do it individually :(*/

    gchar *title = g_strdup_printf ("%s - %dx%d", cam->vid_cap.name, cam->x, cam->y);
    gtk_window_set_title (GTK_WINDOW(mainWindow), title);
    g_free (title);

    gtk_window_set_icon (GTK_WINDOW(mainWindow), logo);

    glade_xml_signal_connect_data (cam->xml, "gtk_main_quit", G_CALLBACK(delete_event), NULL);

    // Menu
    glade_xml_signal_connect_data (cam->xml, "on_quit_activate", G_CALLBACK (on_quit_activate), (gpointer) cam);
    glade_xml_signal_connect_data (cam->xml, "on_about_activate", G_CALLBACK (on_about_activate), (gpointer) cam);

/* remote capture?
    glade_xml_signal_connect_data (cam->xml, "rcap_func",
                                   G_CALLBACK (rcap_func), cam);
    gtk_toggle_button_set_active ((GtkToggleButton *)
                                  glade_xml_get_widget (cam->xml,
                                                        "rcapture"),
                                  cam->rcap);

xxx: automatic capture?
    glade_xml_signal_connect_data (cam->xml, "acap_func",
                                   G_CALLBACK (acap_func), cam);
    gtk_toggle_button_set_active ((GtkToggleButton *)
                                  glade_xml_get_widget (cam->xml,
                                                        "acapture"),
                                  cam->acap);

    glade_xml_signal_connect_data (cam->xml, "interval_change",
                                   G_CALLBACK (interval_change), cam);

    gtk_spin_button_set_value ((GtkSpinButton *)
                               glade_xml_get_widget (cam->xml,
                                                     "interval_entry"),
                               (cam->timeout_interval / 60000));
*/

	//gtk_widget_set_size_request (glade_xml_get_widget(cam->xml, "viewfinder"), cam->x, cam->y);
	gtk_widget_set_size_request (glade_xml_get_widget(cam->xml, "viewfinder"), 1024, 1024*cam->y/cam->x);

}


