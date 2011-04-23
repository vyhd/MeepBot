Register( "say", LEVEL_MOD, "makes MeepBot say something." )

MeepBot.Commands["say"] = function( type, caller, params )
	-- ignore stuff with a leading slash - intended to be a command
	if params:sub(1, 1) == "/" then return end

	MeepBot.Say( ("%s"):format(params) )
end
