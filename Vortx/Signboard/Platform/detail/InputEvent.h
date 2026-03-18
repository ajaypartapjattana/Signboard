#include <vector>
#include <string>

#include "Signboard/Core/Time/Time.h"

namespace platform::detail {

	class InputEvent {
	public:
		enum class Type {
			KEY_BUTTON,
			MOUSE_BUTTON,
			CURSOR_MOVE,
			SCROLL,
			FILEDROP,
			RESIZE
		};

		static InputEvent Key(int key, int scanCode, int action, int mods) {
			InputEvent e(Type::KEY_BUTTON);
			e.m_data.key = { key, scanCode, action, mods };
			return e;
		}

		static InputEvent MouseButton(int button, int action, int mods) {
			InputEvent e(Type::MOUSE_BUTTON);
			e.m_data.mouseButton = { button, action, mods };
			return e;
		}

		static InputEvent CursorMove(double x, double y) {
			InputEvent e(Type::CURSOR_MOVE);
			e.m_data.cursorMove = { x, y };
			return e;
		}

		static InputEvent Scroll(double xOffset, double yOffset) {
			InputEvent e(Type::SCROLL);
			e.m_data.scroll = { xOffset, yOffset };
			return e;
		}

		static InputEvent FileDrop(int count, const char** paths) {
			InputEvent e(Type::FILEDROP);

			e.m_data.fileDrop.count = count;
			e.m_filePaths.reserve(count);

			for (int i = 0; i < count; ++i) {
				e.m_filePaths.emplace_back(paths[i]);
			}

			return e;
		}

		static InputEvent FramebufferResize(int width, int height) {
			InputEvent e(Type::RESIZE);
			e.m_data.resize = { width, height };
			return e;
		}

		Type type() const {
			return m_type;
		}

		const auto& key() const {
			return m_data.key;
		}

		const auto& mouseButton() const {
			return m_data.mouseButton;
		}

		const auto& cursorMove() const {
			return m_data.cursorMove;
		}

		const auto& scroll() const {
			return m_data.scroll;
		}

		const auto& filePaths() const {
			return m_filePaths;
		}

	private:
		explicit InputEvent(Type type)
			: 
			m_type(type),
			m_timestamp(Time::now_ns())
		{

		}

	private:
		Type m_type;

		uint64_t m_timestamp;

		union Data{

			struct {
				int key;
				int scancode;
				int action;
				int mods;
			} key;

			struct {
				int button;
				int action;
				int mods;
			} mouseButton;

			struct {
				double x;
				double y;
			} cursorMove;

			struct {
				double xOffset;
				double yOffset;
			} scroll;

			struct {
				int count;
			} fileDrop;

			struct {
				int width;
				int height;
			} resize;

			Data() = default;
			~Data() = default;
			
		} m_data;

		std::vector<std::string> m_filePaths;

	};

}