#pragma once
#include "BaseDialog.hpp"
#include "RadioButtons.hpp"
#include "Button.hpp"

class AddFilesDialog : public BaseDialog
{
public:
	AddFilesDialog() = default;
	AddFilesDialog(const std::function<void(size_t)>& closeFunc);
	~AddFilesDialog() = default;

	void update() override;
	void draw() const override;

	bool isActive() const override;
	void close() override;

private:
	bool m_isActive = true;
	std::function<void(size_t)> m_closeFunc;

	RadioButtons m_radioButtons{ { U"旧鳴き声を登録", U"新鳴き声を登録", U"非鳴き声を追加" } };

	Button m_okButton{ U"OK" };
	Button m_cancelButton{ U"キャンセル" };

	RoundRect region() const;
};
