#include "Editor.h"

namespace gui
{
    Notify Editor::makeNotify(Editor& editor)
    {
        return [&e = editor](EvtType evt, const void*)
        {
            if (evt == EvtType::ColourSchemeChanged)
            {
                e.updateBgImage(true);
                e.repaint();

                e.setMouseCursor(makeCursor(CursorType::Default));
            }
        };
    }
	
    Editor::Editor(audio::Processor& p) :
        juce::AudioProcessorEditor(p),
        audioProcessor(p),
		
        layout(*this),
        utils(*this, p),

        bgImage(),
        notify(utils.getEventSystem(), makeNotify(*this)),
        imgRefresh(utils, "Click here to request a new background image."),

        tooltip(utils, "The tooltips bar leads to ultimate wisdom."),

        pluginTitle(utils, JucePlugin_Name),

        lowLevel(utils),
        tuningEditor(utils),
        highLevel(utils, &lowLevel, &tuningEditor),

        contextMenuKnobs(utils),
        contextMenuButtons(utils),

        editorKnobs(utils),

        bypassed(false),
        shadr(utils, *this)

    {
        setComponentEffect(&shadr);

        setMouseCursor(makeCursor(CursorType::Default));

        layout.init
        (
            { 2, 7 },
            { 2, 13, 1 }
        );
		
        addAndMakeVisible(tooltip);

        addAndMakeVisible(imgRefresh);
		makeSymbolButton(imgRefresh, ButtonSymbol::Img, false);
        imgRefresh.onClick.push_back([&](Button&, const Mouse&)
        {
			updateBgImage(true);
            repaint();
        });

        pluginTitle.font = getFontLobster();
        addAndMakeVisible(pluginTitle);
        pluginTitle.mode = Label::Mode::TextToLabelBounds;

        addAndMakeVisible(lowLevel);
        addAndMakeVisible(highLevel);

        addAndMakeVisible(tuningEditor);

        highLevel.init();

        addAndMakeVisible(contextMenuKnobs);
        addAndMakeVisible(contextMenuButtons);

        addChildComponent(editorKnobs);

        updateBgImage(false);

        setOpaque(true);
        setResizable(true, true);
        {
            const auto user = audioProcessor.props.getUserSettings();
            const auto w = user->getIntValue("gui/width", PPDEditorWidth);
            const auto h = user->getIntValue("gui/height", PPDEditorHeight);
            setSize(w, h);
        }
    }

    Editor::~Editor()
    {
        setComponentEffect(nullptr);
    }

    void Editor::paint(Graphics& g)
    {
        g.fillAll(Colours::c(ColourID::Bg));
        g.drawImageAt(bgImage, lowLevel.getX(), 0, false);
    }

    void Editor::resized()
    {
        if (getWidth() < MinWidth)
            return setSize(MinWidth, getHeight());
        if (getHeight() < MinHeight)
            return setSize(getWidth(), MinHeight);

        utils.resized();

        layout.resized();

        layout.place(pluginTitle, 1, 0, 1, 1, false);
        layout.place(imgRefresh, 1.9f, .5f, .1f, .5f, true);
        layout.place(lowLevel, 1, 1, 1, 1, false);
        layout.place(highLevel, 0, 0, 1, 2, false);
        
        {
            const auto bnds = lowLevel.getBounds().toFloat();

            tuningEditor.defineBounds
            (
                bnds.withX(static_cast<float>(getRight())),
				bnds
            );

            tuningEditor.updateBounds();
        }

        tooltip.setBounds(layout.bottom().toNearestInt());

        const auto thicc = utils.thicc;
        editorKnobs.setBounds(0, 0, static_cast<int>(thicc * 42.f), static_cast<int>(thicc * 12.f));

        if (bgImage.isValid())
            bgImage = bgImage.rescaled(lowLevel.getWidth(), getHeight(), Graphics::ResamplingQuality::lowResamplingQuality);
        else
            updateBgImage(true);

        saveBounds();
    }

    void Editor::mouseEnter(const Mouse&)
    {
        notify(evt::Type::TooltipUpdated);
    }

    void Editor::mouseExit(const Mouse&)
    {}

    void Editor::mouseDown(const Mouse&)
    {}

    void Editor::mouseDrag(const Mouse&)
    {}

    void Editor::mouseUp(const Mouse&)
    {
        notify(EvtType::ClickedEmpty, this);
        giveAwayKeyboardFocus();
    }

    void Editor::mouseWheelMove(const Mouse&, const MouseWheel&)
    {}

    void Editor::saveBounds()
    {
        const auto w = getWidth();
        const auto h = getHeight();
        auto user = audioProcessor.props.getUserSettings();
        user->setValue("gui/width", w);
        user->setValue("gui/height", h);
    }

    void Editor::updateBgImage(bool forced)
    {
        auto props = audioProcessor.getProps();
        if (!forced)
        {
            if (props != nullptr)
            {
                auto user = props->getUserSettings();
                if (user != nullptr)
                {
                    auto file = user->getFile();
                    file = file.getParentDirectory();
                    const auto findFiles = File::TypesOfFileToFind::findFiles;
                    const auto wildCard = "*.png";
                    for (auto f : file.findChildFiles(findFiles, true, wildCard))
                    {
                        if (f.getFileName() == "bgImage.png")
                        {
                            auto img = juce::ImageFileFormat::loadFrom(f);
                            if (img.isValid())
                            {
                                bgImage = img;
                                return;
                            }
                        }
                    }
                }
            }
        }

        auto width = lowLevel.getWidth();
		auto height = getHeight();

        if (width == 0 || height == 0)
            return;
        
        bgImage = Image(Image::ARGB, width, height, true);
		
        Graphics g{ bgImage };

        Random rand;
        const auto w = static_cast<float>(width);
        const auto h = static_cast<float>(height);
        const BoundsF bounds(0.f, 0.f, w, h);
        const Colour trans(0x000000);
        const auto thicc = utils.thicc;

        { // draw fog
            Colour col(0x03ffffff);

            const auto numFogs = 2;
            for (auto i = 0; i < numFogs; ++i)
            {
                const PointF pt0
                (
                    rand.nextFloat() * w,
                    rand.nextFloat() * h
                );
                const PointF pt1
                (
                    rand.nextFloat() * w,
                    rand.nextFloat() * h
                );
                const Gradient gradient(trans, pt0, col, pt1, false);
                g.setGradientFill(gradient);
                g.fillRect(bounds);
            }
        }
		
        { // draw dust
            const auto randWidth = .05f;
            const auto randRange = randWidth * 2.f;
			auto col = Colours::c(ColourID::Mod).withAlpha(.02f);

            const auto numDusts = 13;
            for (auto i = 0; i < numDusts; ++i)
            {
                col = col.withRotatedHue(rand.nextFloat() * randWidth - randRange);

                const PointF pt0
                (
                    rand.nextFloat() * w,
                    rand.nextFloat() * h
                );
                const PointF pt1
                (
                    rand.nextFloat() * w,
                    rand.nextFloat() * h
                );
                const Gradient gradient(trans, pt0, col, pt1, true);
                g.setGradientFill(gradient);
                g.fillRect(bounds);
            }
        }
		
        { // draw stars
            const auto maxStarSize = thicc * 1.5f;

            const auto numStars = 128;
			for (auto i = 0; i < numStars; ++i)
			{
				const auto x = rand.nextFloat() * w;
				const auto y = rand.nextFloat() * h;
                const auto starSize = 1.f + rand.nextFloat() * maxStarSize;
                auto alpha = rand.nextFloat() * .9f;
                alpha = .05f + alpha * alpha * alpha * alpha * alpha;
                auto habitable = rand.nextFloat();
				habitable = habitable * habitable * habitable * habitable * habitable;
                const auto col = Colour(0xffffffff)
                    .interpolatedWith(Colours::c(ColourID::Mod), habitable)
                    .withAlpha(alpha);
				g.setColour(col);
				g.fillEllipse(x, y, starSize, starSize);
			}
        }

        { // posterize the image
			
            const float depth = 32.f;
			const auto dInv = 1.f / depth;

			for (auto y0 = 0; y0 < bgImage.getHeight(); ++y0)
            {
                for (auto x0 = 0; x0 < bgImage.getWidth(); ++x0)
				{
					const auto pxl = bgImage.getPixelAt(x0, y0);
                    juce::uint8 rgb[3];
					rgb[0] = pxl.getRed();
                    rgb[1] = pxl.getGreen();
                    rgb[2] = pxl.getBlue();
					for(auto j = 0; j < 3; ++j)
					{
                        const auto val = static_cast<float>(rgb[j]) * dInv;
						const auto newVal = static_cast<juce::uint8>(std::round(val) * depth);
						rgb[j] = newVal;
					}
					const auto col = Colour::fromRGBA(rgb[0], rgb[1], rgb[2], rgb[3]);
					bgImage.setPixelAt(x0, y0, pxl.interpolatedWith(col, .1f));
				}
            }
        }

        if (props != nullptr)
        {
            auto user = props->getUserSettings();
            if (user != nullptr)
            {
				auto file = user->getFile();
				file = file.getParentDirectory();
				file = file.getChildFile("bgImage.png");
                if (file.exists())
                    file.deleteFile();
                juce::FileOutputStream stream(file);
                juce::PNGImageFormat pngWriter;
                pngWriter.writeImageToStream(bgImage, stream);
            }
        }
    }

}