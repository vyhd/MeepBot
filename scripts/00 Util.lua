-- As of arbitrary git revision, this is actually executed in 
-- alphabetical order, so we can do preparatory stuff here.

-- this flag allows selected users to enable and disable the bot
-- as needed (to prevent spam, abuse, etc. etc.)
--
-- as it's implemented entirely in Lua, make sure your functions
-- check this flag if you're not using HasAccess; there's no other
-- external mechanism to do it for you.

MeepBot.IsEnabled = true

--
-- HasAccess - returns true if the caller has at least 'level' access and
-- either the bot is enabled, or the caller is at least a moderator.
-- Preface every command function with this or you will probably suffer.
--
function HasAccess( user, level )
	if gettype(level) ~= "number" then
		print( "Unknown level " .. tostring(level) )
		return false
	end

	local ulevel = MeepBot.Users.GetAccessLevel( user )

	-- unless this user is a mod, don't allow when the bot's disabled
	if ulevel < LEVEL_MOD and not MeepBot.IsEnabled then return false end

	return ulevel >= level
end

--
-- Create the help table that contains help information for all commands.
--
MeepBot.Help =
{
	Fun = { },
	Ops = { },
	Mods = { },
	Admins = { }
}

-- easter egg, because why not
MeepBot.Help["me"] = "you're beyond my help, I'm afraid"

-- alias for Lua's type() function so we can use it if needed
-- without interfering with the 'type, caller, params' declarations
gettype = type

--
-- Publicly says or responds via PM, depending on type and user level
--
function MeepBot.SayOrPM( type, caller, response )
	if not MeepBot.IsEnabled then return end

	if type == TYPE_PM then
		MeepBot.PM( caller, response )
	else
		MeepBot.Say( response )
	end
end
