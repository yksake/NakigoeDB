#include <ranges>
#include "TagPanelLoader.hpp"
#include "ParentTagItem.hpp"
#include "ChildTagItem.hpp"

namespace TagPanelLoader
{
	namespace CollapseState
	{
		const FilePath savePath = U"saves/TagPanelCollapseState.csv";

		bool Save(const Array<std::pair<String, bool>>& collapseState)
		{
			CSV csv;

			for (const auto& [id, isOpend] : collapseState)
			{
				if (not id)
				{
					continue;
				}

				csv.write(id);
				csv.write(Format(isOpend));

				csv.newLine();
			}

			return csv.save(savePath);
		}

		Array<std::pair<String, bool>> Load()
		{
			const CSV csv{ savePath };
			if (not csv)
			{
				return {};
			}

			Array<std::pair<String, bool>> collapseState;

			for (size_t row = 0; row < csv.rows(); ++row)
			{
				if (csv.columns(row) != 2)
				{
					continue;
				}

				String id = csv[row][0];
				bool isOpend = ParseBool(csv[row][1]);

				collapseState.push_back({ id, isOpend });
			}

			return collapseState;
		}
	}

	static String searchCustomName(const TagData& key, const Array<TagData>& list)
	{
		for (const auto& tag : list)
		{
			if (tag == key)
			{
				return tag.customName;
			}
		}

		return {};
	}

	Array<std::unique_ptr<TagItem>> Load(const Array<TagData>& basicTags, const Array<TagData>& userTags)
	{
		const Array<std::pair<String, bool>>& collapseState = CollapseState::Load();

		const FilePath path = U"config/TagPanel.txt";

		TextReader reader{ path };
		if (not reader)
		{
			return {};
		}

		Array<std::unique_ptr<TagItem>> panels;

		Array<std::unique_ptr<ParentTagItem>> parents;
		Array<String> categoryList;
		Array<String> collapseIDList;

		String line;
		while (reader.readLine(line))
		{
			line.trim();

			if (not line || line.starts_with(U';'))
			{
				continue;
			}

			if (line == U"/") // グループ終了
			{
				if (parents.size() == 1)
				{
					panels << std::move(parents.back());
				}
				else if (2 <= parents.size())
				{
					parents[parents.size() - 2]->addChild(std::move(parents.back()));
				}

				parents.pop_back();
				categoryList.pop_back();
				collapseIDList.pop_back();
			}
			else if (line.starts_with(U'/')) // グループ開始
			{
				TagData currentTag = TagData::Load(line.substr(1));
				currentTag.value.clear();

				if (not currentTag.category)
				{
					// 親グループのカテゴリIDを適用
					for (const auto& category : categoryList | std::views::reverse)
					{
						if (category)
						{
							currentTag.category = category;
							break;
						}
					}

					categoryList << U"";
				}
				else
				{
					// カテゴリIDをリストへ記録
					categoryList << currentTag.category;
				}

				// タグ一覧から表示名を適用
				if (not currentTag.customName)
				{
					currentTag.customName = searchCustomName(currentTag, basicTags);
				}
				if (not currentTag.customName)
				{
					currentTag.customName = searchCustomName(currentTag, userTags);
				}

				if (not currentTag.category && not currentTag.customName)
				{
					continue;
				}

				// 折りたたみ状態のIDを更新
				{
					String collapseID;

					if (currentTag.category)
					{
						collapseID += currentTag.category;
					}
					if (currentTag.customName)
					{
						collapseID += U'(' + currentTag.customName + U')';
					}

					if (collapseID)
					{
						collapseIDList << collapseID;
					}
				}

				parents << std::make_unique<ParentTagItem>(currentTag);

				// 折りたたみ状態を更新
				{
					String fullCollapseID;

					for (const auto& id : collapseIDList)
					{
						if (fullCollapseID)
						{
							fullCollapseID += U'.';
						}

						fullCollapseID += id;
					}

					for (const auto& [id, isOpend] : collapseState)
					{
						if ((id == fullCollapseID) && not isOpend)
						{
							parents.back()->close();
						}
					}
				}
			}
			else // グループ内
			{
				TagData currentTag = TagData::Load(line);
				if (not currentTag.category)
				{
					continue;
				}

				// 親グループのカテゴリIDを適用
				if (not currentTag.value)
				{
					if (not categoryList)
					{
						continue;
					}

					for (const auto& category : categoryList | std::views::reverse)
					{
						if (category)
						{
							currentTag.value = currentTag.category;
							currentTag.category = category;
							break;
						}
					}

					if (not currentTag.value)
					{
						continue;
					}
				}

				// タグ一覧から表示名を適用
				if (not currentTag.customName)
				{
					currentTag.customName = searchCustomName(currentTag, basicTags);
				}
				if (not currentTag.customName)
				{
					currentTag.customName = searchCustomName(currentTag, userTags);
				}

				if (parents)
				{
					parents.back()->addChild(std::make_unique<ChildTagItem>(currentTag));
				}
				else
				{
					panels << std::make_unique<ChildTagItem>(currentTag);
				}
			}
		}

		while (parents)
		{
			if (parents.size() == 1)
			{
				panels << std::move(parents.back());
			}
			else if (2 <= parents.size())
			{
				parents[parents.size() - 2]->addChild(std::move(parents.back()));
			}

			parents.pop_back();
		}

		return panels;
	}
}
