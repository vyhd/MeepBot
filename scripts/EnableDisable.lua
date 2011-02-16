-- this flag allows selected users to enable and disable the bot
-- as needed (to prevent spam, abuse, etc. etc.)
--
-- as it's implemented entirely in Lua, make sure your custom functions
-- check this flag; there's no external mechanism to do it for you.

MeepBot.IsEnabled = true

-- very temporary hack to allow disabling in case of abuse, until this
-- can be done by rank + user database.

MeepBot.Commands["enablebot"] = function( type, caller, params )
	if IsAllowed( caller ) then
		MeepBot.IsEnabled = true
		MeepBot.Say( "Bot enabled." )
	end
end

MeepBot.Commands["disablebot"] = function( type, caller, params )
	if IsAllowed( caller ) then
		MeepBot.IsEnabled = false
		MeepBot.Say( "Bot disabled." )
	end
end
