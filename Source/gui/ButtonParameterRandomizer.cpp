#include "ButtonParameterRandomizer.h"

namespace gui
{
    ButtonParameterRandomizer::ButtonParameterRandomizer(Utils& u) :
        Button(u, makeTooltip()),
        randomizables(),
        randFuncs()
    {
        makeSymbolButton(*this, ButtonSymbol::Random);
    }

    void ButtonParameterRandomizer::add(Param* p)
    {
        randomizables.push_back(p);
    }

    void ButtonParameterRandomizer::add(std::vector<Param*>&& p)
    {
        randomizables = p;
    }

    void ButtonParameterRandomizer::add(const std::vector<Param*>& p)
    {
        randomizables = p;
    }

    void ButtonParameterRandomizer::add(const RandFunc& p)
    {
        randFuncs.push_back(p);
    }

    void ButtonParameterRandomizer::operator()(bool isAbsolute)
    {
        juce::Random rand;
        for (auto& func : randFuncs)
            func(rand);
		if(isAbsolute)
        {
            for (auto randomizable : randomizables)
                if (randomizable->id != PID::Power && randomizable->id != PID::Clipper)
                {
                    const auto& range = randomizable->range;

                    const auto v = rand.nextFloat();
                    const auto vD = range.convertFrom0to1(v);
                    const auto vL = range.snapToLegalValue(vD);
                    const auto vF = range.convertTo0to1(vL);

                    randomizable->setValueWithGesture(vF);
                }
        }
		else
		{
			for (auto randomizable : randomizables)
				if (randomizable->id != PID::Power && randomizable->id != PID::Clipper)
				{
					const auto valNorm = randomizable->getValue();
                    const auto valRand = rand.nextFloat() * .05f - .025f;
                    const auto nValNorm = juce::jlimit(0.f, 1.f, valNorm + valRand);

                    randomizable->setValueWithGesture(nValNorm);
				}
		}
    }

    void ButtonParameterRandomizer::mouseUp(const Mouse& mouse)
    {
        if (mouse.mouseWasDraggedSinceMouseDown()) return;
        bool isAbsolute = !mouse.mods.isShiftDown();
        this->operator()(isAbsolute);
        setTooltip(makeTooltip());
        Button::mouseUp(mouse);
        Comp::mouseEnter(mouse);
    }

    void ButtonParameterRandomizer::mouseExit(const Mouse&)
    {
        setTooltip(makeTooltip());
        repaint();
    }

    String ButtonParameterRandomizer::makeTooltip()
    {
        Random rand;
        static constexpr float count = 228.f;
        const auto v = static_cast<int>(std::round(rand.nextFloat() * count));
        switch (v)
        {
        case 0: return "Do it!";
        case 1: return "Don't you dare it!";
        case 2: return "But... what if it goes wrong???";
        case 3: return "Nature is random too, so this is basically analog, right?";
        case 4: return "Life is all about exploration..";
        case 5: return "What if I don't even exist?";
        case 6: return "Idk, it's all up to you.";
        case 7: return "This randomizes the parameter values. Yeah..";
        case 8: return "Born too early to explore space, born just in time to hit the randomizer.";
        case 9: return "Imagine someone sitting there writing down all these phrases.";
        case 10: return "This will not save your snare from sucking ass.";
        case 11: return "Producer-san >.< d.. don't tickle me there!!!";
        case 12: return "I mean, whatever.";
        case 13: return "Never commit. Just dream!";
        case 14: return "I wonder, what will happen if I...";
        case 15: return "Hit it for the digital warmth.";
        case 16: return "Do you love cats? They are so cute :3";
        case 17: return "We should collab some time, bro.";
        case 18: return "Did you just hover the button to see what's in here this time?";
        case 19: return "It's not just a phase!";
        case 20: return "No time for figuring out parameter values manually, right?";
        case 21: return "My cat is meowing at the door because there is a mouse.";
        case 22: return "Yeeeaaaaahhhh!!!! :)";
        case 23: return "Ur hacked now >:) no just kidding ^.^";
        case 24: return "What would you do if your computer could handle 1mil phasers?";
        case 25: return "It's " + (juce::Time::getCurrentTime().getHours() < 10 ? juce::String("0") + static_cast<juce::String>(juce::Time::getCurrentTime().getHours()) : static_cast<juce::String>(juce::Time::getCurrentTime().getHours())) + ":" + (juce::Time::getCurrentTime().getMinutes() < 10 ? juce::String("0") + static_cast<juce::String>(juce::Time::getCurrentTime().getMinutes()) : static_cast<juce::String>(juce::Time::getCurrentTime().getMinutes())) + " o'clock now.";
        case 26: return "I once was a beat maker, too, but then I took a compressor to the knee.";
        case 27: return "It's worth a try.";
        case 28: return "Omg, your music is awesome dude. Keep it up!";
        case 29: return "I wish there was an anime about music producers.";
        case 30: return "Days are too short, but I also don't want gravity to get heavier.";
        case 31: return "Yo, let's order some pizza!";
        case 32: return "I wanna be the very best, like no one ever was!!";
        case 33: return "Hm... yeah, that could be cool.";
        case 34: return "Maybe...";
        case 35: return "Well.. perhaps.";
        case 36: return "Here we go again.";
        case 37: return "What is the certainty of a certainty meaning a certain certainty?";
        case 38: return "My favourite car is an RX7 so i found it quite funny when Izotope released that plugin.";
        case 39: return "Do you know Echobode? It's one of my favourite plugins.";
        case 40: return "I never managed to make a proper eurobeat even though I love that genre.";
        case 41: return "Wanna lose control?";
        case 42: return "Do you have any more of dem randomness pls?";
        case 43: return "How random do you want it to be, sir? Yes.";
        case 44: return "Programming is not creative. I am a computer.";
        case 45: return "We should all be more mindful to each other.";
        case 46: return "Next-Level AI will randomize ur parameters!";
        case 47: return "All The Award-Winning Audio-Engineers Use This Button!!";
        case 48: return "The fact that you can't undo it only makes it better.";
        case 49: return "When things are almost as fast as light, reality bends.";
        case 50: return "I actually come from the future. Don't tell anyone pls.";
        case 51: return "You're mad!";
        case 52: return "Your ad could be here! ;)";
        case 53: return "What colour-Scheme does your tune sound like?";
        case 54: return "I wish Dyson Spheres existed already!";
        case 55: return "This is going to be so cool! OMG";
        case 56: return "Plants. There should be more of them.";
        case 57: return "10 Vibrato Mistakes Every Noob Makes: No. 7 Will Make U Give Up On Music!";
        case 58: return "Yes, I'll add more of these some other time.";
        case 59: return "The world wasn't ready for No Man's Sky. That's all.";
        case 60: return "Temposynced Tremolos are not Sidechain Compressors.";
        case 61: return "I can't even!";
        case 62: return "Let's drift off into the distance together..";
        case 63: return "When I started making NEL I wanted to make a tape emulation.";
        case 64: return "Scientists still trying to figure this one out..";
        case 65: return "Would you recommend this button to your friends?";
        case 66: return "This is a very bad feature. Don't use it!";
        case 67: return "I don't know what to say about this button..";
        case 68: return "A parallel universe, in which you will use this button now, exists.";
        case 69: return "This is actually message no. 69, haha";
        case 70: return "Who needs control anyway?";
        case 71: return "I have the feeling this time it will turn out really cool!";
        case 72: return "Turn all parameters up right to 11.";
        case 73: return "Tranquilize Your Music. Idk why, but it sounds cool.";
        case 74: return "I'm indecisive...";
        case 75: return "That's a good idea!";
        case 76: return "Once upon a time there was a traveller who clicked this button..";
        case 77: return "10/10 Best Decision!";
        case 78: return "Beware! Only really skilled audio professionals use this feature.";
        case 79: return "What would be your melody's name if it was a human being?";
        case 80: return "What if humanity was just a failed experiment by a higher species?";
        case 81: return "Enter the black hole to stop time!";
        case 82: return "Did you remember to water your plants yet?";
        case 83: return "I'm just a simple button. Nothing special to see here.";
        case 84: return "You're using this plugin. That makes you a cool person.";
        case 85: return "Only the greatest DSP technology in this parameter randomizer!";
        case 86: return "I am not fun at parties indeed.";
        case 87: return "This button makes it worse!";
        case 88: return "I am not sure what this is going to do.";
        case 89: return "If your music was a mountain, what shape would it be like?";
        case 90: return "NEL is the best Vibrato Plugin in the world. Tell all ur friends!";
        case 91: return "Do you feel the vibrations?";
        case 92: return "Defrost or Reheat? You decide.";
        case 93: return "Don't forget to hydrate yourself, king/queen.";
        case 94: return "How long does it take to get to the next planet at this speed?";
        case 95: return "What if there is a huge wall around the whole universe?";
        case 96: return "Controlled loss of control. So basically drifting! Yeah!";
        case 97: return "I talk to the wind. My words are all carried away.";
        case 98: return "Captain, we need to warp now! There is no time.";
        case 99: return "Where are we now?";
        case 100: return "Randomize me harder, daddy!";
        case 101: return "Drama!";
        case 102: return "Turn it up! Well, this is not a knob, but you know, it's cool.";
        case 103: return "You like it dangerous, huh?";
        case 104: return "We are under attack.";
        case 105: return "Yes, you want this!";
        case 106: return "The randomizer is better than your presets!";
        case 107: return "Are you a decide-fan, or a random-enjoyer?";
        case 108: return "Let's get it started! :D";
        case 109: return "Do what you have to do...";
        case 110: return "This is a special strain of random. ;)";
        case 111: return "Return to the battlefield or get killed.";
        case 112: return "~<* Easy Peazy Lemon Squeezy *>~";
        case 113: return "Why does it sound like dubstep?";
        case 114: return "Excuse me.. Have you seen my sanity?";
        case 115: return "In case of an emergency, push the button!";
        case 116: return "Based.";
        case 117: return "Life is a series of random collisions.";
        case 118: return "It is actually possible to add too much salt to spaghetti.";
        case 119: return "You can't go wrong with random, except when you do.";
        case 120: return "I have not implemented undo yet, but you like to live dangerously :)";
        case 121: return "404 - Creative message not found. Contact our support pls.";
        case 122: return "Press jump twice to perform a doub.. oh wait, wrong app.";
        case 123: return "And now for the ultimate configuration!";
        case 124: return "Subscribe for more random messages! Only 40$/mon";
        case 125: return "I love you <3";
        case 126: return "Me? Well...";
        case 127: return "What happens if I press this?";
        case 128: return "Artificial Intelligence! Not used here, but it sounds cool.";
        case 129: return "My internet just broke so why not just write another msg in here, right?";
        case 130: return "Mood.";
        case 131: return "I'm only a randomizer, after all...";
        case 132: return "There is a strong correlation between you and awesomeness.";
        case 133: return "Yes! Yes! Yes!";
        case 134: return "Up for a surprise?";
        case 135: return "This is not a plugin. It is electricity arranged swag-wise.";
        case 136: return "Chairs do not exist.";
        case 137: return "There are giant spiders all over my house and I have no idea what to do :<";
        case 138: return "My cat is lying on my lap purring and she's so cute omg!!";
        case 139: return "I come here and add more text whenever I procrastinate from fixing bugs.";
        case 140: return "Meow :3";
        case 141: return "N.. Nyan? uwu";
        case 142: return "Let's Go!";
        case 143: return "Never Gonna Let You Down! Never Gonna Give You Up! sry..";
        case 144: return "Push It!";
        case 145: return "Do You Feel The NRG??";
        case 146: return "We could escape the great filter if we only vibed stronger..";
        case 147: return "Check The Clock. It's time for randomization.";
        case 148: return "The first version of NEL was released in 2019.";
        case 149: return "My first plugin NEL was named after my son, Lionel.";
        case 150: return "If this plugin breaks, it's because your beat is too fire!";
        case 151: return "Go for it!";
        case 152: return "<!> nullptr exception: please contact the developer. <!>";
        case 153: return "Wild Missingno. appeared!";
        case 154: return "Do you have cats? Because I love cats. :3";
        case 155: return "There will be a netflix adaption of this plugin soon.";
        case 156: return "Studio Gib Ihm!";
        case 157: return "One Click And It's Perfect!";
        case 158: return "Elon Musk just twittered that this plugin is cool. Wait.. is that a good thing?";
        case 159: return "Remember to drink water, sempai!";
        case 160: return "Love <3";
        case 161: return "Your journey has just begun ;)";
        case 162: return "You will never be the same again...";
        case 163: return "Black holes are just functors that create this universe inside of this universe.";
        case 164: return "Feel the heat!";
        case 165: return "There is no going back. (Literally, because I didn't implement undo/redo)";
        case 166: return "Tbh, that would be crazy.";
        case 167: return "Your horoscope said you'll make the best beat of all time today.";
        case 168: return "Do it again! :)";
        case 169: return "Vibrato is not equal Vibrato, dude.";
        case 170: return "This is going to be all over the place!";
        case 171: return "Pitch and time... it belongs together.";
        case 172: return "A rainbow is actually transcendence that never dies.";
        case 173: return "It is not random. It is destiny!";
        case 174: return "Joy can enable you to change the world.";
        case 175: return "This is a very unprofessional plugin. It sucks the 'pro' out of your music.";
        case 176: return "They tried to achieve perfection, but they didn't hear this beat yet.";
        case 177: return "Dream.";
        case 178: return "Music is a mirror of your soul and has the potential to heal.";
        case 179: return "Lmao, nxt patch is going to be garbage!";
        case 180: return "Insanity is doing the same thing over and over again.";
        case 181: return "If you're uninspired just do household-chores. Your brain will try to justify coming back.";
        case 182: return "You are defining the future standard!";
        case 183: return "Plugins are a lot like games, but you can't speedrun them.";
        case 184: return "When applying NEL's audiorate mod to mellow sounds it can make them sorta trumpet-ish.";
        case 185: return "NEL's oversampling is lightweight and reduces aliasing well, but it also alters the sound a bit.";
        case 186: return "This message was added 2022_03_15 at 18:10! just in case you wanted to know..";
        case 187: return "This is message no 187. Ratatatatatat.";
        case 188: return "Did you ever look at some font like 'This is the font I wanna use for the rest of my life'? Me neither.";
        case 189: return "Rightclick on the power button and lock its parameter to prevent it from being randomized.";
        case 190: return "idk...";
        case 191: return "This is just a placeholder for a real tooltip message.";
        case 192: return "Let's drift into a new soundscape!";
        case 193: return "This is the most generic tooltip.";
        case 194: return "Diffusion can be inharmonic, yet soothing.";
        case 195: return "I always wanted to make a plugin that has something to do with temperature..";
        case 196: return "You can't spell 'random' without 'awesome'.";
        case 197: return "Do you want a 2nd opinion on that?";
        case 198: return "This is essentially gambling, but without wasting money.";
        case 199: return "You can lock parameters in order to avoid randomizing them.";
        case 200: return "Right-click parameters in order to find additional options.";
        case 201: return "Turn it up to 11 for turbo mode! Oh, oops, wrong parameter. sryy";
        case 202: return "Bleep bloop. :> I am a computer! Hihi";
        case 203: return "Mold is basically just tiny mushrooms. You should still not eat it tho.";
        case 204: return "I wish there was a producer anime, where they'd fight for the best beats.";
        case 205: return "These tooltip messages have a deep lore.";
        case 206: return "You can't spell 'awesome' without 'random'. Well, you can, but you shouldn't.";
        case 207: return "Not in the mood today. Please use a different button! :/";
        case 208: return "I know you really want to click on this button, but I want you to try something else instead.";
        case 209: return "Sweet.";
        case 210: return "OMG I wanna tell you everything about me and my parameter-friends!!!";
        case 211: return "Take your phone number and add or subtract 1 from it! That's your textdoor neighbor.";
        case 212: return "Um.. ok?";
        case 213: return "I need more coffee.";
        case 214: return "Btw if you have some spare money to share, there is a donate button in the settings :>";
        case 215: return "Beware! This button causes quantum entanglement.";
        case 216: return "Pink is the craziest colour, because it's a mix between the lowest and highest perceivable frequencies of light.";
        case 217: return "You can't see mirrors. You can only see what mirrors show. What does a mirror look like?";
		case 218: return "If I was you, I would try to make a beat that sounds like a randomizer.";
		case 219: return "I am a computer. I am not a human. I am not a human. I am not a human. I am not a human. I am not a";
		case 220: return "In the future there will be more tooltip messages.";
		case 221: return "Refresh yourself with a cold shower!";
		case 222: return "This is the last tooltip message. I promise.";
		case 223: return "Insanity is the only way to achieve perfection.";
		case 224: return "Destructive forces cause constructive changes.";
        case 225: return "Division by Zero is neither undefined nor infinite, but simply insane.";
        case 226: return "Hold shift as you click this button to randomize sensitively.";
		case 227: return "Did you know you can hold shift to randomize sensitively?";
		case 228: return "If you hold shift while clicking on this button, it will randomize sensitively.";
        default: "Are you sure?";
        }
        return "You are not supposed to read this message!";
    }
}