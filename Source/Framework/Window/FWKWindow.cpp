#include "FWKWindow.h"

bool FWK::Window::Create(const FWK::CommonStruct::Dimension2D& a_size , const std::string& a_titleName , const std::string& a_windowClassName)
{
	// 現在の実行ファイル(.exe)のハンドルを取得
	HINSTANCE l_hInst = GetInstanceHandle();

	// "std::string"型を"std::wstring"型に変換
	// 変換する理由はウィンドウクラス名に文字列を渡す際に"std::wstring"でないと型エラーが発生するため
	std::wstring l_wndClassName = sjis_to_wide(a_windowClassName);
	
	// ウィンドウクラスの定義
	WNDCLASSEX l_wc = {};

	l_wc.cbSize        = sizeof(WNDCLASSEX);						// 構造体のサイズを設定
	l_wc.style         = k_windowStyleDefault;					// ウィンドウの動作スタイル(サイズ変更時に再描画など)			
	l_wc.lpfnWndProc   = &Window::CallWindowProcedure;			// 関数を指定し指定した関数がウィンドウに届いたすべてのメッセージを処理
	l_wc.cbClsExtra    = k_classExtraBytes;						// ウィンドウクラス用に追加で確保するメモリサイズ
	l_wc.cbWndExtra    = k_windowExtraBytes;						// ウィンドウごとに追加で確保するメモリサイズ
	l_wc.hInstance     = l_hInst;								// アプリケーションのインスタンスハンドル
	l_wc.hIcon         = LoadIcon  (nullptr , IDI_APPLICATION); // ウィンドウに表示するアイコン
	l_wc.hIconSm       = LoadIcon  (nullptr , IDI_APPLICATION); // タスクバーやタイトルバーに表示されるアイコン
	l_wc.hCursor       = LoadCursor(nullptr , IDC_ARROW);		// ウィンドウ上で使うマウスカーソルを指定
	l_wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// ウィンドウの背景色を指定
	l_wc.lpszMenuName  = nullptr;								// メニューリソースの名前
	l_wc.lpszClassName = l_wndClassName.c_str();				// ウィンドウクラス名

	// ウィンドウクラスを登録
	// 戻り値は登録されているクラスを一意に識別するクラスアトム
	// (クラスアトムは文字列と対応する識別子を格納するシステム定義テーブル)
	// 失敗した場合戻り値は"0"つまり"false"と同等の評価
	if (!RegisterClassEx(&l_wc))
	{
		return false;
	}

	std::wstring l_titleName = sjis_to_wide(a_titleName);

	// ウィンドウの作成
	m_hWND = CreateWindow(l_wndClassName.c_str()              ,	// 登録済みのウィンドウクラス名
						  l_titleName.c_str()				  , // ウィンドウのタイトル
						  WS_OVERLAPPEDWINDOW - WS_THICKFRAME , // 標準ウィンドウからサイズ変更用の太い枠を除いたスタイル
						  k_defaultWindowPosX				  , // ウィンドウ座標"X"
						  k_defaultWindowPosY				  , // ウィンドウ座標"Y"
						  a_size.width						  , // クライアント領域の幅
						  a_size.height						  , // クライアント領域の高さ
						  nullptr							  , // 親ウィンドウがない
					      nullptr							  , // メニューなし
						  l_hInst							  , // アプリケーションのインスタンスハンドル
						  this);								// 作製するウィンドウに"this"ポインタを渡す、"WM_CREATE"などで取り出せる
	
	// ウィンドウの作製が上手くいっていなければ"false"
	if (!m_hWND)
	{
		// メモリリーク防止
		UnregisterClass(l_wndClassName.c_str() , l_hInst);
		return false;
	}
	
	// クライアントのサイズを設定
	SetClientSize(a_size);

	// ウィンドウの表示を設定
	ShowWindow(m_hWND , SW_SHOW);
	// ウィンドウのクライアント領域をすぐに更新(クライアント領域をすぐに塗りつぶす)
	UpdateWindow(m_hWND);

	// "timeGetTime"関数の精度を"1ms"に設定
	timeBeginPeriod(k_timerResolutionMS);

	return true;
}

bool FWK::Window::ProcessMessage() const
{
	// メッセージ受け取り用構造体
	MSG l_msg = {};

	// メッセージキューからメッセージを取得
	while (PeekMessage(&l_msg               ,  // メッセージを格納する構造体へのポインタ
						nullptr             ,  // 対象のウィンドウハンドル("nullptr"なら全ウィンドウ)
						k_messageFilterNone ,  // 最小メッセージ"ID"(フィルターの下限)
						k_messageFilterNone ,  // 最大メッセージ"ID"(フィルターの上限)
						PM_REMOVE))			   // 取得後にメッセージキューから削除するかどうか
	{
		// 終了メッセージが来たかどうか
		if (l_msg.message == WM_QUIT)
		{
			return false;
		}

		// メッセージ処理
		TranslateMessage(&l_msg);
		DispatchMessage (&l_msg);
	}

	return false;
}

void FWK::Window::Release()
{
	// もしウィンドウハンドルがインスタンス化されていたら解放
	if (!m_hWND) { return; }

	// 指定したウィンドウを破棄
	DestroyWindow(m_hWND);
	m_hWND = nullptr;
}

void FWK::Window::SetClientSize(const FWK::CommonStruct::Dimension2D& a_size)
{
	if (!m_hWND) { return; }

	RECT l_rcWND    = {};
	RECT l_rcClient = {};

	// ウィンドウ全体のサイズ、クライアント領域のサイズを取得
	GetWindowRect(m_hWND , &l_rcWND);
	GetClientRect(m_hWND , &l_rcClient);

	// インスタンスハンドルからウィンドウの位置と寸法を変更
	MoveWindow(m_hWND																			       , // 対象のウィンドウ 
			   l_rcWND.left																		       , // 左上の"X"座標
			   l_rcWND.top																		       , // 左上の"Y"座標
			   a_size.width  + (l_rcWND.right  - l_rcWND.left) - (l_rcClient.right  - l_rcClient.left) , // ウィンドウ全体の幅(枠含む)
			   a_size.height + (l_rcWND.bottom - l_rcWND.top)  - (l_rcClient.bottom - l_rcClient.top)  , // ウィンドウ全体の高さ(枠含む)
			   TRUE);																					 // ウィンドウを再描画するかどうかもし"TRUE"ならウィンドウはメッセージを受信
}

LRESULT CALLBACK FWK::Window::CallWindowProcedure(const HWND   a_hWND    , 
												  const UINT   a_message , 
												  const WPARAM a_wParam  , 
												  const LPARAM a_lParam)
{
	auto* l_this = static_cast<Window*>(GetProp(a_hWND , L"GameWindowInstance"));

	// "nullptr"の場合はデフォルト処理を実行
	if (!l_this)
	{
		if (a_message == WM_CREATE)
		{
			// "CreateWindow"で渡したパラメータを取得
			auto* l_createStruct = (CREATESTRUCT*)a_lParam;
			auto* l_window       = (Window*)l_createStruct->lpCreateParams;

			// ウィンドウプロパティにこのクラスのインスタンスアドレスを埋め込んでおく
			// 次回から、"l_this->WindowProcedure"の方へ処理が流れていく
			SetProp(a_hWND , L"GameWindowInstance" , l_window);

			return k_messageHandled;
		}
		else
		{
			return DefWindowProc(a_hWND    , 
								 a_message ,
								 a_wParam  ,
								 a_lParam);	
		}
	}

	// インスタンス側の"Window"関数を実行する
	return l_this->WindowProcedure(a_hWND    ,
								   a_message ,
								   a_wParam  ,
								   a_lParam);
}

LRESULT FWK::Window::WindowProcedure(const HWND   a_hWND    , 
								     const UINT   a_message , 
									 const WPARAM a_wParam  , 
									 const LPARAM a_lParam)
{
	// メッセージによって処理を選択
	switch(a_message)
	{
		// ウィンドウの"X"ボタンが押された
		case WM_CLOSE:
		{
			Release();
		}
		break;

		// ウィンドウ破棄直前
		case WM_DESTROY:
		{
			RemoveProp(a_hWND , L"GameWindowInstance");
			
			// "OS"に対してこのアプリの終了を伝える
			PostQuitMessage(0);
		}
		break;

		default:
			// メッセージのデフォルト処理
			return DefWindowProc(a_hWND    ,
								 a_message ,
								 a_wParam  ,
								 a_lParam);
		break;
	}

	return 0;
}