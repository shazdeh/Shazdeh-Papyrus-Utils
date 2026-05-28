#include <windows.h>
#include <string>
#include <unordered_set>
#include "SimpleIni.h";

using namespace std::literals;

static inline std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return s;
}

inline std::size_t find_ci(const std::string& haystack, const std::string& needle, std::size_t pos = 0) {
    if (needle.empty()) return pos;  // empty needle, always a match

    for (std::size_t i = pos; i + needle.size() <= haystack.size(); ++i) {
        bool ok = true;
        for (std::size_t j = 0; j < needle.size(); ++j) {
            char h = static_cast<char>(std::tolower(static_cast<unsigned char>(haystack[i + j])));
            char n = static_cast<char>(std::tolower(static_cast<unsigned char>(needle[j])));

            if (h != n) {
                ok = false;
                break;
            }
        }

        if (ok) return i;
    }

    return std::string::npos;
}

template <typename T, typename C, typename... Args>
bool MatchAnyInlist(BGSListForm* a_list, C&& a_callback, Args&&... extraArgs) {
    if (!a_list) return false;
    bool result = false;
    a_list->ForEachForm([&](TESForm* listItem) {
        auto* item = listItem->As<T>();
        if (item && a_callback(item, std::forward<Args>(extraArgs)...)) {
            result = true;
            return BSContainer::ForEachResult::kStop;
        }
        return BSContainer::ForEachResult::kContinue;
    });

    return result;
}

template <typename T, typename C, typename... Args>
bool MatchAllInlist(BGSListForm* a_list, C&& a_callback, Args&&... extraArgs) {
    if (!a_list) return false;
    bool result = true;
    a_list->ForEachForm([&](TESForm* listItem) {
        auto* item = listItem->As<T>();
        if (item && !a_callback(item, std::forward<Args>(extraArgs)...)) {
            result = false;
            return BSContainer::ForEachResult::kStop;
        }
        return BSContainer::ForEachResult::kContinue;
    });

    return result;
}

template <typename T, typename C, typename... Args>
void MapList(BGSListForm* a_list, C&& a_callback, Args&&... extraArgs) {
    if (!a_list) return;
    a_list->ForEachForm([&](TESForm* listItem) {
        auto* item = listItem->As<T>();
        if (item) a_callback(item, std::forward<Args>(extraArgs)...);
        return BSContainer::ForEachResult::kContinue;
    });
}

template <typename T, typename C, typename... Args>
int CountItemsInlist(BGSListForm* a_list, C&& a_callback, Args&&... extraArgs) {
    if (!a_list) return false;
    int result = 0;
    a_list->ForEachForm([&](TESForm* listItem) {
        auto* item = listItem->As<T>();
        if (item && a_callback(item, std::forward<Args>(extraArgs)...)) {
            result++;
        }
        return BSContainer::ForEachResult::kContinue;
    });

    return result;
}

template <typename T, typename C, typename... Args>
bool MatchAnyInArray(std::vector<T*> a_list, C&& a_callback, Args&&... extraArgs) {
    if (a_list.empty()) return false;
    for (auto* item : a_list) {
        if (item && a_callback(item, std::forward<Args>(extraArgs)...)) return true;
    }
    return false;
}

template <typename T, typename C, typename... Args>
bool MatchAllInArray(std::vector<T*> a_list, C&& a_callback, Args&&... extraArgs) {
    if (a_list.empty()) return false;
    for (auto* item : a_list) {
        if (item && !a_callback(item, std::forward<Args>(extraArgs)...)) return false;
    }
    return true;
}

template <typename T, typename C, typename... Args>
void MapArray(std::vector<T*> a_list, C&& a_callback, Args&&... extraArgs) {
    if (a_list.empty()) return;
    for (auto* item : a_list) {
        if (item) a_callback(item, std::forward<Args>(extraArgs)...);
    }
}

template <typename T, typename C, typename... Args>
int CountItemsInArray(std::vector<T*> a_list, C&& a_callback, Args&&... extraArgs) {
    if (a_list.empty()) return 0;
    int result = 0;
    for (auto* item : a_list) {
        if (item && a_callback(item, std::forward<Args>(extraArgs)...)) result++;
    }
    return result;
}

bool ActorHasPerk(BGSPerk* a_perk, Actor* a_actor) { return a_actor->HasPerk(a_perk); }
bool ActorHasSpell(SpellItem* a_spell, Actor* a_actor) { return a_actor->HasSpell(a_spell); }
bool ActorInFaction(TESFaction* a_faction, Actor* a_actor) { return a_actor->IsInFaction(a_faction); }

bool ActorHasAnyPerkInList(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list) {
    if (!a_actor) return false;
    return MatchAnyInlist<BGSPerk>(a_list, ActorHasPerk, a_actor);
}

bool ActorHasAllPerksInList(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list) {
    if (!a_actor) return false;
    return MatchAllInlist<BGSPerk>(a_list, ActorHasPerk, a_actor);
}

bool ActorHasAnyPerkInArray(StaticFunctionTag*, Actor* a_actor, std::vector<BGSPerk*> a_list) {
    if (!a_actor) return false;
    return MatchAnyInArray<BGSPerk>(a_list, ActorHasPerk, a_actor);
}

bool ActorHasAllPerksInArray(StaticFunctionTag*, Actor* a_actor, std::vector<BGSPerk*> a_list) {
    if (!a_actor) return false;
    return MatchAllInArray<BGSPerk>(a_list, ActorHasPerk, a_actor);
}

int CountActorPerksInList(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list) {
    if (!a_actor) return 0;
    return CountItemsInlist<BGSPerk>(a_list, ActorHasPerk, a_actor);
}

int CountActorPerksInArray(StaticFunctionTag*, Actor* a_actor, std::vector<BGSPerk*> a_list) {
    if (!a_actor) return 0;
    return CountItemsInArray<BGSPerk>(a_list, ActorHasPerk, a_actor);
}

void AddAllPerksInListToActor(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list) {
    if (!a_actor) return;
    return MapList<BGSPerk>(a_list, [a_actor](BGSPerk* perk) { a_actor->AddPerk(perk); });
}

void AddAllPerksInArrayToActor(StaticFunctionTag*, Actor* a_actor, std::vector<BGSPerk*> a_list) {
    if (!a_actor) return;
    return MapArray<BGSPerk>(a_list, [a_actor](BGSPerk* perk) { a_actor->AddPerk(perk); });
}

void RemoveAllPerksInListFromActor(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list) {
    if (!a_actor) return;
    return MapList<BGSPerk>(a_list, [a_actor](BGSPerk* perk) { a_actor->RemovePerk(perk); });
}

void RemoveAllPerksInArrayFromActor(StaticFunctionTag*, Actor* a_actor, std::vector<BGSPerk*> a_list) {
    if (!a_actor) return;
    return MapArray<BGSPerk>(a_list, [a_actor](BGSPerk* perk) { a_actor->RemovePerk(perk); });
}

bool ActorHasAnySpellInList(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list) {
    if (!a_actor) return false;
    return MatchAnyInlist<SpellItem>(a_list, ActorHasSpell, a_actor);
}

bool ActorHasAnySpellInArray(StaticFunctionTag*, Actor* a_actor, std::vector<SpellItem*> a_list) {
    if (!a_actor) return false;
    return MatchAnyInArray<SpellItem>(a_list, ActorHasSpell, a_actor);
}

bool ActorHasAllSpellsInList(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list) {
    if (!a_actor) return false;
    return MatchAllInlist<SpellItem>(a_list, ActorHasSpell, a_actor);
}

bool ActorHasAllSpellsInArray(StaticFunctionTag*, Actor* a_actor, std::vector<SpellItem*> a_list) {
    if (!a_actor) return false;
    return MatchAllInArray<SpellItem>(a_list, ActorHasSpell, a_actor);
}

int CountActorSpellsInList(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list) {
    if (!a_actor) return 0;
    return CountItemsInlist<SpellItem>(a_list, ActorHasSpell, a_actor);
}

int CountActorSpellsInArray(StaticFunctionTag*, Actor* a_actor, std::vector<SpellItem*> a_list) {
    if (!a_actor) return 0;
    return CountItemsInArray<SpellItem>(a_list, ActorHasSpell, a_actor);
}

class MySpellCounter : public Actor::ForEachSpellVisitor {
public:
    int count = 0;
    BGSKeyword* keyword;
    virtual BSContainer::ForEachResult Visit(SpellItem* a_spell) {
        if (a_spell->HasKeyword(keyword)) {
            count++;
        }
        return BSContainer::ForEachResult::kContinue;
    }
};

int CountActorSpellsWithKeyword(StaticFunctionTag*, Actor* a_actor, BGSKeyword* a_keyword) {
    if (!a_actor) return 0;
    MySpellCounter counter;
    counter.keyword = a_keyword;
    a_actor->VisitSpells(counter);
    return counter.count;
}

void AddAllSpellsInListToActor(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list) {
    if (!a_actor) return;
    return MapList<SpellItem>(a_list, [a_actor](SpellItem* item) { return a_actor->AddSpell(item); });
}

void AddAllSpellsInArrayToActor(StaticFunctionTag*, Actor* a_actor, std::vector<SpellItem*> a_list) {
    if (!a_actor) return;
    return MapArray<SpellItem>(a_list, [a_actor](SpellItem* item) { return a_actor->AddSpell(item); });
}

void RemoveAllSpellsInListFromActor(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list) {
    if (!a_actor) return;
    return MapList<SpellItem>(a_list, [a_actor](SpellItem* item) { return a_actor->RemoveSpell(item); });
}

void RemoveAllSpellsInArrayFromActor(StaticFunctionTag*, Actor* a_actor, std::vector<SpellItem*> a_list) {
    if (!a_actor) return;
    return MapArray<SpellItem>(a_list, [a_actor](SpellItem* item) { return a_actor->RemoveSpell(item); });
}

bool IsActorInAnyFactionInList(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list) {
    if (!a_actor) return false;
    return MatchAnyInlist<TESFaction>(a_list, ActorInFaction, a_actor);
}

bool IsActorInAnyFactionInArray(StaticFunctionTag*, Actor* a_actor, std::vector<TESFaction*> a_list) {
    if (!a_actor) return false;
    return MatchAnyInArray<TESFaction>(a_list, ActorInFaction, a_actor);
}

bool IsActorInAllFactionsInList(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list) {
    if (!a_actor) return false;
    return MatchAllInlist<TESFaction>(a_list, ActorInFaction, a_actor);
}

bool IsActorInAllFactionsInArray(StaticFunctionTag*, Actor* a_actor, std::vector<TESFaction*> a_list) {
    if (!a_actor) return false;
    return MatchAllInArray<TESFaction>(a_list, ActorInFaction, a_actor);
}

void AddActorToAllFactionsInList(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list, int a_Rank = 0) {
    if (!a_actor) return;
    return MapList<TESFaction>(a_list, [a_actor, a_Rank](TESFaction* item) { return a_actor->AddToFaction(item, a_Rank); });
}

void AddActorToAllFactionsInArray(StaticFunctionTag*, Actor* a_actor, std::vector<TESFaction*> a_list, int a_Rank = 0) {
    if (!a_actor) return;
    return MapArray<TESFaction>(a_list, [a_actor, a_Rank](TESFaction* item) { return a_actor->AddToFaction(item, a_Rank); });
}

void RemoveActorFromAllFactionsInList(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list) {
    if (!a_actor) return;
    return MapList<TESFaction>(a_list, [a_actor](TESFaction* item) { return a_actor->RemoveFromFaction(item); });
}

void RemoveActorFromAllFactionsInArray(StaticFunctionTag*, Actor* a_actor, std::vector<TESFaction*> a_list) {
    if (!a_actor) return;
    return MapArray<TESFaction>(a_list, [a_actor](TESFaction* item) { return a_actor->RemoveFromFaction(item); });
}

BSFixedString GetClipboard(StaticFunctionTag*) {
    if (!OpenClipboard(nullptr)) return {};

    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr) {
        CloseClipboard();
        return {};
    }

    char* pszText = static_cast<char*>(GlobalLock(hData));
    if (pszText == nullptr) {
        CloseClipboard();
        return {};
    }

    std::string text(pszText);

    GlobalUnlock(hData);
    CloseClipboard();

    return text.c_str();
}

bool SetClipboard(StaticFunctionTag*, BSFixedString a_text) {
    if (!OpenClipboard(nullptr)) return false;

    if (!EmptyClipboard()) {
        CloseClipboard();
        return false;
    }

    const std::string& text = a_text.c_str();
    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (!hGlob) {
        CloseClipboard();
        return false;
    }

    memcpy(GlobalLock(hGlob), text.c_str(), text.size() + 1);
    GlobalUnlock(hGlob);

    if (!SetClipboardData(CF_TEXT, hGlob)) {
        GlobalFree(hGlob);
        CloseClipboard();
        return false;
    }

    CloseClipboard();
    return true;
}

TESForm* LookupFormSmart(StaticFunctionTag*, const BSFixedString inputRaw) {
    if (!inputRaw.data() || inputRaw.empty()) return nullptr;

    std::string_view input = inputRaw.data();

    // trim
    while (!input.empty() && std::isspace(static_cast<unsigned char>(input.front()))) input.remove_prefix(1);
    while (!input.empty() && std::isspace(static_cast<unsigned char>(input.back()))) input.remove_suffix(1);

    if (input.empty()) return nullptr;

    bool hasPrefix = false;

    if (input.size() > 2 && input[0] == '0' && (input[1] == 'x' || input[1] == 'X')) {
        hasPrefix = true;
        input.remove_prefix(2);
    }

    bool hexCandidate = true;
    for (char c : input) {
        if (!std::isxdigit(static_cast<unsigned char>(c))) {
            hexCandidate = false;
            break;
        }
    }

    if (hexCandidate) {
        uint32_t formID = 0;
        auto begin = input.data();
        auto end = input.data() + input.size();

        std::from_chars(begin, end, formID, 16);

        return TESForm::LookupByID(formID);
    }

    return TESForm::LookupByEditorID(inputRaw);
}

std::vector<TESForm*> GetFormsByEditorID(StaticFunctionTag*, const reference_array<std::string> a_ids) {
    std::vector<TESForm*> result;
    for (const auto& id : a_ids) {
        if (auto* form = TESForm::LookupByEditorID(id); form) {
            result.push_back(form);
        }
    }
    return result;
}

BSFixedString GetFormTypeString(StaticFunctionTag*, TESForm* a_form) {
    if (a_form) {
        return FormTypeToString(a_form->GetFormType());
    }
    return "NONE";
}

bool HasAnyKeywordInList(StaticFunctionTag*, TESForm* a_form, BGSListForm* a_list) {
    if (!a_form || !a_list) {
        return false;
    }

    return a_form->HasKeywordInList(a_list, false);
}

bool HasAllKeywordsInList(StaticFunctionTag*, TESForm* a_form, BGSListForm* a_list) {
    if (!a_form || !a_list) {
        return false;
    }

    return a_form->HasKeywordInList(a_list, true);
}

bool IsGamepadConnected(StaticFunctionTag*) {
    auto inputMgr = BSInputDeviceManager::GetSingleton();
    if (inputMgr && inputMgr->IsGamepadConnected()) {
        return true;
    }
    return false;
}

void SimulateLeftStickInput(StaticFunctionTag*, float xValue = 0.0f, float yValue = 0.0f) {
    if (auto queue = BSInputEventQueue::GetSingleton()) {
        queue->AddThumbstickEvent(ThumbstickEvent::InputTypes::kLeftThumbstick, xValue, yValue);
    }
}

TESObjectREFR* GetLinkedDoor(StaticFunctionTag*, TESObjectREFR* door) {
    if (!door) {
        return nullptr;
    }

    return door->extraList.GetTeleportLinkedDoor().get().get();
}

BSTArray<RE::Effect*> _GetEffects(TESForm* a_form) {
    switch (a_form->GetFormType()) {
        case FormType::Spell:
            return a_form->As<SpellItem>()->effects;

        case FormType::AlchemyItem:
            return a_form->As<AlchemyItem>()->effects;

        case FormType::Ingredient:
            return a_form->As<IngredientItem>()->effects;

        case FormType::Scroll:
            return a_form->As<ScrollItem>()->effects;

        case FormType::Enchantment:
            return a_form->As<EnchantmentItem>()->effects;
    }
    return {};
}

bool FormHasMagicEffect(StaticFunctionTag*, TESForm* a_form, EffectSetting* magicEffect) {
    BSTArray<RE::Effect*> effects = _GetEffects(a_form);

    if (effects.size()) {
        for (Effect* effect : effects) {
            if (effect->baseEffect == magicEffect) {
                return true;
            }
        }
    }

    return false;
}

bool FormHasMagicEffectKeyword(StaticFunctionTag*, TESForm* a_form, BGSKeyword* keyword) {
    BSTArray<RE::Effect*> effects = _GetEffects(a_form);
    if (effects.size()) {
        for (Effect* effect : effects) {
            if (effect->baseEffect->HasKeyword(keyword)) {
                return true;
            }
        }
    }

    return false;
}

TESForm* GetFormWithStrongestEffect(StaticFunctionTag*, TESObjectREFR* ref, EffectSetting* magicEffect, int formtype,
                                    BGSKeyword* keywordFilter = nullptr) {
    auto inventory = ref->GetInventory();
    float highestMag = -1.0f;
    TESForm* winner = nullptr;
    for (const auto& [item, data] : inventory) {
        if (static_cast<int>(item->GetFormType()) == formtype) {
            if (keywordFilter != nullptr && !item->HasKeywordInArray(std::vector{keywordFilter}, false)) {
                continue;
            }
            for (auto* effect : _GetEffects(item)) {
                if (effect->baseEffect == magicEffect) {
                    if (effect->GetMagnitude() > highestMag) {
                        highestMag = effect->GetMagnitude();
                        winner = item;
                    }
                    break;
                }
            }
        }
    }
    return winner;
}

TESForm* GetFormWithWeakestEffect(StaticFunctionTag*, TESObjectREFR* ref, EffectSetting* magicEffect, int formtype,
                                  BGSKeyword* keywordFilter = nullptr) {
    auto inventory = ref->GetInventory();
    float lowestMag = 0.0f;
    TESForm* winner = nullptr;
    for (const auto& [item, data] : inventory) {
        if (static_cast<int>(item->GetFormType()) == formtype) {
            if (keywordFilter != nullptr && !item->HasKeywordInArray(std::vector{keywordFilter}, false)) {
                continue;
            }
            for (auto* effect : _GetEffects(item)) {
                if (effect->baseEffect == magicEffect) {
                    if (effect->GetMagnitude() < lowestMag || !winner) {
                        lowestMag = effect->GetMagnitude();
                        winner = item;
                    }
                    break;
                }
            }
        }
    }
    return winner;
}

bool FormlistHasFormDeep(StaticFunctionTag*, BGSListForm* a_list, TESForm* a_form) {
    if (!a_list || !a_form) {
        return false;
    } else if (a_list->HasForm(a_form)) {
        return true;
    } else {
        bool bDeepResult = false;
        a_list->ForEachForm([&a_form, &bDeepResult](TESForm* listItem) {
            if (listItem->Is(FormType::FormList) && FormlistHasFormDeep({}, listItem->As<BGSListForm>(), a_form)) {
                bDeepResult = true;
                return BSContainer::ForEachResult::kStop;
            }
            return BSContainer::ForEachResult::kContinue;
        });
        return bDeepResult;
    }
}

void TransferItems(StaticFunctionTag*, TESObjectREFR* a_source, TESObjectREFR* a_target, int a_count = -1,
                   int a_formtype = 0, TESForm* formFilter = nullptr, bool a_excludeWorn = true,
                   bool a_excludeFavorites = true, bool a_excludeQuestItems = true) {
    if (!a_source || !a_target || a_count == 0) return;
    std::vector<BGSKeyword*> keywordFilter;
    BGSListForm* formListFilter = nullptr;
    if (formFilter) {
        if (formFilter->Is(FormType::Keyword)) {
            keywordFilter = {formFilter->As<BGSKeyword>()};
        } else if (formFilter->Is(FormType::FormList)) {
            formListFilter = formFilter->As<BGSListForm>();
        }
    }
    for (const auto& [item, data] : a_source->GetInventory()) {
        if ((a_formtype != 0 && static_cast<int>(item->GetFormType()) != a_formtype) ||
            item->Is(FormType::LeveledItem) || !item->GetPlayable() || (a_excludeWorn && data.second->IsWorn()) ||
            (a_excludeFavorites && data.second->IsFavorited()) ||
            (a_excludeQuestItems && data.second->IsQuestObject()) ||
            (!keywordFilter.empty() && !item->HasKeywordInArray(keywordFilter, false)) ||
            (formListFilter && !formListFilter->HasForm(item)) || (formFilter && item != formFilter)) {
            continue;
        }

        a_source->RemoveItem(item, a_count == -1 ? data.first : a_count, ITEM_REMOVE_REASON::kStoreInContainer, nullptr,
                             a_target);
    }
}

int _getItemCountInRef(TESForm* a_ref, TESForm* a_item) {
    if (a_ref) {
        if (a_ref->Is(FormType::Reference) || a_ref->Is(FormType::ActorCharacter)) {
            auto inventory = a_ref->As<TESObjectREFR>()->GetInventory();
            auto result = inventory.find(a_item->As<TESBoundObject>());
            if (result != inventory.end()) {
                return result->second.first;
            }
        }
    }
    return 0;
}

int GetItemCountInContainersArray(StaticFunctionTag*, std::vector<TESObjectREFR*> a_sources, TESForm* a_form) {
    int count = 0;
    for (auto* ref : a_sources) {
        count += _getItemCountInRef(ref, a_form);
    }
    return 0;
}

int GetItemCountInContainersList(StaticFunctionTag*, BGSListForm* a_list, TESForm* a_form) {
    if (!a_list || !a_form) return 0;

    int count = 0;
    a_list->ForEachForm([a_form, &count](TESForm* listItem) {
        count += _getItemCountInRef(listItem, a_form);
        return BSContainer::ForEachResult::kContinue;
    });
    return count;
}

void TransferItemFromContainersList(StaticFunctionTag*, BGSListForm* a_list, TESObjectREFR* a_target, TESForm* a_form,
                                    int a_count) {
    if (!a_target || !a_form || !a_list) return;

    int transferred = 0;
    a_list->ForEachForm([a_form, a_target, &a_count](TESForm* listItem) {
        TESObjectREFR* currentRef = listItem->As<TESObjectREFR>();
        if (currentRef) {
            int available = _getItemCountInRef(currentRef, a_form);
            if (available > 0) {
                int toTransfer = std::min(available, a_count);
                TransferItems(nullptr, currentRef, a_target, toTransfer, 0, a_form);
                a_count -= toTransfer;
                if (a_count <= 0) {
                    return BSContainer::ForEachResult::kStop;
                }
            }
        }
        return BSContainer::ForEachResult::kContinue;
    });
}

void TransferItemFromContainersArray(StaticFunctionTag*, std::vector<TESObjectREFR*> a_sources, TESObjectREFR* a_target,
                                     TESForm* a_form, int a_count) {
    if (!a_target || !a_form) return;

    int transferred = 0;
    for (auto* ref : a_sources) {
        if (ref) {
            int available = _getItemCountInRef(ref, a_form);
            if (available > 0) {
                int toTransfer = std::min(available, a_count);
                TransferItems(nullptr, ref, a_target, toTransfer, 0, a_form);
                a_count -= toTransfer;
                if (a_count <= 0) {
                    break;
                }
            }
        }
    }
}

std::string GetTranslatedString(StaticFunctionTag*, std::string a_key) {
    std::string result = "";
    SKSE::Translation::Translate(a_key, result);
    return result;
}

std::string StringReplace(StaticFunctionTag*, std::vector<std::string> a_needles, std::vector<std::string> a_replaces,
                          std::string a_haystack, bool a_ignoreCase = false) {
    if (a_haystack.empty() || a_needles.size() != a_replaces.size()) return a_haystack;

    for (std::size_t idx = 0; idx < a_needles.size(); ++idx) {
        const std::string& needle = a_needles[idx];
        const std::string& replace = a_replaces[idx];

        if (needle.empty())  // nothing to search for
            continue;

        std::size_t pos = 0;
        while (true) {
            if (a_ignoreCase)
                pos = find_ci(a_haystack, needle, pos);
            else
                pos = a_haystack.find(needle, pos);

            if (pos == std::string::npos) break;

            a_haystack.replace(pos, needle.size(), replace);
            pos += replace.size();  // continue after the inserted text
        }
    }

    return a_haystack;
}

bool HasStolenWeaponEquipped(StaticFunctionTag*, Actor* a_actor, bool a_leftHand = false) {
    if (!a_actor) return false;
    auto entryData = a_actor->GetEquippedEntryData(a_leftHand);
    if (!entryData) return false;
    if (auto* owner = entryData->GetOwner()) {
        if (owner->GetFormType() == FormType::NPC) {
            return owner->As<TESActorBase>() != a_actor->GetActorBase();
        } else if (owner->GetFormType() == FormType::Faction) {
            return !a_actor->IsInFaction(owner->As<TESFaction>());
        }
    }
    return false;
}

int GetContainerMenuMode(StaticFunctionTag*) {
    if (auto* ui = UI::GetSingleton(); ui) {
        if (auto containerMenu = ui->GetMenu<ContainerMenu>(); containerMenu) {
            return static_cast<int>(containerMenu->GetContainerMode());
        }
    }
    return -1;
}

BSString GetBookText(StaticFunctionTag*, TESObjectBOOK* a_book) {
    if (!a_book) return "";
    BSString text;
    a_book->GetDescription(text, a_book);
    return text;
}

TESObjectBOOK* GetCurrentBook(StaticFunctionTag*) {
    if (auto menu = UI::GetSingleton()->GetMenu<BookMenu>(); menu) {
        return menu->GetTargetForm();
    }
    return nullptr;
}

TESObjectREFR* GetCurrentBookRef(StaticFunctionTag*) {
    if (auto menu = UI::GetSingleton()->GetMenu<BookMenu>(); menu) {
        return menu->GetTargetReference().get();
    }
    return nullptr;
}

template <class Ret>
Ret GetINIValue(StaticFunctionTag*, const BSString& path, const BSString& section, const BSString& key,
                const Ret& def = Ret(), bool normalizeCase = false) {
    CSimpleIniA ini;
    ini.SetUnicode(true);

    if (ini.LoadFile(path.c_str()) != SI_OK) return def;

    std::string sec = normalizeCase ? ToLower(section.c_str()) : std::string(section.c_str());
    std::string k = normalizeCase ? ToLower(key.c_str()) : std::string(key.c_str());

    const char* raw = ini.GetValue(sec.c_str(), k.c_str(), nullptr);
    if (!raw) return def;

    if constexpr (std::is_same_v<Ret, BSString>) {  // string
        return BSString(raw);
    } else if constexpr (std::is_same_v<Ret, int>) {  // integer
        try {
            return std::stoi(raw);
        } catch (...) {
            return def;
        }
    } else if constexpr (std::is_same_v<Ret, float>) {  // floating point
        try {
            return std::stof(raw);
        } catch (...) {
            return def;
        }
    } else if constexpr (std::is_same_v<Ret, bool>) {  // boolean
        return ini.GetBoolValue(sec.c_str(), k.c_str(), def);
    } else {
        static_assert(false, "Unsupported INI return type");
    }
}

BSString GetINIString(StaticFunctionTag* tag, BSString path, BSString section, BSString key, BSString a_default = "",
                      bool normalizeCase = false) {
    return GetINIValue<BSString>(tag, path, section, key, a_default, normalizeCase);
}

int GetINIInt(StaticFunctionTag* tag, BSString path, BSString section, BSString key, int a_default = 0,
              bool normalizeCase = false) {
    return GetINIValue<int>(tag, path, section, key, a_default, normalizeCase);
}

float GetINIFloat(StaticFunctionTag* tag, BSString path, BSString section, BSString key, float a_default = 0.0f,
                  bool normalizeCase = false) {
    return GetINIValue<float>(tag, path, section, key, a_default, normalizeCase);
}

bool GetINIBool(StaticFunctionTag* tag, BSString path, BSString section, BSString key, bool a_default = false,
                bool normalizeCase = false) {
    return GetINIValue<bool>(tag, path, section, key, a_default, normalizeCase);
}

static std::string GetGameLanguage(StaticFunctionTag*) {
    static std::string language;
    if (!language.empty()) return language;

    language = "english";
    auto ini = INISettingCollection::GetSingleton();
    if (ini) {
        auto setting = ini->GetSetting("sLanguage:General");
        if (setting && setting->GetType() == RE::Setting::Type::kString) {
            language = setting->GetString();
        }
    }
    return language;
}

std::vector<Actor*> FindActorsInFactionNearRef(StaticFunctionTag*, TESObjectREFR* a_ref, float a_radius,
                                               TESFaction* a_faction, bool a_ignoreDead = true,
                                               bool a_ignorePlayer = true) {
    std::vector<Actor*> actors;
    if (!a_ref || !a_faction) return actors;
    if (const auto& tes = TES::GetSingleton(); tes) {
        tes->ForEachReferenceInRange(
            a_ref, a_radius, [&actors, a_faction, a_ignoreDead, a_ignorePlayer](TESObjectREFR* ref) {
                Actor* theActor = ref->As<Actor>();
                if (theActor && theActor->IsInFaction(a_faction) && (!a_ignoreDead || !theActor->IsDead()) &&
                    (!a_ignorePlayer || !theActor->IsPlayerRef())) {
                    actors.push_back(theActor);
                }
                return BSContainer::ForEachResult::kContinue;
            });
    }
    return actors;
}

std::vector<Actor*> FindActorsWithVoiceTypeNearRef(StaticFunctionTag*, TESObjectREFR* a_ref, float a_radius,
                                                   BGSVoiceType* a_voicetype, bool a_ignoreDead = true) {
    std::vector<Actor*> actors;
    if (!a_ref || !a_voicetype) return actors;
    if (const auto& tes = TES::GetSingleton(); tes) {
        tes->ForEachReferenceInRange(a_ref, a_radius, [&actors, a_voicetype, a_ignoreDead](TESObjectREFR* ref) {
            if (Actor* theActor = ref->As<Actor>(); theActor) {
                if (auto* base = theActor->GetActorBase(); base) {
                    if (!theActor->IsPlayerRef() && (!a_ignoreDead || !theActor->IsDead())) {
                        BGSVoiceType* voicetype = base->GetVoiceType();
                        if (voicetype && voicetype == a_voicetype) {
                            actors.push_back(theActor);
                        }
                    }
                }
            }
            return BSContainer::ForEachResult::kContinue;
        });
    }
    return actors;
}

std::vector<Actor*> FindActorsOfRaceNearRef(StaticFunctionTag*, TESObjectREFR* a_ref, float a_radius,
                                                   TESForm* a_form, bool a_ignoreDead = true) {
    std::vector<Actor*> actors;
    if (!a_ref || !a_form) return actors;
    TESRace* race = nullptr;
    BGSListForm* raceList = nullptr;
    if (a_form->Is(FormType::Race)) {
        race = a_form->As<TESRace>();
    } else if (a_form->Is(FormType::FormList)) {
        raceList = a_form->As<BGSListForm>();
    } else {
        return actors;
    }

    if (const auto& tes = TES::GetSingleton(); tes) {
        tes->ForEachReferenceInRange(a_ref, a_radius, [&actors, race, raceList, a_ignoreDead](TESObjectREFR* ref) {
            if (Actor* theActor = ref->As<Actor>(); theActor) {
                if (!theActor->IsPlayerRef() && (!a_ignoreDead || !theActor->IsDead())) {
                    auto* raceType = theActor->GetRace();
                    if ((race && raceType == race) || (raceList && raceList->HasForm(raceType))) {
                        actors.push_back(theActor);
                    }
                }
            }
            return BSContainer::ForEachResult::kContinue;
        });
    }
    return actors;
}

float GetAVByIndex(StaticFunctionTag*, Actor* a_actor, int a_id) {
    if (!a_actor) return 0.0f;
    return a_actor->AsActorValueOwner()->GetActorValue(static_cast<ActorValue>(a_id));
}

float GetBaseAVByIndex(StaticFunctionTag*, Actor* a_actor, int a_id) {
    if (!a_actor) return 0.0f;
    return a_actor->AsActorValueOwner()->GetBaseActorValue(static_cast<ActorValue>(a_id));
}

std::string GetAVName(StaticFunctionTag*, int a_id) {
    RE::ActorValue av = static_cast<RE::ActorValue>(a_id);
    const char* name = RE::ActorValueList::GetSingleton()->GetActorValueName(av);
    return name ? name : "None";
}

int CountValuesInBoolArray(StaticFunctionTag*, std::vector<bool> a_arr, bool a_value = false) {
    int result = 0;
    for (auto value : a_arr) {
        if (value == value) result++;
    }
    return result;
}

int CountValuesInIntArray(StaticFunctionTag*, std::vector<int> a_arr, int a_value = 0, std::string a_op = "=") {
    int result = 0;
    for (auto value : a_arr) {
        if (((a_op == "=" || a_op == "==") && value == value) || (a_op == "<" && value < a_value) ||
            (a_op == "<=" && value <= a_value) || (a_op == ">" && value > a_value) ||
            (a_op == ">=" && value >= a_value) || (a_op == "!=" && value != a_value)
        ) {
            result++;
        }
    }
    return result;
}

int CountValuesInFloatArray(StaticFunctionTag*, std::vector<float> a_arr, float a_value = 0.0, std::string a_op = "=") {
    int result = 0;
    for (auto value : a_arr) {
        if (((a_op == "=" || a_op == "==") && value == value) || (a_op == "<" && value < a_value) ||
            (a_op == "<=" && value <= a_value) || (a_op == ">" && value > a_value) ||
            (a_op == ">=" && value >= a_value) || (a_op == "!=" && value != a_value)
        ) {
            result++;
        }
    }
    return result;
}

int CountActorsWithVoiceTypeInArray(StaticFunctionTag*, std::vector<Actor*> arr, BGSVoiceType* a_voiceType) {
    if (!a_voiceType || arr.empty()) return 0;
    int result = 0;
    for (auto* actor : arr) {
        if (!actor) continue;
        if (auto* base = actor->GetActorBase(); base) {
            if (base->voiceType == a_voiceType) result++;
        }
    }
    return result;
}

int CountActorsInFactionInArray(StaticFunctionTag*, std::vector<Actor*> arr, TESFaction* a_faction) {
    if (!a_faction || arr.empty()) return 0;
    int result = 0;
    for (auto* actor : arr) {
        if (!actor) continue;
        if (actor->IsInFaction(a_faction)) result++;
    }
    return result;
}

int CountActorsOfRaceInArray(StaticFunctionTag*, std::vector<Actor*> arr, TESForm* a_form) {
    if (!a_form || arr.empty()) return 0;
    TESRace* race = nullptr;
    BGSListForm* raceList = nullptr;
    if (a_form->Is(FormType::Race)) {
        race = a_form->As<TESRace>();
    } else if (a_form->Is(FormType::FormList)) {
        raceList = a_form->As<BGSListForm>();
    } else {
        return 0;
    }

    int result = 0;
    for (auto* actor : arr) {
        if (!actor) continue;
        TESRace* actorRace = actor->GetRace();
        if ((race && race == actorRace) || (raceList && raceList->HasForm(actorRace))) {
            result++;
        }
    }
    return result;
}

std::vector<int> FilterIntArray(StaticFunctionTag*, std::vector<int> a_arr, int a_value = 0, std::string a_op = "=") {
    std::vector<int> result;
    for (auto value : a_arr) {
        if (((a_op == "=" || a_op == "==") && value == value) || (a_op == "<" && value < a_value) ||
            (a_op == "<=" && value <= a_value) || (a_op == ">" && value > a_value) ||
            (a_op == ">=" && value >= a_value) || (a_op == "!=" && value != a_value)
        ) {
            result.push_back(value);
        }
    }
    return result;
}

std::vector<float> FilterFloatArray(StaticFunctionTag*, std::vector<float> a_arr, float a_value = 0.0,
                                    std::string a_op = "=") {
    std::vector<float> result;
    for (auto value : a_arr) {
        if (((a_op == "=" || a_op == "==") && value == value) || (a_op == "<" && value < a_value) ||
            (a_op == "<=" && value <= a_value) || (a_op == ">" && value > a_value) ||
            (a_op == ">=" && value >= a_value) || (a_op == "!=" && value != a_value)
        ) {
            result.push_back(value);
        }
    }
    return result;
}

std::vector<Actor*> FilterActorsArrayForVoiceType(StaticFunctionTag*, std::vector<Actor*> arr, BGSVoiceType* a_voiceType) {
    std::vector<Actor*> actors;
    if (!a_voiceType || arr.empty()) return actors;
    for (auto* actor : arr) {
        if (!actor) continue;
        if (auto* base = actor->GetActorBase(); base) {
            if (base->voiceType == a_voiceType) actors.push_back(actor);
        }
    }
    return actors;
}

std::vector<Actor*> FilterActorsArrayForFaction(StaticFunctionTag*, std::vector<Actor*> arr,
                                                TESFaction* a_faction) {
    std::vector<Actor*> actors;
    if (!a_faction || arr.empty()) return actors;
    for (auto* actor : arr) {
        if (actor && actor->IsInFaction(a_faction)) actors.push_back(actor);
    }
    return actors;
}

bool IsEnchantingMenuOpen(StaticFunctionTag*) {
    auto ui = UI::GetSingleton();
    if (ui && ui->IsMenuOpen(CraftingMenu::MENU_NAME)) {
        const auto menu = UI::GetSingleton()->GetMenu<CraftingMenu>().get();
        return skyrim_cast<CraftingSubMenus::EnchantConstructMenu*>(menu->GetCraftingSubMenu());
    }
    return false;
}

bool IsAlchemyMenuOpen(StaticFunctionTag*) {
    auto ui = UI::GetSingleton();
    if (ui && ui->IsMenuOpen(CraftingMenu::MENU_NAME)) {
        const auto menu = UI::GetSingleton()->GetMenu<CraftingMenu>().get();
        return skyrim_cast<CraftingSubMenus::CraftingSubMenus::AlchemyMenu*>(menu->GetCraftingSubMenu());
    }
    return false;
}

bool IsTemperingMenuOpen(StaticFunctionTag*) {
    auto ui = UI::GetSingleton();
    if (ui && ui->IsMenuOpen(CraftingMenu::MENU_NAME)) {
        const auto menu = UI::GetSingleton()->GetMenu<CraftingMenu>().get();
        return skyrim_cast<CraftingSubMenus::SmithingMenu*>(menu->GetCraftingSubMenu());
    }
    return false;
}

class CheckDisease : public MagicTarget::ForEachActiveEffectVisitor {
public:
    bool is = false;
    virtual BSContainer::ForEachResult Accept(ActiveEffect* a_effect) override {
        if (a_effect) {
            auto* spell = a_effect->spell;
            if (spell && spell->GetSpellType() == RE::MagicSystem::SpellType::kDisease) {
                is = true;
                return BSContainer::ForEachResult::kStop;
            }
        }
        return BSContainer::ForEachResult::kContinue;
    }
};

bool IsDiseased(StaticFunctionTag*, Actor* a_actor) {
    if (!a_actor) return false;
    CheckDisease checker;
    a_actor->ForEachActiveEffect(checker);
    return checker.is;
}

int GetDiseasesCount(StaticFunctionTag*, Actor* a_actor) {
    return 0;
}

int CountWornWithKeyword(StaticFunctionTag*, Actor* a_actor, BGSKeyword* a_keyword) {
    if (!a_actor || !a_keyword) return 0;
    std::unordered_set<TESObjectARMO*> visited;
    int result = 0;
    for (uint32_t i = 0; i < 32; ++i) {
        auto* armor = a_actor->GetWornArmor(static_cast<BGSBipedObjectForm::BipedObjectSlot>(1u << i), true);
        if (armor && armor->HasKeyword(a_keyword) && !visited.contains(armor)) {
            result++;
            visited.insert(armor);
        }
    }
    return result;
}

int CountCommandingActors(StaticFunctionTag*, Actor* a_actor) {
    if (!a_actor) return 0;
    int count = 0;
    if (a_actor->GetHighProcess()) {
        if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
            for (auto& targetHandle : processLists->highActorHandles) {
                if (const auto target = targetHandle.get(); target && target->GetHighProcess()) {
                    if (target.get()->IsCommandedActor()) {
                        auto commanderPtr = target.get()->GetCommandingActor();
                        if (commanderPtr && commanderPtr.get() == a_actor) {
                            count++;
                        }
                    }
                }
            }
        }
    }
    return count;
}

int CountHostilesToActor(StaticFunctionTag*, Actor* a_actor, TESForm* a_filter = nullptr) {
    if (!a_actor) return 0;
    int count = 0;
    TESRace* race = nullptr;
    BGSListForm* raceList = nullptr;
    if (a_filter->Is(FormType::Race)) {
        race = a_filter->As<TESRace>();
    } else if (a_filter->Is(FormType::FormList)) {
        raceList = a_filter->As<BGSListForm>();
    }

    if (a_actor->GetHighProcess()) {
        if (const auto processLists = RE::ProcessLists::GetSingleton(); processLists) {
            for (auto& targetHandle : processLists->highActorHandles) {
                if (const auto target = targetHandle.get(); target && target->GetHighProcess()) {
                    if (target.get()->IsHostileToActor(a_actor)) {
                        if (!a_filter || (race && target.get()->GetRace() == race) ||
                            (raceList && raceList->HasForm(target.get()->GetRace()))) {
                            count++;
                        }
                    }
                }
            }
        }
    }
    return count;
}

bool PapyrusBinder(BSScript::IVirtualMachine* vm) {
    std::string_view script = "ShazdehUtils";

    // game
    vm->RegisterFunction("GetGameLanguage", script, GetGameLanguage);

    // actor
    vm->RegisterFunction("ActorHasAnyPerkInList", script, ActorHasAnyPerkInList);
    vm->RegisterFunction("ActorHasAllPerksInList", script, ActorHasAllPerksInList);
    vm->RegisterFunction("ActorHasAnyPerkInArray", script, ActorHasAnyPerkInArray);
    vm->RegisterFunction("ActorHasAllPerksInArray", script, ActorHasAllPerksInArray);
    vm->RegisterFunction("CountActorPerksInList", script, CountActorPerksInList);
    vm->RegisterFunction("CountActorPerksInArray", script, CountActorPerksInArray);
    vm->RegisterFunction("AddAllPerksInListToActor", script, AddAllPerksInListToActor);
    vm->RegisterFunction("AddAllPerksInArrayToActor", script, AddAllPerksInArrayToActor);
    vm->RegisterFunction("RemoveAllPerksInListFromActor", script, RemoveAllPerksInListFromActor);
    vm->RegisterFunction("RemoveAllPerksInArrayFromActor", script, RemoveAllPerksInArrayFromActor);
    vm->RegisterFunction("ActorHasAnySpellInList", script, ActorHasAnySpellInList);
    vm->RegisterFunction("ActorHasAnySpellInArray", script, ActorHasAnySpellInArray);
    vm->RegisterFunction("ActorHasAllSpellsInList", script, ActorHasAllSpellsInList);
    vm->RegisterFunction("ActorHasAllSpellsInArray", script, ActorHasAllSpellsInArray);
    vm->RegisterFunction("CountActorSpellsInList", script, CountActorSpellsInList);
    vm->RegisterFunction("CountActorSpellsInArray", script, CountActorSpellsInArray);
    vm->RegisterFunction("CountActorSpellsWithKeyword", script, CountActorSpellsWithKeyword);
    vm->RegisterFunction("AddAllSpellsInListToActor", script, AddAllSpellsInListToActor);
    vm->RegisterFunction("AddAllSpellsInArrayToActor", script, AddAllSpellsInArrayToActor);
    vm->RegisterFunction("RemoveAllSpellsInListFromActor", script, RemoveAllSpellsInListFromActor);
    vm->RegisterFunction("RemoveAllSpellsInArrayFromActor", script, RemoveAllSpellsInArrayFromActor);
    vm->RegisterFunction("IsActorInAnyFactionInList", script, IsActorInAnyFactionInList);
    vm->RegisterFunction("IsActorInAnyFactionInArray", script, IsActorInAnyFactionInArray);
    vm->RegisterFunction("IsActorInAllFactionsInList", script, IsActorInAllFactionsInList);
    vm->RegisterFunction("IsActorInAllFactionsInArray", script, IsActorInAllFactionsInArray);
    vm->RegisterFunction("AddActorToAllFactionsInList", script, AddActorToAllFactionsInList);
    vm->RegisterFunction("AddActorToAllFactionsInArray", script, AddActorToAllFactionsInArray);
    vm->RegisterFunction("RemoveActorFromAllFactionsInList", script, RemoveActorFromAllFactionsInList);
    vm->RegisterFunction("RemoveActorFromAllFactionsInArray", script, RemoveActorFromAllFactionsInArray);
    vm->RegisterFunction("HasStolenWeaponEquipped", script, HasStolenWeaponEquipped);
    vm->RegisterFunction("GetAVByIndex", script, GetAVByIndex);
    vm->RegisterFunction("GetBaseAVByIndex", script, GetBaseAVByIndex);
    vm->RegisterFunction("GetAVName", script, GetAVName);
    vm->RegisterFunction("CountWornWithKeyword", script, CountWornWithKeyword);
    vm->RegisterFunction("CountCommandingActors", script, CountCommandingActors);
    vm->RegisterFunction("CountHostilesToActor", script, CountHostilesToActor);

    // clipboard
    vm->RegisterFunction("SetClipboard", script, SetClipboard);
    vm->RegisterFunction("GetClipboard", script, GetClipboard);

    // forms
    vm->RegisterFunction("GetFormsByEditorID", script, GetFormsByEditorID);
    vm->RegisterFunction("LookupFormSmart", script, LookupFormSmart);
    vm->RegisterFunction("HasAnyKeywordInList", script, HasAnyKeywordInList);
    vm->RegisterFunction("HasAllKeywordsInList", script, HasAllKeywordsInList);
    vm->RegisterFunction("GetFormTypeString", script, GetFormTypeString);
    vm->RegisterFunction("FormHasMagicEffect", script, FormHasMagicEffect);
    vm->RegisterFunction("FormHasMagicEffectKeyword", script, FormHasMagicEffectKeyword);
    vm->RegisterFunction("GetFormWithStrongestEffect", script, GetFormWithStrongestEffect);
    vm->RegisterFunction("GetFormWithWeakestEffect", script, GetFormWithWeakestEffect);
    vm->RegisterFunction("FormlistHasFormDeep", script, FormlistHasFormDeep);
    vm->RegisterFunction("TransferItems", script, TransferItems);
    vm->RegisterFunction("GetItemCountInContainersArray", script, GetItemCountInContainersArray);
    vm->RegisterFunction("GetItemCountInContainersList", script, GetItemCountInContainersList);
    vm->RegisterFunction("TransferItemFromContainersList", script, TransferItemFromContainersList);
    vm->RegisterFunction("TransferItemFromContainersArray", script, TransferItemFromContainersArray);

    // gamepad
    vm->RegisterFunction("IsGamepadConnected", script, IsGamepadConnected);
    vm->RegisterFunction("SimulateLeftStickInput", script, SimulateLeftStickInput);

    // objectreference
    vm->RegisterFunction("GetLinkedDoor", script, GetLinkedDoor);
    vm->RegisterFunction("FindActorsInFactionNearRef", script, FindActorsInFactionNearRef);
    vm->RegisterFunction("FindActorsWithVoiceTypeNearRef", script, FindActorsWithVoiceTypeNearRef);
    vm->RegisterFunction("FindActorsOfRaceNearRef", script, FindActorsOfRaceNearRef);

    // string
    vm->RegisterFunction("GetTranslatedString", script, GetTranslatedString);
    vm->RegisterFunction("StringReplace", script, StringReplace);

    // book
    vm->RegisterFunction("GetBookText", script, GetBookText);

    // UI
    vm->RegisterFunction("GetContainerMenuMode", script, GetContainerMenuMode);
    vm->RegisterFunction("GetCurrentBook", script, GetCurrentBook);
    vm->RegisterFunction("GetCurrentBookRef", script, GetCurrentBookRef);
    vm->RegisterFunction("IsEnchantingMenuOpen", script, IsEnchantingMenuOpen);
    vm->RegisterFunction("IsAlchemyMenuOpen", script, IsAlchemyMenuOpen);
    vm->RegisterFunction("IsTemperingMenuOpen", script, IsTemperingMenuOpen);

    // ini
    vm->RegisterFunction("GetINIString", script, GetINIString);
    vm->RegisterFunction("GetINIFloat", script, GetINIFloat);
    vm->RegisterFunction("GetINIInt", script, GetINIInt);
    vm->RegisterFunction("GetINIBool", script, GetINIBool);

    // array
    vm->RegisterFunction("CountValuesInBoolArray", script, CountValuesInBoolArray);
    vm->RegisterFunction("CountValuesInIntArray", script, CountValuesInIntArray);
    vm->RegisterFunction("CountValuesInFloatArray", script, CountValuesInFloatArray);
    vm->RegisterFunction("CountActorsWithVoiceTypeInArray", script, CountActorsWithVoiceTypeInArray);
    vm->RegisterFunction("CountActorsInFactionInArray", script, CountActorsInFactionInArray);
    vm->RegisterFunction("FilterIntArray", script, FilterIntArray);
    vm->RegisterFunction("FilterFloatArray", script, FilterFloatArray);
    vm->RegisterFunction("FilterActorsArrayForVoiceType", script, FilterActorsArrayForVoiceType);
    vm->RegisterFunction("FilterActorsArrayForFaction", script, FilterActorsArrayForFaction);

    // deprecated
    vm->RegisterFunction("GetContainerMode", script, GetContainerMenuMode);

    // doesn't work
    // vm->RegisterFunction("GetPlayerSpeechTarget", script, GetPlayerSpeechTarget);
    // vm->RegisterFunction("IsDiseased", script, IsDiseased); // ForEachActiveEffect fails
    // vm->RegisterFunction("GetDiseasesCount", script, GetDiseasesCount);
    // vm->RegisterFunction("DispelAllEffectsWithKeyword", script, );
    // vm->RegisterFunction("DispelAllEffectsWithKeyword", script, );

    return false;
}

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);

    SKSE::GetPapyrusInterface()->Register(PapyrusBinder);

    return true;
}