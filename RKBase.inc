#include RKEvents.inc
#include RKPullerBase.inc
#include RKCollections.inc
#include RKTargeting.inc
#include RKPositioning.inc

Sub Init_RKBase
	/keypress F12
	/keypress Num_5
	/timestamp off

	/declare RKCommander string outer ${Group.Leader.Name}
	/declare OverrideCommander bool outer TRUE
	/if (${OverrideCommander}) {
		/if (!${Group.Leader.Name.Equal[Underbyte]} && !${Group.Leader.Name.Equal[Nerdbait]} && !${Group.Leader.Name.Equal[Terranigma]}) {
			/if (${Spawn[pc Bumsnacker].ID}) /varset RKCommander Bumsnacker
			/if (!${Bool[${RKCommander}]}) /varset RKCommander ${Me.Name}
		}
	}
	/declare RKCommanderId int outer ${Spawn[pc ${RKCommander}].ID}

	/if (${RKCommanderId} == ${Me.ID}) {
		/afk off
	} else {
		/afk I'm on ${RKCommander}
	}
	
	/declare TankId int outer ${Group.MainTank.ID}
	/declare IsTank bool outer ${If[${TankId} == ${Me.ID},TRUE,FALSE]}
	/declare memberIndex int outer 0
	/if (!${Group.MainTank.ID}) {
		/for memberIndex 0 to ${Group.Members}
			/if (${Group.Member[${memberIndex}].Class.Name.Equal[Warrior]} || ${Group.Member[${memberIndex}].Class.Name.Equal[Paladin]} || ${Group.Member[${memberIndex}].Class.Name.Equal[Shadow Knight]}) {
				/varset TankId ${Group.Member[${memberIndex}].ID}
			}
		/next memberIndex
	}
	
	/if (${TankId} == 0) /varset TankId ${Me.ID}

	/if (!${Bool[${TankId}]} && ${Spawn[Underbyte pc].ID}) /varset TankId ${Spawn[Underbyte pc].ID}

	/makecamp off

	/if (${IsTank}) {
		/log on
	} else {
		/log off 
	}
	
	|---------- SYSTEM ----------
	/declare MacroPaused bool outer FALSE
	/declare DebugMode bool outer FALSE
	| 0 = nothing / medding, 1 = follow & prevent action, 2 = set up camp & prevent action, 3 = follow & attack, 4 = set up camp & attack, 5 = manual piloting
	/declare BOTMode int outer 3
	| 0 = nothing, 1 = standard, 2 = burn, 3 = burn2
	/declare MOBMode int outer 1
	/declare DefaultMOBMode int outer 1
	/declare DynamicArraySize int outer 30
	/declare EarlyEscapePreviousTick bool outer TRUE
	/declare EarlyEscapePauseDuration int outer 0
	/declare CcTargetsInRange int outer
	
	|---------- BEHAVIOR ----------
	/declare GlyphMode bool outer FALSE
	/declare InCombatStickToCommander bool outer TRUE
	/declare HideCorpsesOnAttack bool outer TRUE
	/declare SitBetweenCombat bool outer TRUE
	/declare IsAutoSitting bool outer FALSE
	/declare MarkMainTarget bool outer TRUE

	/declare Descending bool outer FALSE
	
	/declare MercBurnThreshold int outer 90
	/declare UpperAggroThreshold int outer 95
	/declare TargetHpEngagePercentage int outer 200
	/declare MaxTargetDistance int outer ${If[${IsTank}, 100, 400]}
	/declare MaxMeleeDistance int outer 14
	/declare StickTimer timer outer 10
	/declare StickDistance string outer 13
	/declare StickFlags string outer
	
	/declare MoveTimer timer outer 20	
	/declare FaceTimer timer outer 20		
	
	/declare ShouldBroadcastMed bool outer FALSE
	/declare LowManaMed bool outer FALSE
	/declare MeddingMessageTimer timer outer 10s
	
	/declare ForageMode bool outer FALSE
	/declare KeepAgroOnAdds bool outer FALSE
	
	|---------- PULLING ----------
	/declare PullMode bool outer FALSE
	/declare SaveNewPullTargets bool outer FALSE
	/declare PullTargets[100] string outer
	/declare LoSPullTargets[100] string outer
	/declare LoSPullRadius int outer 5000
	/declare PriorityTargets[100] string outer	
	/declare PullWithAutoFire bool outer FALSE
	/declare PullTargetEvaluationId int outer 0 	
	/declare PullTargetId int outer 0
	/declare PullTargetDistance int outer 10000
		
	|---------- POSITION ----------
	/declare MobsCentroidX float outer
	/declare MobsCentroidY float outer
	/declare AECentroidX float outer
	/declare AECentroidY float outer
	
	|---------- SYSTEM - COMMUNICATION ----------
	/declare IrcConnectionString string outer irc.lavishsoft.com 6667 #Underbyte 
	|${RKCommander}
	/declare ChatCommand string outer fs
	/declare ChatChannelName string outer ${RKCommander}
	
	|---------- SYSTEM - TARGETING ----------
	/declare CurrentTargetId int outer 0
	/declare CurrentStickTargetId int outer 0	
	|/declare BadTargetIds[100] int outer
	
	|---------- SYSTEM - LIFE CYCLE ----------
	/declare InvisModeSubs[${DynamicArraySize}] string outer
	/call AddToArray "InvisModeSubs" "CheckCommanderFollow"	
	/call AddToArray "InvisModeSubs" "CheckInvisModeCombat"
	/call AddToArray "InvisModeSubs" "CheckSitting"

	/declare NonCombatSubs[${DynamicArraySize}] string outer
	/call AddToArray "NonCombatSubs" "CheckSitting"
	/call AddToArray "NonCombatSubs" "ForageAndBag"
	/call AddToArray "NonCombatSubs" "CheckCommanderFollow"	
	
	/declare PreTargetAcquisitionSubs[${DynamicArraySize}] string outer
	/call AddToArray "PreTargetAcquisitionSubs" "CheckCommanderFollow"
	
	/declare CombatActionSubs[${DynamicArraySize}] string outer
	/call AddToArray "CombatActionSubs" "CheckPosition"
	/declare BurnCombatActionSubs[${DynamicArraySize}] string outer
	/declare Burn2CombatActionSubs[${DynamicArraySize}] string outer	
	/declare DefensiveActionSubs[${DynamicArraySize}] string outer	
	/declare XPEarnedSubs[${DynamicArraySize}] string outer
	
	/declare OutOfCombatMessageTimer timer outer 1s	
	/declare EngageOutOfCombatNPC bool outer FALSE
	
	|---------- PULLING ----------
	/call Init_RKPullerBase
	
	|---------- CHANNELS & CHAT ----------
	/call ConfigureChannels	
	/declare ChatLanguageVerified bool outer FALSE
	
	/hidecorpse always
	/echo Done initializing RK Base!
/return

Sub Tick
	/if (${DebugMode}) /echo ------------- BEGIN LOOP! -------------		
	/if (${DebugMode}) /echo ------------- BEGIN LOOP! -------------		
	/if (${DebugMode}) /echo ------------- BEGIN LOOP! -------------		
	/if (!${ChatLanguageVerified}) /lang
	
	/doevents
	/delay 1

	/if (${DebugMode}) /echo tick
	
	/if (${Me.Casting.ID} && !${Me.Class.Name.Equal[Bard]}) /return FALSE
	/if (!${Me.Class.Name.Equal[Bard]} && ${TankId} != ${Me.ID} && ${Me.Moving}) /return FALSE
	/if (${Window[SpellBookWnd].Open}) /return FALSE
	/if (${Bool[${Me.XTarget[1].ID}]} && !${Target.ID}) /target id ${CurrentTargetId}
	/if (${EarlyEscapePreviousTick}) /delay ${EarlyEscapePauseDuration}
	/if (${MacroPaused} || ${Me.State.Equal[HOVER]}) /return FALSE

	/call CheckCommanderAndTank
	
	/if (${Zone.ShortName.Equal[GuildLobby]} && ${BOTMode} != 1) {
		/echo You're in the Guild Lobby.  Bots are disabled unless BOTMode = 1 (Follow Invis)
		/delay 5s
		/return TRUE
	}
	
	/if (${Spawn[id ${CurrentStickTargetId}].Type.Equal[Corpse]} || (${Target.ID} && ${Target.Type.Equal[PC]} && ${Me.XTarget[1].ID} && ${BOTMode} > 3 && ${BOTMode} != 5)) {
		/call ClearStickTarget
		/target clear
	}

	/if (${DebugMode}) /echo tick 2
	
	/if (!${Bool[${CurrentTargetId}]}) /varset CurrentTargetId ${If[${Target.ID},${Target.ID},${Me.ID}]}
	
	/if (${SaveNewPullTargets} && ${Target.ID} && ${Target.PctHPs} < 90 &&  ${Target.Type.Equal[NPC]}) {
		/call TrimName "${Target.CleanName}"
		/declare trimmedName string local ${Macro.Return}
		/call GetStringArrayIndex "LoSPullTargets" "${trimmedName}"
		/if (!${Macro.Return}) {
			|/echo Adding ${Target.CleanName} to pull targets
			|/call AddToArray "PullTargets" "${trimmedName}"
			/if (${DebugMode}) /echo Adding ${Target.CleanName} to LoS pull targets			
			/call AddToArray "LoSPullTargets" "${trimmedName}"
		}
	}

	|If I'm sticking to a Spawn that's not my target and the thing I'm sticking to is another NPC, I'm going to drop stick.
	/if (${CurrentStickTargetId} && ${CurrentStickTargetId} != ${Target.ID} && ${Spawn[id ${CurrentStickTargetId}].Type.Equal[NPC]}) {
		/if (${Stick.Active}) /multiline ; /stick off ; /echo stick off 2
	}
	
	/if (${DebugMode}) /echo tick 3
	
	/call UpdateMercenaryStance
	/if (${DebugMode}) /echo mark - 4
	/call EvaluateTargets
	/if (${Macro.Return}) /return FALSE

	/if (${DebugMode}) /echo tick 4	

	/call DoCombatRoutine
	
	/if (${DebugMode}) /echo mark - 7
	/if (${Target.Type.Equal[PC]} && ${BOTMode} != 1) /target clear
	
	/if (${Macro.Return}) /return TRUE
	
	/if (${DebugMode}) /echo tick 5
/return FALSE

Sub ClearPullTarget
	/if (${DebugMode}) /echo clearing pull target
	/varset PullTargetId 0
	/varset CurrentTargetId 0
	/varset PullTargetDistance 10000
	/target clear
/return FALSE

Sub EvaluateTargets
	/if (${Bool[${Me.XTarget[1].ID}]}) {
		/varset LowManaMed FALSE
		/call EvaluateAETargets
	} else {
		/call DoOutOfCombatRoutine
	
		|/if (!${Target.ID} || ${Target.Type.Equal[PC]} || ${EngageOutOfCombatNPC} == FALSE) /return TRUE
		| --- IF WE DON'T HAVE ANY MOBS ON XTarget ---
		/if (${PullMode}) {
			/if (!${Target.ID}) /return FALSE
			/if (!${Target.Type.Equal[NPC]}) {
				/if (${DebugMode}) /echo target isn't an NPC
				/call ClearPullTarget
			}
			/if (${DebugMode}) /echo Pull mode engaged, engaging ${Target.CleanName}
			/if (!${Stick.Active}) /call SetStickTarget ${Target.ID} 10
			/if (${Target.Distance} > 40 && ${PullWithAutoFire} && !${Me.AutoFire}) {
				/autofire
			} else {
				/if (${DebugMode}) /echo attacking ${Target.CleanName}
				/attack on
			} 

			/if (${Target.Distance} < 40) {
				/if (${Target.Z} < (${Me.Z} - 5)) {
					/if (!${Descending}) {
						/if (${DebugMode}) /echo target: ${Target.Z} < me: ${Me.Z} - Descending
						/keypress end hold
						/varset Descending TRUE
					}
				} else {
					/if (${Descending}) {
						/keypress end
						/varset Descending FALSE
					}
				}
			}	
			
			|/call CheckTargetDistance ${Target.ID}
			/call RKAACast "Banestrike"
		} else {
			/if (!${Target.ID}) /return TRUE
			/if (${Target.Type.Equal[PC]}) /return TRUE
			/if (!${EngageOutOfCombatNPC}) /return TRUE
			/if (${Target.PctHPs} == 100) {
				/if (${OutOfCombatMessageTimer} == 0) {
					/varset OutOfCombatMessageTimer 10
					/if (${DebugMode}) /echo target's hp are 100%
				}
				/return TRUE
			}
			
			|/if (${BOTMode} != 5 && !${Target.Type.Equal[NPC]} && !${Target.Type.Equal[Object]}) {
			|	/if (${OutOfCombatMessageTimer} == 0) {
			|		/varset OutOfCombatMessageTimer 10
			|		/echo target isn't an NPC
			|	}
			|	/return TRUE
			|}
			
			/if (!${EngageOutOfCombatNPC} && ${BOTMode} != 5) {
				/if (${OutOfCombatMessageTimer} == 0) {
					/varset OutOfCombatMessageTimer 10
					/if (${DebugMode}) /echo EngageOutOfCombatNPC is FALSE
				}
				/return TRUE
			}
		}
	}
/return FALSE

Sub CheckCommanderAndTank
	/if (${Group.MainTank.ID} && ${Group.MainTank.ID} != ${TankId}) {
		/echo Changing main tank from ${Spawn[id ${TankId}].Name} to ${Group.MainTank.Name}
		/varset TankId ${Group.MainTank.ID}
	}
	
	/if (${Spawn[pc ${RKCommander}].ID} && ${Spawn[pc ${RKCommander}].ID} != ${RKCommanderId}) {
		/varset RKCommanderId ${Spawn[pc ${RKCommander}].ID}
	}
	
	/if (${DebugMode}) /echo CheckCommanderAndTank 1

	/if (${Bool[${RKCommander}]} && !${OverrideCommander}) /return FALSE
	
	/if (${DebugMode}) /echo CheckCommanderAndTank 2
	
	/if (${Group.Leader.ID} && !${Group.Leader.Name.Equal[${RKCommander}]} && !${Me.Name.Equal[${RKCommander}]}) {
		/echo Changing commander from ${RKCommander} to ${Group.Leader.Name}
		/varset RKCommander ${Group.Leader.Name}
	}
/return FALSE

Sub SelectAndMarkTarget
	/if (${DebugMode}) /echo Starting targeting sequence
	|/call ClearArray "BadTargetIds"
	/call GetTarget
	/if (!${Macro.Return}) /return FALSE
	/if (${DebugMode}) /echo got target
	/if (${CurrentTargetId} != ${Macro.Return}) {
		/varset CurrentTargetId ${Macro.Return}
		/if (${DebugMode}) /echo CurrentTargetId set to ${CurrentTargetId}
	}

	/if (${Me.GroupMarkNPC[1].ID} && ${Me.GroupMarkNPC[1].ID} == ${CurrentTargetId}) /return FALSE
	/if (!${MarkMainTarget} || !${Group} || ${Group.MarkNpc.ID} != ${Me.ID}) /return FALSE
	/gmarknpc 1
/return FALSE

Sub DoCombatRoutine
	/call ExecuteArrayOfSubs "DefensiveActionSubs"
	/if (${DebugMode}) /echo Done with defensives, doing combat routine
	/if (${DebugMode}) /echo combat tick 1
	/if (${BOTMode} != 5) {
		/if (${DebugMode}) /echo doing PreTargetAcquisitionSubs
		/call ExecuteArrayOfSubs "PreTargetAcquisitionSubs"
		/if (${Macro.Return}) {
			/echo Returning early from PreTargetAcquisitionSubs
			/return TRUE
		}
		
		/call SelectAndMarkTarget
	}
	/if (${DebugMode}) /echo combat tick 2
	
	/if (${Spawn[id ${CurrentStickTargetId}].Type.Equal[PC]} && ${BOTMode} != 1) {
		/call ClearStickTarget
		|/stick off
		/keypress s
		/delay 1
	}
		
	/varset EarlyEscapePreviousTick TRUE
	/if (!${Target.ID} || !${Me.XTarget[1].ID}) /return FALSE
	|/if (!${Target.Type.Equal[NPC]} && !${Target.Type.Equal[Object]}) /return FALSE
	/if (${DebugMode}) /echo Selected target ${Target.ID}

	/if (${DebugMode}) /echo combat tick 3
	
	/if (${IsTank}) {
		/if (${TauntMode} && ${Me.PctAggro} < 100) /doability "Taunt"
	} else { 
		/if (${Me.PctAggro} > ${UpperAggroThreshold}) {
			/if (${DebugMode}) /echo Exiting early because of agro
			/return FALSE
		}
	}

	/if (${DebugMode}) /echo combat tick 4 
	
	/if (${Bool[${Me.XTarget[1].ID}]} || ${Target.ID}) {
		/if (!${Bool[${Me.XTarget[1].ID}]} && ${Target.PctHPs} == 100) /return TRUE
		
		/if (${MOBMode} >= 2 || ${Me.XTarget[4].ID}) {
			/call ExecuteArrayOfSubs "BurnCombatActionSubs"
			/if (${Macro.Return}) /return TRUE
		}
		
		/if (${MOBMode} == 3 || ${Me.XTarget[5].ID}) {
			/call ExecuteArrayOfSubs "Burn2CombatActionSubs"
			/if (${Macro.Return}) /return TRUE
		}

		/if (${MOBMode} == 1 && ${Target.Named}) /varset MOBMode 2
		
		/call ExecuteArrayOfSubs "CombatActionSubs"
		/if (${Macro.Return}) /return TRUE
	} else {
		/varset MOBMode ${DefaultMOBMode}
	}
	
	/if (${DebugMode}) /echo combat tick 5

	/varset EarlyEscapePreviousTick FALSE
/return FALSE

Sub DoOutOfCombatRoutine
		/if (${DebugMode}) /echo DoOutOfCombatRoutine
		
		/if (${BOTMode} < 3 || (${BOTMode} == 5 && ${Me.Invis})) {
			/call ExecuteArrayOfSubs "InvisModeSubs"
			/return FALSE
		}
	
		/if (${Me.Class.CanCast} && ${Me.PctMana} < 10) {
			/varset LowManaMed TRUE
		}	
		/if (${DebugMode}) /echo mark - 2
		/if (${LowManaMed}) {
			/if (${Stick.Active}) {
				/call ClearStickTarget
				/if (${DebugMode}) /echo stick off 1
			}
			/declare doneMedding bool local TRUE
			/declare i int local 
			/for i 0 to ${Group.Members}
				/if (${Group.Member[${i}].ID} && !${Group.Member[${i}].Mercenary}) {
					/if (${Group.Member[${i}].Class.CanCast} && ${Group.Member[${i}].PctMana} < 99) {
						/varset doneMedding FALSE
					}
				}
			/next i
		
			/if (${doneMedding}) {
				/varset LowManaMed FALSE
				/docommand /${ChatCommand} "full mana, all done"
				/sit off			
			}
		}

		/if (${LowManaMed} && !(${Me.Sitting} || ${Me.Mount.ID})) {
			/if (${Twist}) {
				/twist off
				/stopcast
			}
			/if (${MeddingMessageTimer} == 0 && ${ShouldBroadcastMed}) {
				/docommand /${ChatCommand} "medding, you should med"
				/varset MeddingMessageTimer 10s
			}
			/sit on
		}

		/if (${LowManaMed}) /return TRUE
	
	
		/if (${DebugMode}) /echo mark - 3
		/call ExecuteArrayOfSubs "NonCombatSubs"
/return FALSE

Sub FaceAECentroid
	/if (${FaceTimer} > 0) {
		/if (${DebugMode}) /echo Exiting FaceAECentroid - Timer = ${FaceTimer}
		/return FALSE
	}
	/varset FaceTimer 4
	/if (${AECentroidY} == 0 || ${AECentroidX} == 0) {
		/call EvaluateAETargets
		/if (!${Macro.Return}) /return FALSE
	}
	/call DistanceToCentroid
	|/if (${DebugMode}) 
	/if (${DebugMode}) /echo Facing AE centroid - distance = ${Macro.Return}
	/if (${Macro.Return} > 25) /return FALSE
	/face fast loc ${AECentroidY}, ${AECentroidX} nolook
/return FALSE

Sub ForageAndBag
	/if (!${ForageMode}) /return FALSE
	/autoinv
 	/if (${Me.AbilityReady[Forage]}) {
		/doability "Forage"
		/delay 2
		/autoinv
		/delay 2
		/autoinv		
		/autoinv
	}
/return FALSE

Sub ConfigureChannels
	/if (${DebugMode}) /echo ConfigureChannels
	/if (!${Irc}) {
		/echo /iconnect ${IrcConnectionString} ${Me.Name}
		/iconnect ${IrcConnectionString} ${Me.Name}
		/delay 10s ${Irc}
	}
	
	/if (${Irc}) {
		/varset ChatCommand i say
	} else {
		/if (${MacroQuest.ChatChannels} == 0) {
			/echo Universal chat service is down.
			/varset ChatCommand echo
			/return FALSE
		}

		/if (!${MacroQuest.ChatChannel[1].Equal[${ChatChannelName}]}) {
			/echo Fixing chat channels - leaving ${MacroQuest.ChatChannels} channels
			/declare currentChannel int local
			/declare channelsToLeave int local ${MacroQuest.ChatChannels}
		
			/for currentChannel 1 to ${channelsToLeave}
				/echo leaving channel ${currentChannel}
				/leave 1
				/delay 1s
			/next currentChannel

			/autojoin ${ChatChannelName}, General
			/delay 5
			/join ${ChatChannelName}, General
		}
		
		/varset ChatCommand 1
	}
/return FALSE

Sub CheckSitting
	/if (${DebugMode}) /echo CheckSitting
	/if (!${SitBetweenCombat}) /return FALSE
	/if (${BOTMode} != 2 && ${BOTMode} != 4 && ${BOTMode} != 0) /return FALSE
	
	/if (!${Bool[${Me.XTarget[1].ID}]} && !${Me.Mount.ID} && !${Me.Sitting} && !${Me.Moving} && (${Me.PctMana} < 70 || ${Me.PctEndurance} < 60)) {
		/sit on
		/varset IsAutoSitting TRUE
	}
		
	/if (${Me.Sitting} && ${IsAutoSitting} && ${Me.PctMana} >= 99 && ${Me.PctEndurance} >= 99) {
		/sit off
		/varset IsAutoSitting FALSE
	}
/return FALSE

|.. check http://www.macroquest2.com/wiki/index.php/Slot_Names
Sub RKItemSlotCast(string itemSlot)
	/declare itemSpell string local ${Me.Inventory[${itemSlot}].Spell}
	/if (!${Bool[${itemSpell}]}) {
		/echo trying to cast item ${itemSlot} - ${Me.Inventory[${itemSlot}]}, but it doesn't have a clicky
		/return FALSE
	}

	/call WillSelfBuffStack "${itemSpell}"
	/if (${Macro.Return}) {
		/if (${DebugMode}) /echo cast item "${Me.Inventory[${itemSlot}].Name}"
		/cast item "${Me.Inventory[${itemSlot}].Name}"
	} else {
		/if (${DebugMode}) /echo ${itemSpell} won't stack
	}
/return ${Cast.Result.Equal[CAST_SUCCESS]}

Sub RKItemCast(string buffOrSongName, string itemName)
	/if (${DebugMode}) /echo RKItemCast ${buffOrSongName} ${itemName}
	/if (${Me.Buff[${buffOrSongName}].ID} || ${Me.Song[${buffOrSongName}].ID}) /return FALSE
	
	/call WillSelfBuffStack "${buffOrSongName}"
	/if (${Macro.Return}) /cast item "${itemName}"
/return ${Cast.Result.Equal[CAST_SUCCESS]}

Sub WillSelfBuffStack(string spellName)
	/if (${DebugMode}) /echo WillSelfBuffStack 
	/declare i int local
	/for i 1 to 42
		/if (${Me.Buff[${i}].ID} && !${Spell[${spellName}].WillStack[${Me.Buff[${i}]}]}) {
			/if (${DebugMode}) /echo Self buff: ${spellName} will not stack with ${Me.Buff[${i}].Name}
			/return FALSE
		}
	/next i
	
	/if (${DebugMode}) /echo ${spellName} WILL stack
/return TRUE

Sub WillBuffStack(string spellName, int targetId)
	/if (${DebugMode}) /echo WillSelfBuffStack 
	/call TargetSync ${targetId}
	/delay 5 ${Bool[${Target.Buff}]}
	/declare i int local
	/for i 1 to 42
		/if (${Target.Buff[${i}].ID} && !${Spell[${spellName}].WillStack[${Target.Buff[${i}]}]}) {
			/if (${DebugMode}) /echo Buff: ${spellName} will not stack with ${Target.Buff[${i}].Name} on Target Id: ${targetId}
			/return FALSE
		}
	/next i
	
	/if (${DebugMode}) /echo ${spellName} WILL stack
/return TRUE

Sub UpdateMercenaryStance
	/if (${DebugMode}) /echo UpdateMercenaryStance
	/if (!${Bool[${Me.Mercenary}]} || ${Me.Mercenary.State.Equal[Suspended]}) /return FALSE

	/if (${Me.Mercenary.Class.ClericType}) /return FALSE
	
	/if (${Me.Mercenary.Stance.Equal[Burn]}) {
		/if (!${Target.ID} || ${Target.PctHPs} > ${MercBurnThreshold} || ${Me.TargetOfTarget.ID} == ${Me.Mercenary.ID}) {
			/stance balanced
		}
	} else /if (${Me.Mercenary.Stance.Equal[Balanced]}) {
		/if (${Target.ID} && ${Target.PctHPs} < ${MercBurnThreshold} && ${Me.TargetOfTarget.ID} != ${Me.Mercenary.ID}) {
			/stance burn
		}
	}
/return FALSE

Sub CheckInvisModeCombat
	/if (${DebugMode}) /echo CheckInvisModeCombat

	/if (${Bool[${Me.XTarget[1].ID}]}) {
		/docommand /${ChatCommand} "followcombat"
		/delay 5
		/return FALSE
	}
/return FALSE

Sub RKAACast(string AAName)
	/if (${DebugMode}) /echo RKAACast ${AAName}
	/if (${Me.Casting.ID} && !${Me.Class.Name.Equal[Bard]}) /return FALSE

	/if (${Me.AltAbilityReady[${AAName}]}) {
		/alt act ${AltAbility[${AAName}].ID}
		/return FALSE
	}
/return FALSE

Sub RKAACast(string AAName, int targetId)
	/if (${DebugMode}) /echo RKAACast name: ${AAName} target:${targetId}
	/if (${Me.Casting.ID} && !${Me.Class.Name.Equal[Bard]}) /return FALSE	

	/if (${AltAbility[${AAName}].Spell.SpellType.Equal[Detrimental]} && ${Spawn[id ${targetId}].Type.Equal[PC]}) /return FALSE
	/if (${AltAbility[${AAName}].Spell.SpellType.Equal[Beneficial]} && ${Spawn[id ${targetId}].Type.Equal[NPC]}) /return FALSE
	
	/if (${Target.ID} != ${targetId}}) {
		/target id ${targetId}
		/delay 1s ${Target.ID} == ${targetId}
	}
	
	/if (${Me.AltAbilityReady[${AAName}]}) {
		/if (!${Target.Buff[${Me.AltAbility[${AAName}].Spell.Name}].ID}) {
			/alt activate ${AltAbility[${AAName}].ID}
			/return FALSE
		}
	}
/return FALSE

Sub RKAACast(string AAName, string buffName, int targetId)
	/if (${DebugMode}) /echo RKAACast ${AAName} buffName: ${buffName} targetId: ${targetId}
	/if (${Me.Casting.ID} && !${Me.Class.Name.Equal[Bard]}) /return FALSE
	if (${Me.Buff[${buffName}].ID}) /return FALSE
	
	/if (${Target.ID} != ${targetId}}) {
		/target id ${targetId}
		/delay 1s ${Target.ID} == ${targetId}
	}
	
	/if (${Me.AltAbilityReady[${AAName}]}) {
		/alt activate ${AltAbility[${AAName}].ID}
		/return FALSE
	}
/return FALSE

Sub RKDisc(string discName)
	/if (${DebugMode}) /echo RKDisc ${discName}
	/if (${Me.Casting.ID} && !${Me.Class.Name.Equal[Bard]}) /return FALSE
	/if (!${Me.CombatAbilityReady[${discName}]}) /return FALSE
	/if (${Bool[${Me.CombatAbility[${discName}]}]}) {
		/if (${Me.CombatAbility[${Me.CombatAbility[${discName}]}].Duration} > 0 && ${Bool[${Me.ActiveDisc}]}) /return FALSE
		/if (${DebugMode}) /echo Casting ${Me.CombatAbility[${Me.CombatAbility[${discName}]}].Name}
	} else {
		/if (${DebugMode}) /echo casting ${discName}, though it's not a CombatAbility
	}
	
	
	/disc ${discName}
	/delay 1
	/disc ${discName}	
	/delay 2	
	/disc ${discName}		
/return FALSE

Sub ExecuteBaseDefensiveActions

/return FALSE

Sub DropInvis
	/attack on
	/delay 5
	/attack off
	/delay 1s !${Me.Invis}
/return FALSE

