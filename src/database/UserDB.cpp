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
		"description TEXT, accessLevel INTEGER, isProtected INTEGER, "
		"warnings INTEGER);", USER_TABLE );

	if( sqlite3_step(stmt) != SQLITE_DONE )
		printf( "error creating table: %s\n", sqlite3_errmsg(m_pDB) );

	sqlite3_finalize( stmt );
}

bool UserDB::AddUser( const char *name, AccessLevel level )
{
	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"INSERT OR IGNORE INTO %s VALUES('%q',NULL,%d,0,0);",
		USER_TABLE, name, level );

	if( !stmt )
	{
		printf( "AddUser: %s\n", sqlite3_errmsg(m_pDB) );
		return false;
	}

	int ret = sqlite3_step( stmt );

	if( ret != SQLITE_DONE )
		printf( "AddUser: %s\n", sqlite3_errmsg(m_pDB) );

	sqlite3_finalize( stmt );
	return ret == SQLITE_DONE;
}


bool UserDB::HasUser( const char *name ) const
{
	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"SELECT * from %s WHERE username = '%q';",
		USER_TABLE, name );

	if( !stmt )
		return false;

	int ret = sqlite3_step( stmt );
	sqlite3_finalize( stmt );

	/* if we had any results at all, we matched the name. */
	return ret == SQLITE_ROW;
}

bool UserDB::IsProtected( const char *name ) const
{
	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"SELECT isProtected FROM %s WHERE username = '%q';", 
		USER_TABLE, name );

	if( stmt == NULL || sqlite3_step(stmt) != SQLITE_ROW )
		return LEVEL_USER;

	bool bIsProtected = (bool)sqlite3_column_int( stmt, 0 );
	sqlite3_finalize( stmt );

	return bIsProtected;
}

bool UserDB::Protect( const char *name, bool bProtect )
{
	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"UPDATE %s SET isProtected = %d WHERE username = '%q';",
		USER_TABLE, int(bProtect), name );

	int ret = sqlite3_step( stmt );
	sqlite3_finalize( stmt );

	return ret == SQLITE_DONE;
}

bool UserDB::GetUserEntry( const char *name, UserEntry &entry ) const
{
	if( name == NULL )
		return false;

	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"SELECT * from %s WHERE username = '%q' LIMIT 1;",
		USER_TABLE, name );

	if( stmt == NULL )
		return false;

	int ret = sqlite3_step(stmt);

	switch( ret )
	{
	case SQLITE_ROW:
		{
		const char *desc = (const char*)sqlite3_column_text(stmt, 1);
		if( desc )
			entry.desc = desc;

		entry.level = (AccessLevel)sqlite3_column_int(stmt, 2);
		entry.locked = sqlite3_column_int(stmt, 3) != 0;
		entry.warnings = sqlite3_column_int(stmt, 4);
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

bool UserDB::SetUserEntry( const char *name, const UserEntry &entry )
{
	printf( "name: %s, desc: %s, level: %d, locked: %d\n",
		name, entry.desc.c_str(), entry.level, int(entry.locked) );

	if( IsProtected(name) )
	{
		printf( "SetUserEntry: protected, not setting.\n" );
		return false;
	}

	sqlite3_stmt *stmt = NULL;

	stmt = SQLite::Prepare( m_pDB,
		"INSERT OR REPLACE INTO %s VALUES('%q','%q',%d,%d,%d);",
		USER_TABLE, name, entry.desc.c_str(), entry.level, 
		int(entry.locked), entry.warnings );

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

	if( !GetUserEntry(name, entry) )
		return LEVEL_BANNED;

	return entry.level;
}

bool UserDB::SetAccessLevel( const char *name, AccessLevel level )
{
	if( IsProtected(name) )
	{
		printf( "SetAccessLevel: protected, not setting\n" );
		return false;
	}

	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"UPDATE %s SET accessLevel = %d WHERE username = '%q';",
		USER_TABLE, level, name );

	int ret = sqlite3_step( stmt );
	sqlite3_finalize( stmt );

	return ret == SQLITE_DONE;
}

string UserDB::GetDescription( const char *name ) const
{
	/* This is probably somewat wasteful, but much simpler. */
	UserEntry entry;

	if( !GetUserEntry(name, entry) )
		return string();

	return entry.desc;
}

bool UserDB::SetDescription( const char *name, const char *desc )
{
	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"UPDATE %s SET description='%q' WHERE username = '%q';",
		USER_TABLE, desc, name );

	int ret = sqlite3_step( stmt );

	if( ret != SQLITE_DONE )
		printf( "SetDesc: %s\n", sqlite3_errmsg(m_pDB) );

	return ret == SQLITE_DONE;
}

int UserDB::GetWarnings( const char *name ) const
{
	/* This is probably somewat wasteful, but much simpler. */
	UserEntry entry;

	if( !GetUserEntry(name, entry) )
		return -1;

	return entry.warnings;
}

bool UserDB::SetWarnings( const char *name, int warnings )
{
	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"UPDATE %s SET warnings=%d WHERE username = '%q';",
		USER_TABLE, warnings, name );

	if( !stmt )
	{
		printf( "SetWarnings: %s\n", sqlite3_errmsg(m_pDB) );
		return false;
	}

	int ret = sqlite3_step(stmt);
	sqlite3_finalize( stmt );

	return ret == SQLITE_DONE;
}
