/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 *  Copyright (C) 2010  Jonathan Matthew  <jonathan@d14n.org>
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

#include "config.h"

#include <gconf/gconf-client.h>

#include "rhythmdb-entry-type.h"
#include "rhythmdb-private.h"
#include "rb-preferences.h"
#include "rb-util.h"
#include "rb-debug.h"

static RhythmDBEntryType *song_entry_type = NULL;
static RhythmDBEntryType *error_entry_type = NULL;
static RhythmDBEntryType *ignore_entry_type = NULL;

static void
update_entry_last_seen (RhythmDB *db, RhythmDBEntry *entry)
{
	GTimeVal time;
	GValue val = {0, };

	g_get_current_time (&time);
	g_value_init (&val, G_TYPE_ULONG);
	g_value_set_ulong (&val, time.tv_sec);
	rhythmdb_entry_set_internal (db, entry, FALSE, RHYTHMDB_PROP_LAST_SEEN, &val);
	g_value_unset (&val);
}

static gboolean
check_entry_grace_period (RhythmDB *db, RhythmDBEntry *entry)
{
	GTimeVal time;
	gulong last_seen;
	gulong grace_period;
	GError *error;
	GConfClient *client;

	client = gconf_client_get_default ();
	if (client == NULL) {
		return FALSE;
	}
	error = NULL;
	grace_period = gconf_client_get_int (client, CONF_GRACE_PERIOD,
					     &error);
	g_object_unref (G_OBJECT (client));
	if (error != NULL) {
		g_error_free (error);
		return FALSE;
	}

	/* This is a bit silly, but I prefer to make sure we won't
	 * overflow in the following calculations
	 */
	if ((grace_period <= 0) || (grace_period > 20000)) {
		return FALSE;
	}

	/* Convert from days to seconds */
	grace_period = grace_period * 60 * 60 * 24;
	g_get_current_time (&time);
	last_seen = rhythmdb_entry_get_ulong (entry, RHYTHMDB_PROP_LAST_SEEN);

	return (last_seen + grace_period < time.tv_sec);
}

static void
song_sync_metadata (RhythmDBEntryType *entry_type,
		    RhythmDBEntry *entry,
		    GSList *changes,
		    GError **error)
{
	RhythmDB *db;

	g_object_get (entry_type, "db", &db, NULL);
	rhythmdb_entry_write_metadata_changes (db, entry, changes, error);
	g_object_unref (db);
}


static gboolean
song_can_sync_metadata (RhythmDBEntryType *entry_type,
			RhythmDBEntry *entry)
{
	const char *mimetype;
	gboolean can_sync;
	RhythmDB *db;

	g_object_get (entry_type, "db", &db, NULL);

	mimetype = rhythmdb_entry_get_string (entry, RHYTHMDB_PROP_MIMETYPE);
	can_sync = rb_metadata_can_save (db->priv->metadata, mimetype);

	g_object_unref (db);
	return can_sync;
}

static void
song_update_availability (RhythmDBEntryType *entry_type,
			  RhythmDBEntry *entry,
			  RhythmDBEntryAvailability avail)
{
	RhythmDB *db;

	g_object_get (entry_type, "db", &db, NULL);
	switch (avail) {
	case RHYTHMDB_ENTRY_AVAIL_MOUNTED:
		rhythmdb_entry_set_visibility (db, entry, TRUE);
		break;

	case RHYTHMDB_ENTRY_AVAIL_CHECKED:
		update_entry_last_seen (db, entry);
		rhythmdb_entry_set_visibility (db, entry, TRUE);
		break;

	case RHYTHMDB_ENTRY_AVAIL_UNMOUNTED:
		/* update the last-seen time if the entry is currently visible */
		if (rhythmdb_entry_get_boolean (entry, RHYTHMDB_PROP_HIDDEN) == FALSE) {
			update_entry_last_seen (db, entry);
		}
		rhythmdb_entry_set_visibility (db, entry, FALSE);
		break;

	case RHYTHMDB_ENTRY_AVAIL_NOT_FOUND:
		if (check_entry_grace_period (db, entry)) {
			rb_debug ("deleting entry %s; not seen for too long",
				  rhythmdb_entry_get_string (entry, RHYTHMDB_PROP_LOCATION));
			rhythmdb_entry_delete (db, entry);
		} else {
			rhythmdb_entry_set_visibility (db, entry, FALSE);
		}
		break;

	default:
		g_assert_not_reached ();
	}
	g_object_unref (db);
}

static void
import_error_update_availability (RhythmDBEntryType *entry_type,
				  RhythmDBEntry *entry,
				  RhythmDBEntryAvailability avail)
{
	RhythmDB *db;

	switch (avail) {
	case RHYTHMDB_ENTRY_AVAIL_MOUNTED:
	case RHYTHMDB_ENTRY_AVAIL_CHECKED:
		/* do nothing; should never happen anyway */
		break;
	case RHYTHMDB_ENTRY_AVAIL_UNMOUNTED:
	case RHYTHMDB_ENTRY_AVAIL_NOT_FOUND:
		/* no need to keep error entries if the file is unavailable */
		g_object_get (entry_type, "db", &db, NULL);
		rhythmdb_entry_delete (db, entry);
		g_object_unref (db);
		break;

	default:
		g_assert_not_reached ();
	}
}


/**
 * rhythmdb_get_song_entry_type:
 *
 * Returns the #RhythmDBEntryType for normal songs.
 *
 * Return value: (transfer none): the entry type for normal songs
 */
RhythmDBEntryType *
rhythmdb_get_song_entry_type (void)
{
	return song_entry_type;
}

/**
 * rhythmdb_get_ignore_entry_type:
 *
 * Returns the #RhythmDBEntryType for ignored files
 *
 * Return value: (transfer none): the entry type for ignored files
 */
RhythmDBEntryType *
rhythmdb_get_ignore_entry_type (void)
{
	return ignore_entry_type;
}

/**
 * rhythmdb_get_error_entry_type:
 *
 * Returns the #RhythmDBEntryType for import errors
 *
 * Return value: (transfer none): the entry type for import errors
 */
RhythmDBEntryType *
rhythmdb_get_error_entry_type (void)
{
	return error_entry_type;
}



void
rhythmdb_register_song_entry_types (RhythmDB *db)
{
	g_assert (song_entry_type == NULL);
	g_assert (error_entry_type == NULL);
	g_assert (ignore_entry_type == NULL);

	song_entry_type = g_object_new (RHYTHMDB_TYPE_ENTRY_TYPE,
					"db", db,
					"name", "song",
					"save-to-disk", TRUE,
					"has-playlists", TRUE,
					NULL);
	song_entry_type->can_sync_metadata = song_can_sync_metadata;
	song_entry_type->sync_metadata = song_sync_metadata;
	song_entry_type->update_availability = song_update_availability;

	ignore_entry_type = g_object_new (RHYTHMDB_TYPE_ENTRY_TYPE,
					  "db", db,
					  "name", "ignore",
					  "save-to-disk", TRUE,
					  "category", RHYTHMDB_ENTRY_VIRTUAL,
					  NULL);
	ignore_entry_type->get_playback_uri = (RhythmDBEntryTypeStringFunc) rb_null_function;
	ignore_entry_type->update_availability = song_update_availability;

	error_entry_type = g_object_new (RHYTHMDB_TYPE_ENTRY_TYPE,
					 "db", db,
					 "name", "import-error",
					 "category", RHYTHMDB_ENTRY_VIRTUAL,
					 NULL);
	error_entry_type->get_playback_uri = (RhythmDBEntryTypeStringFunc) rb_null_function;
	error_entry_type->can_sync_metadata = (RhythmDBEntryTypeBooleanFunc) rb_true_function;
	error_entry_type->sync_metadata = (RhythmDBEntryTypeSyncFunc) rb_null_function;
	error_entry_type->update_availability = import_error_update_availability;

	rhythmdb_register_entry_type (db, song_entry_type);
	rhythmdb_register_entry_type (db, error_entry_type);
	rhythmdb_register_entry_type (db, ignore_entry_type);
}
