MeepBot.Help["help"] = "Helps us help you help us all."

local function GetFunctionsSorted()
	-- create an indexed table and sort it
	local funcs = { }
	for k,v in pairs(MeepBot.Commands) do funcs[#funcs+1] = k end
	table.sort( funcs )

	local ret = funcs[1]
	for i=2,#funcs do ret = ret .. ", " .. funcs[i] end
	return ret .. "."
end

-- returns a clean version of a param, i.e. no exclamation and lowercase
local function clean( str )
	if str:sub(1,1) == "!" then str = str:sub(2) end
	return str:lower()
end

MeepBot.Commands["help"] = function( type, caller, params )
	-- only respond via PM if we don't have access
	if not HasAccess(caller, LEVEL_OP) and type ~= TYPE_PM then return end

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
