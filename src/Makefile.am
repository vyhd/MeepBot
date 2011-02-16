bin_PROGRAMS = MeepBot
noinst_PROGRAMS = MeepBot

AM_CXXFLAGS = -O3 -Wall -pedantic -fno-exceptions

ISAAC = util/ISAAC.cpp util/ISAAC.h util/ISAAC/rand.c util/ISAAC/rand.h
Model = model/UserList.cpp model/UserList.h
Packet = packet/ChatPacket.cpp packet/ChatPacket.h packet/MessageCodes.h \
	packet/PacketUtil.cpp packet/PacketUtil.h
Socket = network/Socket.cpp network/Socket.h
SQLite = database/SQLiteDB.cpp database/SQLiteDB.h \
	database/QuotesDB.cpp database/QuotesDB.h \
	database/UserDB.cpp database/UserDB.h
Util = util/LuaUtil.cpp util/LuaUtil.h util/StringUtil.cpp util/StringUtil.h

MeepBot_LDFLAGS = -llua5.1
MeepBot_LDADD = ../sqlite3/sqlite3.o
MeepBot_SOURCES = Main.cpp MeepBot.cpp MeepBot.h $(ISAAC) $(Model) \
	$(Packet) $(Socket) $(Util)
