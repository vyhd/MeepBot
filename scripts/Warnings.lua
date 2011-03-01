-- if this number is reached, the user is demoted and locked
local MAX_WARNINGS = 3

MeepBot.Help["warn"] = "gives a user a warning. At " .. MAX_WARNINGS .. " warnings, the user loses bot privileges. (Mods)"

MeepBot.Commands["warn"] = function( type, caller, params )
	if not HasAccess(caller, LEVEL_MOD) then return end

	if not params then
		MeepBot.SayOrPM( type, caller, "Usage: !warn [username]" )
		return
	end

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

	local warnings = MeepBot.Users.GetWarnings(target)

	if warnings >= MAX_WARNINGS then
		response = ("%s already has %d warnings!"):format(target, warnings)
		MeepBot.SayOrPM( type, caller, response )
		return
	end

	warnings = warnings + 1
	MeepBot.Users.SetWarnings(target, warnings)

	response = ("%s warned. (%d/%d)"):format(target, warnings, MAX_WARNINGS)

	-- if we've hit the warning limit, demote them
	if warnings >= MAX_WARNINGS then
		response = response .. " -  " .. target
		response = response ..  " is no longer allowed to use the bot."

		MeepBot.Users.SetDescription( target, "" )
		MeepBot.Users.SetAccessLevel( target, LEVEL_BANNED )
		MeepBot.Users.Protect( target, true )
	end

	MeepBot.SayOrPM( type, caller, response )
end

MeepBot.Help["unwarn"] = "removes all warnings from a user. (Admins)"

MeepBot.Commands["unwarn"] = function( type, caller, params )
	if not HasAccess(caller, LEVEL_ADMIN) then return end

	if not params then
		MeepBot.SayOrPM( type, caller, "Usage: !unwarn [username]" )
		return
	end

	local target = MeepBot.Utils.Resolve( caller, params )

	local response = nil
	local entry = MeepBot.Users.GetUserEntry(target)

	if not entry then
		response = "That user doesn't exist!"
	else
		MeepBot.Users.SetWarnings(target, 0)
		response = ("%s now has no warnings."):format(target)
	end

	MeepBot.SayOrPM( type, caller, response )
end

MeepBot.Help["checkwarnings"] = "checks how many warnings a user has. If you're not a mod, you can use it to check your own."

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
