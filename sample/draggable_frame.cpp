#include "draggable_frame.hpp"

#include <GLFW/glfw3.h>

std::shared_ptr<DraggableFrame> DraggableFrame::create() {
	return std::make_shared<DraggableFrame>();
}

bool DraggableFrame::handle_mouse_button(UIContainer&, int button, int action, int mods, double mouseX,
		double mouseY) {
	if (button == GLFW_MOUSE_BUTTON_1 && is_mouse_inside(mouseX, mouseY)) {
		if (action == GLFW_PRESS) {
			m_clickX = get_absolute_position()[0] - static_cast<float>(mouseX);
			m_clickY = get_absolute_position()[1] - static_cast<float>(mouseY);
			m_dragging = true;
		}
		else {
			m_dragging = false;
		}

		return true;
	}

	return false;
}

bool DraggableFrame::handle_mouse_move(UIContainer&, double mouseX, double mouseY) {
	if (m_dragging) {
		auto offsetX = get_absolute_position()[0] - get_position()[0];
		auto offsetY = get_absolute_position()[1] - get_position()[1];

		set_position(offsetX + static_cast<float>(mouseX) + m_clickX,
				offsetY + static_cast<float>(mouseY) + m_clickY);
		return true;
	}

	return false;
}

void DraggableFrame::handle_focus_lost(UIContainer&) {
	m_dragging = false;
}
