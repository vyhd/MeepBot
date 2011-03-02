#ifndef QUOTE_DB_H
#define QUOTE_DB_H

const char* const QUOTES_TABLE = "quotes";

/* All quotes have a unique ID under this database. For simplicity's sake,
 * all external interfaces need to handle quotes by ID instead of by text
 * (e.g. retrieving or deleting quotes). Having the random quote function
 * return a random ID also allows the caller to know the ID. Good, yes? */

struct sqlite3;

class QuotesDB
{
public:
	QuotesDB( sqlite3* db );

	int GetRandomID() const;
	int GetIDByPattern( const char *pattern ) const;

	bool QuoteExists( int iQuoteID ) const;

	/* NULL = no match, otherwise, delete[] when you're finished. */
	const char* GetQuoteByID( int iQuoteID ) const;

	/* returns the person who added the quote with the given ID */
	const char* GetQuoteAuthor( int iQuoteID ) const;

	/* adder = the person whs 'calling' AddQuote. */
	bool AddQuote( const char *adder, const char *quote );

	/* returns true iff a quote was found and deleted */
	bool RemoveQuote( int iQuoteID );
private:
	sqlite3* m_pDB;
};

#endif // QUOTE_DB_H
