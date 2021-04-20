/*
 * xcpc-gtk3-priv.h - Copyright (c) 2001-2021 - Olivier Poncet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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
#ifndef __XCPC_GTK3_PRIV_H__
#define __XCPC_GTK3_PRIV_H__

#include <gtk/gtk.h>
#include <xcpc/machine/machine.h>
#include "xcpc-gtk3.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XcpcLayoutRec      XcpcLayoutRec;
typedef struct _XcpcApplicationRec XcpcApplicationRec;

struct _XcpcLayoutRec
{
    GtkApplication* application;
    GtkWidget*      window;
};

struct _XcpcApplicationRec
{
    XcpcMachine*  machine;
    XcpcLayoutRec layout;
};

#ifdef __cplusplus
}
#endif

#endif /* __XCPC_GTK3_PRIV_H__ */