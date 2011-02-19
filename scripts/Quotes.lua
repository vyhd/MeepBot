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
	local id, response = nil, nil

	if tonumber(params) then
		id = tonumber(params)
	elseif paramtype == "string" then
		id = MeepBot.GetQuoteIDByPattern( params );
	else
		id = MeepBot.GetRandomQuoteID();
	end

	-- a valid index guarantees that a quote is mapped to it
	if not id then
		if not params then
			response = "No quotes found. Maybe you should add some?"
		else
			response = "Quote not found!"
		end
	else
		local quote = MeepBot.GetQuoteByID( id );
		response = "#" .. id .. ": " .. quote
	end

	MeepBot.SayOrPM( type, caller, response )
end
