#pragma once

class Application final : public FWK::SingletonBase<Application>
{
public:



private:

	// ===================
	// ƒVƒ“ƒOƒ‹ƒgƒ“
	// ===================
	friend class FWK::SingletonBase<Application>;

	Application ()          = default;
	~Application() override = default;
};