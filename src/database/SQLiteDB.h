/* Please note that the database handle is static, i.e. every class which
 * derives off of this will have the same handle. This is intentional, but
 * keep that in mind; it could cause trouble if you're not careful editing. */

#ifndef SQLITE_DB_H
#define SQLITE_DB_H

#include <cstdarg>

struct sqlite3;
struct sqlite3_stmt;

/* 'database' is tiresome to keep rewriting. */
class SQLiteDB
{
public:
	bool Open( const char *path );
	bool IsOpen() const { return s_pDB != NULL; }
	void Close();

protected:
	/* This is an abstract class, but we don't want to incur the
	 * overhead of virtual tables, so protect the constructor. */
	SQLiteDB();

	/* compiles a formatted query (think printf). Use %q for strings.
	 * Make sure to call sqlite3_finalize(stmt) when you're done. */
	sqlite3_stmt* Prepare( const char *fmt, ... );

	/* if bUnique, returns a single result, otherwise can be called
	 * in succession to iterate over the statement results. */
	const char* GetString( sqlite3_stmt *stmt, bool bUnique );

	/* like GetString, but for numbers. */
	int GetInt( sqlite3_stmt *stmt, bool bUnique );

	static sqlite3* s_pDB;
};

#endif // SQLITE_DB_H
