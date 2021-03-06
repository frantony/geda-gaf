/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2010 gEDA Contributors (see ChangeLog for details)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*! \file x_colorcb.c
 *
 *  \brief A GtkComboBox with the gschem colors.
 */
#include <config.h>
#include <version.h>
#include <missing.h>

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "gschem.h"




/*! \brief The columns in the GtkListStore
 */
enum
{
    COLUMN_NAME,
    COLUMN_INDEX,
    COLUMN_COLOR,
    COLUMN_COUNT
};



/*! \brief Stores the list of colors for use in GtkComboBox
 */
static GtkListStore* color_list_store = NULL;



static GtkListStore* create_color_list_store();
static char* get_color_name(int index);



/*! \brief Create the GtkListStore of the avaialble colors.
 */
static GtkListStore*
create_color_list_store ()
{
  int color_index;
  GtkTreeIter iter;
  GtkListStore *store;

  store = gtk_list_store_new (COLUMN_COUNT, G_TYPE_STRING, G_TYPE_INT, GDK_TYPE_COLOR);

  for (color_index = 0; color_index < MAX_COLORS; color_index++) {
    if (x_color_display_enabled (color_index)) {
      gtk_list_store_append (store, &iter);

      gtk_list_store_set (store, &iter,
          COLUMN_NAME,  get_color_name (color_index),
          COLUMN_INDEX, color_index,
          COLUMN_COLOR, x_get_color (color_index),
          -1
          );
    }
  }

  return store;
}



/*! \brief Given the color index, obtain a human readable name
 */
static char*
get_color_name (int index)
{
  switch(index) {
    case BACKGROUND_COLOR:         return _("Background");
    case PIN_COLOR:                return _("Pin");
    case NET_ENDPOINT_COLOR:       return _("Net endpoint");
    case GRAPHIC_COLOR:            return _("Graphic");
    case NET_COLOR:                return _("Net");
    case ATTRIBUTE_COLOR:          return _("Attribute");
    case LOGIC_BUBBLE_COLOR:       return _("Logic bubble");
    case DOTS_GRID_COLOR:          return _("Grid point");
    case DETACHED_ATTRIBUTE_COLOR: return _("Detached attribute");
    case TEXT_COLOR:               return _("Text");
    case BUS_COLOR:                return _("Bus");
    case SELECT_COLOR:             return _("Selection");
    case BOUNDINGBOX_COLOR:        return _("Bounding box");
    case ZOOM_BOX_COLOR:           return _("Zoom box");
    case STROKE_COLOR:             return _("Stroke");
    case LOCK_COLOR:               return _("Lock");
    case OUTPUT_BACKGROUND_COLOR:  return _("Output background");
    case JUNCTION_COLOR:           return _("Net junction");
    case MESH_GRID_MAJOR_COLOR:    return _("Mesh grid major");
    case MESH_GRID_MINOR_COLOR:    return _("Mesh grid minor");
    default:
      break;
  }
  return _("Unknown");
}


/*! \brief Create a ComboBox with the gschem colors.
 *
 *  \return The currently selected color index
 */
GtkWidget*
x_colorcb_new ()
{
  GtkComboBox *combo;
  GtkCellLayout *layout;
  GtkCellRenderer *text_cell;
  GtkCellRenderer *color_cell;

  if (color_list_store == NULL) {
    color_list_store = create_color_list_store ();
  }

  combo = GTK_COMBO_BOX (gtk_combo_box_new_with_model (GTK_TREE_MODEL (color_list_store)));
  layout = GTK_CELL_LAYOUT (combo); /* For convenience */

  /* Renders the color swatch. Since this won't contain text, set a
   * minimum width. */
  color_cell = GTK_CELL_RENDERER (gschem_swatch_column_renderer_new ());
  g_object_set (color_cell, "width", 25, NULL);
  gtk_cell_layout_pack_start (layout, color_cell, FALSE);
  gtk_cell_layout_add_attribute (layout, color_cell, "color", COLUMN_COLOR);

  /* Renders the name of the color */
  text_cell = GTK_CELL_RENDERER (gtk_cell_renderer_text_new());
  g_object_set (text_cell, "xpad", 5, NULL);
  gtk_cell_layout_pack_start (layout, text_cell, TRUE);
  gtk_cell_layout_add_attribute (layout, text_cell, "text", COLUMN_NAME);

  return GTK_WIDGET (combo);
}



/*! \brief Get the currently selected color index
 *
 *  \param [in,out] widget      The color combo box
 *  \return The currently selected color index
 */
int
x_colorcb_get_index (GtkWidget *widget)
{
  int index = -1;
  GtkTreeIter iter;
  GValue value = {0};

  if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (widget), &iter)) {
    gtk_tree_model_get_value (GTK_TREE_MODEL (color_list_store), &iter, COLUMN_INDEX, &value);
    index = g_value_get_int (&value);
    g_value_unset (&value);
  }

  return index;
}



/*! \brief Select the given color index
 *
 *  \param [in,out] widget      The color combo box
 *  \param [in]     color_index The color index to select
 */
void
x_colorcb_set_index (GtkWidget *widget, int color_index)
{
  g_return_if_fail (color_list_store != NULL);

  if (color_index >= 0) {
    GtkTreeIter iter;
    gboolean success = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (color_list_store), &iter);
    GValue value = {0};
    while (success) {
      gtk_tree_model_get_value (GTK_TREE_MODEL (color_list_store), &iter, COLUMN_INDEX, &value);
      if (g_value_get_int (&value) == color_index) {
        g_value_unset (&value);
        gtk_combo_box_set_active_iter (GTK_COMBO_BOX(widget), &iter);
        break;
      }
      g_value_unset (&value);
      success = gtk_tree_model_iter_next (GTK_TREE_MODEL(color_list_store), &iter);
    }
  }
  else {
    gtk_combo_box_set_active_iter (GTK_COMBO_BOX(widget), NULL);
  }
}
