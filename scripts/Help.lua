-- UGLY: until we have a guaranteed order of execution (e.g. alphabetical
-- iteration over the scripts directory), we need this in every file.
-- FIX ASAP!
MeepBot.CommandHelp = MeepBot.CommandHelp and MeepBot.CommandHelp or { }
MeepBot.CommandHelp["help"] = "Helps us help you help us all."

local function GetFunctionsSorted()
	local funcs = { }

	for k,v in pairs(MeepBot.Commands) do funcs[#funcs+1] = k end

	-- this isn't in Lua, but it's a command regardless
	funcs[#funcs+1] = "reload"
	table.sort( funcs )

	local ret = ""

	for i=1,#funcs do ret = ret .. " " .. funcs[i] end
	return ret .. "."
end

MeepBot.Commands["help"] = function( type, caller, params )
	if not MeepBot.IsEnabled then return end

	local response = MeepBot.CommandHelp[params]

	if not params then
		response = "Registered commands:" .. GetFunctionsSorted()
		response = response .. " For info on a specific command, use !help [command]"
	elseif not response then
		response = ("Sorry, no help available for \"%s\"."):format(params)
	else
		response = "!" .. params .. " - " .. response
	end

	MeepBot.SayOrPM( type, caller, response )
end
