ScriptName ShazdehUtils Hidden

;----------------------------------------------------------------------------------------------------------
;Game
;----------------------------------------------------------------------------------------------------------

; game language, default is "english"
String Function GetGameLanguage() Global Native


;----------------------------------------------------------------------------------------------------------
;Actor
;----------------------------------------------------------------------------------------------------------

; HasPerk
Bool Function ActorHasAnyPerkInList(Actor akActor, FormList akList) Global Native
Bool Function ActorHasAllPerksInList(Actor akActor, FormList akList) Global Native
Bool Function ActorHasAnyPerkInArray(Actor akActor, Perk[] akList) Global Native
Bool Function ActorHasAllPerksInArray(Actor akActor, Perk[] akList) Global Native

; Counts how many perks in akList does akActor have
Int Function CountActorPerksInList(Actor akActor, FormList akList) Global Native
Int Function CountActorPerksInArray(Actor akActor, Perk[] akList) Global Native

; AddPerk
Function AddAllPerksInListToActor(Actor akActor, FormList akList) Global Native
Function AddAllPerksInArrayToActor(Actor akActor, Perk[] akList) Global Native

; RemovePerk
Function RemoveAllPerksInListFromActor(Actor akActor, FormList akList) Global Native
Function RemoveAllPerksInArrayFromActor(Actor akActor, Perk[] akList) Global Native

; HasSpell
Bool Function ActorHasAnySpellInList(Actor akActor, FormList akList) Global Native
Bool Function ActorHasAnySpellInArray(Actor akActor, Spell[] akList) Global Native
Bool Function ActorHasAllSpellsInList(Actor akActor, FormList akList) Global Native
Bool Function ActorHasAllSpellsInArray(Actor akActor, Spell[] akList) Global Native

; Count how many spells in akList does akActor have
Int Function CountActorSpellsInList(Actor akActor, FormList akList) Global Native
Int Function CountActorSpellsInArray(Actor akActor, Spell[] akList) Global Native
Int Function CountActorSpellsWithKeyword(Actor akActor, Keyword akKeyword) Global Native

; AddSpell
Function AddAllSpellsInListToActor(Actor akActor, FormList akList) Global Native
Function AddAllSpellsInArrayToActor(Actor akActor, Spell[] akList) Global Native

; RemoveSpell
Function RemoveAllSpellsInListFromActor(Actor akActor, FormList akList) Global Native
Function RemoveAllSpellsInArrayFromActor(Actor akActor, Spell[] akList) Global Native

; IsInFaction
Bool Function IsActorInAnyFactionInList(Actor akActor, FormList akList) Global Native
Bool Function IsActorInAnyFactionInArray(Actor akActor, Faction[] akList) Global Native
Bool Function IsActorInAllFactionsInList(Actor akActor, FormList akList) Global Native
Bool Function IsActorInAllFactionsInArray(Actor akActor, Faction[] akList) Global Native

; AddToFaction
Function AddActorToAllFactionsInList(Actor akActor, FormList akList, Int aiRank = 0) Global Native
Function AddActorToAllFactionsInArray(Actor akActor, Faction[] akList, Int aiRank = 0) Global Native

; RemoveFromFaction
Function RemoveActorFromAllFactionsInList(Actor akActor, FormList akList) Global Native
Function RemoveActorFromAllFactionsInArray(Actor akActor, Faction[] akList) Global Native

Bool Function HasStolenWeaponEquipped(Actor akActor, Bool abLeftHand = False) Global Native

; similar to Actor.GetAV() but takes the Actor Value index instead, easier for looping through
Float Function GetAVByIndex(Actor akActor, Int aiIndex) Global Native

; similar to Actor.GetBaseAV() but takes the Actor Value index instead, easier for looping through
Float Function GetBaseAVByIndex(Actor akActor, Int aiIndex) Global Native

String Function GetAVName(Int aiIndex) Global Native

; Bool Function IsDiseased(Actor akActor) Global Native
; Int Function GetDiseasesCount(Actor akActor) Global Native

Int Function CountWornWithKeyword(Actor akActor, keyword akKeyword) Global Native

Int Function CountCommandingActors(Actor akActor) Global Native

; akFilter can be Race, or a FormList of Races
Int Function CountHostilesToActor(Actor akActor, Form akFilter = None) Global Native


;----------------------------------------------------------------------------------------------------------
;Clipboard
;----------------------------------------------------------------------------------------------------------

Bool Function SetClipboard(String asText) Global Native

String Function GetClipboard() Global Native


;----------------------------------------------------------------------------------------------------------
;Forms
;----------------------------------------------------------------------------------------------------------

; Finds form by either its Editor ID, or by Form ID
Form Function LookupFormSmart(String asID) Global Native

Form[] Function GetFormsByEditorID(String[] IDs) Global Native

; returns the 4 character Form signature
String Function GetFormTypeString(Form akForm) Global Native

Bool Function HasAnyKeywordInList(Form akForm, FormList akList) Global Native

Bool Function HasAllKeywordsInList(Form akForm, FormList akList) Global Native

; akForm can be either: Spell, Potion, Ingredient, Scroll, Enchantment
Bool Function FormHasMagicEffect(Form akForm, MagicEffect akEffect) Global Native

; akForm can be either: Spell, Potion, Ingredient, Scroll, Enchantment
Bool Function FormHasMagicEffectKeyword(Form akForm, Keyword akKeyword) Global Native

Form Function GetFormWithStrongestEffect(ObjectReference akSource, MagicEffect akEffect, Int aiFormType, Keyword akKeywordFilter = None) Global Native

Form Function GetFormWithWeakestEffect(ObjectReference akSource, MagicEffect akEffect, Int aiFormType, Keyword akKeywordFilter = None) Global Native

Potion Function GetStrongestPotion(ObjectReference akSource, MagicEffect akEffect, Keyword akKeywordFilter = None) Global
    Return GetFormWithStrongestEffect(akSource, akEffect, 46, akKeywordFilter) as Potion
EndFunction

Spell Function GetStrongestSpell(ObjectReference akSource, MagicEffect akEffect, Keyword akKeywordFilter = None) Global
    Return GetFormWithStrongestEffect(akSource, akEffect, 22, akKeywordFilter) as Spell
EndFunction

Potion Function GetWeakestPotion(ObjectReference akSource, MagicEffect akEffect, Keyword akKeywordFilter = None) Global
    Return GetFormWithWeakestEffect(akSource, akEffect, 46, akKeywordFilter) as Potion
EndFunction

Spell Function GetWeakestSpell(ObjectReference akSource, MagicEffect akEffect, Keyword akKeywordFilter = None) Global
    Return GetFormWithWeakestEffect(akSource, akEffect, 22, akKeywordFilter) as Spell
EndFunction

; Recursive HasForm that checks FormList(s) within akList as well
Bool Function FormlistHasFormDeep(FormList akList, Form akForm) Global Native

; akFormFilter can be:
;   None;
;   a Keyword (only trasnfer items with that keyword);
;   or a FormList (only transfer items within that FormList),
;   or Form
Function TransferItems(ObjectReference akSource, ObjectReference akTarget, Int aiCount = -1, Int aiFormType = 0, Form akFormFilter = None, Bool abExcludeWorn = True, Bool abExcludeFavorites = True, Bool abExcludeQuestItems = True) Global Native

Int Function GetItemCountInContainersArray(ObjectReference[] aReferences, Form akForm) Global Native

Int Function GetItemCountInContainersList(FormList akList, Form akForm) Global Native

Function TransferItemFromContainersArray(FormList akList, ObjectReference[] akRef, Form akItem, Int aiCount) Global Native

Function TransferItemFromContainersList(FormList akList, ObjectReference akRef, Form akItem, Int aiCount) Global Native


;----------------------------------------------------------------------------------------------------------
;Gamepad
;----------------------------------------------------------------------------------------------------------

; Game.IsUsingGamepad() return false if the last pressed key is from keyboard,
; whereas this strictly checks whether gamepad is connected.
Bool Function IsGamepadConnected() Global Native

; Simulate pressing the left thumb stick on gamepad
Function SimulateLeftStickInput(Float afXValue = 0.0, Float afYValue = 0.0) Global Native


;----------------------------------------------------------------------------------------------------------
;ObjectReference
;----------------------------------------------------------------------------------------------------------

; used for teleport doors, gives you the other door connected to this Reference
ObjectReference Function GetLinkedDoor(ObjectReference akRef) Global Native

Actor[] Function FindActorsInFactionNearRef(ObjectReference akRef, Float afRadius, Faction akFaction, Bool abIgnoreDead = True, Bool abIgnorePlayer = True) Global Native

Actor[] Function FindActorsWithVoiceTypeNearRef(ObjectReference akRef, Float afRadius, VoiceType akVoice, Bool abIgnoreDead = True) Global Native

; akFilter can be either Race, or FormList of races
Actor[] Function FindActorsOfRaceNearRef(ObjectReference akRef, Float afRadius, Form akFilter, Bool abIgnoreDead = True) Global Native


;----------------------------------------------------------------------------------------------------------
;String
;----------------------------------------------------------------------------------------------------------

; get text from interface translation file by asKey
String Function GetTranslatedString(String asKey) Global Native

; search & replace all Needles with Replaces inside Haystack
String Function StringReplace(String[] asNeedles, String[] asReplaces, String asHaystack, Bool abIgnoreCase = False) Global Native


;----------------------------------------------------------------------------------------------------------
;Book
;----------------------------------------------------------------------------------------------------------

String Function GetBookText(Book akBook) Global Native


;----------------------------------------------------------------------------------------------------------
;UI
;----------------------------------------------------------------------------------------------------------

; Returns the "mode" that ContainerMenu is opened in:
;  0: looting a container or dead npc
;  1: stealing from a container not belonging to PC
;  2: pickpocketing
;  3: trading items (not bartering) with an NPC, like followers
;  -1: error
Int Function GetContainerMenuMode() Global Native

; returns the current Book object opened in Book Menu
Book Function GetCurrentBook() Global Native

; returns the current Book reference opened in Book Menu, if the book was *not* opened from Inventory
ObjectReference Function GetCurrentBookRef() Global Native

Bool Function IsEnchantingMenuOpen() Global Native
Bool Function IsAlchemyMenuOpen() Global Native
Bool Function IsTemperingMenuOpen() Global Native


;----------------------------------------------------------------------------------------------------------
;INI
;----------------------------------------------------------------------------------------------------------

String Function GetINIString(String asFile, String asSection, String asKey, String asDefault = "", Bool abNormalizeCase = False) Global Native

Float Function GetINIFloat(String asFile, String asSection, String asKey, Float afDefault = 0.0, Bool abNormalizeCase = False) Global Native

Int Function GetINIInt(String asFile, String asSection, String asKey, Int aiDefault = 0, Bool abNormalizeCase = False) Global Native

Bool Function GetINIBool(String asFile, String asSection, String asKey, Bool abDefault = False, Bool abNormalizeCase = False) Global Native


;----------------------------------------------------------------------------------------------------------
;Array
;----------------------------------------------------------------------------------------------------------

; Functions for traversing arrays and find how many items match value
Int Function CountValuesInBoolArray(Bool[] asArray, Bool abValue = False) Global Native
Int Function CountValuesInIntArray(Int[] asArray, Int aiValue = 0, String asCondition = "=") Global Native
Int Function CountValuesInFloatArray(Float[] asArray, Float afValue = 0.0, String asCondition = "=") Global Native
Int Function CountActorsWithVoiceTypeInArray(Actor[] asArray, VoiceType akVoice) Global Native
Int Function CountActorsInFactionInArray(Actor[] asArray, Faction akFaction) Global Native
Int Function CountActorsOfRaceInArray(Actor[] asArray, Form akFilter) Global Native

; removes array items not matching Value parameter, returns the result
Int[] Function FilterIntArray(Int[] asArray, Int aiValue = 0, String asCondition = "=") Global Native
Float[] Function FilterFloatArray(Float[] asArray, Float afValue = 0.0, String asCondition = "=") Global Native
Actor[] Function FilterActorsArrayForVoiceType(Actor[] asArray, VoiceType akVoice) Global Native
Actor[] Function FilterActorsArrayForFaction(Actor[] asArray, Faction akFaction) Global Native


;----------------------------------------------------------------------------------------------------------
;Deprecated
;----------------------------------------------------------------------------------------------------------

Int Function GetContainerMode() Global Native