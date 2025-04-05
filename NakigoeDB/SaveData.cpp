#include "SaveData.hpp"

namespace NakigoeDB
{
	const FilePath SaveData::SavePath = U"saves/Save.json";

	bool SaveData::save() const
	{
		JSON json;

		json[U"JSONVersion"] = SaveVersion;

		json[U"Window"][U"Pos"] = m_windowPos;
		json[U"Window"][U"Size"] = m_windowSize;
		json[U"Window"][U"Maximized"] = m_isMaximized;

		json[U"DisplayGen"][U"LPLE"] = m_lpleDisplayGen;
		json[U"DisplayGen"][U"LA"] = m_laDisplayGen;

		return json.save(SavePath);
	}

	bool SaveData::load()
	{
		// 初期化
		{
			const auto& state = Window::GetState();

			m_windowPos = Window::GetPos();
			m_windowSize = state.virtualSize;
			m_isMaximized = state.maximized;
		}

		if (not FileSystem::IsFile(SavePath))
		{
			return save();
		}

		const JSON json = JSON::Load(SavePath);
		if (not json)
		{
			return save();
		}

		// Save.json のバージョン
		const int8 version = json[U"JSONVersion"].get<int8>();
		if (SaveVersion != version)
		{
			return false;
		}

		m_windowPos = json[U"Window"][U"Pos"].get<Point>();
		m_windowSize = json[U"Window"][U"Size"].get<Size>();
		m_isMaximized = json[U"Window"][U"Maximized"].get<bool>();

		m_lpleDisplayGen = json[U"DisplayGen"][U"LPLE"].getString();
		m_laDisplayGen = json[U"DisplayGen"][U"LA"].getString();

		// バリデーションチェック
		{
			const size_t currentMonitorIndex = System::GetCurrentMonitorIndex();
			const MonitorInfo currentMonitor = System::EnumerateMonitors()[currentMonitorIndex];
			const auto& workArea = currentMonitor.workArea;

			// ウィンドウの X 座標
			if (m_windowPos.x <= workArea.leftX() - m_windowSize.x)
			{
				m_windowPos.x = workArea.leftX();
			}
			else if (workArea.rightX() <= m_windowPos.x)
			{
				m_windowPos.x = workArea.rightX() - m_windowSize.x;
			}

			// ウィンドウの Y 座標
			if (m_windowPos.y < workArea.topY())
			{
				m_windowPos.y = workArea.topY();
			}
			else if (workArea.bottomY() <= m_windowPos.y + Window::GetState().titleBarHeight)
			{
				m_windowPos.y = workArea.bottomY() - Window::GetState().titleBarHeight;
			}

			// ウィンドウサイズ
			if ((m_windowSize.x <= 0) || (m_windowSize.y <= 0))
			{
				m_windowSize = DisplayResolution::HD_1280x720;
			}
			else if ((workArea.w * 2 <= m_windowSize.x) || (workArea.h * 2 <= m_windowSize.y))
			{
				m_windowSize = DisplayResolution::HD_1280x720;
			}
		}

		Window::SetPos(m_windowPos);
		Window::ResizeActual(m_windowSize, Centering::No);
		if (m_isMaximized)
		{
			Window::Maximize();
		}

		return true;
	}


	void SaveData::update()
	{
		const auto& state = Window::GetState();

		if (state.sizeMove)
		{
			m_isSizeMoving = true;
			return;
		}

		if (not state.sizeMove && m_isSizeMoving)
		{
			bool isUpdated = false;

			if (m_isMaximized != state.maximized)
			{
				m_isMaximized = state.maximized;
				isUpdated = true;
			}

			if (not m_isMaximized)
			{
				if (m_windowPos != Window::GetPos())
				{
					m_windowPos = Window::GetPos();
					isUpdated = true;
				}

				if (m_windowSize != state.virtualSize)
				{
					m_windowSize = state.virtualSize;
					isUpdated = true;
				}
			}

			if (isUpdated)
			{
				save();
			}
		}
	}


	String SaveData::getLPLEDisplayGen() const
	{
		return m_lpleDisplayGen;
	}

	String SaveData::getLADisplayGen() const
	{
		return m_laDisplayGen;
	}

	bool SaveData::setDisplayGen(const StringView lple, const StringView la)
	{
		if ((m_lpleDisplayGen == lple) && (m_laDisplayGen == la))
		{
			return false;
		}

		m_lpleDisplayGen = lple;
		m_laDisplayGen = la;

		return save();
	}
}
