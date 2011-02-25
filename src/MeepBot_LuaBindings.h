#ifndef MEEPBOT_LUA_BINDINGS_H
#define MEEPBOT_LUA_BINDINGS_H

namespace MeepBot_LuaBindings
{
	/* Register all MeepBot Lua functions. After CloseLua is called,
	 * this must be re-called to re-create the global MeepBot table. */
	void Register( lua_State *L );

	/* true if the command can be parsed, false if not */
	bool Command( lua_State *L, int type, const char *cmd, const char *caller, const char *params );
}

#endif // MEEPBOT_LUA_BINDINGS_H
