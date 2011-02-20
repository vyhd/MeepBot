MeepBot.Commands["geico"] = function( type, caller, params )
	if not MeepBot.IsEnabled then return end

	local saved = MeepBot.Rand( 10000 )
	local target = params and MeepBot.Resolve(caller, params) or caller

	local pronoun = (target == caller) and "your" or "their"
	local response = ("%s just saved $%d on %s car insurance"):format(target, saved, pronoun)
	response = response .. " by switching to Geico!"

	MeepBot.SayOrPM( type, caller, response )
end
