/*
 * Neotonic ClearSilver Templating System
 *
 * This code is made available under the terms of the 
 * Neotonic ClearSilver License.
 * http://www.neotonic.com/clearsilver/license.hdf
 *
 * Copyright (C) 2001 by Brandon Long
 */

#ifndef __WDB_H_
#define __WDB_H_ 1

#include "skiplist.h"
#include "dict.h"
#include "ulist.h"
#include <db.h>

typedef struct _column
{
  char *name;
  int ondisk_index;       /* index# on disk, constant for life of db,
			     must be 1 or higher */
  int inmem_index;        /* load time specific, needs to be flushed on 
				 alter table */
  char type;
} WDBColumn;

typedef struct _row
{
  int table_version;     /* random number which maps to the same number
			    of the table defn when loaded to verify they 
			    match */
  char *key_value;
  int data_count;
  void *data[1];
} WDBRow;

typedef struct _cursor
{
  int table_version;     /* random number which maps to the same number
			    of the table defn when loaded to verify they 
			    match */
  DBC *db_cursor;
} WDBCursor;

typedef struct _wdb
{
  char *name;
  char *key;
  char *path;
  dictCtx attrs;
  dictCtx cols;
  skipList ondisk;
  ULIST *cols_l;
  DB *db;
  int last_ondisk;
  int defn_dirty;        /* must save defn on destroy */
  int table_version;     /* random number which maps to the same number
			    of the table defn when loaded/changed to
			    verify they match */
} WDB;


#define WDB_TYPE_STR 's'
#define WDB_TYPE_INT 'i'

#define WDBC_FIRST (1<<0)
#define WDBC_NEXT  (1<<1)
#define WDBC_FIND  (1<<2)

#define WDBR_INSERT (1<<0)

NEOERR * wdb_open (WDB **wdb, char *name, int flags);
NEOERR * wdb_save (WDB *wdb);
NEOERR * wdb_update (WDB *wdb, char *name, char *key);
NEOERR * wdb_create (WDB **wdb, char *path, char *name, char *key, 
                ULIST *col_def, int flags);
void wdb_destroy (WDB **wdb);
NEOERR * wdb_column_insert (WDB *wdb, int loc, char *key, char type);
NEOERR * wdb_column_delete (WDB *wdb, char *name);
NEOERR * wdb_column_update (WDB *wdb, char *oldkey, char *newkey);
NEOERR * wdb_column_exchange (WDB *wdb, char *key1, char *key2);

/*
 * function: wdb_keys
 * description: this function returns the key and column names for the
 *              current database
 * input: wdb - open database
 * output: primary_key - pointer to the primary key
 *         data - pointer to a ULIST of the columns.
 *         both of these are allocated structures, you can clear data
 *         with uListDestroy (data, ULIST_FREE)
 * return: STATUS_OK on no error or egerr.h error 
 */ 
NEOERR * wdb_keys (WDB *wdb, char **primary_key, ULIST **data);

NEOERR * wdb_attr_get (WDB *wdb, char *key, char **value);
NEOERR * wdb_attr_set (WDB *wdb, char *key, char *value);
NEOERR * wdb_attr_next (WDB *wdb, char **key, char **value);
NEOERR * wdbr_lookup (WDB *wdb, char *key, WDBRow **row);
NEOERR * wdbr_create (WDB *wdb, char *key, WDBRow **row);
NEOERR * wdbr_save (WDB *wdb, WDBRow *row, int flags);
NEOERR * wdbr_delete (WDB *wdb, char *key);
NEOERR * wdbr_destroy (WDB *wdb, WDBRow **row);
NEOERR * wdbr_get (WDB *wdb, WDBRow *row, char *key, void **value);
NEOERR * wdbr_set (WDB *wdb, WDBRow *row, char *key, void *value);
NEOERR * wdbr_dump (WDB *wdb, WDBRow *row);
NEOERR * wdbr_next (WDB *wdb, WDBCursor *cursor, WDBRow **row, int flags);
NEOERR * wdbr_find (WDB *wdb, WDBCursor *cursor, char *key, WDBRow **row);
NEOERR * wdbc_create (WDB *wdb, WDBCursor **cursor);
NEOERR * wdbc_destroy (WDB *wdb, WDBCursor **cursor);

#endif /* __WDB_H_ */
