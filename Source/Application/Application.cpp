#include "Application.h"

// ========================================================================
// エントリーポイント
// プログラムを実行するうえでプログラムやサブルーチンの実行を開始する場所
// ========================================================================
int WINAPI WinMain(_In_     HINSTANCE ,
				   _In_opt_ HINSTANCE ,
				   _In_     LPSTR     ,
				   _In_     int)
{
	// メモリリークを知らせる(デバックビルド時のみ有効)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// "COM"初期化
	if (FAILED(CoInitializeEx(nullptr , COINIT_MULTITHREADED)))
	{
		CoUninitialize();
		return 0;
	}

	// mbstowcs_s関数で日本語対応にするために呼ぶ
	setlocale(LC_ALL , "japanese");

	// アプリケーション実行
	Application::GetInstance().Execute();

	// "COM"解放
	CoUninitialize();
	return 0;
}

void Application::Execute()
{
	// ウィンドウのサイズを読み取る
	LoadWindowSize();

	// 初期設定(ウィンドウ作製)
	if (!Application::GetInstance().Init(m_windowSize))
	{
		return;
	}

	// ゲームループ
	while(true)
	{
		// "FPS"の計測
		m_fpsController.UpdateStartTime();

		// ウィンドウメッセージの処理
		m_window.ProcessMessage();

		// ウィンドウが破棄されているか"Escape"キーが押されていたらゲームループ終了
		if (!m_window.IsCreated() || GetAsyncKeyState(VK_ESCAPE))
		{
			EndGameLoop();
		}

		// もしゲームループ終了フラグが立っていたら"break"
		if (m_isEndGameLoop) { break; }

		// フレームレート制御
		m_fpsController.Update();

		// タイトルバーの更新
		UpdateWindowTitleBar();
	}

	// ウィンドウの解像度を保存(ウィンドウサイズの設定は保存しておくべきだから)
	SaveWindowSize();

	// アプリケーション開放
	Release();
}
void Application::EndGameLoop()
{
	m_isEndGameLoop = true;
}

bool Application::Init(const FWK::CommonStruct::Dimension2D& a_size)
{
	HWND l_hWND = GetHWND();

	// タイトル名 + "FPS"の表示
	const std::string& l_titleBar = GenerateWindowTitleText();
	SetWindowTextA									       (l_hWND , l_titleBar.c_str());

	if (!m_window.Create(a_size , l_titleBar , "Window"))
	{
		MessageBoxA(nullptr				   ,
					"ウィンドウ作成に失敗" ,
					"エラー"			   ,
					MB_OK);

		return false;
	}

	// グラフィックスデバイスの初期化
	if (!FWK::Graphics::GraphicsDevice::GetInstance().Init(GetHWND() , a_size))
	{
		assert(false && "グラフィックスデバイス初期化失敗。");
		return false ;
	}

	// フルスクリーン
	bool l_isFullScreen = false;
	/*if (MessageBoxA(l_hWND                       , 
					"フルスクリーンにしますか？" , 
					"確認"						 , 
					 MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) 
	{
		l_isFullScreen = true;
	}*/

	// "FPSController"の初期化
	m_fpsController.Init();

	return true;
}

void Application::LoadWindowSize()
{
	const auto& l_rootJson = FWK::FileIOUtility::LoadJsonFile(k_windowSizeFileIOPath);
	if (l_rootJson.is_null()) { return; }

	m_windowSize.height = l_rootJson.value("Height" , FWK::CommonConstant::k_defaultWindowHeight);
	m_windowSize.width  = l_rootJson.value("Width"  , FWK::CommonConstant::k_defaultWindowWidth);
}
void Application::SaveWindowSize()
{
	auto l_rootJson = nlohmann::json();

	l_rootJson["Height"] = m_windowSize.height;
	l_rootJson["Width"] = m_windowSize.width;

	FWK::FileIOUtility::SaveJsonFile(l_rootJson, k_windowSizeFileIOPath);
}

void Application::Release()
{
	// ウィンドウ開放
	m_window.Release();
}

void Application::UpdateWindowTitleBar() const
{
	// タイトル名 + "FPS"の表示
	// 一時オブジェクトは"const"参照で有効期限を延ばせる
	const std::string& l_titleBar = GenerateWindowTitleText();
	SetWindowTextA                                         (GetHWND() , l_titleBar.c_str());
}

std::string Application::GenerateWindowTitleText() const
{
	return std::format("{} : {}", k_titleName , GetNowFPS());
}