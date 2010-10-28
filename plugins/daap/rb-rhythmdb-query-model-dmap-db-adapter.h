/*
 *  Database adapter class for DMAP sharing
 *
 *  Copyright (C) 2008 W. Michael Petullo <mike@flyn.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  The Rhythmbox authors hereby grant permission for non-GPL compatible
 *  GStreamer plugins to be used and distributed together with GStreamer
 *  and Rhythmbox. This permission is above and beyond the permissions granted
 *  by the GPL license by which Rhythmbox is covered. If you modify this code
 *  you may extend this exception to your version of the code, but you are not
 *  obligated to do so. If you do not wish to do so, delete this exception
 *  statement from your version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA.
 *
 */

#ifndef __RB_RHYTHMDB_QUERY_MODEL_DMAP_DB_ADAPTER
#define __RB_RHYTHMDB_QUERY_MODEL_DMAP_DB_ADAPTER

#include <libdmapsharing/dmap.h>

G_BEGIN_DECLS

#define RB_TYPE_DMAP_DB_ADAPTER         (rb_rhythmdb_query_model_dmap_db_adapter_get_type ())
#define RB_RHYTHMDB_QUERY_MODEL_DMAP_DB_ADAPTER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), RB_TYPE_DMAP_DB_ADAPTER, RBRhythmDBQueryModelDMAPDbAdapter))
#define RB_RHYTHMDB_QUERY_MODEL_DMAP_DB_ADAPTER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), RB_TYPE_DMAP_DB_ADAPTER, RBRhythmDBQueryModelDMAPDbAdapterClass))
#define RB_IS_DMAP_DB_ADAPTER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), RB_TYPE_DMAP_DB_ADAPTER))
#define RB_IS_DMAP_DB_ADAPTER_CLASS (k) (G_TYPE_CHECK_CLASS_TYPE ((k), RB_TYPE_DMAP_DB_ADAPTER_CLASS))
#define RB_RHYTHMDB_QUERY_MODEL_DMAP_DB_ADAPTER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), RB_TYPE_DMAP_DB_ADAPTER, RBRhythmDBQueryModelDMAPDbAdapterClass))
#define RB_RHYTHMDB_QUERY_MODEL_DMAP_DB_ADAPTER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), RB_TYPE_DMAP_DB_ADAPTER, RBRhythmDBQueryModelDMAPDbAdapterPrivate))

typedef struct RBRhythmDBQueryModelDMAPDbAdapterPrivate RBRhythmDBQueryModelDMAPDbAdapterPrivate;

typedef struct {
	GObject parent;
	RBRhythmDBQueryModelDMAPDbAdapterPrivate *priv;
} RBRhythmDBQueryModelDMAPDbAdapter;

typedef struct {
	GObjectClass parent;
} RBRhythmDBQueryModelDMAPDbAdapterClass;

RBRhythmDBQueryModelDMAPDbAdapter *rb_rhythmdb_query_model_dmap_db_adapter_new (RhythmDBQueryModel *db);
GType rb_rhythmdb_query_model_dmap_db_adapter_get_type (void);

#endif /* _RB_RHYTHMDB_QUERY_MODEL_DMAP_DB_ADAPTER */

G_END_DECLS
