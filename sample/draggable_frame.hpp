#pragma once

#include "frame.hpp"

class DraggableFrame final : public Frame {
	public:
		static std::shared_ptr<DraggableFrame> create();

		bool handle_mouse_button(UIContainer&, int button, int action, int mods, double mouseX,
				double mouseY) override;

		bool handle_mouse_move(UIContainer&, double mouseX, double mouseY) override;

		void handle_focus_lost(UIContainer&) override;
	private:
		float m_clickX{};
		float m_clickY{};
		bool m_dragging{};
};
