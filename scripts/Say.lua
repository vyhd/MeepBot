MeepBot.Help["say"] = "makes MeepBot say something. (Mods)"

MeepBot.Commands["say"] = function( type, caller, params )
	if not HasAccess(caller, LEVEL_MOD) then return end

	-- ignore stuff with a leading slash - intended to be a command
	if params:sub(1, 1) == "/" then return end

	MeepBot.Say( ("%s"):format(params) )
end
