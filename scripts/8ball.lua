MeepBot.Help["8ball"] = "Ask a question, get an answer. (Ops)"

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

	-- if we don't have access, only respond via PM
	if not HasAccess(caller, LEVEL_OP) and type ~= TYPE_PM then return end

	if not params or params:sub(-1, -1) ~= "?" then
		MeepBot.SayOrPM( type, caller, "I need a question." )
		return
	end

	local idx = MeepBot.Utils.Rand(#answers)
	local response = ("%s: %s."):format(caller, answers[idx])

	MeepBot.SayOrPM( type, caller, response )
end
