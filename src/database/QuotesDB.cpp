#include "QuotesDB.h"

int QuoteDB::GetRandomQuoteID()
{
	if( !IsOpen() )
		return -1;

	sqlite3_stmt *stmt = Prepare( "SELECT id FROM quotes ORDER BY RANDOM() LIMIT 1;" );
	unsigned idx = GetNumber( stmt, false );
	sqlite3_finalize( stmt );

	return idx;
}
