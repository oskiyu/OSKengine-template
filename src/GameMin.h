#pragma once

#include <OSKengine/Game.h>

/// @brief Ejemplo mínimo de OSKengine.
/// Renderiza un fondo de colores y una imagen.
class GameMin : public OSK::IGame {

public:

	GameMin();

protected:

	void CreateWindow() override;
	void SetupEngine() override;
	void OnCreate() override;
	void OnTick_Keyboard(const OSK::IO::IKeyboardInput& kb) override;
	void OnWindowResize(const OSK::Vector2ui& res) override;

private:

	/// @brief Referencia a las imágenes del swapchain.
	OSK::DynamicArray<OSK::GRAPHICS::GdrImageUuid> m_swapchainImages;

};
