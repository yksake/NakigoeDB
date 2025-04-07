#pragma once
#include "BaseDialog.hpp"
#include "TagData.hpp"
#include "PulldownMenu.hpp"
#include "Button.hpp"
#include "Margin.hpp"

class TypeFilteringDialog : public BaseDialog
{
public:
	TypeFilteringDialog(const Optional<TagData>& filter, const std::function<void(Optional<TagData>)>& closeFunc);
	~TypeFilteringDialog() = default;

	void update() override;
	void draw() const override;

	bool isActive() const override;
	void close() override;

private:
	bool m_isActive = true;
	std::function<void(Optional<TagData>)> m_closeFunc;

	static const Array<String> Categories;
	static const Array<String> Types;

	PulldownMenu m_categoryPulldown;
	PulldownMenu m_typePulldown;

	Button m_okButton{ U"OK" };
	Button m_clearButton{ U"絞り込み解除" };
	Button m_cancelButton{ U"キャンセル" };

	RoundRect region() const;
};
