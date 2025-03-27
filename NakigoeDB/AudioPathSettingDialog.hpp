#pragma once
#include "BaseDialog.hpp"
#include "Button.hpp"
#include "CryData.hpp"

class AudioPathSettingDialog : public BaseDialog
{
public:
	AudioPathSettingDialog() = default;
	AudioPathSettingDialog(const std::weak_ptr<CryData>& cryData, const std::function<void()>& closeFunc);
	~AudioPathSettingDialog() = default;

	void update() override;
	void draw() const override;

	bool isActive() const override;
	void close() override;

private:
	bool m_isActive = true;
	std::weak_ptr<CryData> m_data;
	std::function<void()> m_closeFunc;

	FilePath m_path;
	bool m_isDeletePath = false;

	Button m_browseButton{ U"参照" };
	Button m_deleteButton{ U"登録解除" };
	Button m_okButton{ U"OK" };
	Button m_cancelButton{ U"キャンセル" };

	RoundRect region() const;
};
