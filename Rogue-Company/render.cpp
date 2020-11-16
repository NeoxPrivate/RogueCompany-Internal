#pragma once
#include "render.h"

namespace Render {

	/*
	* Global Variables.
	*/

	BOOL status = FALSE;
	HWND hWnd;
	float width, height;

	
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext = NULL;
	ID3D11RenderTargetView* renderTargetView = NULL;
	WNDPROC WndProc;

	LRESULT CALLBACK WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

		if (msg == WM_KEYUP && 
			(wParam == VK_INSERT || 
			(Settings.ShowMenu && wParam == VK_ESCAPE))
			) {
			Settings.ShowMenu = !Settings.ShowMenu;
			ImGui::GetIO().MouseDrawCursor = Settings.ShowMenu;

		}

		if (Settings.ShowMenu) {
			// To allow input into ImGui.
			ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
			return TRUE;
		}

		return CallWindowProcA(WndProc, hWnd, msg, wParam, lParam);
	}

	
	

			return *ImGui::GetCurrentWindow();
		};

		if (!device) {

			swapChain->GetDevice(__uuidof(device), (VOID**)(&device));
			device->GetImmediateContext(&deviceContext);

			ID3D11Texture2D* renderTarget = NULL;
			swapChain->GetBuffer(0, __uuidof(renderTarget), (VOID**)(&renderTarget));
			device->CreateRenderTargetView(renderTarget, NULL, &renderTargetView);
			renderTarget->Release();

			ID3D11Texture2D* backBuffer = 0;
			swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (VOID**)&backBuffer);
			D3D11_TEXTURE2D_DESC desc = { 0 };
			backBuffer->GetDesc(&desc);


			DXGI_SWAP_CHAIN_DESC Window;
			swapChain->GetDesc(&Window);

			hWnd = Window.OutputWindow;

			width = (float)desc.Width;
			height = (float)desc.Height;
			backBuffer->Release();
			WndProc = (WNDPROC)SetWindowLongPtrW(hWnd, GWLP_WNDPROC, (LONG_PTR)WndProcHook);

			ImGui::GetIO().Fonts->AddFontFromMemoryTTF(arial, 16, 16);

			ImGui_ImplDX11_Init(hWnd, device, deviceContext);
			ImGui_ImplDX11_CreateDeviceObjects();
		}

		deviceContext->OMSetRenderTargets(1, &renderTargetView, NULL);

		float closestDistance = FLT_MAX;
		AActor* closestPawn = NULL;

		std::vector<AActor*> pawns;
		auto& window = beginScene();
		
		/*
		* To handle errors.
		*/
		do {

			window.DrawList->AddCircle(ImVec2(width / 2, height / 2), Settings.FOV, ImGui::GetColorU32({ 255.0f, 255.0f, 255.0f, 1.0f }), 64);

			auto World = Util::World->GetWorld();
			if (!World) break;

			auto GameInstance = World->OwningGameInstance;
			if (!GameInstance) break;

			auto LocalPlayer = GameInstance->LocalPlayers[0]; 
			if (!LocalPlayer) break;

			auto PlayerController = LocalPlayer->Controller;
			if (!PlayerController) break;

			auto PlayerPawn = PlayerController->AcknowledgedPawn;
			if (!PlayerPawn) break;

			auto PlayerState = PlayerPawn->PlayerState;
			if (!PlayerState) break;

			auto TeamComponent = PlayerState->TeamComponent;
			if (!TeamComponent) break;
			
			for (auto levels = 0; levels < World->Levels.Num(); levels++) {

				if (!World->Levels.IsValidIndex(levels)) break;

				auto level = World->Levels[levels];
				if (!level) continue;
				
				for (auto actors = 0; actors < level->Actors.Num(); actors++) {

					if (!level || !level->Actors.IsValidIndex(actors)) break;

					auto actor = level->Actors[actors];
					if (!actor || !actor->RootComponent ||
						(PlayerPawn && PlayerPawn == actor))
						continue;

					auto name = Util::GetObjectName(actor);
					
					
					if (strstr(name, xorstr_("DefaultPVPBotCharacter_C")) ||
						strstr(name, xorstr_("MainCharacter_C"))) {

						pawns.push_back(actor);
					}
					
				}
				
			}
			
			for (auto pawn : pawns) {

				if (!pawn) continue;

				auto mesh = pawn->Mesh;
				if (!mesh) continue;

				auto state = pawn->PlayerState;
				if (!state) continue;

				auto eliminatedState = state->EliminatedState;
				if (eliminatedState != TRUE) continue;

				auto teamComponent = state->TeamComponent;
				if (!teamComponent) continue;

				auto color = ImGui::GetColorU32({ 255.0f, 255.0f, 255.0f, 1.0f });

				auto headPos = Util::GetBoneMatrix(mesh, 110).ToScreen(PlayerController);

				if (headPos.IsValid()) {

					if (!Util::LineOfSightTo(PlayerController, pawn) ||
						teamComponent->TeamIndex == TeamComponent->TeamIndex) {
						color = ImGui::GetColorU32({ 255.0f, 0.0, 0.0f, 1.0f });
					}
					else {

						headPos.X -= width / 2;
						headPos.Y -= height / 2;

						auto distance = headPos.distance();

						if (distance < Settings.FOV && distance < closestDistance) {

							closestDistance = distance;
							closestPawn = pawn;
						}
					}
				}

				if (teamComponent->TeamIndex == TeamComponent->TeamIndex) {
					color = ImGui::GetColorU32({ 0.0f, 191.0f, 255.0f, 1.0f });
				}

				if (Settings.ESP) {

					auto box = Util::GetBoundingBox(mesh, pawn->RootComponent->RelativeLocation);

					auto min = box.Min.ToScreen(PlayerController);
					auto max = box.Max.ToScreen(PlayerController);

					if (min.IsValid() && max.IsValid()) {

						auto topLeft = ImVec2(min.X, min.Y);
						auto bottomRight = ImVec2(max.X, max.Y);

						auto topRight = ImVec2(bottomRight.x, topLeft.y);
						auto bottomLeft = ImVec2(topLeft.x, bottomRight.y);
						auto bottomCenter = ImVec2(((topLeft.x + bottomRight.x) - 20.0f) / 2.0f, topLeft.y + 20.0f);

						// Top Left.
						window.DrawList->AddLine(topLeft, ImVec2(topLeft.x, topLeft.y + ((bottomRight.y - topLeft.y) / 3)), color, 1.0);
						window.DrawList->AddLine(topLeft, ImVec2(topLeft.x + ((bottomRight.x - topLeft.x) / 3), topLeft.y), color, 1.0);


						// Bottom Left
						window.DrawList->AddLine(bottomLeft, ImVec2(bottomLeft.x, bottomLeft.y - ((bottomRight.y - topLeft.y) / 3)), color, 1.0);
						window.DrawList->AddLine(bottomLeft, ImVec2(bottomLeft.x + ((bottomRight.x - topLeft.x) / 3), bottomLeft.y), color, 1.0);

						// Top Right
						window.DrawList->AddLine(topRight, ImVec2(topRight.x, topRight.y + ((bottomRight.y - topLeft.y) / 3)), color, 1.0);
						window.DrawList->AddLine(topRight, ImVec2(topRight.x - ((bottomRight.x - topLeft.x) / 3), topRight.y), color, 1.0);

						// Bottom Right
						window.DrawList->AddLine(bottomRight, ImVec2(bottomRight.x, bottomRight.y - ((bottomRight.y - topLeft.y) / 3)), color, 1.0);
						window.DrawList->AddLine(bottomRight, ImVec2(bottomRight.x - ((bottomRight.x - topLeft.x) / 3), bottomRight.y), color, 1.0);

						// Filled Box
						window.DrawList->AddRectFilled(ImVec2(min.X, min.Y), ImVec2(max.X, max.Y), ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 0.15 }));

						if (Settings.PlayerName) {

							auto playerName = state->PlayerName;
							if (playerName.data()) {

								CHAR buffer[32]; // -> Name won't be longer than 32 characters, so lets not abuse the stack.
								strcpy_s(buffer, sizeof(buffer), playerName.c_str());

								auto size = ImGui::GetFont()->CalcTextSizeA(window.DrawList->_Data->FontSize, FLT_MAX, NULL, buffer);
								window.DrawList->AddText(ImVec2(bottomCenter.x - size.x / 2.0f, bottomCenter.y - size.y), color, buffer);
							}
						}

					}
				}
			}

			status = TRUE;

		} while (FALSE); // executes ONCE.


		if (Settings.ShowMenu) {

			auto& style = ImGui::GetStyle();

			style.WindowRounding = 5.3f;
			style.FrameRounding = 2.3f;
			style.ScrollbarRounding = 0.f;
			style.WindowPadding = ImVec2(10.f, 10.f);
			style.PopupRounding = 0.f;
			style.FramePadding = ImVec2(2.f, 2.f);
			style.ItemSpacing = ImVec2(4.f, 4.f);
			style.ItemInnerSpacing = ImVec2(6.f, 6.f);
			style.TouchExtraPadding = ImVec2(0.f, 0.f);
			style.IndentSpacing = 21.f;
			style.ScrollbarSize = 15.f;
			style.GrabMinSize = 8.f;
			style.WindowBorderSize = 1.5f;
			style.ChildBorderSize = 1.5f;
			style.PopupBorderSize = 1.5f;
			style.FrameBorderSize = 0.f;
			style.ChildRounding = 0.f;
			style.GrabRounding = 0.f;
			style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
			style.DisplaySafeAreaPadding = ImVec2(3.f, 3.f);

			style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
			style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
			style.Colors[ImGuiCol_Button] = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
			style.Colors[ImGuiCol_Column] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
			style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
			style.Colors[ImGuiCol_CloseButton] = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);
			style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);
			style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
			style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

			ImGui::Begin(xorstr_(" Rouge Company - bright#1331"), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize); {
				
				ImGui::SetWindowSize(ImVec2(564, 347));
				ImGui::Checkbox(xorstr_("Aimbot"), &Settings.Aimbot);
				ImGui::Checkbox(xorstr_("ESP"), &Settings.ESP);
				ImGui::Checkbox(xorstr_("PlayerName"), &Settings.PlayerName);

				if (Settings.Aimbot) {
					ImGui::SliderFloat(xorstr_("FOV Circle"), &Settings.FOV, 100.0f, 1000.0f, "%.2f");
				}
			}

			ImGui::End();
		}

		ImGui::End();
		ImGui::Render();

		if (!status) {
			Core::Pawn = closestPawn = NULL;
		}
		else {
			Core::Pawn = closestPawn;
		}

		return PresentOriginal(swapChain, buffer, syncInterval);
	}

	VOID Init(HMODULE module) {
		
		if (!module) {
			Log::DebugText(xorstr_("Invalid Image Base."));
			return;
		}

		Log::DebugText(xorstr_("dxgi.dll Image Base -> 0x%x"), module);

		auto addr = Util::FindPattern(module,
			xorstr_(""),//Hello find sigsyour self no paster ezpz
			xorstr_(""));

		if (!addr) {
			Log::DebugText(xorstr_("Failed to find signature for -> IDXGISwapChain::Present."));
			return;
		}

		Log::DebugText(xorstr_("IDXGISwapChain::Present -> 0x%x"), addr);

		MH_CreateHook(addr, Present, (VOID**)&PresentOriginal);
		MH_EnableHook(addr);
	}
}
