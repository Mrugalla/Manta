#pragma once
#include "Using.h"

namespace gui
{
    enum class ColourID
    {
        Bg,
        Txt,
        Abort,
        Interact,
        Inactive,
        Darken,
        Hover,
        Transp,
        Mod,
        Bias,
        NumCols
    };

    static constexpr int NumColours = static_cast<int>(ColourID::NumCols);

    class Colours
    {
        using Array = std::array<Colour, static_cast<int>(ColourID::NumCols)>;
    public:
        Colours() :
            cols(),
            props(nullptr)
        {
            setInternal(ColourID::Transp, Colour(0x00000000));
            setInternal(ColourID::Abort, Colour(0xffff0000));
        }

        Colour defaultColour() noexcept
        {
            return Colour(0xff6800ff);
        }

        void init(Props* p)
        {
            props = p;
            if (props->isValidFile())
            {
                const auto colStr = props->getValue(coloursID(), defaultColour().toString());
                set(juce::Colour::fromString(colStr));
            }
        }

        bool set(Colour col)
        {
            if (props->isValidFile())
            {
                setInternal(ColourID::Interact, col);
                props->setValue(coloursID(), col.toString());

                setInternal(ColourID::Bg, col.darker(8.f).withMultipliedSaturation(.15f));
                setInternal(ColourID::Txt, col.withMultipliedBrightness(.6f));
                setInternal(ColourID::Mod, col.withRotatedHue(.4f));
                setInternal(ColourID::Bias, col.withRotatedHue(.6f));
                setInternal(ColourID::Darken, col.darker(2.f).withMultipliedAlpha(.5f));
                setInternal(ColourID::Hover, col.withMultipliedSaturation(2.f).brighter(2.f).withMultipliedAlpha(.15f));
                setInternal(ColourID::Inactive, col.withMultipliedSaturation(.1f));
                
                if (props->needsToBeSaved())
                {
                    props->save();
                    props->sendChangeMessage();
                    return true;
                }
            }
            return false;
        }

        Colour operator()(ColourID i) const noexcept
        {
            return get(static_cast<int>(i));
        }

        Colour operator()(int i) const noexcept
        {
            return get(i);
        }

        Colour get(int i) const noexcept
        {
            return cols[i];
        }

        static Colours c;
    private:
        Array cols;
        Props* props;

        void setInternal(ColourID cID, Colour col) noexcept
        {
            cols[static_cast<int>(cID)] = col;
        }

        String coloursID()
        {
            return "coloursMain";
        }
    };

    // GET FONT
    inline Font getFont(const char* ttf, size_t size)
    {
        auto typeface = juce::Typeface::createSystemTypefaceFor(ttf, size);
        return Font(typeface);
    }
    
    // GET FONT NEL
    inline Font getFontNEL()
    {
        return getFont(BinaryData::nel19_ttf, BinaryData::nel19_ttfSize);
    }

    // GET FONT LOBSTER
    inline Font getFontLobster()
    {
        return getFont(BinaryData::LobsterRegular_ttf, BinaryData::LobsterRegular_ttfSize);
    }

    // GET FONT MS MADI
    inline Font getFontMsMadi()
    {
        return getFont(BinaryData::MsMadiRegular_ttf, BinaryData::MsMadiRegular_ttfSize);
    }

    // GET FONT DOSIS
    inline Font getFontDosisSemiBold()
    {
        return getFont(BinaryData::DosisSemiBold_ttf, BinaryData::DosisSemiBold_ttfSize);
    }

    inline Font getFontDosisBold()
    {
        return getFont(BinaryData::DosisBold_ttf, BinaryData::DosisBold_ttfSize);
    }

    inline Font getFontDosisExtraBold()
    {
        return getFont(BinaryData::DosisExtraBold_ttf, BinaryData::DosisExtraBold_ttfSize);
    }

    inline Font getFontDosisLight()
    {
        return getFont(BinaryData::DosisLight_ttf, BinaryData::DosisLight_ttfSize);
    }

    inline Font getFontDosisExtraLight()
    {
        return getFont(BinaryData::DosisExtraLight_ttf, BinaryData::DosisExtraLight_ttfSize);
    }

    inline Font getFontDosisMedium()
    {
        return getFont(BinaryData::DosisMedium_ttf, BinaryData::DosisMedium_ttfSize);
    }

    inline Font getFontDosisRegular()
    {
        return getFont(BinaryData::DosisRegular_ttf, BinaryData::DosisRegular_ttfSize);
    }

    inline Font getFontDosisVariable()
    {
        return getFont(BinaryData::DosisVariableFont_wght_ttf, BinaryData::DosisVariableFont_wght_ttfSize);
    }
}