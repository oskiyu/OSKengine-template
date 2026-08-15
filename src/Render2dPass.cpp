#include "Render2dPass.h"

#include <OSKengine/OSKengine.h>
#include <OSKengine/Logger.h>

#include <glm/glm.hpp>

using namespace OSK::GRAPHICS;

Render2dPass::Render2dPass(GdrImageUuid targetImage) : ISdfRenderPass() {
	m_img = OSK::Engine::GetAssetManager()->Load<OSK::ASSETS::Texture>("Resources/Assets/Textures/engine_logo.json");

	auto imageDependency = RgImageDependency::ColorTarget(targetImage);
	imageDependency.clearIfTargetImage = false;
	AddImageDependency(imageDependency);
}

void Render2dPass::Execute(OSK::GRAPHICS::ICommandList* cmdList) {

	const auto& windowRec = OSK::Engine::GetDisplay()->GetResolution();
	auto* sdfRenderer = GetSdfRenderer();

	sdfRenderer->Begin(cmdList);
	sdfRenderer->SetCamera(OSK::Vector2f::Zero, windowRec.ToVector2f());

	auto drawCall = SdfDrawCall2D::Texture(m_img->GetTextureView2D());
	drawCall.transform.SetPosition(OSK::Vector2f((windowRec.x - m_img->GetSize().x) / 2.0f, (windowRec.y - m_img->GetSize().y) / 2.0f));
	drawCall.transform.SetScale(m_img->GetSize().ToVector2f());

	sdfRenderer->Draw(drawCall);

	sdfRenderer->End(); // ERR
}

std::string_view Render2dPass::GetName() const {
	return "Render2dPass";
}
