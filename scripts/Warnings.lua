-- this is set as a simple check against accidental warns
local lastWarned = nil

-- if this number is reached, the user is demoted and locked
local MAX_WARNINGS = 3

MeepBot.Help["warn"] = "gives a user a warning. At " .. MAX_WARNINGS .. " warnings, the user loses bot privileges. (Mods)"

MeepBot.Commands["warn"] = function( type, caller, params )
	if not HasAccess(caller, LEVEL_MOD) then return end

	local target = MeepBot.Utils.Resolve( caller, params )
	local entry = MeepBot.Users.GetUserEntry( target )

	if not entry then
		local response = ("User %s not found!"):format(target)
		MeepBot.SayOrPM( type, caller, response )
		return
	end

	if entry.level > LEVEL_OP then
		MeepBot.SayOrPM( type, caller, "You can't warn staff!" )
		return
	end

	local warnings = MeepBot.Users.GetWarnings(target) + 1
	local response

	if target ~= lastWarned then
		response = "Are you sure you want to warn this user?"
		response = response .. " If so, call '!warn [user]' again."
		lastWarned = target
	else
		MeepBot.Users.SetWarnings(target, warnings)
		response = ("User warned. (%d/%d)"):format(warnings,MAX_WARNINGS)
	end

	-- if we've hit the warning limit, demote them
	if warnings >= MAX_WARNINGS then
		MeepBot.Users.SetDescription( target, "" )
		MeepBot.Users.SetAccessLevel( target, LEVEL_BANNED )
		MeepBot.Users.Protect( target, true )
	end

	MeepBot.SayOrPM( type, caller, response )
end

MeepBot.Help["checkwarnings"] = "checks how many warnings a user has. (Mods)"

MeepBot.Commands["checkwarnings"] = function( type, caller, params )

	-- if we're not a moderator, only check our own warnings via PM
	if not HasAccess(caller, LEVEL_MOD) then
		type = TYPE_PM
		params = "me"
	 end

	local target = MeepBot.Utils.Resolve( caller, params )
	local warnings = MeepBot.Users.GetWarnings( target )

	local response

	-- negative warnings = sentinel for 'not found'
	if warnings < 0 then
		response = ("User %s not found!"):format(caller)
	else
		response = ("%s has %d/%d warnings."):format(target, warnings, MAX_WARNINGS)
	end

	MeepBot.SayOrPM( type, caller, response )

end

MeepBot.Commands["unwarn"] = function( type, caller, params )
	if not HasAccess(caller, LEVEL_MOD) then return end

	local target = MeepBot.Utils.Resolve( caller, params )
end
