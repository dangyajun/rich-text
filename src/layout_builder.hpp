#pragma once

#include "common.hpp"
#include "font.hpp"
#include "functor_ref_wrapper.hpp"
#include "text_alignment.hpp"

#include <unicode/uversion.h>

#include <vector>

U_NAMESPACE_BEGIN

class BreakIterator;
class Locale;

U_NAMESPACE_END

struct hb_buffer_t;
struct _SBParagraph;

namespace Text {

class LayoutInfo;
template <typename> class ValueRuns;
template <typename> class ValueRunsIterator;
template <typename> class MaybeDefaultRunsIterator;

enum class LayoutInfoFlags : uint8_t {
	NONE = 0,
	// Whether the text direction default should be RTL when no strongly directional characters are detected.
	// Leave unset to default to LTR.
	RIGHT_TO_LEFT = 1, 
	// Whether the configured text direction should override the paragraph base direction, regardless of the
	// presence of strongly-directional scripts.
	OVERRIDE_DIRECTIONALITY = 2, 
	// Whether the text is composed vertically. Leave unset for horizontal text.
	VERTICAL = 4, 
	// Whether the tab width parameter is in pixels. Leave unset for tab width in terms of space-widths.
	TAB_WIDTH_PIXELS = 8, 
	// Whether to ignore soft line-breaks  even when a non-zero text area width is provided. 
	IGNORE_SOFT_BREAKS = 16,
	// Truncate text if it runs out of bounds
	TRUNCATE = 32,
};

RICHTEXT_DEFINE_ENUM_BITFLAG_OPERATORS(LayoutInfoFlags)

struct LayoutBuildParams {
	// Text area width for calculating soft line breaks and text truncation. Setting this to 0 will disable
	// both.
	float textAreaWidth;
	float textAreaHeight;
	float tabWidth;
	LayoutInfoFlags flags;
	XAlignment xAlignment;
	YAlignment yAlignment;
	const ValueRuns<bool>* pSmallcapsRuns;
	const ValueRuns<bool>* pSubscriptRuns;
	const ValueRuns<bool>* pSuperscriptRuns;
};

class LayoutBuilder {
	public:
		explicit LayoutBuilder();
		~LayoutBuilder();

		LayoutBuilder(LayoutBuilder&&) noexcept;
		LayoutBuilder& operator=(LayoutBuilder&&) noexcept;

		LayoutBuilder(const LayoutBuilder&) = delete;
		void operator=(const LayoutBuilder&) = delete;

		void build_layout_info(LayoutInfo&, const char* chars, int32_t count, const ValueRuns<Font>& fontRuns,
				const LayoutBuildParams& params);

		/**
		 * Builds the layout info using an additional LineWidthProvider which is used in place of the
		 * textAreaWidth to determine line breaking limits at a per-line level. The functor provides
		 * the line number and paragraph height so far (aka the totalDescent of this line, retrievable
		 * later from the LayoutInfo).
		 */
		template <typename LineWidthProvider>
		void build_layout_info(LayoutInfo&, const char* chars, int32_t count, const ValueRuns<Font>& fontRuns,
				const LayoutBuildParams& params, LineWidthProvider&& lineWidthProvider);
	private:
		struct LogicalRun {
			SingleScriptFont font;
			int32_t charEndIndex;
			uint32_t glyphEndIndex;
		};

		struct ParagraphBuildParams;
		struct ParagraphBuildResult;

		icu::BreakIterator* m_lineBreakIterator{};
		hb_buffer_t* m_buffer{};
		std::vector<uint32_t> m_glyphs;
		std::vector<uint32_t> m_charIndices;
		// Glyph positions are stored as 26.6 fixed point values, always in logical order. On the primary axis,
		// positions are stored as glyph widths. On the secondary axis, positions are stored in absolute
		// position as calculated from the offsets and advances.
		std::vector<int32_t> m_glyphPositions[2];

		int32_t m_cursor;

		std::vector<LogicalRun> m_logicalRuns;

		void build_layout_info_internal(LayoutInfo&, const char* chars, int32_t count,
				const ValueRuns<Font>& fontRuns, const LayoutBuildParams& params,
				FunctorRefWrapper<float(size_t, float)>&& lineWidthProvider);

		ParagraphBuildResult build_paragraph(LayoutInfo& result, _SBParagraph* sbParagraph,
				const char* fullText, int32_t paragraphLength, int32_t paragraphStart,
				ParagraphBuildParams& params, FunctorRefWrapper<float(size_t, float)>& lineWidthProvider);
		void shape_logical_run(const SingleScriptFont& font, const char* paragraphText, int32_t offset,
				int32_t count, int32_t paragraphStart, int32_t paragraphLength, int script,
				const icu::Locale& locale, bool reversed, bool vertical);
		bool compute_line_visual_runs(LayoutInfo& result, _SBParagraph* sbParagraph, const char* chars,
				int32_t count, int32_t lineStart, int32_t lineEnd, size_t& highestRun,
				int32_t& highestRunCharEnd, float textAreaHeight, bool truncate, bool vertical);
		void append_visual_run(LayoutInfo& result, size_t logicalRunIndex, int32_t charStartIndex,
				int32_t charEndIndex, int32_t& visualRunWidth, size_t& highestRun, int32_t& highestRunCharEnd,
				bool reversed, bool vertical);

		void apply_tab_widths_no_line_break(const char* fullText, int32_t tabWidthFixed,
				bool tabWidthFromPixels);

		void reset(size_t capacity);
};

template <typename LineWidthProvider>
void LayoutBuilder::build_layout_info(LayoutInfo& info, const char* chars, int32_t count,
		const ValueRuns<Font>& fontRuns, const LayoutBuildParams& params,
		LineWidthProvider&& lineWidthProvider) {
	build_layout_info_internal(info, chars, count, fontRuns, params,
			FunctorRefWrapper<float(size_t, float)>(lineWidthProvider));
}

}
