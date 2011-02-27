--[[
MeepBot.Commands["set"] = function( type, caller, params )
	if not HasAccess(caller, LEVEL_ADMIN) then return end

	-- TODO: get everything but the last word and resolve
end
--]]

MeepBot.Help["protect"] = "prevents moderators from changing a user's rank. (Admins)"

MeepBot.Commands["protect"] = function( type, caller, params )
	if not HasAccess(caller, LEVEL_ADMIN) then return end

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

MeepBot.Help["unprotect"] = "unprotects a user's rank, so moderators can change it. (Admins)"

MeepBot.Commands["unprotect"] = function( type, caller, params )
	if not HasAccess(caller, LEVEL_ADMIN) then return end

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
