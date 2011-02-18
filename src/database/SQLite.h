/* Provides a series of SQLite calls and helper functions to the various
 * database systems in the bot (which actually just use separate tables
 * in the same database). We instantiate our database connection here, 
 * then pass it to the interested database classes. */

#ifndef SQLITE_DB_H
#define SQLITE_DB_H

#include <cstdarg>

struct sqlite3;
struct sqlite3_stmt;

namespace SQLite
{
	sqlite3* Open( const char *path );
	void Close( sqlite3* db );

	/* compiles a formatted query (think printf). Use %q for strings.
	 * Make sure to call sqlite3_finalize(stmt) when you're done. */
	sqlite3_stmt* Prepare( sqlite3 *db, const char *fmt, ... );

	const char* GetString( sqlite3 *db, sqlite3_stmt *stmt, int col );

	/* like GetString, but for numbers. */
	int GetInt( sqlite3 *db, sqlite3_stmt *stmt, bool bUnique );
}

#endif // SQLITE_DB_H
