#pragma once

// 定数などの定義
#include "Definition/Alias/FWKCommonAlias.h"
#include "Definition/Constant/FWKCommonConstant.h"
#include "Definition/Struct/FWKCommonStruct.h"

#include "Utility/FileIO/FWKFileIOUtility.h"

// 安全なシングルトンを作成するための基底クラス
#include "Singleton/FWKSingletonBase.h"

// "Window"クラス
#include "Window/FWKWindow.h"

// "FPS"管理クラス
#include "FPS/FWKFPSController.h"

// 描画関係のデバイス
#include "Graphics/RTVHeap/FWKRTVHeap.h"
#include "Graphics/FWKGraphicsDevice.h"