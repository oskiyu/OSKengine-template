#include "GameMin.h"

#include <OSKengine/IKeyboardInput.h>
#include <OSKengine/RenderGraph.h>
#include <OSKengine/RenderToSwapchainPass.h>

#include "MinComputePass.h"
#include "Render2dPass.h"

#include <OSKengine/IFullscreenableDisplay.h>
#include <OSKengine/DefineIuuid.h>

#include <OSKengine/AssetLoaderJob.h>


GameMin::GameMin() : OSK::IGame(OSK::GAME::DefaultContentProfile::MINIMAL) {

}

void GameMin::CreateWindow() {
	OSK::Engine::GetDisplay()->Create({ 1800u, 900u }, "GameMin");
}

void GameMin::SetupEngine() {
	OSK::Engine::GetRenderer()->Initialize(
		"GameMin",
		OSK::Version{ .mayor = 1, .menor = 0, .parche = 0 },
		*OSK::Engine::GetDisplay(),
		OSK::GRAPHICS::PresentMode::VSYNC_ON);
}

void GameMin::OnCreate() {
	OSK::IGame::OnCreate();
	OSK::Engine::GetJobSystem()->WaitForJobs<OSK::ASSETS::AssetLoaderJob>();

	auto renderGraph = OSK::MakeUnique<OSK::GRAPHICS::RenderGraph>(OSK::Engine::GetRenderer());
	renderGraph->SetFramebufferResolution(OSK::Engine::GetDisplay()->GetResolution());

	m_swapchainImages.Resize(OSK::Engine::GetRenderer()->GetSwapchainImagesCount());
	for (OSK::UIndex16 i = 0; i < m_swapchainImages.GetSize(); i++) {
		auto* img = OSK::Engine::GetRenderer()->_GetSwapchain()->GetImage(i);
		m_swapchainImages[i] = renderGraph->RegisterExternalImage(img);
	}

	OSK::GRAPHICS::RgImageRegisterInfo target{};
	target.numLayers = 1;
	target.dimension = OSK::GRAPHICS::GpuImageDimension::d2D;
	target.format = OSK::GRAPHICS::Format::RGBA8_UNORM;
	target.name = "Target Image";
	const auto targetUuid = renderGraph->RegisterImage(target, OSK::GRAPHICS::RgRelativeSizeImageRegisterArgs::From2D(OSK::Vector2f::One));

	renderGraph->RegisterRenderpass(MakeUnique<MinComputePass>(targetUuid), OSK::GRAPHICS::RenderPassDependencies::Empty());
	renderGraph->RegisterRenderpass(MakeUnique<Render2dPass>(targetUuid), OSK::GRAPHICS::RenderPassDependencies::After({ "MinCompute" }));
	renderGraph->RegisterRenderpass(MakeUnique<OSK::GRAPHICS::RenderToSwapchainPass>(targetUuid, m_swapchainImages.GetFullSpan()), OSK::GRAPHICS::RenderPassDependencies::After({ "Render2dPass" }));

	renderGraph->Compile();

	SetRenderGraph(std::move(renderGraph));
}

void GameMin::OnTick_Keyboard(const OSK::IO::IKeyboardInput& kb) {
	if (kb.IsKeyDown(OSK::IO::Key::ESCAPE)) {
		Exit();
	}

	if (kb.IsKeyStroked(OSK::IO::Key::F11)) {
		OSK::IO::IFullscreenableDisplay* display = nullptr;
		OSK::Engine::GetDisplay()->QueryInterface(OSK_IUUID(OSK::IO::IFullscreenableDisplay), (void**)&display);

		if (display) {
			display->ToggleFullscreen();
		}
	}
}

void GameMin::OnWindowResize(const OSK::Vector2ui& res) {
	auto* renderGraph = GetRenderGraph();

	for (OSK::UIndex16 i = 0; i < m_swapchainImages.GetSize(); i++) {
		auto* img = OSK::Engine::GetRenderer()->_GetSwapchain()->GetImage(i);
		renderGraph->RebindExternalImage(m_swapchainImages[i], img);
	}

	renderGraph->SetFramebufferResolution(res);
	renderGraph->RecreateRelativeImages();
}
