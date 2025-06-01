#pragma once

#include "font.hpp"

#include "ui_object.hpp"

class CollidableTextBox final : public UIObject {
	public:
		static std::shared_ptr<CollidableTextBox> create();

		void render(UIContainer&) override;

		void set_font(Text::Font);
		void set_text(std::string);

		void set_collider(std::shared_ptr<UIObject>);
	private:
		Text::Font m_font{};
		std::string m_text{};
		std::string m_contentText{};

		std::shared_ptr<UIObject> m_collider;

		bool is_box_outside() const;
		void draw_text_collided(UIContainer&);
		void draw_text_normally(UIContainer&);
};
