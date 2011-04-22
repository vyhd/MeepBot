-- Function factory! I love Lua. So much. <3
local function ModAction( func, witticism )
	return function( type, caller, params )
		if not HasAccess(caller, LEVEL_MOD) then return end
		if gettype(params) ~= "string" then return end

		local target = MeepBot.Utils.Resolve( caller, params )

		-- say something witty
		if witticism then
			MeepBot.SayOrPM( type, caller, witticism:format(target) )
		end

		-- do something clever
		if target then MeepBot[func]( target )	end
	end
end

-- %s in 'witticism' will be replaced with the resolved target's name
MeepBot.Commands["kick"] = ModAction( "Kick", "Hasta la vista, %s!" )
MeepBot.Commands["disable"] = ModAction( "Disable", "Something witty." )
MeepBot.Commands["ban"] = ModAction( "Ban", "%s: it's what's for dinner. BANNED." )
MeepBot.Commands["unban"] = ModAction( "Unban", "%s unbanned." )
MeepBot.Commands["mute"] = ModAction( "Mute", "But what good is a phone call, %s, if you are... unable to speak?")
MeepBot.Commands["unmute"] = ModAction( "Unmute", "%s unmuted." )
