Register( "geico", LEVEL_OP, "saves you money on your car insurance. Maybe." )

MeepBot.Commands["geico"] = function( type, caller, params )
	local saved = MeepBot.Rand( 10000 )
	local target = params and MeepBot.Utils.Resolve(caller, params) or caller

	local direction = MeepBot.Rand(20) == 1 and "lost" or "saved"
	local pronoun = (target == caller) and "your" or "their"
	local response = ("%s just %s $%d on %s car insurance"):format(target, direction, saved, pronoun)
	response = response .. " by switching to Geico!"

	MeepBot.SayOrPM( type, caller, response )
end
