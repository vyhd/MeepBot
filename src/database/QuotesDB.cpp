#include <sqlite3.h>
#include "QuotesDB.h"
#include "SQLite.h"
#include <cstdio>
#include <cstring>

/* SQL layout:
 * id (INTEGER): unique quote ID.
 * quote (TEXT): the quote itself.
 * addedBy (TEXT): the person who added the quote.
 */

QuotesDB::QuotesDB( sqlite3 *db ) : m_pDB(db)
{
	/* we're assuming the database is open at this point. */
	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"CREATE TABLE IF NOT EXISTS %s(id INTEGER PRIMARY KEY AUTOINCREMENT, quote TEXT, addedBy TEXT);",
		QUOTES_TABLE );

	if( sqlite3_step(stmt) != SQLITE_DONE )
		printf( "error creating table: %s\n", sqlite3_errmsg(m_pDB) );

	sqlite3_finalize( stmt );
}

/* gets the first quote from the results, assuming 'quote' is in column 0. */
static const char* GetFirstQuote( sqlite3 *db, sqlite3_stmt *stmt )
{
	int status = sqlite3_step( stmt );
	char *ret = NULL;

	if( status == SQLITE_ROW )
	{
		const char *val = (const char*)sqlite3_column_text( stmt, 0 );
		unsigned len = sqlite3_column_bytes(stmt, 0);

		ret = new char[len + 1];
		strncpy( ret, val, len );

		/* XXX: strncpy doesn't seem to do this for us. The chat 
		 * server freaks out and does terrible things when it's sent
		 * garbage data. We need to trace/fix that ASAP. */
		ret[len] = '\0';
	}
	else if( status != SQLITE_DONE )
	{
		printf( "GetQuote: %s\n", sqlite3_errmsg(db) );
	}

	return ret;
}

const char* QuotesDB::GetQuoteByID( int iQuoteID ) const
{
	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"SELECT quote FROM %s WHERE id = %d;",
		QUOTES_TABLE, iQuoteID );

	const char *ret = GetFirstQuote( m_pDB, stmt );
	sqlite3_finalize( stmt );

	return ret;
}

int QuotesDB::GetIDByPattern( const char *pattern ) const
{
	int ret = -1;

	if( pattern == NULL )
		return ret;

	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"SELECT id FROM %s WHERE quote LIKE '%%%q%%' ORDER BY RANDOM() LIMIT 1;",
		QUOTES_TABLE, pattern );

	int status = sqlite3_step( stmt );

	if( status == SQLITE_ROW )
		ret = sqlite3_column_int( stmt, 0 );
	else
		printf( "No match for pattern \"%s\"", pattern );
	
	sqlite3_finalize( stmt );

	return ret;
}

const char* QuotesDB::GetQuoteAuthor( int iQuoteID ) const
{
	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"SELECT addedBy from %s WHERE id = %d;",
		QUOTES_TABLE, iQuoteID );

	char *ret = NULL;

	switch( sqlite3_step(stmt) )
	{
	case SQLITE_ROW:
		{
			const char *result = (const char*)sqlite3_column_text(stmt, 0);
			unsigned len = sqlite3_column_bytes( stmt, 0 );
			ret = new char[len + 1];

			strncpy( ret, result, len );

			/* XXX: strncpy doesn't seem to do this for us. The
			 * chat server freaks out and does terrible things
			 * when it's sent garbage data. We need to trace
			 * and fix that ASAP. */
			ret[len] = '\0';
		}
		break;
	case SQLITE_DONE:
		printf( "GetQuoteAuthor: quote doesn't exist\n" );
		break;
	default:
		printf( "GetQuoteAuthor: %s\n", sqlite3_errmsg(m_pDB) );
		break;
	}

	return ret;
}

int QuotesDB::GetRandomID() const
{
	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"SELECT id FROM %s ORDER BY RANDOM() LIMIT 1;",
		QUOTES_TABLE );

	int idx = -1;	/* invalid sentinel */

	/* We should have a result here. */

	if( sqlite3_step(stmt) == SQLITE_ROW )
		idx = sqlite3_column_int( stmt, 0 );
	else
		printf( "GetRandomQuoteID: %s\n", sqlite3_errmsg(m_pDB) );

	sqlite3_finalize( stmt );

	return idx;
}

bool QuotesDB::AddQuote( const char *adder, const char *quote )
{
	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"INSERT INTO %s (id,quote,AddedBy) VALUES(NULL,'%q','%q');",
		QUOTES_TABLE, quote, adder );

	int ret = sqlite3_step(stmt);
	if( ret != SQLITE_DONE )
		printf( "AddQuote: %s\n", sqlite3_errmsg(m_pDB) );

	sqlite3_finalize( stmt );
	return ret == SQLITE_DONE;
}

bool QuotesDB::RemoveQuote( int iQuoteID )
{
	sqlite3_stmt *stmt = SQLite::Prepare( m_pDB,
		"DELETE FROM %s WHERE id = %d;",
		QUOTES_TABLE, iQuoteID
		);

	int ret = sqlite3_step(stmt);

	if( ret != SQLITE_DONE )
		printf( "RemoveQuote: %s\n", sqlite3_errmsg(m_pDB) );

	sqlite3_finalize( stmt );
	return ret == SQLITE_DONE;
}
