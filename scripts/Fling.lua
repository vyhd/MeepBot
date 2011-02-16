local objects =
{
	"a desk lamp",
	"a crying baby",
	"a sailboat",
	"a little green army dude",
	"a dismembered doll head",
	"an ace of clubs",
	"a glass of milk",
	"a centipede",
	"a pepperoni Hot Pocket",
	"a carrot",
	"an orange stapler",
	"a pillow",
	"a 7-iron",
	"a shoe",
	"a Bible",
	"a can of Spam",
	"a cold water balloon",
	"a baguette",
	"a sock full of nickels",
	"a beehive",
	"a banana phone",
	"a rusty spoon",
	"a fuzzy bunny",
	"Hiker's walking stick",
	"a broken refridgerator",
	"a Pokéball",
	"Chat's bell",
	"Henner's nose",
	"a spicy meatball",
	"an unplugged electric guitar",
	"a 20-sided die",
	"a steel tire iron",
	"a Trigonometry textbook",
	"a flying cuttlefish",
	"a glow stick",
	"a bucket of Legos",
	"a winning lottery ticket",
	"a grappling hook",
	"a satellite dish",
	"an ant farm",
	"a CRT monitor",
	"a cardboard box",
	"Morgan Freeman"
}

MeepBot.Commands["fling"] = function( type, caller, params )
	if not MeepBot.IsEnabled then return end
	if type ~= TYPE_CHAT then return end

	local idx = MeepBot.Rand( #objects )
	local target = MeepBot.Resolve( caller, params )

	local msg = ("flings %s at %s"):format( objects[idx], target )
	MeepBot.Emote( msg )
end
