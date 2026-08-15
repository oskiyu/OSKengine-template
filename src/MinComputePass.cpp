#include "MinComputePass.h"

#include <OSKengine/OSKengine.h>
#include <glm/glm.hpp>

using namespace OSK::GRAPHICS;

MinComputePass::MinComputePass(GdrImageUuid targetImage) : IRenderPass("Resources/Materials/Testing/min_compute.json", RenderPassType::COMPUTE) {
	AddImageDependency(RgImageDependency::ComputeTarget(targetImage, "image", "image"));
}

void MinComputePass::Execute(OSK::GRAPHICS::ICommandList* cmdList) {
	const auto windowRec = OSK::Engine::GetDisplay()->GetResolution();

	cmdList->PushMaterialConstants("time", OSK::Engine::GetCurrentTime());

	cmdList->DispatchCompute({
		(OSK::USize32)glm::ceil(windowRec.x / 16.0f),
		(OSK::USize32)glm::ceil(windowRec.y / 16.0f), 1 });
}

std::string_view MinComputePass::GetName() const {
	return "MinComputePass";
}
