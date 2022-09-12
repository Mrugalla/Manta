#include "Editor.h"

namespace gui
{
    Editor::Editor(audio::Processor& p) :
        juce::AudioProcessorEditor(p),
        audioProcessor(p),

        layout(*this),
        utils(*this, p),

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
            { 1, 3 },
            { 2, 13, 1 }
        );
		
        addAndMakeVisible(tooltip);

        pluginTitle.font = getFontDosisExtraLight();
        addAndMakeVisible(pluginTitle);
        pluginTitle.mode = Label::Mode::TextToLabelBounds;

        addAndMakeVisible(lowLevel);
        addAndMakeVisible(highLevel);

        addAndMakeVisible(tuningEditor);

        highLevel.init();

        addAndMakeVisible(contextMenuKnobs);
        addAndMakeVisible(contextMenuButtons);

        addChildComponent(editorKnobs);

        setOpaque(true);
        setResizable(true, true);
        {
            auto user = audioProcessor.props.getUserSettings();
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
        g.fillAll(Colours::c(gui::ColourID::Bg));
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

        saveBounds();
    }

    void Editor::mouseEnter(const Mouse&)
    {
        auto& evtSys = utils.getEventSystem();
        evtSys.notify(evt::Type::TooltipUpdated);
    }

    void Editor::mouseExit(const Mouse&)
    {}

    void Editor::mouseDown(const Mouse&)
    {}

    void Editor::mouseDrag(const Mouse&)
    {}

    void Editor::mouseUp(const Mouse&)
    {
        utils.getEventSystem().notify(EvtType::ClickedEmpty, this);
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

}