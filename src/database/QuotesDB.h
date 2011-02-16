#ifndef QUOTE_DB_H
#define QUOTE_DB_H

const char* const QUOTES_TABLE = "quotes";

/* general SQL setup (for later reference):
 * id - int, unique for each quote, used to reference them
 * quote - varchar, the actual quote itself
 * addedBy - varchar, name of the person who added this quote
 */

class QuotesDB: public SQLiteDB
{
public:
	/* we require the caller to get a random ID, then dereference it,
	 * so the caller knows the ID and can get the quote from it. */

	const char* GetQuote( int iQuoteID );
	int GetRandomQuoteID();
};

#endif // QUOTE_DB_H
