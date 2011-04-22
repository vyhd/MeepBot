#ifndef MEEPBOT_LUA_BINDINGS_H
#define MEEPBOT_LUA_BINDINGS_H

struct lua_State;

namespace MeepBot_LuaBindings
{
	/* Register all MeepBot Lua functions. After CloseLua is called,
	 * this must be re-called to re-create the global MeepBot table. */
	void Register( lua_State *L );

	/* Reloads the Lua scripts. */
	bool DoReload( int type, const char *caller );

	/* Responds to a chat line from a user, returns 'false' on error. */
	bool Respond( lua_State *L, int type, const char *user, const char *line );
}

#endif // MEEPBOT_LUA_BINDINGS_H
