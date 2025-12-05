#include <windows.h>
#include <string>

bool HasAnyPerkInList(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list) {
    if (!a_actor || !a_list) return false;
    bool result = false;
    a_list->ForEachForm([&](TESForm& listItem) {
        BGSPerk* perk = listItem.As<BGSPerk>();
        if (perk && a_actor->HasPerk(perk)) {
            result = true;
            return BSContainer::ForEachResult::kStop;
        }
        return BSContainer::ForEachResult::kContinue;
    });

    return result;
}

bool HasAllPerksInList(StaticFunctionTag*, Actor* a_actor, BGSListForm* a_list) {
    if (!a_actor || !a_list) return false;
    bool result = true;
    a_list->ForEachForm([&](TESForm& listItem) {
        BGSPerk* perk = listItem.As<BGSPerk>();
        if (perk && ! a_actor->HasPerk(perk)) {
            result = false;
            return BSContainer::ForEachResult::kStop;
        }
        return BSContainer::ForEachResult::kContinue;
    });

    return result;
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
        a_list->ForEachForm([&](TESForm& listItem) {
            if (listItem.Is(FormType::FormList) && FormlistHasFormDeep({}, listItem.As<BGSListForm>(), a_form)) {
                bDeepResult = true;
                return BSContainer::ForEachResult::kStop;
            }
            return BSContainer::ForEachResult::kContinue;
        });
        return bDeepResult;
    }
}

void TransferItems(StaticFunctionTag*, TESObjectREFR* a_source, TESObjectREFR* a_target, int a_count = -1, int a_formtype = 0,
                   TESForm* formFilter = nullptr, bool a_excludeWorn = true, bool a_excludeFavorites = true,
                   bool a_excludeQuestItems = true) {
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
        if ((a_formtype != 0 && static_cast<int>(item->GetFormType()) != a_formtype)
            || item->Is(FormType::LeveledItem)
            || !item->GetPlayable()
            || (a_excludeWorn && data.second->IsWorn())
            || (a_excludeFavorites && data.second->IsFavorited())
            || (a_excludeQuestItems && data.second->IsQuestObject())
            || (!keywordFilter.empty() && !item->HasKeywordInArray(keywordFilter, false))
            || (formListFilter && !formListFilter->HasForm(item))
            || (formFilter && item != formFilter)
        ) {
            continue;
        }

        a_source->RemoveItem(item, a_count == -1 ? data.first : a_count, ITEM_REMOVE_REASON::kStoreInContainer, nullptr, a_target);
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
    a_list->ForEachForm([&](TESForm& listItem) {
        count += _getItemCountInRef(&listItem, a_form);
        return BSContainer::ForEachResult::kContinue;
    });
    return count;
}

void TransferItemFromContainersList(StaticFunctionTag*, BGSListForm* a_list,
                                    TESObjectREFR* a_target, TESForm* a_form, int a_count) {
    if (!a_target || !a_form || !a_list) return;

    int transferred = 0;
    a_list->ForEachForm([&](TESForm& listItem) {
        TESObjectREFR* currentRef = listItem.As<TESObjectREFR>();
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

void TransferItemFromContainersArray(StaticFunctionTag*, std::vector<TESObjectREFR*> a_sources,
    TESObjectREFR* a_target, TESForm* a_form, int a_count) {
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

bool PapyrusBinder(BSScript::IVirtualMachine* vm) {
    // actor
    vm->RegisterFunction("HasAnyPerkInList", "ShazdehUtils", HasAnyPerkInList);
    vm->RegisterFunction("HasAllPerksInList", "ShazdehUtils", HasAllPerksInList);

    // clipboard
    vm->RegisterFunction("SetClipboard", "ShazdehUtils", SetClipboard);
    vm->RegisterFunction("GetClipboard", "ShazdehUtils", GetClipboard);

    // forms
    vm->RegisterFunction("LookupFormSmart", "ShazdehUtils", LookupFormSmart);
    vm->RegisterFunction("HasAnyKeywordInList", "ShazdehUtils", HasAnyKeywordInList);
    vm->RegisterFunction("HasAllKeywordsInList", "ShazdehUtils", HasAllKeywordsInList);
    vm->RegisterFunction("GetFormTypeString", "ShazdehUtils", GetFormTypeString);
    vm->RegisterFunction("FormHasMagicEffect", "ShazdehUtils", FormHasMagicEffect);
    vm->RegisterFunction("FormHasMagicEffectKeyword", "ShazdehUtils", FormHasMagicEffectKeyword);
    vm->RegisterFunction("GetFormWithStrongestEffect", "ShazdehUtils", GetFormWithStrongestEffect);
    vm->RegisterFunction("GetFormWithWeakestEffect", "ShazdehUtils", GetFormWithWeakestEffect);
    vm->RegisterFunction("FormlistHasFormDeep", "ShazdehUtils", FormlistHasFormDeep);
    vm->RegisterFunction("TransferItems", "ShazdehUtils", TransferItems);
    vm->RegisterFunction("GetItemCountInContainersArray", "ShazdehUtils", GetItemCountInContainersArray);
    vm->RegisterFunction("GetItemCountInContainersList", "ShazdehUtils", GetItemCountInContainersList);
    vm->RegisterFunction("TransferItemFromContainersList", "ShazdehUtils", TransferItemFromContainersList);
    vm->RegisterFunction("TransferItemFromContainersArray", "ShazdehUtils", TransferItemFromContainersArray);

    // gamepad
    vm->RegisterFunction("IsGamepadConnected", "ShazdehUtils", IsGamepadConnected);
    vm->RegisterFunction("SimulateLeftStickInput", "ShazdehUtils", SimulateLeftStickInput);

    // objectreference
    vm->RegisterFunction("GetLinkedDoor", "ShazdehUtils", GetLinkedDoor);

    return false;
}

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);

    SKSE::GetPapyrusInterface()->Register(PapyrusBinder);

    return true;
}