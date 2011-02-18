MeepBot.Commands["addquote"] = function( type, caller, params )
	if not IsMod(caller) then return end

	if MeepBot.AddQuote(caller, params) then
		MeepBot.Say( "Quote added." )
	else
		MeepBot.Say( "Oh noes! Quote not added! :(" )
	end
end

MeepBot.Commands["removequote"] = function( type, caller, params )
	if not IsMod(caller) then return end

	local idx = tonumber(params)
	if not idx then return end

	local author = MeepBot.GetQuoteAuthor( idx )

	if not author then
		MeepBot.SayOrPM( type, caller, "That quote doesn't exist!" )
		return
	end

	local msg = ("#%d was added by %s. Maybe you should smite them?"):format(idx, author)

	MeepBot.RemoveQuote( idx )
	MeepBot.SayOrPM( type, caller, msg )
end
	

MeepBot.Commands["quote"] = function( type, caller, params )
	if not MeepBot.IsEnabled then return end

	local idx = params and tonumber(params) or MeepBot.GetRandomQuoteID()
	local quote = MeepBot.GetQuote( idx )

	if quote and gettype(quote) == "string" then
		MeepBot.Say( "#" .. idx .. ": " .. quote )
	elseif not params then
		MeepBot.Say( "No quotes found. Maybe you should add some?" )
	else
		MeepBot.Say( "Quote not found!" );
	end
end
