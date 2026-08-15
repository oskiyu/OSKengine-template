#pragma once

#include <OSKengine/RenderPass.h>

class MinComputePass : public OSK::GRAPHICS::IRenderPass {

public:

	explicit MinComputePass(OSK::GRAPHICS::GdrImageUuid targetImage);

	void Execute(OSK::GRAPHICS::ICommandList* cmdList) override;

	std::string_view GetName() const override;

};
