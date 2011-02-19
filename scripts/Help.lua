MeepBot.Help["help"] = "Helps us help you help us all."

-- no better place for this currently...
MeepBot.Help["reload"] = "Reloads the bot's Lua scripting. Call it if you like, it's harmless."

local function GetFunctionsSorted()
	local funcs = { }

	for k,v in pairs(MeepBot.Commands) do funcs[#funcs+1] = k end

	-- this isn't in Lua, but it's a command regardless
	funcs[#funcs+1] = "reload"
	table.sort( funcs )

	local ret = funcs[1]
	for i=2,#funcs do ret = ret .. ", " .. funcs[i] end
	return ret .. "."
end

MeepBot.Commands["help"] = function( type, caller, params )
	if not MeepBot.IsEnabled then return end

	local response = MeepBot.Help[params]

	if not params then
		response = "Registered commands: " .. GetFunctionsSorted()
		response = response .. " For info on a specific command, use !help [command]"
	elseif not response then
		response = ("Sorry, no help available for \"%s\"."):format(params)
	else
		response = "!" .. params .. " - " .. response
	end

	MeepBot.SayOrPM( type, caller, response )
end
