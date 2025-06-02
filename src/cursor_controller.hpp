#pragma once

#include "cursor_position.hpp"
#include "functor_ref_wrapper.hpp"
#include "text_alignment.hpp"

#include <unicode/uversion.h>

#include <string_view>

U_NAMESPACE_BEGIN

class BreakIterator;

U_NAMESPACE_END

namespace Text {

class LayoutInfo;

class CursorController {
	public:
		explicit CursorController();
		~CursorController();

		CursorController(CursorController&&) noexcept;
		CursorController& operator=(CursorController&&) noexcept;

		CursorController(const CursorController&) = delete;
		void operator=(const CursorController&) = delete;

		void set_text(std::string_view);

		CursorPosition next_character(CursorPosition);
		CursorPosition prev_character(CursorPosition);

		CursorPosition next_word(CursorPosition);
		CursorPosition prev_word(CursorPosition);

		CursorPosition closest_in_line(const LayoutInfo&, float textAreaWidth, XAlignment, size_t lineIndex,
				float posX);
		CursorPosition closest_to_position(const LayoutInfo&, float textAreaWidth, XAlignment, float posX,
				float posY);

		template <typename LineOffsetProvider>
		CursorPosition closest_to_position(const LayoutInfo&, float textAreaWidth, XAlignment, float posX,
				float posY, LineOffsetProvider&&);

		std::string_view get_text() const {
			return m_text;
		}
	private:
		icu::BreakIterator* m_iter;
		std::string_view m_text;

		CursorPosition closest_to_position_internal(const LayoutInfo&, float, XAlignment, float, float,
				FunctorRefWrapper<float(size_t)>&& lineOffsetProvider);
};

template <typename LineOffsetProvider>
CursorPosition CursorController::closest_to_position(const LayoutInfo& layout, float textAreaWidth,
		XAlignment xAlignment, float posX, float posY, LineOffsetProvider&& lineOffsetProvider) {
	return closest_to_position_internal(layout, textAreaWidth, xAlignment, posX, posY, lineOffsetProvider);
}

}

