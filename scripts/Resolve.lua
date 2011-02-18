-- UGLY: until we have a guaranteed order of execution (e.g. alphabetical
-- iteration over the scripts directory), we need this in every file.
-- FIX ASAP!
MeepBot.Help = MeepBot.Help and MeepBot.Help or { }
MeepBot.Help["resolve"] = "Given a partial string, returns a unique username matching it. For debugging."

MeepBot.Commands["resolve"] = function( type, caller, params )
	if not MeepBot.IsEnabled then return end

	local name = MeepBot.Resolve( caller, params )
	local response = ("%s -> %s"):format(params, name)

	MeepBot.SayOrPM( type, caller, response )
end
