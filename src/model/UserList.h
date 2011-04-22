#ifndef USER_LIST_H
#define USER_LIST_H

#include <set>
#include <string>

/* string comparison function taking two char pointers and a length */
typedef int (*cmpfunc)(const char*,const char*,size_t);

class UserList
{
public:
	UserList();

	void Add( const std::string &name );
	void Remove( const std::string &name );

	int GetNumUsers() const;

	const std::string Resolve( const std::string &caller, const std::string &pattern );

	void PrintList() const;
private:
	/* whee, namespaces */
	std::set<std::string>::iterator FindMatch( const std::string &pattern, cmpfunc func );
	std::set<std::string> m_List;
};

#endif // USER_LIST_H
