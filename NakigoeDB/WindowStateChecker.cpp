#include "WindowStateChecker.hpp"

namespace WindowStateChecker
{
	Point pos = Point::Zero();
	Size size = Size::Zero();
	bool isMaximized = false;

	bool isSizeMoving = false;

	const FilePath savePath = U"saves/WindowState.json";
	const int8 saveVersion = 1;

	static bool save()
	{
		JSON json;

		json[U"JSONVersion"] = saveVersion;

		json[U"Window"][U"Pos"] = pos;
		json[U"Window"][U"Size"] = size;
		json[U"Window"][U"Maximized"] = isMaximized;

		return json.save(savePath);
	}

	bool Load()
	{
		// 初期化
		{
			const auto& state = Window::GetState();

			pos = Window::GetPos();
			size = state.virtualSize;
			isMaximized = state.maximized;
		}

		if (not FileSystem::IsFile(savePath))
		{
			return save();
		}

		const JSON json = JSON::Load(savePath);
		if (not json)
		{
			return save();
		}

		// WindowState.json のバージョン
		const int8 version = json[U"JSONVersion"].get<int8>();
		if (saveVersion != version)
		{
			return false;
		}

		pos = json[U"Window"][U"Pos"].get<Point>();
		size = json[U"Window"][U"Size"].get<Size>();
		isMaximized = json[U"Window"][U"Maximized"].get<bool>();

		// バリデーションチェック
		{
			const size_t currentMonitorIndex = System::GetCurrentMonitorIndex();
			const MonitorInfo currentMonitor = System::EnumerateMonitors()[currentMonitorIndex];
			const auto& workArea = currentMonitor.workArea;

			if (pos.x <= workArea.leftX() - size.x)
			{
				pos.x = workArea.leftX();
			}
			else if (workArea.rightX() <= pos.x)
			{
				pos.x = workArea.rightX() - size.x;
			}

			if (pos.y < workArea.topY())
			{
				pos.y = workArea.topY();
			}
			else if (workArea.bottomY() <= pos.y + Window::GetState().titleBarHeight)
			{
				pos.y = workArea.bottomY() - Window::GetState().titleBarHeight;
			}

			if ((size.x <= 0) || (size.y <= 0))
			{
				size = DisplayResolution::HD_1280x720;
			}
			else if ((workArea.w * 2 <= size.x) || (workArea.h * 2 <= size.y))
			{
				size = DisplayResolution::HD_1280x720;
			}
		}

		Window::SetPos(pos);
		Window::ResizeActual(size, Centering::No);
		if (isMaximized)
		{
			Window::Maximize();
		}

		return true;
	}

	void Update()
	{
		const auto& state = Window::GetState();

		if (state.sizeMove)
		{
			isSizeMoving = true;
			return;
		}

		if (not state.sizeMove && isSizeMoving)
		{
			bool isUpdated = false;

			if (isMaximized != state.maximized)
			{
				isMaximized = state.maximized;
				isUpdated = true;
			}

			if (not isMaximized)
			{
				if (pos != Window::GetPos())
				{
					pos = Window::GetPos();
					isUpdated = true;
				}

				if (size != state.virtualSize)
				{
					size = state.virtualSize;
					isUpdated = true;
				}
			}

			if (isUpdated)
			{
				save();
			}
		}
	}
}
