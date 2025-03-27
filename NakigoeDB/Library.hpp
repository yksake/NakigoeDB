#pragma once
#include "Common.hpp"
#include "MenuBar.hpp"
#include "FilePanel.hpp"
#include "TagPanel.hpp"
#include "AudioPlayer.hpp"
#include "BaseDialog.hpp"
#include "ItemRightClickMenu.hpp"
#include "DraggedFilesIcon.hpp"

class Library : public App::Scene
{
public:
	Library(const InitData& init);

	void update() override;
	void draw() const override;

private:
	MenuBar m_menubar;

	FilePanel m_filePanel;

	double m_sidePanelWidth = 250.0;
	TagPanel m_tagPanel;
	AudioPlayer m_audioPlayer;

	std::unique_ptr<BaseDialog> m_dialog;

	// ピカブイタグの表示オプション
	// 0: 第7世代に含める
	// 1: 他世代に含めない
	// 2: 第8世代に含める
	size_t m_lpleDisplayOption = 0;
	// Legendsアルセウスタグの表示オプション
	// 0: 第8世代に含める
	// 1: 他世代に含めない
	// 2: 第9世代に含める
	size_t m_laDisplayOption = 0;

	std::unique_ptr<ItemRightClickMenu> m_rightClickMenu;

	std::unique_ptr<DraggedFilesIcon> m_dragIcon;

	void updateMenuBar();
	void registerFile(const int32 cryVer);

	std::pair<Array<TagData>, Array<TagData>> separateFilters(const Array<TagData>& filters) const;

	RectF fileArea() const;
	RectF tagArea() const;
};
