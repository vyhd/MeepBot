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

bool UserDB::HasUser( const char *name ) const
{
	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"SELECT * from %s WHERE username = %q;",
		USER_TABLE, name );

	if( !stmt )
		return false;

	int ret = sqlite3_step( stmt );
	sqlite3_finalize( stmt );

	/* if we had any results at all, we matched the name. */
	return ret == SQLITE_ROW;
}

bool UserDB::GetUserEntry( const char *name, UserEntry &entry ) const
{
	if( name == NULL )
		return false;

	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"SELECT * from %s WHERE username = %q LIMIT 1;",
		USER_TABLE, name );

	if( stmt == NULL )
		return false;

	int ret = sqlite3_step(stmt);

	switch( ret )
	{
	case SQLITE_ROW:
		entry.desc = (const char*)sqlite3_column_text(stmt, 1);
		entry.level = (AccessLevel)sqlite3_column_int(stmt, 2);
		entry.locked = sqlite3_column_int(stmt, 3) != 0;
		break;
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

bool UserDB::SetUserEntry( const char *name, const UserEntry &entry )
{
	printf( "name: %s, desc: %s, level: %d, locked: %d\n",
		name, entry.desc.c_str(), entry.level, int(entry.locked) );

	sqlite3_stmt *stmt = NULL;

	if( entry.desc.empty() )
	{
		stmt = SQLite::Prepare( m_pDB,
			"INSERT OR REPLACE INTO %s VALUES(%q,NULL,%d,%d);",
			USER_TABLE, name, entry.level, int(entry.locked) );
	}
	else
	{
		stmt = SQLite::Prepare( m_pDB,
			"INSERT OR REPLACE INTO %s VALUES(%q,%q,%d,%d);",
			USER_TABLE, name, entry.desc.c_str(),
			entry.level, int(entry.locked) );
	}

	if( !stmt )
		return false;

	int ret = sqlite3_step( stmt );
	sqlite3_finalize( stmt );
	return ret == SQLITE_DONE;
}

AccessLevel UserDB::GetAccessLevel( const char *name ) const
{
	/* This is probably somewat wasteful, but much simpler. */
	UserEntry entry;

	if( !GetUserEntry( name, entry) )
		return LEVEL_BANNED;

	return entry.level;
}

bool UserDB::SetAccessLevel( const char *name, AccessLevel level )
{
	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"SELECT isProtected FROM %s WHERE username = %q;", 
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

string GetDescription( const char *name )
{
	/* magic happens */
	return string();
}

bool SetDescription( const char *name, const char *desc )
{
	/* magic happens */
	return true;
}
