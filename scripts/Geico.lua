MeepBot.Commands["geico"] = function( type, caller, params )
	local saved = MeepBot.Rand( 10000 )
	local target = params and MeepBot.Resolve(params) or caller

	if type == TYPE_CHAT then
		MeepBot.Say( ("%s just saved $%d on their car insurance!"):format(target, saved) )
	else
		MeepBot.PM( caller, ("$%d (debug)"):format(saved) )
	end
end
