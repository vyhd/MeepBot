MeepBot.Commands["say"] = function( type, caller, params )
	if not MeepBot.IsEnabled then return end

	if params:sub(1, 1) == "/" then
		print( ("%s wants me to %s."):format(caller, params) )
		return
	end

	MeepBot.Say( ("%s"):format(params) )
end
