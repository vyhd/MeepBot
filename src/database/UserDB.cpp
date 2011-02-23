#include <sqlite3.h>
#include "UserDB.h"
#include "SQLite.h"

using namespace std;

UserDB::UserDB( sqlite3* db )
{
	m_pDB = db;

	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"CREATE TABLE IF NOT EXISTS %s("
		"username TEXT UNIQUE PRIMARY KEY COLLATE NOCASE, "
		"description TEXT, accessLevel INTEGER, isProtected INTEGER);",
		USER_TABLE );

	if( sqlite3_step(stmt) != SQLITE_DONE )
		printf( "error creating table: %s\n", sqlite3_errmsg(m_pDB) );

	sqlite3_finalize( stmt );
}

bool UserDB::GetUserEntry( const char *name, UserEntry *entry ) const
{
	if( name == NULL || entry == NULL )
		return false;

	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"SELECT * from %s WHERE username = %q LIMIT 1",
		USER_TABLE, name );

	if( stmt == NULL )
		return false;

	int ret = sqlite3_step(stmt);

	switch( ret )
	{
	case SQLITE_ROW:
		{
			AccessLevel l = AccessLevel( sqlite3_column_int(stmt, 2) );
			const char *desc = (const char*)sqlite3_column_text(stmt, 1);
			bool bLocked = sqlite3_column_int(stmt, 3) != 0;

			entry->desc = desc;
			entry->level = l;
			entry->locked = bLocked;
			break;
		}
	case SQLITE_DONE:
		printf( "user not found: %s\n", name );
		break;
	default:
		printf( "GetUserEntry: %s\n", sqlite3_errmsg(m_pDB) );
		break;
	}

	sqlite3_finalize( stmt );
	return ret == SQLITE_ROW;
}

AccessLevel UserDB::GetAccessLevel( const char *name ) const
{
	/* This is probably somewat wasteful, but much simpler. */
	UserEntry entry;

	if( !GetUserEntry( name, &entry) )
		return LEVEL_BANNED;

	return entry.level;
}

bool UserDB::SetAccessLevel( const char *name, AccessLevel level )
{
	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"SELECT isProtected FROM %s WHERE username = %q", 
		USER_TABLE, name );

	if( stmt == NULL || sqlite3_step(stmt) != SQLITE_ROW )
		return LEVEL_USER;

	bool bIsProtected = sqlite3_column_int( stmt, 0 );
	sqlite3_finalize( stmt );

	if( bIsProtected )
	{
		printf( "%s is protected, not setting level\n", name );
		return false;
	}

	/* update the access level */
	/* magic happens */

	return true;
}

const char* GetDescription( const char *name )
{
	/* magic happens */
	return NULL;
}

bool SetDescription( const char *name, const char *desc )
{
	/* magic happens */
	return true;
}
