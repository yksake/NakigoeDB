#include <Siv3D.hpp> // Siv3D v0.6.16
#include "Library.hpp"

void Main()
{
	Window::SetTitle(U"鳴き声DB");
	Window::SetStyle(WindowStyle::Sizable);
	Window::ResizeActual(DisplayResolution::HD_1280x720);
	Window::SetToggleFullscreenEnabled(false);
	Scene::SetResizeMode(ResizeMode::Actual);
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });

	App manager;
	manager
		.add<Library>(SceneState::Library)
		.init(SceneState::Library, 0);

	FontAsset::Register(U"Font", FontMethod::MSDF, 20);
	FontAsset::Register(U"FontMedium", FontMethod::MSDF, 20, Typeface::Medium);
	FontAsset::Register(U"FontEmoji", 20, Typeface::ColorEmoji);
	FontAsset::Register(U"FontIconMD", FontMethod::MSDF, 20, Typeface::Icon_MaterialDesign);

	const auto& emojiFont = FontAsset(U"FontEmoji");
	FontAsset(U"Font").addFallback(emojiFont);
	FontAsset(U"FontMedium").addFallback(emojiFont);

	while (System::Update())
	{
		if (not manager.update())
		{
			break;
		}
	}
}
