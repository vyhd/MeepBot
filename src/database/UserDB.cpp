#include <sqlite3.h>
#include <cstring>	/* for strlen */
#include "UserDB.h"

/* User DB layout (version 1):
 * CREATE TABLE users(
 *	userName TEXT UNIQUE PRIMARY KEY COLLATE NOCASE,
 *	description TEXT,
 *	accessLevel INTEGER,
 *	isProtected INTEGER)
 */

struct UserEntry
{
	AccessLevel level;
	string name, desc;
};

bool UserDB::GetUserEntry( const char *name, UserEntry *entry )
{
	if( name == NULL || entry == NULL )
		return false;

	sqlite3_stmt stmt = Prepare(
		"SELECT * from %s WHERE username = %q LIMIT 1",
		USER_TABLE, name );

	if( stmt == NULL )
		return false;

	/* magic happens */

	sqlite3_finalize( stmt );
	return true;
}

AccessLevel UserDB::GetAccessLevel( const char *name )
{
	sqlite3_stmt *stmt = Prepare( "SELECT accessLevel FROM users WHERE username = %q", username );

	/* if we didn't get the statement done, assume the worst :V */
	if( stmt == NULL )
		return LEVEL_USER;

	AccessLevel level = GetInt(stmt, false);
	sqlite3_finalize( stmt );

	return level;
}

bool UserDB::SetAccessLevel( const char *name, AccessLevel level )
{
	sqlite3_stmt *stmt = Prepare( "SELECT isProtected FROM users WHERE username = %q", username );

	if( stmt == NULL )
		return LEVEL_USER;

	bool bIsProtected = GetInt( stmt, true );
	sqlite3_finalize( stmt );

	if( bIsProtected )
		return false;

	/* update the access level */
	/* magic happens */

	return true;
}

const char* GetDescription( const char *name )
{
	sqlite3_stmt *stmt = Prepare( "SELECT description FROM users WHERE username = %q", name );

	if( stmt == NULL )
		return NULL;

	const char *result = GetString( stmt, true );
	sqlite3_finalize( stmt );

	return result;
}

bool SetDescription( const char *name, const char *desc )
{
	/* magic happens */
	return true;
}
