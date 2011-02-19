-- As of arbitrary git revision, this is actually executed in 
-- alphabetical order, so we can do preparatory stuff here.

-- Create the help table that contains help information for all commands.
MeepBot.Help = { }

-- alias for Lua's type() function so we can use it if needed
-- without interfering with the 'type, caller, params' declarations
gettype = type

-- Either says in public or responds to the user in PM, whichever
-- is appropriate. We can stuff the permissions logic in here later.
function MeepBot.SayOrPM( type, caller, response )
	if type == TYPE_PM then
		MeepBot.PM( caller, response )
	else
		MeepBot.Say( response )
	end
end

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
	"Fire_Adept",
	"Glodenox"
}

-- not strictly true, since I added myself for debugging ease
function IsMod( caller )
	for i=1,#AllowedUsers do
		if caller:lower() == AllowedUsers[i]:lower() then return true end
	end

	return false
end
