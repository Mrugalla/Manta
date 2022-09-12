#include "Shader.h"

gui::Shader::Shader(Utils& u, Component& _comp) :
    Timer(),
    comp(_comp),
    utils(u),
    notify(u.getEventSystem()),
    bypassed(false)
{
    startTimerHz(12);
}

void gui::Shader::applyEffect(Image& img, Graphics& g, float, float)
{
    g.drawImageAt(img, 0, 0, false);

    if (bypassed)
        paintBypassed(img, g);
}

void gui::Shader::paintBypassed(Image& img, Graphics& g)
{
    const auto h = static_cast<float>(img.getHeight()) * .5f;
    const auto r = static_cast<float>(img.getWidth());

    PointF left(0.f, h);
    PointF right(r, h);

    juce::ColourGradient grad(
        Colour(0x00000000),
        left,
        Colour(0xff000000),
        right,
        false
    );
    g.setGradientFill(grad);
    g.fillAll();
    g.setColour(Colours::c(ColourID::Abort));
    g.drawFittedText("bypassed", img.getBounds(), Just::centredRight, 1);
}

void gui::Shader::timerCallback()
{
    bool shallRepaint = false;

    auto b = utils.getParam(PID::Power)->getValue() < .5f;
    if (bypassed != b)
    {
        bypassed = b;
        shallRepaint = true;
    }

    if (shallRepaint)
    {
        repaintWithChildren(&comp);
    }
}