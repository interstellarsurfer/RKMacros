|------------ BOTMode

#Event FollowInvis "#*#Follow me, but stay invis#*#"
#Event FollowCombat "#*#Okay, follow me#*#"
#Event CampInvis "#*#Medding for a bit#*#"
#Event CampCombat "#*#Medding while I pull#*#"
#Event CampCharacter "#*#CampCharacter#*#"
#Event CampDesktop "#*#CampDesktop#*#"
#Event LowManaMedding "#*#medding, you should med#*#"
#Event DoneMedding "#*#full mana, all done#*#"

|------------ Game Events

#Event XP "#*#experience!!#*#"
#Event XP2 "#*#has been slain by#*#"
#Event XP3 "#*#You have slain#*#"
#Event XP4 "#*#as your split.#*#"
#Event GotGroupInvite "#*#invites you to join a group.#*#"
#Event TaskGranted "#*#you have been assigned the task#*#"
#Event TaskProgress "#*#' has been updated.#*#"
#Event LanguageSetToErudian "#*#Language set to Erudian#*#"
#Event LanguageSetToErudian "#*#Current Language is Erudian#*#"
#Event LanguageSetToElvish "#*#Current Language is Elvish#*#"
#Event Camping "#*#It will take you about 30 seconds to prepare your camp#*#"
#Event ReceivedTell "#1# tells you, in #2#, #3#"

|------------ Pulling

#Event AddPullTarget "#*#add pull target#*#"
#Event AddLOSPullTarget "#*#add los pull target#*#"
#Event AddPriorityPullTarget "#*#add priority pull target#*#"
#Event AddExplicitLOSPullTarget "#*#add los: #1#"
#Event AddExplicitPullTarget "#*#add pull: #1#"
#Event AddExplicitPriorityTarget "#*#add priority: #1#"
#Event ListPullTargets "#*#list pull targets#*#"

|------------ Commands

#Event LoadBuffs "#*#switch to buffs#*#"
#Event LoadDPS "#*#switch to dps spells#*#"
#Event ResetMacro "#*#okay, going to kill some shit#*#"
#Event Say "#*#say: #1#"
#Event FSay "#*#fs: #1#'"
#Event SwitchCommanderIrc "<#1#> I'm in charge now"
#Event TakeThePortal "#*#take the portal#*#"
#Event TimeToBail "#*#time to bail#*#"
#Event AssistMe "#*#assist me with#*#"
#Event Burn "#*#burn this guy down#*#"
#Event Burn2 "#*#burn this guy hard#*#"
#Event Hail "#*#hail this guy#*#"
#Event HailInvis "#*#hail this dude#*#"

#Event AssistMe "#*#AssistMe Plz#*#"


Sub Event_LoadBuffs
	/delay 4s !${Me.Casting.ID}
	/memspellset Buffs
	/delay 1s
	/delay 10s !${Window[SpellBookWnd].Open}
	/docommand /${ChatCommand} "loaded buffs"
/return TRUE

Sub Event_LoadDPS
	/delay 4s !${Me.Casting.ID}
	/memspellset DPS
	/delay 1s
	/delay 10s !${Window[SpellBookWnd].Open}
	/docommand /${ChatCommand} "loaded DPS spells"
/return TRUE

Sub Event_ResetMacro
	/docommand /${ChatCommand} "Restarting ${Macro.Name}"
	/i quit
	/delay 3s
	/keypress Ctrl+F12
	/delay 5
	/keypress Ctrl+F12
	/delay 5
	/keypress Ctrl+F12
	/delay 5
	/keypress Ctrl+F12
/return TRUE

Sub Event_AssistMe
	/assist ${RKCommander}
	/delay 1s ${Target.ID}
	/if (!${Target.ID}) /return FALSE

	/varset CurrentTargetId ${Target.ID}
	/if (${MarkMainTarget} && ${Group.Leader.ID} == ${Me.ID}) {
		/gmarknpc 1
	}

	/echo assist called
	/varset EngageOutOfCombatNPC TRUE

	/attack on
	/if (${Me.ID} == ${Group.Leader.ID}) /delay 1s
	/if (!${IsTank}) /delay 1s

	/if (${IsTank} && ${Me.Pet.ID}) {
		/pet back
		/pet attack
	}
	
	/face fast nolook
	/call SetStickTarget 13
/return

Sub Event_XP
	/call ResetMobMode
/return

Sub Event_XP2
	/call ResetMobMode
/return

Sub Event_XP3
	/call ResetMobMode
/return

Sub Event_XP4
	/call ResetMobMode
/return

Sub ResetMobMode
	/if (${Defined[ShouldCheckTankBuffs]}) /varset ShouldCheckTankBuffs TRUE
	/if (${Defined[ShouldICharm]} && ${CharmMode}) /varset ShouldICharm TRUE

	/varset CurrentTargetId 0
	/varset MOBMode ${DefaultMOBMode}
	/varset EngageOutOfCombatNPC FALSE
	/call ExecuteArrayOfSubs "XPEarnedSubs"
/return

Sub Event_Slowed
	|/bandolier Activate Standard
/return

Sub Event_FollowInvis
	/call SetBotMode 1
	/varset CurrentStickTargetId 0
	/varset SitBetweenCombat FALSE	
	/if (${Macro.Return}) {
		/makecamp off
		/call SetStickTarget ${RKCommanderId} 5
	}
	
	/i say "following invis"
/return

Sub Event_CampInvis
	/echo camping invis
	/call SetBotMode 2
	/varset SitBetweenCombat TRUE	
	/if (${Macro.Return}) {
		/call ClearStickTarget
		/makecamp on leash ${MaxTargetDistance} radius 120
	}
/return

Sub Event_FollowCombat
	/echo following combat
	/call SetBotMode 3
	/varset SitBetweenCombat FALSE	
	/varset CurrentStickTargetId 0
	/varset MaxTargetDistance ${If[${RKCommander.Equal[${Me.Name}]},5000,500]}
	
	/if (${Macro.Return}) {
		/echo SetStickTarget FollowCombat
		/call SetStickTarget ${RKCommanderId} 5
		/call ExecuteArrayOfSubs "SwitchToCombatModeSubs"
		/makecamp off
	}
	
	/i say "following combat"
/return

Sub Event_CampCombat
	/echo camping combat
	/call SetBotMode 4
	/varset SitBetweenCombat TRUE
	/varset MaxTargetDistance 700
	/if (${Macro.Return}) {
		/call ExecuteArrayOfSubs "SwitchToCombatModeSubs"
		/call ClearStickTarget
		/makecamp on leash ${MaxTargetDistance} radius 80	
	}
/return

Sub SetBotMode(int botMode)
	/if (${BOTMode} == 5) /return FALSE
	/attack off
	/keypress home
	/if (${Twist}) /twist off
	/varset BOTMode ${botMode}
	
	/return FALSE
	/declare botModeName string inner

	|0 = nothing / medding, 1 = follow & prevent action, 2 = set up camp & prevent action, 3 = follow & attack, 4 = set up camp & attack, 5 = manual piloting
	/if (${BOTMode} == 0) /varset botModeName nothing / medding
	/if (${BOTMode} == 1) /varset botModeName follow & prevent action
	/if (${BOTMode} == 2) /varset botModeName set up camp & prevent action
	/if (${BOTMode} == 3) /varset botModeName follow & attack
	/if (${BOTMode} == 4) /varset botModeName set up camp & attack
	/if (${BOTMode} == 5) /varset botModeName manual piloting
	
	|/echo "Changed BOTMode to ${botMode} - ${botModeName}"
/return TRUE

Sub Event_Attack
	/call GetTarget
	/if (!${Macro.Return}) {
		/assist ${RKCommander}
		/delay 5
	}
	/if (${HideCorpsesOnAttack}) /hidecorpse always
	/if (!${Target.ID}) /return
	/varset MOBMode ${DefaultMOBMode}

	/attack on
	
	/if (!${InCombatStickToCommander}) {
		/if (${IsTank} || ${Target.PctHPs} < 100) {
			/call SetStickTarget ${Target.ID} 5
		}
	}
/return

Sub Event_Burn
	/call RKAACast "Intensity of the Resolute"
	/varset MOBMode 2
/return

Sub Event_Burn2
	/call RKAACast "Intensity of the Resolute"
	/varset MOBMode 3
/return

Sub Event_Hail
	/call DropInvis
	/if (!${Me.Name.Equal[${RKCommander}]}) {
		/assist ${RKCommander}
		/delay 1s
	}
	/hail
/return

Sub Event_HailInvis
	/assist ${RKCommander}
	/delay 1s
	/hail
/return

Sub Event_FSay(eventText,sayText)
	/echo saying ${sayText}
	/call DropInvis
	/assist ${RKCommander}
	/delay 1s
	/say ${sayText}
	/delay 5
/return

Sub Event_Say(eventText,sayText)
	/echo saying ${sayText}
	/call DropInvis
	/assist ${RKCommander}
	/delay 1s
	/say ${sayText}
	/delay 5
/return

Sub SwitchCommander(string commanderName)
	/if (${commanderName.Left[1].Compare[ ]}<0) /varset commanderName ${commanderName.Right[-2].Left[-1]}
	/if (${Bool[${Raid.Members}]}) /return
	/varset RKCommander ${commanderName}
	/makeleader ${commanderName}
	/varset BOTMode 3
	/call ClearStickTarget
/return

Sub Event_SwitchCommanderIrc(eventText, commanderName)
	/echo irc message detected
	/call SwitchCommander "${commanderName}"
/return

Sub Event_CampCharacter
	/if (${Me.Mount.ID}) {
		/dismount
		/delay 1s
	}
	/camp
/return

Sub Event_CampDesktop
	/if (${Me.Mount.ID}) {
		/dismount
		/delay 1s
	}
	/camp desktop
	/endmac
/return

Sub Event_GotGroupInvite
	/invite
/return

Sub Event_TakeThePortal
	/notify LargeDialogWindow LDW_YesButton leftmouseup
/return

Sub Event_TimeToBail
	/if (${Me.AltAbilityReady[Exodus]}) {
		/call RKAACast "Exodus"
	} else {
		/call RKAACast "Gate"
	}
/return

Sub Event_TaskGranted
	/docommand /${ChatCommand} "Got the quest"
/return

Sub TrimName(string name)
	/declare trimLength int local 0
	/if (${name.Left[2].Equal[a ]}) /varset trimLength 2
	/if (${name.Left[3].Equal[an ]}) /varset trimLength 3
	/declare remainingStringLength int local
	/varcalc remainingStringLength ${name.Length} - ${trimLength}
/return ${name.Right[${remainingStringLength}]}

Sub Event_AddPullTarget
	/call TrimName "${Target.CleanName}"
	/declare trimmedName string local ${Macro.Return}
	/call AddToArray "PullTargets" "${trimmedName}"
	/call AddToArray "LoSPullTargets" "${trimmedName}"	
/return

Sub Event_AddExplicitLOSPullTarget(eventText, targetName)
	/call TrimName "${targetName}"
	/call AddToArray "LoSPullTargets" "${Macro.Return}"
/return 

Sub Event_AddExplicitPullTarget(eventText, targetName)
	/call TrimName "${targetName}"
	/call AddToArray "PullTargets" "${Macro.Return}"
/return 

Sub Event_AddExplicitPriorityTarget(eventText, targetName)
	/call TrimName "${targetName}"
	/call AddToArray "PriorityTargets" "${Macro.Return}"
/return 

Sub Event_AddLOSPullTarget
	/call TrimName "${Target.CleanName}"
	/call AddToArray "LoSPullTargets" "${Macro.Return}"
/return

Sub Event_AddPriorityPullTarget
	/call TrimName "${Target.CleanName}"
	/call AddToArray "PriorityTargets" "${Macro.Return}"
/return

Sub Event_ListPullTargets
	/declare i int local 0

	/echo --- Priority Targets ---
	/for i 1 to ${PriorityTargets.Size}
		/if (${Bool[${PriorityTargets[${i}]}]}) /echo ${PriorityTargets[${i}]}
	/next i
	
	/echo --- Pull Targets ---
	/for i 1 to ${PullTargets.Size}
		/if (${Bool[${PullTargets[${i}]}]}) /echo ${PullTargets[${i}]}
	/next i

	/echo --- LoS Targets ---
	/for i 1 to ${LoSPullTargets.Size}
		/if (${Bool[${LoSPullTargets[${i}]}]}) /echo ${LoSPullTargets[${i}]}
	/next i
/return FALSE

Sub Event_LowManaMedding
	/varset LowManaMed TRUE
/return FALSE

Sub Event_DoneMedding
	/varset LowManaMed FALSE
/return FALSE

Sub Event_LanguageSetToErudian
	/echo trying to fix Erudian language
	/delay 1s
	/declare languageId int local
	/varcalc languageId ${Me.Language[Elvish]} + 1
	/language ${languageId}
/return FALSE

Sub Event_LanguageSetToElvish
	/echo language is elvish
	/varset ChatLanguageVerified TRUE
/return FALSE

Sub Event_Camping
	/docommand /${ChatCommand} "Camping, ending macro."
	/makecamp off
	/endmac
/return FALSE

Sub Event_ReceivedTell(eventText, player, language, message)
	/if (${Me.Pet.ID} && ${Me.Pet.Name.Equal[${player}]}) /return FALSE
	/docommand /${ChatCommand} "tell from ${player} in ${language}: ${message}"
/return FALSE

Sub Event_TaskProgress
	|/delay 2
	|/fs got the quest update
	|/docommand /${ChatCommand} "got quest update."
/return FALSE