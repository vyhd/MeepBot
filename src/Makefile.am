bin_PROGRAMS = MeepBot
noinst_PROGRAMS = MeepBot

# UGLY: manually include sqlite3 here until we make autotools do it for us.
AM_CXXFLAGS = -Wall -Wextra -fno-exceptions -pedantic-errors -isystem ../sqlite3

ISAAC = util/ISAAC.cpp util/ISAAC.h util/ISAAC/rand.c util/ISAAC/rand.h
Lua = MeepBot_LuaBindings.cpp MeepBot_LuaBindings.h util/LuaUtil.cpp util/LuaUtil.h
Model = model/UserList.cpp model/UserList.h
Packet = packet/ChatPacket.cpp packet/ChatPacket.h packet/MessageCodes.h \
	packet/PacketUtil.cpp packet/PacketUtil.h
Socket = network/Socket.cpp network/Socket.h
SQLite = database/SQLite.cpp database/SQLite.h \
	database/QuotesDB.cpp database/QuotesDB.h \
	database/UserDB.cpp database/UserDB.h
Util = util/StringUtil.cpp util/StringUtil.h

MeepBot_LDFLAGS = -llua5.1
MeepBot_LDADD = ../sqlite3/sqlite3.o
MeepBot_SOURCES = Main.cpp MeepBot.cpp MeepBot.h $(ISAAC) $(Lua) \
	$(Model) $(Packet) $(Socket) $(SQLite) $(Util)
