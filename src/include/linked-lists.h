/*
  Pacman Arena
  Copyright (C) 2003 Nuno Subtil

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/* $Id: linked-lists.h,v 1.1 2003/11/22 16:22:08 nsubtil Exp $ */

#ifndef _LINKED_LISTS_H
#define _LINKED_LISTS_H

/*
 * Macros for handling linked lists
 */

/* adds element to head of list */
#define LLIST_ADD(TYPE, root, new) \
if(root == NULL) {\
  root = new; \
  new->next = NULL; \
} else {\
  new->next = root; \
  root = new; \
}

/* removes element from list */
#define LLIST_REMOVE(TYPE, root, remove) \
if(root == remove) {\
  root = remove->next; \
} else {\
  TYPE *_llist_cur, *_llist_prev; \
  _llist_prev = root; \
  _llist_cur = _llist_prev->next; \
  while(_llist_cur) {\
    if(_llist_cur == remove) {\
      _llist_prev->next = _llist_cur->next; \
      break; \
    }\
    _llist_prev = _llist_cur; \
    _llist_cur = _llist_cur->next; \
  }\
  printf("LLIST_REMOVE: %p not found at %p", remove, root);\
  SDL_Quit();\
  exit(1);\
}

#endif /* ifndef _LINKED_LISTS_H */
