/* gEDA - GPL Electronic Design Automation
 * gschem - gEDA Schematic Capture
 * Copyright (C) 1998-2000 Ales V. Hvezda
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 */
#include <config.h>
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <math.h>

#include <libgeda/libgeda.h>

#include "../include/globals.h"
#include "../include/prototype.h"

#ifdef HAVE_LIBDMALLOC
#include <dmalloc.h>
#endif

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
/* Makes a list of all attributes currently connected to curr_object. *
 * Principle stolen from o_attrib_return_attribs */
SCM g_make_attrib_smob_list(TOPLEVEL *curr_w, OBJECT *curr_object)
{
  ATTRIB *a_current;      
  OBJECT *object;
  SCM smob_list = SCM_EOL;

  object = (OBJECT *) o_list_search(curr_object, curr_object);

  if (!object) {
    return(SCM_EOL);   
  }

  if (!object->attribs) {
    return(SCM_EOL);
  }

  if (!object->attribs->next) {
    return(SCM_EOL);
  }

  /* go through attribs */
  a_current = object->attribs->next;      
  while(a_current != NULL) {
    if (a_current->object->type == OBJ_TEXT && 
        a_current->object->text) {
      if (a_current->object->text->string) {
        smob_list = scm_cons (g_make_attrib_smob (curr_w, a_current), 
                              smob_list);
      }
    } else {
      printf(_("Attribute failed ot find.\n"));
    }
    a_current = a_current->next;
  }

  return smob_list;
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
/**************************************************************************
 * This function partly part of libgeda, since it belongs to the smob     *
 * definition. But since I use o_text_change, which is defined in gschem, *
 * we have to do it like this.                                            *
 **************************************************************************/
SCM g_set_attrib_value_x(SCM attrib_smob, SCM scm_value)
{
  SCM returned;
  TOPLEVEL *world;
  OBJECT *o_attrib;
  char *new_string;

  returned = g_set_attrib_value_internal(attrib_smob, scm_value, 
                                         &world, &o_attrib, &new_string);

  o_text_change(world, o_attrib, new_string, o_attrib->visibility, o_attrib->show_name_value);

  g_free(new_string);

  return returned;
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
/*
 * Adds an attribute <B>scm_attrib_name</B> with value <B>scm_attrib_value</B> to the given <B>object</B>.
The attribute has the visibility <B>scm_vis</B> and show <B>scm_show</B> flags.
The possible values are:
  - <B>scm_vis</B>: scheme boolean. Visible (TRUE) or hidden (FALSE).
  - <B>scm_show</B>: a list containing what to show: "name", "value", or both.
The return value is always TRUE.
 */
SCM g_add_attrib(SCM object, SCM scm_attrib_name, 
		 SCM scm_attrib_value, SCM scm_vis, SCM scm_show)
{
  TOPLEVEL *w_current=NULL; 
  OBJECT *o_current=NULL;
  gboolean vis;
  int show=0;
  gchar *attrib_name=NULL;
  gchar *attrib_value=NULL;
  gchar *value=NULL;
  int i;
  gchar *newtext=NULL;

  SCM_ASSERT (SCM_STRINGP(scm_attrib_name), scm_attrib_name,
	      SCM_ARG2, "add-attribute-to-object");
  SCM_ASSERT (SCM_STRINGP(scm_attrib_value), scm_attrib_value,
	      SCM_ARG3, "add-attribute-to-object");
  SCM_ASSERT (scm_boolean_p(scm_vis), scm_vis,
	      SCM_ARG4, "add-attribute-to-object");
  SCM_ASSERT (scm_list_p(scm_show), scm_show,
	      SCM_ARG5, "add-attribute-to-object");
  
  /* Get w_current and o_current */
  SCM_ASSERT (g_get_data_from_object_smob (object, &w_current, &o_current),
	      object, SCM_ARG1, "add-attribute-to-object");

  /* Get parameters */
  attrib_name = SCM_STRING_CHARS(scm_attrib_name);
  attrib_value = SCM_STRING_CHARS(scm_attrib_value);
  vis = SCM_NFALSEP(scm_vis);

  for (i=0; i<=SCM_INUM(scm_length(scm_show))-1; i++) {
    value = SCM_STRING_CHARS(scm_list_ref(scm_show, SCM_MAKINUM(i)));
    SCM_ASSERT(!((strcasecmp(value, "name") != 0) &&
		 (strcasecmp(value, "value") != 0) ), scm_show,
	       SCM_ARG5, "add-attribute-to-object"); 
    /* show = 1 => show value; 
       show = 2 => show name; 
       show = 3 => show both */
    if (strcasecmp(value, "value") == 0) {
      show |= 1;
    }
    else if (strcasecmp(value, "name") == 0) {
      show |= 2;
    }	  
  }
  /* Show name and value (show = 3) => show=0 for gschem */
  if (show == 3) {
    show = 0;
  }
  
  newtext = g_strdup_printf("%s=%s", attrib_name, attrib_value);
  o_attrib_add_attrib (w_current, newtext, vis, show, o_current);
  g_free(newtext);

  return SCM_BOOL_T;

}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
/*
 * Returns a list with coords of the ends of  the given pin <B>object</B>.
The list is ( (x0 y0) (x1 y1) ), where the beginning is at (x0,y0) and the end at (x1,y1). 
The active connection end of the pin is the beginning, so this function cares about the whichend property of the pin object. If whichend is 1, then it has to reverse the ends.
 */
SCM g_get_pin_ends(SCM object)
{
  TOPLEVEL *w_current;
  OBJECT *o_current;
  SCM coord1 = SCM_EOL;
  SCM coord2 = SCM_EOL;
  SCM coords = SCM_EOL;

  /* Get w_current and o_current */
  SCM_ASSERT (g_get_data_from_object_smob (object, &w_current, &o_current),
	      object, SCM_ARG1, "get-pin-ends");
  
  /* Check that it is a pin object */
  SCM_ASSERT (o_current != NULL,
	      object, SCM_ARG1, "get-pin-ends");
  SCM_ASSERT (o_current->type == OBJ_PIN,
	      object, SCM_ARG1, "get-pin-ends");
  SCM_ASSERT (o_current->line != NULL,
	      object, SCM_ARG1, "get-pin-ends");

  coord1 = scm_cons(SCM_MAKINUM(o_current->line->x[0]), 
		    SCM_MAKINUM(o_current->line->y[0]));
  coord2 = scm_cons(SCM_MAKINUM(o_current->line->x[1]),
		    SCM_MAKINUM(o_current->line->y[1]));
  if (o_current->whichend == 0) {
    coords = scm_cons(coord1, scm_list(coord2));
  } else {
    coords = scm_cons(coord2, scm_list(coord1));
  }    
		     
  return coords;  
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
/*
 * Sets several text properties of the given <B>attribute smob</B>:
  - <B>colorname</B>: The colorname of the text, or "" to keep previous color.
  - <B>size</B>: Size (numeric) of the text, or -1 to keep the previous size.
  - <B>alignment</B>: String with the alignment of the text. Possible values are:
    * ""           : Keep the previous alignment.
    * "Lower Left"
    * "Middle Left"
    * "Upper Left"
    * "Lower Middle"
    * "Middle Middle"
    * "Upper Middle"
    * "Lower Right"
    * "Middle Right"
    * "Upper Right"
  - <B>rotation</B>: Angle of the text, or -1 to keep previous angle.
  - <B>x</B>, <B>y</B>: Coords of the text.
 */
SCM g_set_attrib_text_properties(SCM attrib_smob, SCM scm_colorname,
				 SCM scm_size, SCM scm_alignment,
				 SCM scm_rotation, SCM scm_x, SCM scm_y)
{
  struct st_attrib_smob *attribute = 
  (struct st_attrib_smob *)SCM_CDR(attrib_smob);
  OBJECT *object;
  TOPLEVEL *w_current = (TOPLEVEL *) attribute->world;

  char *colorname=NULL;
  int color=0;
  int size = -1;
  char *alignment_string;
  int alignment = -2;
  int rotation = 0;
  int x = -1, y = -1;
  gboolean changed = FALSE;

  SCM_ASSERT (SCM_STRINGP(scm_colorname), scm_colorname,
	      SCM_ARG2, "set-attribute-text-properties!");
  SCM_ASSERT ( SCM_INUMP(scm_size),
               scm_size, SCM_ARG3, "set-attribute-text-properties!");
  SCM_ASSERT (SCM_STRINGP(scm_alignment), scm_alignment,
	      SCM_ARG4, "set-attribute-text-properties!");
  SCM_ASSERT ( SCM_INUMP(scm_rotation),
               scm_rotation, SCM_ARG5, "set-attribute-text-properties!");
  SCM_ASSERT ( SCM_INUMP(scm_x),
               scm_x, SCM_ARG6, "set-attribute-text-properties!");
  SCM_ASSERT ( SCM_INUMP(scm_y),
               scm_y, SCM_ARG7, "set-attribute-text-properties!");

  colorname = SCM_STRING_CHARS(scm_colorname);

  if (colorname && strlen(colorname) != 0) {
    SCM_ASSERT ( (color = s_color_get_index(colorname)) != -1,
		 scm_colorname, SCM_ARG2, "set-attribute-text-properties!");
  }
  else {
    color = -1;
  }
  
  size = SCM_INUM(scm_size);
  rotation = SCM_INUM(scm_rotation);
  x = SCM_INUM(scm_x);
  y = SCM_INUM(scm_y);
  
  alignment_string = SCM_STRING_CHARS(scm_alignment);

  if (strlen(alignment_string) == 0) {
    alignment = -1;
  }
  if (strcmp(alignment_string, "Lower Left") == 0) {
    alignment = 0;
  }
  if (strcmp(alignment_string, "Middle Left") == 0) {
    alignment = 1;
  }
  if (strcmp(alignment_string, "Upper Left") == 0) {
    alignment = 2;
  }
  if (strcmp(alignment_string, "Lower Middle") == 0) {
    alignment = 3;
  }
  if (strcmp(alignment_string, "Middle Middle") == 0) {
    alignment = 4;
  }
  if (strcmp(alignment_string, "Upper Middle") == 0) {
    alignment = 5;
  }
  if (strcmp(alignment_string, "Lower Right") == 0) {
    alignment = 6;
  }
  if (strcmp(alignment_string, "Middle Right") == 0) {
    alignment = 7;
  }
  if (strcmp(alignment_string, "Upper Right") == 0) {
    alignment = 8;
  }
  if (alignment == -2) {
    /* Bad specified */
    SCM_ASSERT (SCM_STRINGP(scm_alignment), scm_alignment,
		SCM_ARG4, "set-attribute-text-properties!");
  }

  if (attribute &&
      attribute->attribute &&
      attribute->attribute->object) {
    object = (OBJECT *) attribute->attribute->object;
    if (object &&
	object->text) {
      o_text_erase(w_current, object);
      if (x != -1) {
	object->text->x = x;
	changed = TRUE;
      }
      if (y != -1) {
	object->text->y = y;
	changed = TRUE;
      }
      if (changed) {
	WORLDtoSCREEN(w_current, x, y, &object->text->screen_x, &object->text->screen_y);
      }
      if (size != -1) {
	object->text->size = size;
	changed = TRUE;
      }
      if (alignment != -1) {
	object->text->alignment = alignment;
	changed = TRUE;
      }
      if (rotation != -1) {
	object->text->angle = rotation;
	changed = TRUE;
      }
      o_text_recreate(w_current, object);
      if (!w_current->DONT_REDRAW) {
	o_text_draw(w_current, object);
      }
    }
  }
  return SCM_BOOL_T;
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
/*! \bug FIXME
 *  Returns a list of the bounds of the <B>object smob</B>. 
 *  The list has the format: ( (left right) (top bottom) )
 *  I got top and bottom values reversed from world_get_complex_bounds,
 *  so don\'t rely on the position in the list. 
 */
SCM g_get_object_bounds (SCM object_smob, SCM scm_inc_attribs)
{
  TOPLEVEL *w_current=NULL;
  OBJECT *object=NULL;
  int left=0, right=0, bottom=0, top=0; 
  SCM returned = SCM_EOL;
  SCM vertical = SCM_EOL;
  SCM horizontal = SCM_EOL;
  OBJECT *new_object = NULL;
  gboolean include_attribs;

  SCM_ASSERT (scm_boolean_p(scm_inc_attribs), scm_inc_attribs,
	      SCM_ARG2, "get-object-bounds");
  include_attribs = SCM_NFALSEP(scm_inc_attribs);

  /* Get w_current and o_current */
  SCM_ASSERT (g_get_data_from_object_smob (object_smob, &w_current, &object),
	      object_smob, SCM_ARG1, "get-object-bounds");

  if (!include_attribs) {
    new_object = (OBJECT *) g_malloc(sizeof(OBJECT));
    memcpy (new_object, object, sizeof(OBJECT));
    new_object->attribs = NULL;
    new_object->next = NULL;
    new_object->prev = NULL;
  }
  else { 
    new_object = object;
  } 
    
  world_get_complex_bounds (w_current, new_object, 
			    &left, &top, &right, &bottom);
  
  if (!include_attribs) {
    /* Free the newly created object */
    g_free(new_object);
  }
  
  horizontal = scm_cons (SCM_MAKINUM(left), SCM_MAKINUM(right));
  vertical = scm_cons (SCM_MAKINUM(top), SCM_MAKINUM(bottom));
  returned = scm_cons (horizontal, vertical);
  return (returned);
}

/*! \todo Finish function documentation!!!
 *  \brief
 *  \par Function Description
 *
 */
/*
 *Returns a list of the pins of the <B>object smob</B>.
 */
SCM g_get_object_pins (SCM object_smob)
{
  TOPLEVEL *w_current=NULL;
  OBJECT *object=NULL;
  OBJECT *prim_obj;
  SCM returned=SCM_EOL;

  /* Get w_current and o_current */
  SCM_ASSERT (g_get_data_from_object_smob (object_smob, &w_current, &object),
	      object_smob, SCM_ARG1, "get-object-pins");

  if (!object) {
    return (returned);
  }
  if (object->complex && object->complex->prim_objs) {
    prim_obj = object->complex->prim_objs;
    while (prim_obj != NULL) {
      if (prim_obj->type == OBJ_PIN) {
	returned = scm_cons (g_make_object_smob(w_current, prim_obj),returned);
      }
      prim_obj = prim_obj->next;
    }
  }
  
  return (returned);
}