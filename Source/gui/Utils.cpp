#include "Utils.h"

namespace gui
{

	Utils::Utils(Component& _pluginTop, Processor& _audioProcessor) :
		pluginTop(_pluginTop),
		audioProcessor(_audioProcessor),
		params(audioProcessor.params),
		eventSystem(),
		evt(eventSystem),
		thicc(1.f)
	{
		Colours::c.init(audioProcessor.props.getUserSettings());
	}

	Param* Utils::getParam(PID pID) noexcept
	{
		return params[pID];
	}

	const Param* Utils::getParam(PID pID) const noexcept
	{
		return params[pID];
	}

	Params& Utils::getParams() noexcept
	{
		return audioProcessor.params;
	}

	const Params& Utils::getParams() const noexcept
	{
		return audioProcessor.params;
	}

	std::vector<Param*>& Utils::getAllParams() noexcept
	{
		return params.data();
	}
	
	const std::vector<Param*>& Utils::getAllParams() const noexcept
	{
		return params.data();
	}

	ValueTree Utils::getState() const noexcept
	{
		return audioProcessor.state.getState();
	}

	void Utils::assignMIDILearn(PID pID) noexcept
	{
		audioProcessor.midiManager.midiLearn.assignParam(params[pID]);
	}
	
	void Utils::removeMIDILearn(PID pID) noexcept
	{
		audioProcessor.midiManager.midiLearn.removeParam(params[pID]);
	}
	
	const audio::MIDILearn& Utils::getMIDILearn() const noexcept
	{
		return audioProcessor.midiManager.midiLearn;
	}

	void Utils::resized()
	{
		auto a = std::min(pluginTop.getWidth(), pluginTop.getHeight());
		auto t = static_cast<float>(a) * .004f;
		thicc = t < 1.f ? 1.f : t;
	}

	float Utils::getDragSpeed() const noexcept
	{
		const auto height = static_cast<float>(pluginTop.getHeight());
		const auto speed = DragSpeed * height;
		return speed;
	}

	float Utils::fontHeight() const noexcept
	{
		const auto w = static_cast<float>(pluginTop.getWidth());
		const auto h = static_cast<float>(pluginTop.getHeight());

		const auto avr = (w + h) * .5f;
		const auto norm = (avr - 500.f) / 500.f;
		return std::floor(8.5f + norm * 5.f);
	}

	EventSystem& Utils::getEventSystem()
	{
		return eventSystem;
	}

	const std::atomic<float>& Utils::getMeter(int i) const noexcept
	{
		return audioProcessor.meters(i);
	}

	ValueTree Utils::savePatch()
	{
		audioProcessor.savePatch();
		return audioProcessor.state.getState();
	}

	void Utils::loadPatch(const ValueTree& vt)
	{
		audioProcessor.state.loadPatch(vt);
		audioProcessor.loadPatch();
		audioProcessor.forcePrepareToPlay();
	}

	AppProps& Utils::getProps() noexcept
	{
		return *audioProcessor.getProps();
	}

	Point Utils::getScreenPosition() const noexcept
	{
		return pluginTop.getScreenPosition();
	}

	juce::MouseCursor makeCursor(CursorType c)
	{
		Image img = juce::ImageCache::getFromMemory(BinaryData::cursor_png, BinaryData::cursor_pngSize).createCopy();

		const auto w = img.getWidth();
		const auto h = img.getHeight();

		const Colour imgCol(0xff37946e);

		Colour col;

		if (c == CursorType::Default)
			col = Colours::c(ColourID::Txt);
		else if (c == CursorType::Interact)
			col = Colours::c(ColourID::Interact);
		else if (c == CursorType::Inactive)
			col = Colours::c(ColourID::Inactive);
		else if (c == CursorType::Mod)
			col = Colours::c(ColourID::Mod);
		else if (c == CursorType::Bias)
			col = Colours::c(ColourID::Bias);

		for (auto y = 0; y < h; ++y)
			for (auto x = 0; x < w; ++x)
				if (img.getPixelAt(x, y) == imgCol)
					img.setPixelAt(x, y, col);

		static constexpr int scale = 3;
		img = img.rescaled(w * scale, h * scale, Graphics::ResamplingQuality::lowResamplingQuality);

		return { img, 0, 0 };
	}

	void hideCursor()
	{
		juce::Desktop::getInstance().getMainMouseSource().enableUnboundedMouseMovement(true, false);
	}

	void showCursor(const Component& comp, const Point* pos)
	{
		auto mms = juce::Desktop::getInstance().getMainMouseSource();
		const Point centre(comp.getWidth() / 2, comp.getHeight() / 2);
		if (pos == nullptr)
			pos = &centre;
		mms.setScreenPosition((comp.getScreenPosition() + *pos).toFloat());
		mms.enableUnboundedMouseMovement(false, true);
	}

	void appendRandomString(String& str, Random& rand, int length, const String& legalChars)
	{
		const auto max = static_cast<float>(legalChars.length() - 1);

		for (auto i = 0; i < length; ++i)
		{
			auto idx = static_cast<int>(rand.nextFloat() * max);
			str += legalChars[idx];
		}
	}

	int snapToJordanPolyaSequence(int number, int maxOrder) noexcept
	{
		// number snap to next in 1, 2, 4, 6, 8, 12, 16, 24, 32..

		auto x0 = 1;
		auto minDist = 1 << maxOrder;
		auto minDif = minDist;
		for (auto i = 1; i <= maxOrder; ++i)
		{
			const auto x1 = 1 << i;
			const auto x2 = (x0 + x1) / 2;

			const auto x2Dist = x2 - number;
			const auto x1Dist = x1 - number;

			const auto x2Dif = std::abs(x2Dist);
			const auto x1Dif = std::abs(x1Dist);

			if (minDif > x2Dif)
			{
				minDif = x2Dif;
				minDist = x2Dist;
			}
			if (minDif > x1Dif)
			{
				minDif = x1Dif;
				minDist = x1Dist;
			}

			x0 = x1;
		}

		return number + minDist;
	}
}