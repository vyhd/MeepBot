Register( "help", LEVEL_USER, "Helps us help you help us all." )

local function GetFunctionsForLevel( level )
	local funcs = { }

	-- we can use AccessLevel because commands that don't
	-- have an entry in that table are not considered valid.
	for k,v in pairs(MeepBot.AccessLevel) do
		if v == level then funcs[#funcs+1] = k end
	end

	table.sort( funcs )
	return funcs
end

local LevelNames =
{
	LEVEL_USER = "Users",
	LEVEL_OP = "Ops",
	LEVEL_MOD = "Mods",
	LEVEL_ADMIN = "Admins",
}

local function Stringify( name, funcs )
	local ret = name .. ": " .. funcs[1]
	for i=2,#funcs do ret = ret .. ", " .. funcs[i] end
	return ret .. "."
end

local function GetFunctionsSorted( level )
	local ret = ""

	-- get sections per access level
	for lvl = LEVEL_USER, level do
		local funcs = GetFunctionsForLevel( lvl )
		ret = ret .. Stringify( LevelNames[lvl], lvl )
	end

	return ret
end

-- returns a clean version of a param, i.e. no exclamation and lowercase
local function clean( str )
	if str:sub(1,1) == "!" then str = str:sub(2) end
	return str:lower()
end

MeepBot.Commands["help"] = function( type, caller, params )
	-- only respond via PM if we're a normal user
	if GetAccessLevel(caller) < LEVEL_OP and type ~= TYPE_PM then return end

	local response = nil

	if params and gettype(params) == "string" then
		local params_ = clean(params)
		response = MeepBot.Help[params_]

		if response then
			response = params .. " - " .. response
		else
			response = ("Sorry, no help available for \"%s\"."):format(params)
		end
	else
		response = "Registered commands: " .. GetFunctionsSorted()
		response = response .. " For info on a specific command, use !help [command]"
	end

	MeepBot.SayOrPM( type, caller, response )
end
