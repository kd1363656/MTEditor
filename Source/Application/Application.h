#pragma once

class Application final : public FWK::SingletonBase<Application>
{
public:

	void Execute    ();
	void EndGameLoop();

	HWND GetHWND() const { return m_window.GetHWND(); }

	int GetNowFPS() const { return m_fpsController.GetNowFPS(); }

private:

	bool Init(const FWK::CommonStruct::Dimension2D& a_size);

	void LoadWindowSize();
	void SaveWindowSize();

	void Release();

	void UpdateWindowTitleBar() const;

	std::string GenerateWindowTitleText() const;

	static constexpr std::string_view k_titleName = "MTEditor";

	const std::string k_windowSizeFileIOPath = "Asset/Data/Setting/Window/WindowConfig.json";

	FWK::Window m_window = {};

	FWK::FPSController m_fpsController = {};

	FWK::CommonStruct::Dimension2D m_windowSize = { FWK::CommonConstant::k_defaultWindowWidth , FWK::CommonConstant::k_defaultWindowHeight};

	bool m_isEndGameLoop = false;

	// ===================
	// ƒVƒ“ƒOƒ‹ƒgƒ“
	// ===================
	friend class FWK::SingletonBase<Application>;

	Application ()          = default;
	~Application() override = default;
};