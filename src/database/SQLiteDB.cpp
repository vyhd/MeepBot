#include <sqlite3.h>
#include <cstdio>
#include <cstring>
#include "SQLiteDB.h"

sqlite3* SQLiteDB::s_pDB = NULL;

SQLiteDB::SQLiteDB()
{
}

bool SQLiteDB::Open( const char *path )
{
	if( sqlite3_open(path, &s_pDB) != SQLITE_OK )
	{
		printf( "Error opening database %s: %s\n", path, sqlite3_errmsg(s_pDB) );
		return false;
	}

	return true;
}

void SQLiteDB::Close()
{
	sqlite3_interrupt( s_pDB );
	sqlite3_close( s_pDB );
	s_pDB = NULL;
}

sqlite3_stmt *SQLiteDB::Prepare( const char *fmt, ... )
{
	if( !IsOpen() )
		return NULL;

	va_list args;
	va_start( args, fmt );
	char *query = sqlite3_vmprintf( fmt, args );
	va_end( args );

	const char *szTail;	/* XXX: not needed. check to see if NULL will work */

	sqlite3_stmt *stmt;
	sqlite3_prepare( s_pDB, query, strlen(query), &stmt, &szTail );
	sqlite3_free( query );

	return stmt;
}

const char* SQLiteDB::GetString( sqlite3_stmt *stmt, bool bUnique )
{
	const char *result = NULL;
	int ret, results = 0;

	/* we can only handle one-column results */
	if( sqlite3_column_count(stmt) != 1 )
	{
		printf( "GetString: cannot use multiple columns!\n" );
		return NULL;
	}

	while( result != NULL )
	{
		/* step once; if we don't have a row, return. */
		if( (ret = sqlite3_step(stmt)) != SQLITE_ROW )
			break;

		result = (const char*) sqlite3_column_text(stmt, 0);

		/* if we want this result, just return it */
		if( !bUnique )
			break;

		++results;
		if( results > 1 )
		{
			printf( "GetString: not unique query\n" );
			return NULL;
		}

		/* see if any other results were returned for this query */
	}

	/* ignore SQLITE_ROW since we may break early to return a result */
	if( ret != SQLITE_DONE && ret != SQLITE_ROW )
	{
		printf( "GetString: %s\n", sqlite3_errmsg(s_pDB) );
		return NULL;
	}

	return result;	
}
