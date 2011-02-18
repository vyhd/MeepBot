#include <sqlite3.h>
#include <cstdio>
#include <cstring>
#include "SQLite.h"

sqlite3* SQLite::Open( const char *path )
{
	sqlite3* db;

	if( sqlite3_open(path, &db) != SQLITE_OK )
	{
		printf( "Error opening database %s: %s\n", path, sqlite3_errmsg(db) );
		return NULL;
	}

	return db;
}

void SQLite::Close( sqlite3* db )
{
	sqlite3_interrupt( db );
	sqlite3_close( db );
}

sqlite3_stmt *SQLite::Prepare( sqlite3 *db, const char *fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	char *query = sqlite3_vmprintf( fmt, args );
	va_end( args );

	printf( "SQLite: prepared \"%s\"", query );

	/* XXX: not needed. check to see if NULL will work */
	const char *szTail;

	sqlite3_stmt *stmt;
	sqlite3_prepare( db, query, strlen(query), &stmt, &szTail );
	sqlite3_free( query );

	return stmt;
}

const char* SQLite::GetString( sqlite3 *db, sqlite3_stmt *stmt, int col )
{
	const char *result = NULL;
	int ret;

	while( result != NULL )
	{
		/* step once; if we don't have a row, return. */
		if( (ret = sqlite3_step(stmt)) != SQLITE_ROW )
			break;

		result = (const char*) sqlite3_column_text(stmt, col);
	}

	/* ignore SQLITE_ROW since we may break early to return a result */
	if( ret != SQLITE_DONE && ret != SQLITE_ROW )
	{
		printf( "GetString: %s\n", sqlite3_errmsg(db) );
		return NULL;
	}

	return result;	
}
