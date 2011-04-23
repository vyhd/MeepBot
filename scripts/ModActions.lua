-- Function factory! I love Lua. So much. <3
local function ModAction( func, help, witticism )
	Register( func, LEVEL_MOD, help )

	return function( type, caller, params )
		if gettype(params) ~= "string" then return end

		local target = MeepBot.Utils.Resolve( caller, params )

		-- say something witty
		if witticism then
			MeepBot.Say( witticism:format(target) )
		end

		-- do something clever
		if target then MeepBot[func]( target )	end
	end
end

-- %s in 'witticism' will be replaced with the resolved target's name
MeepBot.Commands["kick"] = ModAction( "Kick", "kicks a mildly unruly user.", "Hasta la vista, %s!" )
MeepBot.Commands["disable"] = ModAction( "Disable", "disables a somewhat unruly user.", "Something witty." )
MeepBot.Commands["ban"] = ModAction( "Ban", "bans a very unruly user.", "%s: it's what's for dinner. BANNED." )
MeepBot.Commands["unban"] = ModAction( "Unban", "unbans a previously unruly user.", "%s unbanned." )
MeepBot.Commands["mute"] = ModAction( "Mute", "mutes a user.", "But what good is a phone call, %s, if you are... unable to speak?")
MeepBot.Commands["unmute"] = ModAction( "Unmute", "unmuts a user.", "%s unmuted." )
