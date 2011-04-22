#include "UserList.h"
#include "util/StringUtil.h"

using namespace std;
using namespace StringUtil;

UserList::UserList()
{
	m_List.clear();
}

void UserList::Add( const string &name )
{
	printf( "Added user %s\n", name.c_str() );
	m_List.insert( name );
}

void UserList::Remove( const string &name )
{
	printf( "Removed user %s\n", name.c_str() );
	m_List.erase( name );
}

int UserList::GetNumUsers() const
{
	return m_List.size();
}

set<string>::iterator UserList::FindMatch( const string &pattern, cmpfunc func )
{
	set<string>::iterator it;
	unsigned count = 0;

	printf( "checking matches for \"%s\", length %d\n", pattern.c_str(), pattern.length() );

	/* check to see if multiple matches exist for this pattern */
	for( it = m_List.begin(); it != m_List.end(); ++it )
	{
		if( count < 2 && func(pattern.c_str(), it->c_str(), pattern.length()) == 0 )
		{
			printf( "Match: %s, %s, %d\n", pattern.c_str(), it->c_str(), pattern.length() );
			++count;
		}
	}

	/* if we can't find a unique match, return the end of the set. */
	if( count != 1 )
		return m_List.end();

	/* there's a unique match. find it and return it. */
	for( it = m_List.begin(); it != m_List.end(); ++it )
		if( func(pattern.c_str(), it->c_str(), pattern.length()) == 0 )
			return it;

	/* fallback */
	return m_List.end();
}

const string UserList::Resolve( const string &caller, const string &pattern )
{
	/* always resolve "me" to the caller */
	if( pattern.length() == 2 && CompareNoCase(pattern, "me") == 0 )
		return caller;

	/* do first a case-sensitive match, then a case-insensitive match. */
	set<string>::iterator it = FindMatch( pattern, strncasecmp );

	if( it == m_List.end() )
	{
		it = FindMatch( pattern, strncmp );

		if( it == m_List.end() )
			return pattern;
	}

	return *it;
}

void UserList::PrintList() const
{
	set<string>::const_iterator it = m_List.begin();

	for( it = m_List.begin(); it != m_List.end(); ++it )
		printf( "%p: %s\n", (void*)(&*it), it->c_str() );
}
