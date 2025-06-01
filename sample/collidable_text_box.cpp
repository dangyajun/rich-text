#include "collidable_text_box.hpp"

#include "layout_info.hpp"
#include "layout_builder.hpp"
#include "ui_container.hpp"
#include "value_runs.hpp"

std::shared_ptr<CollidableTextBox> CollidableTextBox::create() {
	return std::make_shared<CollidableTextBox>();
}

void CollidableTextBox::render(UIContainer& container) {
	if (is_box_outside()) {
		draw_text_normally(container);
	}
	else {
		draw_text_collided(container);
	}
}

void CollidableTextBox::set_font(Text::Font font) {
	m_font = font;
}

void CollidableTextBox::set_text(std::string text) {
	m_text = std::move(text);
}

void CollidableTextBox::set_collider(std::shared_ptr<UIObject> collider) {
	m_collider = std::move(collider);
}

bool CollidableTextBox::is_box_outside() const {
	if (!m_collider) {
		return true;
	}

	auto myPos = get_absolute_position();
	auto mySize = get_size();

	auto boxPos = m_collider->get_absolute_position();
	auto boxSize = m_collider->get_size();

	return boxPos[0] + boxSize[0] < myPos[0] || myPos[0] + mySize[0] < boxPos[0]
			|| boxPos[1] + boxSize[1] < myPos[1] || myPos[1] + mySize[1] < boxPos[1];
}

void CollidableTextBox::draw_text_collided(UIContainer& container) {
	Text::LayoutBuilder builder;

	float cursorX = 0, cursorY = 0;
	uint32_t charIndex = 0;

	auto myPos = get_absolute_position();

	auto boxPos = m_collider->get_absolute_position();
	auto boxSize = m_collider->get_size();

	Text::ValueRuns<Text::Font> fontRuns(m_font, static_cast<int32_t>(m_text.size()));
	Text::LayoutInfo layout;
	
	while (charIndex < m_text.size()) {
	//for (int i = 0; i < 7; ++i) {
		Text::LayoutBuildParams params{
			.textAreaWidth = get_size()[0],
			.textAreaHeight = get_size()[1],
			.tabWidth = 8.f,
			.flags = Text::LayoutInfoFlags::TRUNCATE,
			.xAlignment = Text::XAlignment::LEFT,
			.yAlignment = Text::YAlignment::TOP,
		};

		float localX = boxPos[0] - myPos[0];
		float localY = boxPos[1] - myPos[1];

		float paddedBoxHeight = boxSize[1] + static_cast<float>(m_font.get_size());

		auto lineWidthProvider = [&](size_t lineNumber, float totalAscent) {
			totalAscent += cursorY + static_cast<float>(m_font.get_size());

			if (totalAscent < localY || totalAscent >= localY + paddedBoxHeight) {
				return get_size()[0];
			}
			else if (totalAscent >= localY && totalAscent < localY + paddedBoxHeight) {
				if (cursorX < localX) {
					return localX - cursorX;
				}
				else if (cursorX >= localX + boxSize[0]) {
					return get_size()[0] - cursorX;
				}
			}

			return 0.f;
		};

		bool advanceLine = false;

		if (cursorY >= localY && cursorY + static_cast<float>(m_font.get_size()) <= localY + paddedBoxHeight) {
			if (cursorX >= localX && cursorX <= localX + boxSize[0]) {
				cursorX = localX + boxSize[0];
			}
		}

		if (cursorY + static_cast<float>(m_font.get_size()) <= localY + paddedBoxHeight) {
			if (cursorX < localX) {
				params.textAreaWidth = localX - cursorX;
				if (localY > cursorY) {
					params.textAreaHeight = localY + static_cast<float>(m_font.get_size()) - cursorY;
				}
				else {
					params.textAreaHeight = cursorY + static_cast<float>(m_font.get_size()) - localY;
					params.flags |= Text::LayoutInfoFlags::IGNORE_SOFT_BREAKS;
				}
			}
			else if (cursorX >= localX + boxSize[0]) {
				params.textAreaWidth = get_size()[0] - cursorX;
				params.textAreaHeight = static_cast<float>(m_font.get_size()) + 1;
				params.flags |= Text::LayoutInfoFlags::IGNORE_SOFT_BREAKS;
				advanceLine = true;
			}
		}

		builder.build_layout_info(layout, m_text.data() + charIndex, m_text.size() - charIndex, fontRuns,
				params, lineWidthProvider);

		container.draw_text(layout, get_absolute_position()[0] + cursorX, get_absolute_position()[1] + cursorY,
				get_size()[0] - cursorX, Text::XAlignment::LEFT, {0, 0, 0, 1});

		cursorX += layout.empty() ? 0.f : layout.get_line_width(layout.get_line_count() - 1);
		cursorY += layout.get_text_height();

		if (cursorY >= localY && cursorY <= localY + paddedBoxHeight && cursorX <= localX + boxSize[0]) {
			cursorX = localX + boxSize[0];
		}

		cursorY -= static_cast<float>(m_font.get_size());

		if (advanceLine) {
			cursorX = 0;
			cursorY += static_cast<float>(m_font.get_size());
		}

		charIndex += (layout.empty() ? 0 : layout.get_char_index(layout.get_char_index_count() - 1)) + 1;

		//container.emit_rect(get_absolute_position()[0] + cursorX, get_absolute_position()[1] + cursorY, 10,
				//10, {0, 0.5f, 0, 1}, PipelineIndex::RECT);
	}
}

void CollidableTextBox::draw_text_normally(UIContainer& container) {
	Text::LayoutBuilder builder;

	Text::LayoutBuildParams params{
		.textAreaWidth = get_size()[0],
		.textAreaHeight = get_size()[1],
		.tabWidth = 8.f,
		.flags = Text::LayoutInfoFlags::TRUNCATE,
		.xAlignment = Text::XAlignment::LEFT,
		.yAlignment = Text::YAlignment::TOP,
	};
	Text::ValueRuns<Text::Font> fontRuns(m_font, static_cast<int32_t>(m_text.size()));

	Text::LayoutInfo layout;
	builder.build_layout_info(layout, m_text.data(), m_text.size(), fontRuns, params);

	container.draw_text(layout, get_absolute_position()[0], get_absolute_position()[1], get_size()[0],
			Text::XAlignment::LEFT, {0, 0, 0, 1});
}
