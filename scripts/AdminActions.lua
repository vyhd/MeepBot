Register( "protect", LEVEL_ADMIN, "prevents moderators from changing a user's rank." )
Register( "unprotect", LEVEL_ADMIN, "unprotects a user's rank, allowing mods to change it." )
Register( "setmask", LEVEL_ADMIN, "masks a user's rank with the given string." )

--[[
MeepBot.Commands["set"] = function( type, caller, params )
	-- TODO: get everything but the last word and resolve
end
--]]

MeepBot.Commands["protect"] = function( type, caller, params )
	if not params then
		MeepBot.SayOrPM( type, caller, "Usage: !protect [username]" )
		return
	end

	local target = MeepBot.Utils.Resolve( caller, params )
	local response = nil

	if MeepBot.Users.Protect(target, true) then
		response = ("%s is now protected."):format(target)
	else
		response = ("User \"%s\" doesn't exist!"):format(target)
	end

	MeepBot.SayOrPM( type, caller, response )
end

MeepBot.Commands["unprotect"] = function( type, caller, params )
	if not params then
		MeepBot.SayOrPM( type, caller, "Usage: !unProtect [username]" )
		return
	end

	local target = MeepBot.Utils.Resolve( caller, params )
	local response = nil

	if MeepBot.Users.Protect(target, false) then
		response = ("%s is no longer protected."):format(target)
	else
		response = ("User \"%s\" doesn't exist!"):format(target)
	end

	MeepBot.SayOrPM( type, caller, response )
end

MeepBot.Commands["setmask"] = function( type, caller, params )
	if not params then
		MeepBot.SayOrPM( type, caller, "Usage: !setMask [user] [mask]" )
		return
	end

	MeepBot.SayOrPM( type, caller, "Not implemented yet, check back later" )
end
