-- UGLY: until we have a guaranteed order of execution (e.g. alphabetical
-- iteration over the scripts directory), we need this in every file.
-- FIX ASAP!
MeepBot.CommandHelp = MeepBot.CommandHelp and MeepBot.CommandHelp or { }
MeepBot.CommandHelp["8ball"] = "Ask a question, get an answer."

local answers =
{
	-- ambiguous
	"as I see it, yes",
	"it is certain",
	"it is decidedly so",
	"most likely",
	"outlook good",
	"signs point to yes",
	"without a doubt",
	"yes",
	"yes - definitely",
	"you may rely on it",

	-- ambiguous
	"reply hazy, try again",
	"ask again later",
	"better not tell you now",
	"cannot predict now",
	"concentrate and ask again",

	-- negative
	"don't count on it",
	"my reply is no",
	"my sources say no",
	"outlook not so good",
	"very doubtful"
}


MeepBot.Commands["8ball"] = function( type, caller, params )
	if not MeepBot.IsEnabled or not params then return end

	if params:sub(-1, -1) ~= "?" then
		MeepBot.SayOrPM( type, caller, "I need a question." )
		return
	end

	local idx = MeepBot.Rand(#answers)
	local response = ("%s: %s."):format(caller, answers[idx])

	MeepBot.SayOrPM( type, caller, response )
end
