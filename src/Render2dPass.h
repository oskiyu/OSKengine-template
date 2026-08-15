#pragma once

#include <OSKengine/ISdfRenderPass.h>
#include <OSKengine/AssetRef.h>
#include <OSKengine/Texture.h>

class Render2dPass : public OSK::GRAPHICS::ISdfRenderPass {

public:

	explicit Render2dPass(OSK::GRAPHICS::GdrImageUuid targetImage);

	void Execute(OSK::GRAPHICS::ICommandList* cmdList) override;

	std::string_view GetName() const override;

private:

	OSK::ASSETS::AssetRef<OSK::ASSETS::Texture> m_img;

};
