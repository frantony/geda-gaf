/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2010 Ales Hvezda
 * Copyright (C) 1998-2011 gEDA Contributors (see ChangeLog for details)
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
/*! The code in this file is sometimes not obvious, especially
 * o_select_object (which implements the selection of objects either
 * when doing a single or multi select)
 *
 * Also, there are cases where it looks like there is redundant code, which
 * could be removed/merged, but I purposely didn't do so to keep the code
 * readable
 *
 * the count == 0 stuff really only applies to when you are coming from a
 * multi select case
 */
#include <config.h>

#include <math.h>
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "gschem.h"

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_select_run_hooks(GschemToplevel *w_current, OBJECT *o_current, int flag)
{
  switch (flag) {
  /* If flag == 0, then we are deselecting something. */
  case 0:
    g_run_hook_object (w_current, "%deselect-objects-hook", o_current);
    break;
  /* If flag == 1, then we are selecting something. */
  case 1:
    g_run_hook_object (w_current, "%select-objects-hook", o_current);
    break;
  default:
    g_assert_not_reached ();
  }
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 *  \note
 *  type can be either SINGLE meaning selection is a single mouse click 
 *      or it can be MULTIPLE meaning selection is a selection box
 */
void o_select_object(GschemToplevel *w_current, OBJECT *o_current,
		     int type, int count)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  int SHIFTKEY;
  int CONTROLKEY;
  int removing_obj = 0;

  SHIFTKEY = w_current->SHIFTKEY;
  CONTROLKEY = w_current->CONTROLKEY;

#if DEBUG 
  printf("OBJECT id: %d\n", o_current->sid);
#endif

  switch(o_current->selected) {

    case(FALSE): /* object not selected */

      switch(SHIFTKEY) { /* shift key pressed? */

        case(TRUE): /* shift key pressed */
          /* just fall through */
          break;

        case(FALSE):

          /* condition: first object being added */
          /* condition: control key not pressed */
          /* condition: for both multiple and single object added */
          /* result: remove all objects from selection */
          if (count == 0 && !CONTROLKEY) {
            o_select_unselect_all(w_current);
          }
          break;

      } /* end shift key switch */

      /* object not select, add it to the selection list */
      o_select_run_hooks( w_current, o_current, 1 );
      o_selection_add (toplevel,
                       toplevel->page_current->selection_list, o_current);

      break;


    case(TRUE): /* object was already selected */

      switch(SHIFTKEY) { /* shift key pressed ? */

        case(TRUE): /* shift key pressed */

          /* condition: not doing multiple */
          /* result: remove object from selection */
          if (type != MULTIPLE) {
            o_select_run_hooks( w_current, o_current, 0 );
            o_selection_remove (toplevel, toplevel->page_current->
                                            selection_list, o_current);
            removing_obj = 1;
          }

          break;

        case(FALSE): /* shift key not pressed */

          /* condition: doing multiple */
          /* condition: first object being added */
          /* condition: control key not pressed */
          /* 1st result: remove all objects from selection */
          /* 2nd result: add object to selection */
          if (type == MULTIPLE && count == 0 && !CONTROLKEY) {
            o_select_unselect_all (w_current);

            o_select_run_hooks( w_current, o_current, 1 );
            o_selection_add (toplevel,
                             toplevel->page_current->selection_list, o_current);
          }	

          /* condition: doing single object add */
          /* condition: control key not pressed */
          /* 1st result: remove all objects from selection */
          /* 2nd result: add object to selection list */
          if (type == SINGLE && !CONTROLKEY) {
            o_select_unselect_all (w_current);

            o_select_run_hooks (w_current, o_current, 1);
            o_selection_add (toplevel, toplevel->page_current->
                                         selection_list, o_current);
          }

          if (CONTROLKEY) {
            o_select_run_hooks(w_current, o_current, 0);
            o_selection_remove (toplevel, toplevel->page_current->
                                            selection_list, o_current);
            removing_obj = 1;
          }

          break;
      } 
      break; /* end object selected switch */
  }

  /* do the attributes */
  if (removing_obj) {
    /* Remove the invisible attributes from the object list as well,
     * so they don't remain selected without the user knowing.
     */
    o_attrib_deselect_invisible (w_current,
                                 toplevel->page_current->selection_list,
                                 o_current);
  } else {
    /* If the type is MULTIPLE (meaning a select box was/is being used), only
     * select invisible attributes on objects.  Otherwise attributes will be
     * "double selected", causing them to remain unselected if using
     * invert-selection (CONTROLKEY is pressed)
     */
    if( type == MULTIPLE) {
      o_attrib_select_invisible (w_current,
                                 toplevel->page_current->selection_list,
                                 o_current);
    } else {
      /* Select all attributes of the object for a single click select */
      o_attrib_add_selected (w_current, toplevel->page_current->selection_list,
                             o_current);
    }
  }
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
int o_select_box_start(GschemToplevel *w_current, int w_x, int w_y)
{
  int diff_x, diff_y, dist;

  diff_x = abs(w_current->first_wx - w_x);
  diff_y = abs(w_current->first_wy - w_y);

  /* if we are still close to the button press location,
     then don't enter the selection box mode */
  dist = gschem_page_view_SCREENabs (GSCHEM_PAGE_VIEW (w_current->drawing_area), max(diff_x, diff_y));

  if (dist < 10) {
    return FALSE;
  }

  w_current->second_wx = w_x;
  w_current->second_wy = w_y;
  return TRUE;
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_select_box_end(GschemToplevel *w_current, int w_x, int w_y)
{
  o_select_box_invalidate_rubber (w_current);
  w_current->rubber_visible = 0;

  o_select_box_search(w_current);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_select_box_motion (GschemToplevel *w_current, int w_x, int w_y)
{
  if (w_current->rubber_visible)
    o_select_box_invalidate_rubber (w_current);
    
  w_current->second_wx = w_x; 
  w_current->second_wy = w_y;

  o_select_box_invalidate_rubber (w_current);
  w_current->rubber_visible = 1;
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 */
void o_select_box_invalidate_rubber (GschemToplevel *w_current)
{
  g_return_if_fail (w_current != NULL);

  gschem_page_view_invalidate_world_rect (GSCHEM_PAGE_VIEW (w_current->drawing_area),
                                          w_current->first_wx,
                                          w_current->first_wy,
                                          w_current->second_wx,
                                          w_current->second_wy);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_select_box_draw_rubber (GschemToplevel *w_current, EdaRenderer *renderer)
{
  o_box_draw_rubber (w_current, renderer);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_select_box_search(GschemToplevel *w_current)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  OBJECT *o_current=NULL;
  int count = 0; /* object count */
  int SHIFTKEY = w_current->SHIFTKEY;
  int CONTROLKEY = w_current->CONTROLKEY;
  int left, right, top, bottom;
  const GList *iter;
	
  left = min(w_current->first_wx, w_current->second_wx);
  right = max(w_current->first_wx, w_current->second_wx);
  top = min(w_current->first_wy, w_current->second_wy);
  bottom = max(w_current->first_wy, w_current->second_wy);

  iter = s_page_objects (toplevel->page_current);
  while (iter != NULL) {
    o_current = iter->data;
    /* only select visible objects */
    if (o_is_visible (toplevel, o_current) || toplevel->show_hidden_text) {
      int cleft, ctop, cright, cbottom;

      if ( world_get_single_object_bounds(toplevel, o_current,
                                          &cleft, &ctop, &cright, &cbottom) &&
           cleft   >= left &&
           cright  <= right  &&
           ctop    >= top  &&
           cbottom <= bottom ) {

        o_select_object(w_current, o_current, MULTIPLE, count);
        count++;
      }
    }
    iter = g_list_next (iter);
  }

  /* if there were no objects to be found in select box, count will be */
  /* zero, and you need to deselect anything remaining (except when the */
  /* shift or control keys are pressed) */
  if (count == 0 && !SHIFTKEY && !CONTROLKEY) {
    o_select_unselect_all (w_current);
  }
  i_update_menus(w_current);
}

/*! \brief Select all nets connected to the current net
 *  \par Depending on the state of the w_current->net_selection_mode variable
 *   and the net_selection_state of the current net this function will either
 *   select the single net, all directly connected nets or all nets connected
 *   with netname labels.
 *  \param [in] w_current  GschemToplevel struct.
 *  \param [in] o_net      Pointer to a single net object
 */
void o_select_connected_nets(GschemToplevel *w_current, OBJECT* o_net)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  const GList *o_iter;
  GList *iter1;
  OBJECT *o_current;
  int count=0;
  gchar* netname;

  GList *netstack = NULL;
  GList *netnamestack = NULL;
  GList *netnameiter;

  g_assert(o_net->type == OBJ_NET);

  if (!o_net->selected) {
    w_current->net_selection_state = 1;
  }

  /* the current net is the startpoint for the stack */
  netstack = g_list_prepend(netstack, o_net);

  count = 0; 
  while (1) {
    netnameiter = g_list_last(netnamestack);
    for (iter1 = g_list_last(netstack);
	 iter1 != NULL; 
	 iter1 = g_list_previous(iter1), count++) {
      o_current = iter1->data;
      if (o_current->type == OBJ_NET && 
	  (!o_current->selected || count == 0)) {
	o_select_object (w_current, o_current, SINGLE, count);
	if (w_current->net_selection_state > 1) {
	  /* collect nets */
	  netstack = g_list_concat(s_conn_return_others(NULL, o_current), netstack);
	}
	if (w_current->net_selection_state > 2) {
	  /* collect netnames */
	  netname = o_attrib_search_object_attribs_by_name (o_current, "netname", 0);
	  if (netname != NULL) {
	    if (g_list_find_custom(netnamestack, netname, (GCompareFunc) strcmp) == NULL) {
	      netnamestack = g_list_append(netnamestack, netname);
	    }
	    else {
	      g_free(netname);
	    }
	  }
	}
      }
    }
    g_list_free(netstack);
    netstack = NULL;

    if (netnameiter == g_list_last(netnamestack))
      break; /* no new netnames in the stack --> finished */

    /* get all the nets of the stacked netnames */
    for (o_iter = s_page_objects (toplevel->page_current);
         o_iter != NULL;
         o_iter = g_list_next (o_iter)) {
      o_current = o_iter->data;
      if (o_current->type == OBJ_TEXT
	  && o_current->attached_to != NULL) {
	if (o_current->attached_to->type == OBJ_NET) {
	  netname = o_attrib_search_object_attribs_by_name (o_current->attached_to, "netname", 0);
	  if (netname != NULL) {
	    if (g_list_find_custom(netnamestack, netname, (GCompareFunc) strcmp) != NULL) {
	      netstack = g_list_prepend(netstack, o_current->attached_to);
	    }
	    g_free(netname);
	  }
	}
      }
    }
  }

  w_current->net_selection_state += 1;
  if (w_current->net_selection_state > w_current->net_selection_mode)
    w_current->net_selection_state = 1;

  for (iter1 = netnamestack; iter1 != NULL; iter1 = g_list_next(iter1))
    g_free(iter1->data);
  g_list_free(netnamestack);
}

/* This is a wrapper for o_selection_return_first_object */
/* This function always looks at the current page selection list */ 
OBJECT *o_select_return_first_object(GschemToplevel *w_current)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  if (! (w_current && toplevel->page_current && geda_list_get_glist( toplevel->page_current->selection_list )))
    return NULL;
  else
    return (OBJECT *)g_list_first( geda_list_get_glist( toplevel->page_current->selection_list ))->data;
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 * \return TRUE if the selection list is not empty, otherwise false.
 * also make sure item is valid
 */
int o_select_selected(GschemToplevel *w_current)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  if ( geda_list_get_glist( toplevel->page_current->selection_list )) {
    return(TRUE);
  }
  return(FALSE);
}


/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void o_select_unselect_all(GschemToplevel *w_current)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  SELECTION *selection = toplevel->page_current->selection_list;
  GList *removed = NULL;
  GList *iter;

  removed = g_list_copy (geda_list_get_glist (selection));
  for (iter = removed; iter != NULL; iter = g_list_next (iter)) {
    o_selection_remove (toplevel, selection, (OBJECT *) iter->data);
  }

  /* Call hooks */
  if (removed != NULL) {
    g_run_hook_object_list (w_current, "%deselect-objects-hook", removed);
  }
}

/*! \brief Selects all visible objects on the current page.
 * \par Function Description
 * Clears any existing selection, then selects everything visible and
 * unlocked on the current page, and any attached attributes whether
 * visible or invisible..
 *
 * \param w_current  The current #GschemToplevel structure.
 */
void
o_select_visible_unlocked (GschemToplevel *w_current)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  SELECTION *selection = toplevel->page_current->selection_list;
  const GList *iter;
  GList *added;

  o_select_unselect_all (w_current);
  for (iter = s_page_objects (toplevel->page_current);
       iter != NULL;
       iter = g_list_next (iter)) {
    OBJECT *obj = (OBJECT *) iter->data;

    /* Skip invisible objects. */
    if (!o_is_visible (toplevel, obj) && !toplevel->show_hidden_text)
      continue;

    /* Skip locked objects. */
    if (!obj->selectable) continue;

    /* Add object to selection. */
    /*! \bug We can't call o_select_object() because it
     * behaves differently depending on the state of
     * w_current->SHIFTKEY and w_current->CONTROLKEY, which may well
     * be set if this function is called via a keystroke
     * (e.g. Ctrl-A). */
    o_selection_add (toplevel, selection, obj);

    /* Add any attributes of object to selection as well. */
    o_attrib_add_selected (w_current, selection, obj);
  }

  /* Run hooks for all items selected */
  added = geda_list_get_glist (selection);
  if (added != NULL) {
    g_run_hook_object_list (w_current, "%select-objects-hook", added);
  }
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
void
o_select_move_to_place_list(GschemToplevel *w_current)
{
  TOPLEVEL *toplevel = gschem_toplevel_get_toplevel (w_current);
  GList *selection;
  GList *selection_copy;

  /* remove the old place list if it exists */
  s_delete_object_glist(toplevel, toplevel->page_current->place_list);
  toplevel->page_current->place_list = NULL;

  selection = geda_list_get_glist( toplevel->page_current->selection_list );
  selection_copy = g_list_copy( selection );
  toplevel->page_current->place_list = selection_copy;
}
