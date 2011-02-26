MeepBot.Help["position"] = "displays the access level of a user. (Ops)"

local LevelToName =
{
	[LEVEL_BANNED]	= "BANNED from MeepBot!",
	[LEVEL_USER]	= "a normal user.",
	[LEVEL_OP]	= "an Operator.",
	[LEVEL_MOD]	= "a Moderator.",
	[LEVEL_ADMIN]	= "an Administrator.",
}

MeepBot.Commands["position"] = function( type, caller, params )
	-- if we don't have access, only respond via PM
	if not HasAccess(caller, LEVEL_OP) and type ~= TYPE_PM then return end

	-- if we have no params, set the params to 'me' and resolve it
	if not params then params = "me" end

	local target = MeepBot.Utils.Resolve( caller, params )
	local entry = MeepBot.Users.GetUserEntry( target )

	if not entry then
		MeepBot.SayOrPM( "That user doesn't exist!" )
		return
	end

	local response = ("%s is %s"):format(target, LevelToName[entry.level])
	MeepBot.SayOrPM( type, caller, response )
end
