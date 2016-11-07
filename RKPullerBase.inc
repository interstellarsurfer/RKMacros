Sub Init_RKPullerBase
	/if (${Group.Leader.ID} == ${Me.ID}) /call AddPullTargets
	/echo Done initializing puller base!
/return

Sub AddPullTargets

	/if (${Zone.ShortName.Equal[Postorms]}) {
		/call AddToArray "PullTargets" "scorned"
		/call AddToArray "PullTargets" "wanton"
		/call AddToArray "PullTargets" "floral"
		/call AddToArray "PullTargets" "armantis"
		/call AddToArray "PullTargets" "wandering"
		/call AddToArray "PullTargets" "slitherer"
	}

	/if (${Zone.ShortName.Equal[Barren]}) {
		/call AddToArray "PullTargets" "ironhide"
		/call AddToArray "PullTargets" "pirate"
		/call AddToArray "PullTargets" "sailor"
		/call AddToArray "PullTargets" "dog"
		/call AddToArray "PullTargets" "shark"		
		/call AddToArray "PullTargets" "vicious"				
		/call AddToArray "PullTargets" "miscreant"				
	}

	/if (${Zone.ShortName.Equal[Feerrott2]}) {
		/call AddToArray "PullTargets" "Tae Ew"
		/call AddToArray "PullTargets" "lizardman"
		/call AddToArray "PullTargets" "viper"
		/call AddToArray "PullTargets" "hunter"
		/call AddToArray "PullTargets" "prowler"
		/call AddToArray "PullTargets" "rot dog"
		/call AddToArray "PullTargets" "shade"
		/call AddToArray "PullTargets" "crawler"
		/call AddToArray "PullTargets" "amygdalan"
		/call AddToArray "PullTargets" "tainted"		
		/call AddToArray "PullTargets" "adder"
		/call AddToArray "PullTargets" "spirit"
		/call AddToArray "PullTargets" "skeleton"
	}
	
	/if (${Zone.ShortName.Equal[Mesa]}) {
		/call AddToArray "LoSPullTargets" "cave"
		/call AddToArray "LoSPullTargets" "ooze"
		/call AddToArray "LoSPullTargets" "mesa"
		/call AddToArray "LoSPullTargets" "centaur"
	}

	/if (${Zone.ShortName.Equal[Poinnovation]}) {
		/call AddToArray "LoSPullTargets" "rust"		
		/call AddToArray "LoSPullTargets" "obsolete"
		/call AddToArray "LoSPullTargets" "scrap"
		/call AddToArray "LoSPullTargets" "corroded"
		/call AddToArray "LoSPullTargets" "clock"				
		/call AddToArray "LoSPullTargets" "pool"		
		/call AddToArray "LoSPullTargets" "defective"
		/call AddToArray "LoSPullTargets" "archaic"
	}

	/if (${Zone.ShortName.Equal[Poeartha]}) {
		/call AddToArray "PullTargets" "arborean"
		|/call AddToArray "PullTargets" "rock"
		|/call AddToArray "PullTargets" "stone"
		|/call AddToArray "LoSPullTargets" "Earthcrafter"
		|/call AddToArray "PullTargets" "vegerog"
		|/call AddToArray "PullTargets" "Crusader"
		|/call AddToArray "LoSPullTargets" "mud"
		|/call AddToArray "LoSPullTargets" "gem"
		|/call AddToArray "LoSPullTargets" "crystal"
		|/call AddToArray "LoSPullTargets" "mass"		
		|/call AddToArray "LoSPullTargets" "earthen"				
		|/call AddToArray "PullTargets" "basilisk"
		|/call AddToArray "PullTargets" "armadillo"

		|/call AddToArray "PullTargets" "frog"		
		|/call AddToArray "PullTargets" "monstrosity"				
	}


	/if (${Zone.ShortName.Equal[Skyfire]}) {
		/call AddToArray "PullTargets" "walker"
		/call AddToArray "PullTargets" "mature"
		/call AddToArray "PullTargets" "drake"
		/call AddToArray "PullTargets" "devourer"
		/call AddToArray "PullTargets" "wurm"
	}


	/if (${Zone.ShortName.Equal[BurningWood]}) {
		/call AddToArray "PullTargets" "hornet"
		/call AddToArray "PullTargets" "Sarnak"
		/call AddToArray "PullTargets" "skeleton"
		/call AddToArray "PullTargets" "gorilla"
		/call AddToArray "PullTargets" "wurm"
	}


	/if (${Zone.ShortName.Equal[Letalis]}) {
	/call AddToArray "PullTargets" "rockhopper"
	/call AddToArray "PullTargets" "Shiknar"
	/call AddToArray "PullTargets" "Stonegrabber"	
	}

	/if (${Zone.ShortName.Equal[Stonebrunt]}) {
		/call AddToArray "PullTargets" "graniteback"
		/call AddToArray "PullTargets" "tiger"
		/call AddToArray "PullTargets" "elder panda"
	}

	/if (${Zone.ShortName.Equal[Gfaydark]}) {
		/call AddToArray "PullTargets" "bat"
		/call AddToArray "PullTargets" "skeleton"
		/call AddToArray "PullTargets" "wolf"
		/call AddToArray "PullTargets" "orc"
		/call AddToArray "PullTargets" "bandit"
		/call AddToArray "PullTargets" "widow"
		/call AddToArray "PullTargets" "wasp"	
		/call AddToArray "PullTargets" "drakeling"
		/call AddToArray "PullTargets" "drakeling"
	}

	/if (${Zone.ShortName.Equal[pohealth_errand]}) {
		/call AddToArray "LoSPullTargets" "pestilent"
		/call AddToArray "LoSPullTargets" "invader"
		/call AddToArray "LoSPullTargets" "bubonian"
		/call AddToArray "LoSPullTargets" "tainted"
		/call AddToArray "LoSPullTargets" "pusling"
		/call AddToArray "LoSPullTargets" "leprous"
		/call AddToArray "LoSPullTargets" "ballista"
		/call AddToArray "LoSPullTargets" "fragment"
		|/call AddToArray "LoSPullTargets" "presence"
		/call AddToArray "LoSPullTargets" "strider"
	}
	
	
	/if (${Zone.ShortName.Equal[cosul_quest]}) {
		/call AddToArray "LoSPullTargets" "awakened"
		/call AddToArray "LoSPullTargets" "servant"
		/call AddToArray "LoSPullTargets" "supplicant"
	}
	
	
	/if (${Zone.ShortName.Equal[exalted]}) {
		/call AddToArray "LoSPullTargets" "citizen"
		/call AddToArray "LoSPullTargets" "resident"
		/call AddToArray "LoSPullTargets" "functionary"
	}
	
	
	/if (${Zone.ShortName.Equal[pillarsalra]}) {
		/call AddToArray "PullTargets" "hopper"
		/call AddToArray "PullTargets" "toad"
		/call AddToArray "PullTargets" "selyrah"
		/call AddToArray "PullTargets" "wanderer"
		/call AddToArray "PullTargets" "siren"
	}
	
	/if (${Zone.ShortName.Equal[brotherisland]}) {
		/call AddToArray "PullTargets" "praetor"
		/call AddToArray "PullTargets" "razorlatch"
		/call AddToArray "PullTargets" "pincer"
		/call AddToArray "PullTargets" "eyepecker"
		/call AddToArray "PullTargets" "squawker"
		/call AddToArray "PullTargets" "furthick"
		/call AddToArray "PullTargets" "pounder"
		/call AddToArray "PullTargets" "tasselvine"
		/call AddToArray "PullTargets" "mulcher"
		/call AddToArray "PullTargets" "wanderer"
		/call AddToArray "PullTargets" "siren"
	}

	/if (${Zone.ShortName.Equal[deadhills_errand]}) {
		/call AddToArray "PullTargets" "water mephit"
		/call AddToArray "PullTargets" "scavenger"

		|Gribbles 1 - Into the Hills
		/call AddToArray "PullTargets" "undead"
		/call AddToArray "PullTargets" "pusling"
		/call AddToArray "PullTargets" "bloodthirsty pirate"
		/call AddToArray "PullTargets" "pile of bile"

		/call AddToArray "PullTargets" "tiny zombie"
		/call AddToArray "PullTargets" "tsetsian"
		/call AddToArray "PullTargets" "filthy"
		/call AddToArray "PullTargets" "hired guard"
		/call AddToArray "PullTargets" "mosquito"

		/call AddToArray "PullTargets" "filthy scavenger"

		/call AddToArray "PullTargets" "tortured soul"

		|Gribbles 2 - Scouting Ahead
		/call AddToArray "PullTargets" "monstrosity"
		/call AddToArray "PullTargets" "scout"
		/call AddToArray "PullTargets" "zombie"

		/call AddToArray "PullTargets" "vegerog"
		/call AddToArray "PullTargets" "excavator"	
		/call AddToArray "PullTargets" "xulous elder"	

		|Gribbles 3 - Disrupting the ritual
		/call AddToArray "PullTargets" "living ooze"	
		/call AddToArray "PullTargets" "xulous ritualist"
		/call AddToArray "PullTargets" "xulous warpriest"	
	}

	/if (${Zone.ShortName.Equal[ethernere_errand]}) {
		|/call AddToArray "PullTargets" "water mephit"
	}

	/if (${Zone.ShortName.Equal[arelis]}) { 
		/call AddToArray "PriorityTargets" "Scout VI"	
		/call AddToArray "PriorityTargets" "Scout VII"
		/call AddToArray "PriorityTargets" "Scout IX"
		/call AddToArray "PriorityTargets" "Scout XII"
		/call AddToArray "PriorityTargets" "Bloodborn"
		/call AddToArray "PriorityTargets" "Defector"


		/call AddToArray "PullTargets" "wicked grendlaen"	
		/call AddToArray "PullTargets" "ferocious grendlaen"
		/call AddToArray "PullTargets" "savage grendlaen"
		/call AddToArray "LoSPullTargets" "dirty oashim"
		/call AddToArray "LoSPullTargets" "mature oashim"
		/call AddToArray "LoSPullTargets" "adult oashim"	

		/call AddToArray "LoSPullTargets" "wicked"
		/call AddToArray "LoSPullTargets" "savage"
		/call AddToArray "LoSPullTargets" "ferocious"
		/call AddToArray "LoSPullTargets" "huntsman"
	}

	/if (${Zone.ShortName.Equal[beastsdomain]}) {
		/call AddToArray "PriorityTargets" "Tangledeath"
		/call AddToArray "PriorityTargets" "Night"	
		/call AddToArray "PriorityTargets" "Twisted Strangler"	
		/call AddToArray "PriorityTargets" "Deathsquirm"
		/call AddToArray "PriorityTargets" "goral striker"	

		/call AddToArray "PullTargets" "tainted ape"
		/call AddToArray "PullTargets" "a naeya scavenger"
		/call AddToArray "PullTargets" "a fierce wyvern"

		/call AddToArray "LoSPullTargets" "an okiina vinethrasher"
		/call AddToArray "LoSPullTargets" "goral scavenger"	
		/call AddToArray "LoSPullTargets" "prowler"
		/call AddToArray "LoSPullTargets" "braxi"
		/call AddToArray "LoSPullTargets" "grendlaen"
	} 
	
	/if (${Zone.ShortName.Equal[cityofbronze]}) {
		/call AddToArray "LoSPullTargets" "Guggles"
		/call AddToArray "LoSPullTargets" "wild selyrah"
		/call AddToArray "LoSPullTargets" "feral grendlaen"
		/call AddToArray "LoSPullTargets" "oashim"
		/call AddToArray "LoSPullTargets" "strangleweed"
		/call AddToArray "LoSPullTargets" "wild braxi"

		/call AddToArray "PullTargets" "Guggles"
		/call AddToArray "PullTargets" "wild selyrah"
		/call AddToArray "PullTargets" "feral grendlaen"
		/call AddToArray "PullTargets" "oashim"
		/call AddToArray "PullTargets" "strangleweed"
		/call AddToArray "PullTargets" "wild braxi"

	}
	
	/if (${Zone.ShortName.Equal[ethernere]}) {
|/call AddToArray "PriorityTargets" "bear"
|/call AddToArray "PullTargets" "gehein"
|/call AddToArray "PullTargets" "orafik"
|/call AddToArray "LoSPullTargets" "skirth"
	}
	
	/if (${Zone.ShortName.Equal[windsong]}) {
		/call AddToArray "PullTargets" "apparition"
		/call AddToArray "PullTargets" "devotee"
		/call AddToArray "PullTargets" "spirit"
		/call AddToArray "PullTargets" "elemental"
		/call AddToArray "PullTargets" "glider"
		/call AddToArray "PullTargets" "muted"
		/call AddToArray "PullTargets" "leaper"
	}

	/if (${Zone.ShortName.Equal[shardslanding]}) {
		/call AddToArray "PullTargets" "slitherer"
		/call AddToArray "PullTargets" "worntooth"
		/call AddToArray "PullTargets" "verdantbeak"
		/call AddToArray "PullTargets" "ivyscale"
		/call AddToArray "PullTargets" "slatescale"
		/call AddToArray "PullTargets" "venomshell"
	}
	
/return FALSE
