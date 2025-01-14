#include "../pch.h"
#include "../include/Menu.hpp"
#include "SDK.hpp"
#include "config.h"
#include <algorithm>


int InputTextCallback(ImGuiInputTextCallbackData* data) {
    char inputChar = data->EventChar;
    Config.Update(Config.inputTextBuffer);

    return 0;
}

//SDK::FPalDebugOtomoPalInfo palinfo = SDK::FPalDebugOtomoPalInfo();
//SDK::TArray<SDK::EPalWazaID> EA = { 0U };

CatchRate CRate;
CatchRate OldRate;

void DetourCatchRate(SDK::APalCaptureJudgeObject* p_this) {
    if (p_this) {
        //p_this->ChallengeCapture_ToServer(Config.localPlayer, Config.CatchRate);
        p_this->ChallengeCapture(Config.localPlayer, Config.CatchRate);
    }
}

void Damage(SDK::APalCharacter* character, int32 damage)
{
    SDK::FPalDamageInfo  info = SDK::FPalDamageInfo();
    info.AttackElementType = SDK::EPalElementType::Normal;
    info.Attacker = Config.GetPalPlayerCharacter();
    info.AttackerGroupID = Config.GetPalPlayerState()->IndividualHandleId.PlayerUId;
    info.AttackerLevel = 50;
    info.AttackType = SDK::EPalAttackType::Weapon;
    info.bApplyNativeDamageValue = true;
    info.bAttackableToFriend = true;
    info.IgnoreShield = true;
    info.NativeDamageValue = damage;
    Config.GetPalPlayerState()->SendDamage_ToServer(character, info);
}


void ToggleCatchRate(bool catchrate) {
    if (catchrate) {
        if (CRate == NULL) {
            CRate = (CatchRate)(Config.ClientBase + Config.offset_CatchRate);
            MH_CreateHook(CRate, DetourCatchRate, reinterpret_cast<void**>(OldRate));
            MH_EnableHook(CRate);
            return;
        }
        MH_EnableHook(CRate);
        return;
    }
    else
    {
        MH_DisableHook(CRate);

    }
}

void AddItem(SDK::UPalPlayerInventoryData* data, char* itemName, int count)
{
    SDK::UKismetStringLibrary* lib = SDK::UKismetStringLibrary::GetDefaultObj();

    //Convert FNAME
    wchar_t  ws[255];
    swprintf(ws, 255, L"%hs", itemName);
    SDK::FName Name = lib->Conv_StringToName(SDK::FString(ws));
    //Call
    data->RequestAddItem(Name, count, true);
}

//void SpawnPal(char* PalName, int rank, int lvl = 1)
//{
//    SDK::UKismetStringLibrary* lib = SDK::UKismetStringLibrary::GetDefaultObj();
//
//    //Convert FNAME
//    wchar_t  ws[255];
//    swprintf(ws, 255, L"%hs", PalName);
//    SDK::FName Name = lib->Conv_StringToName(SDK::FString(ws));
//    //Call
//    if (Config.GetPalPlayerCharacter() != NULL)
//    {
//        if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
//        {
//            if (Config.GetPalPlayerCharacter()->GetPalPlayerController())
//            {
//                if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState())
//                {
//                    EA[0] = SDK::EPalWazaID::AirCanon;
//                    palinfo.Level = lvl;
//                    palinfo.Rank = rank;
//                    palinfo.PalName.Key = Name;
//                    palinfo.WazaList = EA;
//                    palinfo.PassiveSkill = NULL;
//                    Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->Debug_CaptureNewMonsterByDebugOtomoInfo_ToServer(palinfo);
//                }
//            }
//        }
//    }
//}

void Spawn_Multiple(config::QuickItemSet Set)
{
    SDK::UPalPlayerInventoryData* InventoryData = Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->GetInventoryData();
    switch (Set)
    {
    case 0:
        for (int i = 0; i < IM_ARRAYSIZE(database::basic_items_stackable); i++) {
            AddItem(InventoryData, _strdup(database::basic_items_stackable[i].c_str()), 100);
        }
    case 1:
        for (int i = 0; i < IM_ARRAYSIZE(database::basic_items_single); i++)
        {
            AddItem(InventoryData, _strdup(database::basic_items_single[i].c_str()), 1);
        }
    case 2:
        for (int i = 0; i < IM_ARRAYSIZE(database::pal_unlock_skills); i++) {
            AddItem(InventoryData, _strdup(database::pal_unlock_skills[i].c_str()), 1);
        }
    case 3:
        for (int i = 0; i < IM_ARRAYSIZE(database::spheres); i++) {
            AddItem(InventoryData, _strdup(database::spheres[i].c_str()), 100);
        }
    case 4:
        for (int i = 0; i < IM_ARRAYSIZE(database::tools); i++) {
            AddItem(InventoryData, _strdup(database::tools[i].c_str()), 1);
        }
    default:
        break;
    }
}//Creadit:asashi

void AnyWhereTP(SDK::FVector& vector, bool IsSafe)
{
    if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
    {

        SDK::FGuid guid = Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPlayerUId();
        vector = { vector.X,vector.Y + 100,vector.Z };
        Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->Player->RegisterRespawnLocation_ToServer(guid, vector);
        Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->RequestRespawn();
    }
    return;
}

void ExploitFly(bool IsFly)
{
    SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
    if (p_appc != NULL)
    {
        if (IsFly)
        {
            if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
            {
                Config.GetPalPlayerCharacter()->GetPalPlayerController()->StartFlyToServer();
            }
        }
        else
        {
            if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
            {
                Config.GetPalPlayerCharacter()->GetPalPlayerController()->EndFlyToServer();
            }
        }

    }
}

namespace DX11_Base {
    // helper variables
    char inputBuffer_getFnAddr[100];
    namespace Styles {
        void InitStyle()
        {
            ImGuiStyle& style = ImGui::GetStyle();
            ImVec4* colors = ImGui::GetStyle().Colors;

            //	STYLE PROPERTIES
            style.WindowPadding = ImVec2(15, 15);
            style.WindowRounding = 5.0f;
            style.FramePadding = ImVec2(5, 5);
            style.FrameRounding = 4.0f;
            style.ItemSpacing = ImVec2(12, 8);
            style.ItemInnerSpacing = ImVec2(8, 6);
            style.IndentSpacing = 25.0f;
            style.ScrollbarSize = 15.0f;
            style.ScrollbarRounding = 9.0f;
            style.GrabMinSize = 5.0f;
            style.GrabRounding = 3.0f;

            //  Base ImGui Styling , Aplying a custyom style is left up to you.
            ImGui::StyleColorsDark();

            /// EXAMPLE COLOR 
            //colors[ImGuiCol_FrameBg] = ImVec4(0, 0, 0, 0);

            //	COLORS
            if (g_Menu->dbg_RAINBOW_THEME) {
                //  RGB MODE STLYE PROPERTIES
                colors[ImGuiCol_Separator] = ImVec4(g_Menu->dbg_RAINBOW);
                colors[ImGuiCol_TitleBg] = ImVec4(0, 0, 0, 1.0f);
                colors[ImGuiCol_TitleBgActive] = ImVec4(0, 0, 0, 1.0f);
                colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0, 0, 0, 1.0f);
            }
            else {
                /// YOUR DEFAULT STYLE PROPERTIES HERE
                colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
                colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
                colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
                colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
                colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
                colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
                colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
                colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
                colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
                colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
                colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
                colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
                colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
                colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
                colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
                colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
                colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
                colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
                colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
                colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
                colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
                colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
                colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
                colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
                colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
                colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
                colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
                colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
                colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
                colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
                colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
                colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
                colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
                colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
                colors[ImGuiCol_Tab] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
                colors[ImGuiCol_TabActive] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
                colors[ImGuiCol_TabHovered] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
            }
        }
    }

    namespace Tabs {
        void TABPlayer()
        {

            //�л�����һ��
            ImGui::Checkbox("Toggle Speed", &Config.IsSpeedHack);

            ImGui::Checkbox("Toggle Atk Up", &Config.IsAttackModiler);

            ImGui::Checkbox("Toggle Def Up", &Config.IsDefuseModiler);

            ImGui::Checkbox("Inf Stamina", &Config.IsInfStamina);

            ImGui::Checkbox("Infinite Ammo", &Config.IsInfinAmmo);

            ImGui::Checkbox("Godmode", &Config.IsMuteki);

            ImGui::Checkbox("Revive", &Config.IsRevive);

            //��������һ��
            ImGui::SliderFloat("Speed Modifier", &Config.SpeedModiflers, 1, 10);
            ImGui::SliderInt("Attack Modifier", &Config.DamageUp, 0, 200000);
            ImGui::SliderInt("Defense Modifier", &Config.DefuseUp, 0, 200000);
            ImGui::Text("Player Position");
            SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
            if (p_appc != NULL)
            {
                if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                {
                    SDK::FVector PlayerLocation = p_appc->K2_GetActorLocation();
                    std::string MyLocation = std::format("X: {} | Y: {} | Z: {}", PlayerLocation.X, PlayerLocation.Y, PlayerLocation.Z);
                    ImGui::Text(MyLocation.c_str());
                    if (ImGui::Button("Print Coords", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20)))
                    g_Console->printdbg(MyLocation.c_str(), g_Console->color.green, p_appc);
                }
            }
        }

        void TABExploit()
        {
            //�����õİ�
            //Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->RequestSpawnMonsterForPlayer(name, 5, 1);
            ImGui::Checkbox("SafeTeleport", &Config.IsSafe);
            ImGui::Checkbox("Show Quick Tab", &Config.IsQuick);
            ImGui::Checkbox("Open Entity List", &Config.bisOpenManager);
            ImGui::InputFloat3("Pos:", Config.Pos);
            ImGui::InputInt("EXP:", &Config.EXP);
            if (ImGui::Button("Give exp", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20)))
            {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
                        {
                            if (Config.EXP >= 0)
                            {
                                Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->GrantExpForParty(Config.EXP);
                            }
                        }
                    }
                }
            }
            if (ImGui::Button("Teleport Home", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20)))
            {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
                        {
                            if (Config.IsSafe)
                            {
                                Config.GetPalPlayerCharacter()->GetPalPlayerController()->TeleportToSafePoint_ToServer();
                            }
                            else
                            {
                                Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->RequestRespawn();
                            }

                        }
                    }
                }

            }
            /*if (ImGui::Button("AnywhereTP", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20)))
            {
                if (Config.GetPalPlayerCharacter() != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        if (Config.Pos != NULL)
                        {
                            SDK::FVector vector = { Config.Pos[0],Config.Pos[1],Config.Pos[2] };
                            AnyWhereTP(vector, Config.IsSafe);
                        }
                    }
                }
            }*/

            if (ImGui::Button("ToggleFly", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20)))
            {
                Config.IsToggledFly = !Config.IsToggledFly;
                ExploitFly(Config.IsToggledFly);
            }

            if (ImGui::Button("NormalHealth", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20)))
            {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
                        {
                            SDK::FFixedPoint fixpoint = SDK::FFixedPoint();
                            fixpoint.Value = Config.GetPalPlayerCharacter()->CharacterParameterComponent->GetMaxHP().Value;
                            Config.GetPalPlayerCharacter()->ReviveCharacter_ToServer(fixpoint);
                        }
                    }
                }
            }

            if (ImGui::Button("GodHealth", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20)))
            {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
                        {
                            SDK::FFixedPoint fixpoint = SDK::FFixedPoint();
                            fixpoint.Value = 99999999;
                            Config.GetPalPlayerCharacter()->ReviveCharacter_ToServer(fixpoint);
                        }
                    }
                }
            }

            if (ImGui::Button("MaxWeight", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20)))
            {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
                        {
                            Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->InventoryData->MaxInventoryWeight = Config.MaxWeight;
                        }
                    }
                }
            }

            if (ImGui::Button("Catch Rate", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20)))
            {
                Config.isCatchRate = !Config.isCatchRate;
                ToggleCatchRate(Config.isCatchRate);
            }
            ImGui::InputFloat("Catch Rate Modifier", &Config.CatchRate);
        }

        void TABItemSpawner()
        {
            static int num_to_add = 1;
            static int category = 0;

            ImGui::InputInt("Num To Add", &num_to_add);

            ImGui::Combo("Item Category", &category, "Accessories\0Ammo\0Armor\0Crafting Materials\0Eggs\0Food\0Hats\0\Medicine\0Money\0Other\0Pal Spheres\0Seeds\0Tools\0Weapons\0\All\0");

            std::initializer_list list = database::all;

            switch (category)
            {
            case 1:
                list = database::ammo;
                break;
            case 2:
                list = database::armor;
                break;
            case 3:
                list = database::craftingmaterials;
                break;
            case 4:
                list = database::eggs;
                break;
            case 5:
                list = database::food;
                break;
            case 6:
                list = database::hats;
                break;
            case 7:
                list = database::medicine;
                break;
            case 8:
                list = database::money;
                break;
            case 9:
                list = database::other;
                break;
            case 10:
                list = database::palspheres;
                break;
            case 11:
                list = database::seeds;
                break;
            case 12:
                list = database::toolss;
                break;
            case 13:
                list = database::weapons;
                break;
            case 14:
                list = database::all;
                break;
            default:
                list = database::all;
            }

            int cur_size = 0;

            static char item_search[100];;

            ImGui::InputText("Search", item_search, IM_ARRAYSIZE(item_search));
            ImGui::BeginChild("ScrollingRegion", ImVec2(0, 800), true);
            for (const auto& item : list) {
                std::istringstream ss(item);
                std::string left_text, right_text;

                std::getline(ss, left_text, '|');
                std::getline(ss, right_text);

                auto right_to_lower = right_text;
                std::string item_search_to_lower = item_search;

                std::transform(right_to_lower.begin(), right_to_lower.end(), right_to_lower.begin(), ::tolower);
                std::transform(item_search_to_lower.begin(), item_search_to_lower.end(), item_search_to_lower.begin(), ::tolower);

                if (item_search[0] != '\0' && (right_to_lower.find(item_search_to_lower) == std::string::npos))
                    continue;

                if (cur_size != 0 && cur_size < 20)
                {
                    ImGui::Columns(1);
                }
                else if (cur_size != 0)
                {
                    cur_size = 0;
                }

                cur_size += right_text.length();

                ImGui::PushID(item);
                if (ImGui::Button(right_text.c_str()))
                {
                    SDK::UPalPlayerInventoryData* InventoryData = Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->GetInventoryData();//rebas
                    AddItem(InventoryData, (char*)left_text.c_str(), num_to_add);
                }
                ImGui::PopID();
            }
            ImGui::EndChild();
        }
        class TeleportTabs {
        public:
            TeleportTabs() : currentPage(0), buttonsPerPage(10) {
                // Add your teleport locations here
                locationMap["Anubis"] = Config.AnubisLocation;
                locationMap["Azurobe"] = Config.AzurobeLocation;
                locationMap["Beakon"] = Config.BeakonLocation;
                locationMap["Broncherry Aqua"] = Config.BroncherryAquaLocation;
                locationMap["Broncherry"] = Config.BroncherryLocation;
                locationMap["Bushi"] = Config.BushiLocation;
                locationMap["Chillet"] = Config.ChilletLocation;
                locationMap["Dinossom Lux"] = Config.DinossomLuxLocation;
                locationMap["Elizabee"] = Config.ElizabeeLocation;
                locationMap["Felbat"] = Config.FelbatLocation;
                locationMap["Fenglope"] = Config.FenglopeLocation;
                locationMap["Frostallion"] = Config.FrostallionLocation;
                locationMap["Grintale"] = Config.GrintaleLocation;
                locationMap["Gumoss"] = Config.GumossLocation;
                locationMap["Jetragon"] = Config.JetragonLocation;
                locationMap["Jormuntide2"] = Config.Jormuntide2Location;
                locationMap["Jormuntide"] = Config.JormuntideLocation;
                locationMap["Katress"] = Config.KatressLocation;
                locationMap["Kingpaca"] = Config.KingpacaLocation;
                locationMap["Lunaris"] = Config.LunarisLocation;
                locationMap["Lyleen Noct"] = Config.LyleenNoctLocation;
                locationMap["Mammorest"] = Config.MammorestLocation;
                locationMap["Menasting"] = Config.MenastingLocation;
                locationMap["Mossanda Lux"] = Config.MossandaLuxLocation;
                locationMap["Nitewing"] = Config.NitewingLocation;
                locationMap["Paladius"] = Config.PaladiusLocation;
                locationMap["Penking"] = Config.PenkingLocation;
                locationMap["Petallia"] = Config.PetalliaLocation;
                locationMap["Quivern"] = Config.QuivernLocation;
                locationMap["Relaxasaurus"] = Config.RelaxasaurusLuxLocation;
                locationMap["Siblex"] = Config.SiblexLocation;
                locationMap["Suzaku"] = Config.SuzakuLocation;
                locationMap["Univolt"] = Config.UnivoltLocation;
                locationMap["Vaelet"] = Config.VaeletLocation;
                locationMap["Verdash"] = Config.VerdashLocation;
                locationMap["Warsect"] = Config.WarsectLocation;
                locationMap["Wumpo Botan"] = Config.WumpoBotanLocation;
                // Add more teleport locations as needed

                // Extract button names from the map
                for (const auto& pair : locationMap) {
                    teleportLocations.push_back(pair.first);
                }

                UpdateCurrentPageButtons();
            }

            void Draw() {
                // Draw buttons only if there are locations
                if (teleportLocations.empty()) {
                    ImGui::Text("No teleport locations available.");
                    return;
                }

                // Draw buttons
                for (int i = 0; i < currentButtons.size(); ++i) {
                    if (ImGui::Button(currentButtons[i].c_str(), ImVec2(ImGui::GetContentRegionAvail().x - 3, 20))) {
                        // Button logic
                        TeleportToLocation(currentButtons[i]);
                    }
                }

                // Draw page navigation
                ImGui::Separator();
                ImGui::Text("Page: %d", currentPage + 1);
                ImGui::SameLine();
                if (ImGui::Button("Previous")) {
                    currentPage = (currentPage - 1 + totalPages) % totalPages;
                    UpdateCurrentPageButtons();
                }
                ImGui::SameLine();
                if (ImGui::Button("Next")) {
                    currentPage = (currentPage + 1) % totalPages;
                    UpdateCurrentPageButtons();
                }
            }

            void TeleportToLocation(const std::string& location) {
                // Check if the location exists in the map
                if (locationMap.find(location) != locationMap.end()) {
                    SDK::FVector vector = { locationMap[location][0], locationMap[location][1], locationMap[location][2] };
                    AnyWhereTP(vector, Config.IsSafe);
                }
            }

        private:
            void UpdateCurrentPageButtons() {
                int startIdx = currentPage * buttonsPerPage;
                int endIdx = (currentPage + 1) * buttonsPerPage;

                if (startIdx < 0) {
                    startIdx = 0;
                }

                if (endIdx > teleportLocations.size()) {
                    endIdx = teleportLocations.size();
                }

                currentButtons.clear();
                for (int i = startIdx; i < endIdx; ++i) {
                    currentButtons.push_back(teleportLocations[i]);
                }

                totalPages = (teleportLocations.size() + buttonsPerPage - 1) / buttonsPerPage;
            }

        private:
            std::vector<std::string> teleportLocations;
            std::vector<std::string> currentButtons;
            int currentPage;
            int buttonsPerPage;
            int totalPages;
            std::map<std::string, float*> locationMap; // Map button names to their corresponding locations in Config
        };

        void TABQuickTP() {
            static TeleportTabs teleportTabs; // Static to retain state between calls
            // Render your user interface
            teleportTabs.Draw();
        }
        void TABMisc()
        {
            if (ImGui::Button("Max Level<50>", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20)))
            {
                SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
                if (p_appc != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                    {
                        if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
                        {
                            Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->GrantExpForParty(99999999);
                        }
                    }
                }
            }
            if (ImGui::Button("All Effigies", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20))) //credit to bennett
            {
                SDK::APalPlayerCharacter* pPalCharacter = Config.GetPalPlayerCharacter();
                if (!pPalCharacter)
                    return;

                SDK::UWorld* world = Config.GetUWorld();
                if (!world)
                    return;

                SDK::TUObjectArray* objects = world->GObjects;

                for (int i = 0; i < objects->NumElements; ++i) {
                    SDK::UObject* object = objects->GetByIndex(i);

                    if (!object) {
                        continue;
                    }

                    if (!object->IsA(SDK::APalLevelObjectRelic::StaticClass())) {
                        continue;
                    }

                    SDK::APalLevelObjectObtainable* relic = (SDK::APalLevelObjectObtainable*)object;
                    if (!relic) {
                        continue;
                    }

                    ((SDK::APalPlayerState*)pPalCharacter->PlayerState)->RequestObtainLevelObject_ToServer(relic);
                }
            }
            if (ImGui::Button("Unlock Fast Travel", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20))) // Credit to arcomit, aaacaaac, ShieldSupporter
            {
                std::vector<std::string> keyStrings = {
                    "6E03F8464BAD9E458B843AA30BE1CC8F","DDBBFFAF43D9219AE68DF98744DF0831","603ED0CD4CFB9AFDC9E11F805594CCE5","6282FE1E4029EDCDB14135AA4C171E4C","9FBB93D84811BE424A37C391DBFBB476","979BF2044C8E8FE559B598A95A83EDE3","923B781345D2AB7ECED6BABD6E97ECE8",
                    "16C7164C43E2B96BEDCC6696E2E592F7","D27AFCAD472164F000D33A8D8B61FE8B","41727100495D21DC905D309C53989914","6DB6B7B2431CA2EFFFC88EB45805BA6A","74270C2F45B8DCA66B6A1FAAA911D024","DF9FB9CB41B43052A9C74FA79A826A50","8CA5E9774FF1BBC1156ABCA09E616480",
                    "15314BE94E2FB8D018D5749BE9A318F0","79C561B747335A7A0A8FBF9FAE280E62","23B9E99C4A454B99220AF7B4A58FD8DE","A1BC65AA445619EF338E0388BC010648","BF8B123244ADB794A06EA8A10503FBDD","F8DF603B4C750B37D943C9AF6A911946","596996B948716D3FD2283C8B5C6E829C",
                    "28D514E74B51FD9EB078A891DB0787C2","ACAE5FB04D48DE4197443E9C0993086B","4D2F204549AB656CA1EA4B8E39C484F3","1BDEABA240B1699541C17F83E59E61DF","2BC5E46049E69D3549CFB58948BE3288","91DAC6F34D2A9FD7F01471B5166C6C02","41E36D9A4B2BA79A3AD1B7B83B16F77D",
                    "76B000914943BADDC56BCFBAE2BF051E","DC0ECF9241B4410C59EE619F56D1106A","71C4B2B2407F2BBBD77572A20C7FF0F5","EC94023A4CA571FF0FD19E90944F4231","2A2B744B41AC79964DAE6B89CAC51FC3","E0819EFB41581AEAC3A029B0EE2FE195","22095BFA48A46172F8D154B2EBEB7493",
                    "7C5E91514F6E84B0C1DEFFB52C7C4DBA","AECFED0D41AFEE11F30B4F9687BC3243","2EC07ACC4505CB726DE38A84246CA999","F8E5CB8143F4FA2F6213E6B454569F87","5F426B49469368B0C131D3A6DB8F7831","A277AE6341EF40D84D711EA52303353F","6231802D40C81C00445379AE238D9F90",
                    "F6C005A14B38FE0B57F1C7869FD899CB","7170881D44249E90902F728E240493AF","3E8E504B4A3975FD3862E1BC85A5D4F6","B001852C491FF5E70C4747BFF9972924","2DE1702048A1D4A82126168C49BE51A9","E88379634CB5B6117DA2E7B8810BFE0A","3697999C458BF8A3C7973382969FBDF9",
                    "65B10BB14ABDA9C2109167B21901D195","4669582D4081BF550AFB66A05D043A3D","FE90632845114C7FBFA4669D071E285F","5970E8E648D2A83AFDFF7C9151D9BEF5","B639B7ED4EE18A7AA09BA189EA703032","099440764403D1508D9BADADF4848697","B44AA24445864494E7569597ACCAEFC6",
                    "3A0F123947BE045BC415C6B061A5285A","F382ADAE4259150BF994FF873ECF242B", "01ACCA6E4BDAA68220821FB05AB54E4D", "866881DB443444AA7F4E7C8E5DCDAA29", "75BD9923489E2A4EBCED5A81175D5928", "513E166044565A0BD3360F94142577E8"
                };

                SDK::APalPlayerCharacter* pPalCharacter = Config.GetPalPlayerCharacter();

                for (const std::string& keyString : keyStrings)
                {
                    SDK::UKismetStringLibrary* lib = SDK::UKismetStringLibrary::GetDefaultObj();
                    //Convert FNAME
                    wchar_t  ws[255];
                    swprintf(ws, 255, L"%hs", keyString.c_str());
                    SDK::FName FNameKS = lib->Conv_StringToName(SDK::FString(ws));

                    ((SDK::APalPlayerState*)pPalCharacter->PlayerState)->RequestUnlockFastTravelPoint_ToServer(FNameKS);
                }

            }
         }

        void TABQuick()//Creadit:asashi
        {
            if (ImGui::Button("Basic Items stack", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20))) {
                Spawn_Multiple(config::QuickItemSet::basic_items_stackable);
            }
            if (ImGui::Button("Basic Items single", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20))) {
                Spawn_Multiple(config::QuickItemSet::basic_items_single);
            }
            if (ImGui::Button("Unlock Pal skills", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20))) {
                Spawn_Multiple(config::QuickItemSet::pal_unlock_skills);
            }
            if (ImGui::Button("Spheres", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20))) {
                Spawn_Multiple(config::QuickItemSet::spheres);
            }
            if (ImGui::Button("Tools", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20))) {
                Spawn_Multiple(config::QuickItemSet::tools);
            }
        }
     
        void TABConfig()
        {
            ImGui::Text("NuLL Menu");
            ImGui::Text("Version: v1.5");
            ImGui::Text("Credits to: bluesword007");
            ImGui::Text("Credits to: UnknownCheats.me");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            if (ImGui::Button("Unload DLL", ImVec2(ImGui::GetContentRegionAvail().x - 3, 20))) {
#if DEBUG
                g_Console->printdbg("\n\n[+] Unloading Initialized [+]\n\n", g_Console->color.red);

#endif
                g_KillSwitch = TRUE;
            }
        }
	}

    void Menu::Draw()
    {
        if (Config.IsESP)
        {
            ESP();
        }
        if (g_GameVariables->m_ShowMenu)
            MainMenu();
        if (Config.bisOpenManager && g_GameVariables->m_ShowMenu)
        {
            EntityList();
        }

        if (g_GameVariables->m_ShowHud)
            HUD(&g_GameVariables->m_ShowHud);

        if (g_GameVariables->m_ShowDemo)
            ImGui::ShowDemoWindow();
    }

    void Menu::EntityList()
    {
        if (ImGui::Begin("Entity List", &g_GameVariables->m_ShowMenu, 96))
        {
            if (Config.GetUWorld() != NULL)
            {
                ImGui::Checkbox("filterPlayer", &Config.filterPlayer);
                SDK::TArray<SDK::AActor*> T = Config.GetUWorld()->PersistentLevel->Actors;
                for (int i = 0; i < T.Count(); i++)
                {
                    if (T[i] != NULL)
                    {
                        if (T[i]->IsA(SDK::APalCharacter::StaticClass()))
                        {
                            SDK::APalCharacter* Character = (SDK::APalCharacter*)T[i];
                            SDK::FString name;
                            if (Config.filterPlayer)
                            {
                                if (!T[i]->IsA(SDK::APalPlayerCharacter::StaticClass()))
                                {
                                    continue;
                                }
                            }
                            if (T[i]->IsA(SDK::APalPlayerCharacter::StaticClass()))
                            {
                                if (!Character) { continue; }
                                Character->CharacterParameterComponent->GetNickname(&name);
                            }
                            else
                            {
                                SDK::UKismetStringLibrary* lib = SDK::UKismetStringLibrary::GetDefaultObj();
                                if (!Character) { continue; }
                                std::string s = Character->GetFullName();
                                size_t firstUnderscorePos = s.find('_');

                                if (firstUnderscorePos != std::string::npos) {
                                    std::string result = s.substr(firstUnderscorePos + 1);

                                    size_t secondUnderscorePos = result.find('_');

                                    if (secondUnderscorePos != std::string::npos) {
                                        result = result.substr(0, secondUnderscorePos);
                                    }
                                    wchar_t  ws[255];
                                    swprintf(ws, 255, L"%hs", result);
                                    name = SDK::FString(ws);
                                }
                            }
                            ImGui::Text(name.ToString().c_str());
                            ImGui::SameLine();
                            ImGui::PushID(i);
                            if (ImGui::Button("Kill"))
                            {
                                if (T[i]->IsA(SDK::APalCharacter::StaticClass()))
                                {
                                    Damage(Character, 99999999999);
                                }
                                continue;
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("TP"))
                            {
                                if (Config.GetPalPlayerCharacter() != NULL)
                                {
                                    if (!Character) { continue; }
                                    SDK::FVector vector = Character->K2_GetActorLocation();
                                    AnyWhereTP(vector, Config.IsSafe);
                                }
                            }

                            /*if (Character->IsA(SDK::APalPlayerCharacter::StaticClass()))
                            {
                                ImGui::SameLine();
                                if (ImGui::Button("Boss"))
                                {
                                    if (Config.GetPalPlayerCharacter() != NULL)
                                    {
                                        auto controller = Config.GetPalPlayerCharacter()->GetPalPlayerController();
                                        if (controller != NULL)
                                        {
                                            controller->Transmitter->BossBattle->RequestBossBattleEntry_ToServer(SDK::EPalBossType::ElectricBoss, (SDK::APalPlayerCharacter*)Character);
                                            controller->Transmitter->BossBattle->RequestBossBattleStart_ToServer(SDK::EPalBossType::ElectricBoss, (SDK::APalPlayerCharacter*)Character);
                                        }
                                    }
                                }
                            }*/
                            if (Character->IsA(SDK::APalPlayerCharacter::StaticClass()))
                            {
                                ImGui::SameLine();
                                if (ImGui::Button("MaskIt"))
                                {
                                    if (Config.GetPalPlayerCharacter() != NULL)
                                    {
                                        auto controller = Config.GetPalPlayerCharacter()->GetPalPlayerController();
                                        if (controller != NULL)
                                        {
                                            auto player = (SDK::APalPlayerCharacter*)Character;
                                            SDK::FString fakename;
                                            player->CharacterParameterComponent->GetNickname(&fakename);
                                            Config.GetPalPlayerCharacter()->GetPalPlayerController()->Transmitter->NetworkIndividualComponent->UpdateCharacterNickName_ToServer(Config.GetPalPlayerCharacter()->CharacterParameterComponent->IndividualHandle->ID, fakename);
                                        }
                                    }
                                }
                            }
                            ImGui::PopID();
                        }
                    }
                }
            }
        }
    }

	void Menu::MainMenu()
	{
        if (!g_GameVariables->m_ShowDemo)
            Styles::InitStyle();

        if (g_Menu->dbg_RAINBOW_THEME) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(g_Menu->dbg_RAINBOW));
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(g_Menu->dbg_RAINBOW));
            ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(g_Menu->dbg_RAINBOW));
            //ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(g_Menu->dbg_RAINBOW));
        }
        if (!ImGui::Begin("NuLLxD", &g_GameVariables->m_ShowMenu, 96))
        {
            ImGui::End();
            return;
        }
        if (g_Menu->dbg_RAINBOW_THEME) {
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }
        
        //  Display Menu Content
        //Tabs::TABMain();

        ImGui::Text("NuLL Internal v1.5");

        if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
        {
          if (ImGui::BeginTabItem("Player"))
           {
               Tabs::TABPlayer();
               ImGui::EndTabItem();
           }
          if (ImGui::BeginTabItem("Exploits"))
          {
              Tabs::TABExploit();
              ImGui::EndTabItem();
          }
          if (ImGui::BeginTabItem("Item Spawner"))
          {
              Tabs::TABItemSpawner();
              ImGui::EndTabItem();
          }
          if (ImGui::BeginTabItem("Quick TP"))
          {
              Tabs::TABQuickTP();
              ImGui::EndTabItem();
          }
          if (ImGui::BeginTabItem("Misc"))
          {
              Tabs::TABMisc();
              ImGui::EndTabItem();
          }
          if (Config.IsQuick && ImGui::BeginTabItem("Quick"))
          {
              Tabs::TABQuick();
              ImGui::EndTabItem();
          }
          if (ImGui::BeginTabItem("Info"))
          {
              Tabs::TABConfig();
              ImGui::EndTabItem();
          }
         
           ImGui::EndTabBar();
        }
        ImGui::End();

        
	}

	void Menu::HUD(bool* p_open)
	{

	}

    void Menu::Loops()
    {
        if ((GetAsyncKeyState(VK_F5) & 1))
        {
            SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
            if (p_appc != NULL)
            {
                if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
                    {
                        if (Config.IsSafe)
                        {
                            Config.GetPalPlayerCharacter()->GetPalPlayerController()->TeleportToSafePoint_ToServer();
                        }
                        else
                        {
                            Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState()->RequestRespawn();

                        }

                    }
                }
            }
        }
        if ((GetAsyncKeyState(VK_F6) & 1))
        {
            SDK::APalPlayerCharacter* p_appc = Config.GetPalPlayerCharacter();
            if (p_appc != NULL)
            {
                if (Config.GetPalPlayerCharacter()->GetPalPlayerController() != NULL)
                {
                    if (Config.GetPalPlayerCharacter()->GetPalPlayerController()->GetPalPlayerState() != NULL)
                    {
                        SDK::FFixedPoint fixpoint = SDK::FFixedPoint();
                        fixpoint.Value = 99999999;
                        Config.GetPalPlayerCharacter()->ReviveCharacter_ToServer(fixpoint);

                    }
                }
            }
        }
        if (Config.IsSpeedHack)
        {
            if (Config.GetUWorld()
                || Config.GetUWorld()->PersistentLevel
                || Config.GetUWorld()->PersistentLevel->WorldSettings)
            {
                Config.GetUWorld()->OwningGameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn->CustomTimeDilation = Config.SpeedModiflers;
            }
        }
        if (Config.GetPalPlayerCharacter() != NULL)
        {
            if (Config.GetPalPlayerCharacter()->CharacterParameterComponent != NULL)
            {
                Config.GetPalPlayerCharacter()->CharacterParameterComponent->bIsEnableMuteki = Config.IsMuteki;
            }
        }
        if (Config.IsAttackModiler)
        {
            if (Config.GetPalPlayerCharacter() != NULL && Config.GetPalPlayerCharacter()->CharacterParameterComponent->AttackUp != Config.DamageUp)
            {
                if (Config.GetPalPlayerCharacter()->CharacterParameterComponent != NULL)
                {
                    Config.GetPalPlayerCharacter()->CharacterParameterComponent->AttackUp = Config.DamageUp;
                }
            }
        }
        if (Config.IsDefuseModiler)
        {
            if (Config.GetPalPlayerCharacter() != NULL && Config.GetPalPlayerCharacter()->CharacterParameterComponent->DefenseUp != Config.DefuseUp)
            {
                if (Config.GetPalPlayerCharacter()->CharacterParameterComponent != NULL)
                {
                    Config.GetPalPlayerCharacter()->CharacterParameterComponent->DefenseUp = Config.DefuseUp;
                }
            }
        }
        if (Config.IsInfStamina)
        {
            if (Config.GetPalPlayerCharacter() != NULL)
            {
                if (Config.GetPalPlayerCharacter()->CharacterParameterComponent != NULL)
                {
                    Config.GetPalPlayerCharacter()->CharacterParameterComponent->ResetSP();
                }
            }
        }
        if (Config.IsRevive)
        {
            if (Config.GetPalPlayerCharacter() != NULL)
            {
                if (Config.GetPalPlayerCharacter()->CharacterParameterComponent != NULL)
                {
                    Config.GetPalPlayerCharacter()->CharacterParameterComponent->ResetDyingHP();
                }
            }
        }
        if (Config.GetPalPlayerCharacter() != NULL)
        {
            if (Config.GetPalPlayerCharacter()->ShooterComponent != NULL && Config.GetPalPlayerCharacter()->ShooterComponent->CanShoot())
            {
                if (Config.GetPalPlayerCharacter()->ShooterComponent->GetHasWeapon() != NULL)
                {
                    Config.GetPalPlayerCharacter()->ShooterComponent->GetHasWeapon()->IsRequiredBullet = !Config.IsInfinAmmo;
                }
            }
        }
 
    }
}