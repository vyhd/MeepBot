MeepBot.Help["resolve"] = "Given a partial string, returns a unique username matching it. For debugging."

MeepBot.Commands["resolve"] = function( type, caller, params )
	if not MeepBot.IsEnabled then return end

	local name = MeepBot.Resolve( caller, params )
	local response = ("%s -> %s"):format(params, name)

	MeepBot.SayOrPM( type, caller, response )
end
