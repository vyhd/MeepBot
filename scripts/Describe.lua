Register( "describe", LEVEL_OP, "displays a user's personal description." )
Register( "setdesc", LEVEL_OP, "sets your personal description." )
Register( "admindesc", LEVEL_ADMIN, "overrides a user's personal description." )

MeepBot.Commands["describe"] = function( type, caller, params )
	local target = MeepBot.Utils.Resolve( caller, params )
	local entry = MeepBot.Users.GetUserEntry( target )
	local desc = entry.desc

	local response = "User not found!"

	if desc and gettype(desc) == "string" then
		if desc:len() == 0 then
			response = target .. " hasn't set a description yet."
		else
			response = target .. " " .. entry.desc
		end
	end

	MeepBot.SayOrPM( type, caller, response )	
end

-- maximum allowed description length
local MAX_DESC_LEN = 64

MeepBot.Commands["setdesc"] = function( type, caller, params )
	local response = "Usage: !setDesc [description]"

	if gettype(params) ~= "string" then
		MeepBot.SayOrPM( type, caller, response )
		return
	end

	if params:len() > MAX_DESC_LEN then
		response = "Your description is too long! "
		response = response .. ("(%d/%d characters)"):format(params:len(), MAX_DESC_LEN)
	elseif MeepBot.Users.SetDescription(caller, params) then
		response = "Description set."
	else
		response = "Oh noes! Couldn't set description :("
	end

	MeepBot.SayOrPM( type, caller, response )
end

MeepBot.Commands["admindesc"] = function( type, caller, params )
	MeepBot.SayOrPM( type, caller, "Sorry, not implemented yet." )
end
