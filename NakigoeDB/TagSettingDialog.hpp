#pragma once
#include "BaseDialog.hpp"
#include "RadioButtons.hpp"
#include "Button.hpp"
#include "Margin.hpp"

class TagSettingDialog : public BaseDialog
{
public:
	TagSettingDialog() = default;
	TagSettingDialog(const StringView lpleDisplayGen, const StringView laDisplayOption, const std::function<void(StringView, StringView)>& closeFunc);
	~TagSettingDialog() = default;

	void update() override;
	void draw() const override;

	bool isActive() const override;
	void close() override;

private:
	bool m_isActive = true;
	std::function<void(StringView, StringView)> m_closeFunc;

	static constexpr double TitleHeight = 30.0;
	static constexpr Margin TextMargin = { 20.0, 10.0 };

	RadioButtons m_lpleOptions{ { U"第7世代に含める", U"第8世代に含める", U"他世代に含めない" } };
	RadioButtons m_legendsArceusOptions{ { U"第8世代に含める", U"第9世代に含める", U"他世代に含めない" } };

	Button m_okButton{ U"OK" };
	Button m_cancelButton{ U"キャンセル" };

	RoundRect region() const;
};
