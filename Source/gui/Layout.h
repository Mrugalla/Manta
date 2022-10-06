#pragma once
#include "Using.h"

namespace gui
{
	BoundsF smallestBoundsIn(const LineF& line) noexcept;
	
	BoundsF maxQuadIn(const BoundsF&) noexcept;
	
	void repaintWithChildren(Component*);

	std::unique_ptr<juce::XmlElement> loadXML(const char*, const int);

	class Layout
	{
	public:
		Layout(const Component&);

		void init(const std::vector<int>& /*xDist*/, const std::vector<int>& /*yDist*/);

		void fromStrings(const String& /*xDist*/, const String& /*yDist*/);

		void resized() noexcept;

		template<typename X, typename Y>
		PointF operator()(X, Y) const noexcept;

		template<typename PointType>
		PointF operator()(PointType) const noexcept;

		template<typename X, typename Y>
		BoundsF operator()(X, Y, X, Y, bool /*isQuad*/ = false) const noexcept;

		template<typename PointType0, typename PointType1>
		LineF getLine(PointType0, PointType1) const noexcept;

		template<typename X0, typename Y0, typename X1, typename Y1>
		LineF getLine(X0, Y0, X1, Y1) const noexcept;

		BoundsF bottom(bool /*isQuad*/ = false) const noexcept;

		BoundsF top(bool /*isQuad*/ = false) const noexcept;

		BoundsF right(bool /*isQuad*/ = false) const noexcept;

		float getX(int) const noexcept;
		float getY(int) const noexcept;
		float getX(float) const noexcept;

		float getY(float) const noexcept;

		template<typename X>
		float getW(X) const noexcept;

		template<typename Y>
		float getH(Y) const noexcept;

		template<typename X, typename Y>
		void place(Component&, X, Y y, X = static_cast<X>(1), Y = static_cast<Y>(1), bool /*isQuad*/ = false) const noexcept;

		template<typename X, typename Y>
		void place(Component*, X, Y, X = static_cast<X>(1), Y  = static_cast<Y>(1), bool /*isQuad*/ = false) const noexcept;
		
		void paint(Graphics&);

		template<typename X, typename Y>
		void label(Graphics&, String&&, X, Y, X = static_cast<X>(1), Y = static_cast<Y>(1), bool /*isQuad*/ = false) const;

	protected:
		const Component& comp;
		std::vector<float> rXRaw, rX;
		std::vector<float> rYRaw, rY;
	};

	void make(Path&, const Layout&, std::vector<Point>&&);

	void drawHorizontalLine(Graphics&, int /*y*/, float /*left*/, float /*right*/, int /*thicc*/ = 1);
	
	void drawVerticalLine(Graphics&, int /*x*/, float /*top*/, float /*bottom*/, int /*thicc*/ = 1);

	/* graphics, bounds, edgeWidth, edgeHeight, stroke */
	void drawRectEdges(Graphics&, const BoundsF&, float, float, Stroke);

	/* graphics, bounds, edgeWidth, stroke */
	void drawRectEdges(Graphics&, const BoundsF&, float, Stroke);

	/* graphics, bounds, text */
	void drawHeadLine(Graphics&, const BoundsF&, const String&);

	template<class ArrayCompPtr>
	inline void distributeVertically(const Component& parent, ArrayCompPtr& compArray)
	{
		const auto w = static_cast<float>(parent.getWidth());
		const auto h = static_cast<float>(parent.getHeight());
		const auto x = 0.f;

		auto y = 0.f;

		const auto cH = h / static_cast<float>(compArray.size());

		for (auto& cmp : compArray)
		{
			cmp->setBounds(BoundsF(x, y, w, cH).toNearestInt());
			y += cH;
		}
	}

	template<class SizeType>
	inline void distributeVertically(const Component& parent, Component* compArray, SizeType size)
	{
		const auto w = static_cast<float>(parent.getWidth());
		const auto h = static_cast<float>(parent.getHeight());
		const auto x = 0.f;

		auto y = 0.f;

		const auto cH = h / static_cast<float>(size);

		for (auto i = 0; i < size; ++i)
		{
			auto& cmp = compArray[i];
			cmp.setBounds(BoundsF(x, y, w, cH).toNearestInt());
			y += cH;
		}
	}

	BoundsF boundsOf(const Font&, const String&) noexcept;

	namespace imgPP
	{
		void blur(Image&, Graphics&, int /*its*/ = 3) noexcept;
	}
}

/*

layout reduced by text width and/or height
	for example for tooltips

*/