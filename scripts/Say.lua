-- UGLY: until we have a guaranteed order of execution (e.g. alphabetical
-- iteration over the scripts directory), we need this in every file.
-- FIX ASAP!
MeepBot.Help = MeepBot.Help and MeepBot.Help or { }
MeepBot.Help["say"] = "makes MeepBot say something. (Ops)"

MeepBot.Commands["say"] = function( type, caller, params )
	if not MeepBot.IsEnabled then return end

	if params:sub(1, 1) == "/" then
		print( ("%s wants me to %s."):format(caller, params) )
		return
	end

	MeepBot.Say( ("%s"):format(params) )
end
