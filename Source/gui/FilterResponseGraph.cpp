#include "FilterResponseGraph.h"

namespace gui
{
	//FilterResponseGraph

	FilterResponseGraph::FilterResponseGraph(Utils& u) :
		Comp(u, "", CursorType::Default),
		responseCurveCID(ColourID::Hover),
		processFilters(nullptr),
		needsUpdate(nullptr),

		impulse(),
		processBuffer(),
		dftBuffer(),

		responseCurve()
	{
		SIMD::clear(impulse.data(), Size);
		impulse[0] = 1.f;

		setInterceptsMouseClicks(false, false);
		startTimerHz(4);
	}

	void FilterResponseGraph::paint(Graphics& g)
	{
		if (responseCurve.isEmpty())
			return;
		const auto thicc = utils.thicc;
		const Stroke stroke(thicc, Stroke::PathStrokeType::JointStyle::curved, Stroke::PathStrokeType::EndCapStyle::rounded);

		g.setColour(Colours::c(responseCurveCID));
		g.strokePath(responseCurve, stroke);
	}

	void FilterResponseGraph::resized()
	{
		if (responseCurve.isEmpty())
			return;

		updateResponseCurve();
	}

	void FilterResponseGraph::timerCallback()
	{
		if (needsUpdate())
		{
			updateResponseCurve();
			repaint();
		}
	}

	void FilterResponseGraph::updateResponseCurve()
	{
		resetBuffers();

		processFilters(processBuffer.data(), impulse, Size);

		applyForwardDFT();

		generateResponseCurve();
	}

	void FilterResponseGraph::resetBuffers()
	{
		SIMD::clear(processBuffer.data(), processBuffer.size());

		for (auto& d : dftBuffer)
			d = { 0.f, 0.f };
	}

	void FilterResponseGraph::applyForwardDFT()
	{
		auto buf = processBuffer.data();
		auto dft = dftBuffer.data();

		for (auto n = 0; n < Size; ++n)
		{
			const auto f = Pi * static_cast<float>(n) * SizeInv;
			auto real = 0.f;
			auto imag = 0.f;
			for (auto s = 0; s < Size; ++s)
			{
				const auto sf = s * f;
				real += buf[s] * std::cos(sf);
				imag -= buf[s] * std::sin(sf);
			}
			dft[n] += std::complex<float>{ real, imag };
		}

		for (auto i = 0; i < Size; ++i)
			buf[i] = std::abs(dft[i]);
	}

	void FilterResponseGraph::generateResponseCurve()
	{
		const auto thicc = utils.thicc;
		const auto w = static_cast<float>(getWidth());
		const auto h = static_cast<float>(getHeight());
		const auto wInv = 1.f / w;
		const auto buf = processBuffer.data();
		const auto& xen = utils.audioProcessor.xenManager;
		const auto fsInv = 1.f / static_cast<float>(utils.audioProcessor.getSampleRate());

		responseCurve.clear();
		{
			const auto y = h - h * buf[0];
			responseCurve.startNewSubPath(0.f, y);
		}
		for (auto x = thicc; x < w; x += 1.f)
		{
			const auto r = x * wInv;
			const auto pitch = r * 128.f;
			const auto freqHz = xen.noteToFreqHzWithWrap(pitch + xen.getXen());
			const auto fc = freqHz * fsInv;
			const auto idx = fc * SizeF;

			auto y = h - h * interpolate::lerp(buf, idx);
			y = juce::jlimit(0.f, h, y);

			responseCurve.lineTo(x, y);
		}
		responseCurve.lineTo(w, h - h * buf[Size - 1]);
	}

	//FilterResponseGraph2

	FilterResponseGraph2::FilterResponseGraph2(Utils& u) :
		Comp(u, "", CursorType::Default),
		responseCurveCID(ColourID::Hover),
		shallUpdate(),
		update(),
		responseCurve()
	{
		setInterceptsMouseClicks(false, false);
		startTimerHz(PPDFPSKnobs);
	}

	void FilterResponseGraph2::paint(Graphics& g)
	{
		if (responseCurve.isEmpty())
			return;
		const auto thicc = utils.thicc;
		const Stroke stroke(thicc, Stroke::PathStrokeType::JointStyle::curved, Stroke::PathStrokeType::EndCapStyle::rounded);

		g.setColour(Colours::c(responseCurveCID));
		g.strokePath(responseCurve, stroke);
	}

	void FilterResponseGraph2::timerCallback()
	{
		if (shallUpdate())
			resized();
	}

	void FilterResponseGraph2::resized()
	{
		const auto w = static_cast<float>(getWidth());
		const auto h = static_cast<float>(getHeight());

		update(responseCurve, w, h);
		repaint();
	}
}