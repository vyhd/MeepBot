-- Don't use the stuff in this file in a global scope
-- (i.e., only in functions that would be called by the bot):
-- the loading order of scripts isn't guaranteed.

-- Either says in public or responds to the user in PM, whichever
-- is appropriate. We can stuff the permissions logic in here later.
function MeepBot.SayOrPM( type, caller, response )
	if type == TYPE_PM then
		MeepBot.PM( caller, response )
	else
		MeepBot.Say( response )
	end
end

-- alias for Lua's type() function so we can use it if needed
-- without interfering with the 'type, caller, params' declarations
gettype = type

-- Allow only certain users privileged bot instructions.
-- this is a hack until we get the user database up and running.

local AllowedUsers =
{
	"hiker",
	"Henner",
	"Eadwulf",
	"Burks",
	"Tanksandguns",
	"Leanan Sidhe",
	"Mackerel",
	"Spelda",
	"Jackstick",
	"Rory",
	"rangerpker",
	"TheAnimal",
	"bluecoat",
	"Fire_Adept"
}

-- not strictly true, since I added myself for debugging ease
function IsMod( caller )
	for i=1,#AllowedUsers do
		if caller:lower() == AllowedUsers[i]:lower() then return true end
	end

	return false
end
