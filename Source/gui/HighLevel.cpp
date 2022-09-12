#include "HighLevel.h"

namespace gui
{
	HighLevel::HighLevel(Utils& u, LowLevel* _lowLevel, CompWidgetable* tuningEditor) :
		Comp(u, "", CursorType::Default),
#if PPDHasPatchBrowser
		patchBrowser(u),
		patchBrowserButton(u, patchBrowser),
#endif
		tuningEditorButton(u, tuningEditor),
		macro(u),
		modDepthLocked(u, "(Un-)Lock this patch's modulation patch."),
		swapParamWithModDepth(u, "Swap parameter patch with modulation patch."),
		saveModPatch(u, "Save the current Modulation Patch to disk."),
		loadModPatch(u, "Load some Modulation Patch from disk."),
		removeCurModPatch(u, "Remove all current modulations from this patch."),

		parameterRandomizer(u),
#if PPDHasGainIn
		gainIn(u),
#endif
		gainOut(u),
		mix(u),
#if PPDHasUnityGain && PPDHasGainIn
		unityGain(u, param::toTooltip(PID::UnityGain)),
#endif
#if PPDHasHQ
		hq(u, param::toTooltip(PID::HQ)),
#endif
#if PPDHasStereoConfig
		stereoConfig(u, param::toTooltip(PID::StereoConfig)),
#endif
		power(u, param::toTooltip(PID::Power)),
#if PPDHasPolarity
		polarity(u, param::toTooltip(PID::Polarity)),
#endif

		ccMonitor(u, u.getMIDILearn()),
		midiVoices(u),

		lowLevel(_lowLevel),

		menu(nullptr),
		menuButton(u, "Click here to open or close the panel with the advanced settings."),

		fileChooser(nullptr)
	{
#if PPDHasPatchBrowser
		layout.init
		(
			{ 1, 8, 1, 8, 1, 8, 1, 8, 1, 1 },
			{ 1, 8, 1, 5, 1, 21, 1, 21, 1, 21, 1, 8, 8, 2 }
		);
#else
		layout.init
		(
			{ 1, 8, 1, 8, 1, 8, 1, 8, 1, 1 },
			{ 1, 8, 1, 21, 1, 21, 1, 21, 1, 8, 8, 2 }
		);
#endif

		addAndMakeVisible(modDepthLocked);
		{
			auto& params = utils.getParams();

			modDepthLocked.toggleState = params.isModDepthLocked();

			modDepthLocked.onClick.push_back([&prms = params](Button&)
				{
					prms.switchModDepthLocked();
				});
			modDepthLocked.onClick.push_back([](Button& btn)
				{
					btn.toggleState = btn.toggleState == 0 ? 1 : 0;
				});

			makeSymbolButton(modDepthLocked, ButtonSymbol::ModDepthLock, 1);
		}

		addAndMakeVisible(swapParamWithModDepth);
		{
			auto& params = utils.getParams();

			swapParamWithModDepth.onClick.push_back([&prms = params](Button&)
				{
					for (auto i = static_cast<int>(prms.numParams() - 1); i > 0; --i)
					{
						auto& param = *prms[i];

						const auto p = param.getValue();
						const auto d = param.getMaxModDepth();
						const auto b = param.getModBias();

						const auto m = p + d;

						param.setValueWithGesture(m);
						param.setMaxModDepth(-d);
						param.setModBias(1.f - b);
					}

					auto& macro = *prms[PID::Macro];
					macro.setValueWithGesture(1.f - macro.getValue());

				});

			makeSymbolButton(swapParamWithModDepth, ButtonSymbol::SwapParamModDepth);
		}

		addAndMakeVisible(saveModPatch);
		{
			saveModPatch.onClick.push_back([](Button& btn)
			{
				auto& utils = btn.getUtils();
				const auto& params = utils.getParams();

				sta::State modPatch;

				for (auto i = 1; i < param::NumParams; ++i)
				{
					const auto& prm = *params[i];
					const String key(param::Param::getIDString(prm.id));
					String id("value");
					const auto val = prm.range.convertFrom0to1(prm.calcValModOf(1.f));

					modPatch.set(key, id, val, false);
				}

				auto& props = utils.getProps();
				auto& user = *props.getUserSettings();
				auto file = user.getFile();
				auto pathStr = file.getFullPathName();
				for (auto i = pathStr.length() - 1; i != 0; --i)
					if (pathStr.substring(i, i + 1) == File::getSeparatorString())
					{
						pathStr = pathStr.substring(0, i + 1) + "Patches";
						break;
					}
						

				file = File(pathStr);
				if (!file.isDirectory())
					file.createDirectory();

				const auto author = user.getValue("patchBrowserLastAuthor", "user");
				String name = "modPatch";

				const auto fileTypes = File::TypesOfFileToFind::findFiles;
				const String extension(".patch");
				const auto wildCard = "*" + extension;
				const RangedDirectoryIterator files(
					file,
					true,
					wildCard,
					fileTypes
				);

				auto idx = 0;
				for (const auto& it : files)
				{
					const auto patchFile = it.getFile();
					if (patchFile.getFileName().contains(name))
						++idx;
				}

				name += String(idx);

				file = File(file.getFullPathName() + "\\" + getFileName(name, author));
				file.appendText(modPatch.getState().toXmlString());

				file.revealToUser();
			});

			makeSymbolButton(saveModPatch, ButtonSymbol::Save);
		}
		
		addAndMakeVisible(loadModPatch);
		{
			loadModPatch.onClick.push_back([&](Button&)
			{
				auto& props = utils.getProps();
				auto& user = *props.getUserSettings();
				auto file = user.getFile();
				auto pathStr = file.getFullPathName();
				for (auto i = pathStr.length() - 1; i != 0; --i)
					if (pathStr.substring(i, i + 1) == File::getSeparatorString())
					{
						pathStr = pathStr.substring(0, i + 1) + "Patches";
						break;
					}

				file = File(pathStr);
				if (!file.isDirectory())
					file.createDirectory();

				const auto fileTypes = File::TypesOfFileToFind::findFiles;
				const String extension(".patch");
				const auto wildCard = "*_-_*" + extension;
				const RangedDirectoryIterator files(
					file,
					true,
					wildCard,
					fileTypes
				);

				fileChooser = std::make_unique<FileChooser>(
					"Load ModPatch",
					file,
					wildCard
				);

				using Flag = juce::FileBrowserComponent::FileChooserFlags;
				auto flags = Flag::canSelectFiles
					+ !Flag::canSelectMultipleItems
					+ Flag::openMode;
				fileChooser->launchAsync(flags, [&u = utils, &chooser = fileChooser](const FileChooser& fc)
				{
					auto result = fc.getResult();
					if (!result.existsAsFile())
						return;

					sta::State modPatch(result.loadFileAsString());
					auto& params = u.getParams();

					for (auto i = 1; i < param::NumParams; ++i)
					{
						const auto id = static_cast<PID>(i);
						const auto idStr = param::Param::getIDString(id);

						const auto mmdPtr = modPatch.get(idStr, "value");
						if (mmdPtr != nullptr)
						{
							auto& prm = *params[i];

							const auto val = prm.getValue();
							const auto mmd = prm.range.convertTo0to1(static_cast<float>(*mmdPtr));
							
							prm.setMaxModDepth(mmd - val);
							prm.setModBias(.5f);
						}
					}

					chooser.reset(nullptr);
				});
			});

			makeSymbolButton(loadModPatch, ButtonSymbol::Load);
		}

		addAndMakeVisible(removeCurModPatch);
		{
			removeCurModPatch.onClick.push_back([](Button& btn)
			{
				auto& utils = btn.getUtils();
				auto& params = utils.getParams();

				for (auto i = 0; i < param::NumParams; ++i)
				{
					auto& param = *params[i];
					param.setModBias(.5f);
					param.setMaxModDepth(0.f);
				}
			});

			makeSymbolButton(removeCurModPatch, ButtonSymbol::Remove);
		}

		makeParameter(macro, PID::Macro, "Macro", false);

#if PPDHasPatchBrowser
		addAndMakeVisible(patchBrowserButton);
#endif
		addAndMakeVisible(tuningEditorButton);

		addAndMakeVisible(macro);
		addAndMakeVisible(parameterRandomizer);
		parameterRandomizer.add(utils.getAllParams());
#if PPDHasGainIn
		makeParameter(gainIn, PID::GainIn, "In", true, &utils.getMeter(0));
		addAndMakeVisible(gainIn);
#if PPDHasUnityGain
		makeParameterSwitchButton(unityGain, PID::UnityGain, ButtonSymbol::UnityGain);
		addAndMakeVisible(unityGain);
#endif
#endif
		makeParameter(gainOut, PID::Gain, "Out", true, &utils.getMeter(PPDHasGainIn ? 1 : 0));
		addAndMakeVisible(gainOut);
		makeParameter(mix, PID::Mix, "Mix");
		addAndMakeVisible(mix);
#if PPDHasHQ
		makeParameterSwitchButton(hq, PID::HQ, "HQ");
		hq.getLabel().mode = Label::Mode::TextToLabelBounds;
		addAndMakeVisible(hq);
#endif
#if PPDHasStereoConfig
		makeParameterSwitchButton(stereoConfig, PID::StereoConfig, ButtonSymbol::StereoConfig);
		stereoConfig.getLabel().mode = Label::Mode::TextToLabelBounds;
		addAndMakeVisible(stereoConfig);
#endif
		makeParameterSwitchButton(power, PID::Power, ButtonSymbol::Power);
		addAndMakeVisible(power);
#if PPDHasPolarity
		makeParameterSwitchButton(polarity, PID::Polarity, ButtonSymbol::Polarity);
		addAndMakeVisible(polarity);
#endif
		addAndMakeVisible(ccMonitor);

		makeSymbolButton(menuButton, ButtonSymbol::Settings);
		menuButton.toggleState = 0;
		menuButton.onClick.push_back([this](Button& btn)
			{
				auto& ts = btn.toggleState;
				ts = ts == 0 ? 1 : 0;
				repaintWithChildren(&btn);

				if (ts == 1)
				{
					auto& pluginTop = utils.pluginTop;

					const auto xml = loadXML(BinaryData::menu_xml, BinaryData::menu_xmlSize);
					if (xml == nullptr)
						return;
					const auto vt = ValueTree::fromXml(*xml);
					if (!vt.isValid())
						return;

					menu.reset(new Menu(utils, vt));
					pluginTop.addAndMakeVisible(*menu);

					const auto bounds1 = lowLevel->getBounds().toFloat();
					const auto bounds0 = bounds1.withLeft(static_cast<float>(pluginTop.getRight()));

					menu->defineBounds(bounds0, bounds1);
					menu->initWidget(.1f, false);
				}
				else
				{
					menu.reset(nullptr);
				}
			});
		addAndMakeVisible(menuButton);

		addAndMakeVisible(midiVoices);

		setInterceptsMouseClicks(false, true);
	}

	void HighLevel::init()
	{
#if PPDHasPatchBrowser
		auto& pluginTop = utils.pluginTop;
		pluginTop.addChildComponent(patchBrowser);
#endif
	}

	void HighLevel::paint(Graphics& g)
	{
		g.setFont(getFontDosisMedium());
		g.setColour(Colours::c(ColourID::Hover));

		g.fillRect(layout.right());

		const auto thicc = utils.thicc;
		const auto thicc3 = thicc * 3.f;
		const Stroke stroke(thicc, Stroke::JointStyle::curved, Stroke::EndCapStyle::rounded);

		const auto macroArea = layout(1.f, 5.f, 7.f, 1.f);
		g.drawFittedText("Modulation:", macroArea.toNearestInt(), Just::centredTop, 1);
		drawRectEdges(g, macroArea, thicc3, stroke);

#if PPDHasGainIn
		const auto gainArea = layout(1.f, 7.f, 7.f, 1.f);
		g.drawFittedText("Gain:", gainArea.toNearestInt(), Just::centredTop, 1);
		drawRectEdges(g, gainArea, thicc3, stroke);
#endif
	}

	void HighLevel::resized()
	{
		layout.resized();

		layout.place(tuningEditorButton, 3.f, 1.f, 1.f, 1.f, true);
		layout.place(menuButton, 5.f, 1.f, 1.f, 1.f, true);
		layout.place(parameterRandomizer, 7.f, 1.f, 1.f, 1.f, true);

#if PPDHasPatchBrowser
		layout.place(patchBrowserButton, 1.f, 3.f, 7.f, 1.f, false);
#endif
		const auto patchBrowserOffset = PPDHasPatchBrowser ? 2.f : 0.f;
		
		layout.place(macro, 3.f, 3.f + patchBrowserOffset + .2f, 3.f, .8f, false);

		layout.place(saveModPatch, 1.f, 3.f + patchBrowserOffset, 1.f, .3333f, true);
		layout.place(loadModPatch, 1.f, 3.f + patchBrowserOffset + .3333f, 1.f, .3333f, true);
		layout.place(removeCurModPatch, 1.f, 3.f + patchBrowserOffset + .6666f, 1.f, .3333f, true);

		layout.place(modDepthLocked, 7.f, 3.f + patchBrowserOffset, 1.f, .5f, true);
		layout.place(swapParamWithModDepth, 7.f, 3.5f + patchBrowserOffset, 1.f, .5f, true);

#if PPDHasGainIn
		layout.place(gainIn, 1.f, 5.f + patchBrowserOffset, 2.5f, 2.f, true);
#if PPDHasUnityGain
		layout.place(unityGain, 3.6f, 5.2f + patchBrowserOffset, 1.8f, .6f, true);
#endif
		layout.place(gainOut, 5.5f, 5.f + patchBrowserOffset, 2.5f, 2.f, true);
#else
		layout.place(gainOut, 3.f, 5.2f + patchBrowserOffset, 3.f, 1.6f, true);
#endif

		layout.place(mix, 3.f, 7.f + patchBrowserOffset, 3.f, 1.f, true);

		layout.place(power, 1.f, 9.f + patchBrowserOffset, 1.f, 1.f, true);
#if PPDHasPolarity
		layout.place(polarity, 3.f, 9.f + patchBrowserOffset, 1.f, 1.f, true);
#endif
#if PPDHasStereoConfig
		layout.place(stereoConfig, 5.f, 9.f + patchBrowserOffset, 1.f, 1.f, true);
#endif
		layout.place(hq, 7.f, 9.f + patchBrowserOffset, 1.f, 1.f, true);

		layout.place(ccMonitor, 1.f, 10.f + patchBrowserOffset, 7.f, 1.f, false);
		layout.place(midiVoices, 1.f, 11.f + patchBrowserOffset, 7.f, 1.f, false);

#if PPDHasPatchBrowser
		patchBrowser.setBounds(lowLevel->getBounds());
#endif

		if (menu != nullptr)
		{
			menu->defineBounds(menu->getBounds().toFloat(), lowLevel->getBounds().toFloat());
			menu->initWidget(.3f, false);
		}
	}
}