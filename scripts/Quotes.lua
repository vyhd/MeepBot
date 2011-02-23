MeepBot.Help["quote"] = "displays a quote from the database. If no ID or pattern is given, a random quote is displayed."
MeepBot.Help["addquote"] = "adds a quote to the quotes database. (Ops)"
MeepBot.Help["removequote"] = "removes a quote from the database by its ID. (Mods)"

MeepBot.Commands["addquote"] = function( type, caller, params )
	if not IsMod(caller) then return end

	local response

	if MeepBot.AddQuote(caller, params) then
		response = "Quote added."
	else
		response = "Oh noes! Quote not added! :("
	end

	MeepBot.SayOrPM( type, caller, response )
end

MeepBot.Commands["removequote"] = function( type, caller, params )
	if not IsMod(caller) then return end

	local id = tonumber(params)
	if not id then return end

	local author = MeepBot.GetQuoteAuthor( id )

	if not author then
		MeepBot.SayOrPM( type, caller, "That quote doesn't exist!" )
		return
	end

	local msg = ("Quote #%d was added by %s."):format(id, author)
	msg = msg .. " Maybe you should smite them?"

	MeepBot.RemoveQuote( id )
	MeepBot.SayOrPM( type, caller, msg )
end
	
MeepBot.Commands["quote"] = function( type, caller, params )
	if not MeepBot.IsEnabled then return end

	local paramtype = gettype(params)
	local _, id, id_to_try, response

	if paramtype ~= "string" then
		id = MeepBot.GetRandomQuoteID()
	else
		-- HACK: if the param is "me", replace params with the caller's name
		if params:lower() == "me" then params = caller end

		-- see if we're trying to get a specific quote ID
		local _, _, id_to_try = params:find( "#(%d+)" )

		-- is the passed ID valid (i.e. does it map to a quote)?
		if id_to_try and MeepBot.GetQuoteByID(id_to_try) then
			id = id_to_try
		else
			id = MeepBot.GetQuoteIDByPattern( params )
		end
	end

	-- at this point, if a quote matching our criteria is found, 'id'
	-- will be the ID for that quote. If 'id' is nil, we have no match.

	if id then
		local quote = MeepBot.GetQuoteByID( id )
		response = "#" .. id .. ": " .. quote
	else
		-- if we have no params, returning a random value failed,
		-- which means we have no quotes at all; acknowledge that.
		local no_match = "Quote not found!"
		local none = "No quotes found. Maybe you should add some?"

		response = params and no_match or none
	end

	MeepBot.SayOrPM( type, caller, response )
end

