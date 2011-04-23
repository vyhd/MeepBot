Register( "enablebot", LEVEL_MOD, "Enables all operator commands." )
Register( "disablebot", LEVEL_MOD, "Disables all operator commands." )

MeepBot.Commands["enablebot"] = function( type, caller, params )
	MeepBot.IsEnabled = true
	MeepBot.Say( "Bot enabled." )
end

MeepBot.Commands["disablebot"] = function( type, caller, params )
	MeepBot.IsEnabled = false
	MeepBot.Say( "Bot disabled." )
end
