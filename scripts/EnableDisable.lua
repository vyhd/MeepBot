MeepBot.Help["enablebot"] = "Enables all operator commands. (Mods)"
MeepBot.Help["disablebot"] = "Disables all operator commands. (Mods)"

MeepBot.Commands["enablebot"] = function( type, caller, params )
	if HasAccess(caller, LEVEL_MOD) then
		MeepBot.IsEnabled = true
		MeepBot.Say( "Bot enabled." )
	end
end

MeepBot.Commands["disablebot"] = function( type, caller, params )
	if HasAccess(caller, LEVEL_MOD) then
		MeepBot.IsEnabled = false
		MeepBot.Say( "Bot disabled." )
	end
end
