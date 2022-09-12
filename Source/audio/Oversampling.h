#pragma once
#include "AudioUtils.h"
#include "WHead.h"
#include <array>
#include <vector>

namespace audio
{
	struct ImpulseResponse
	{
		ImpulseResponse(const std::vector<float> & = { 1.f });

		ImpulseResponse(const ImpulseResponse&);

		void operator=(const std::vector<float>&);

		float operator[](int i) const noexcept;
		
		const size_t size() const noexcept;

		const int getLatency() const noexcept;

		const float* data() const noexcept;
	protected:
		std::vector<float> buf;
		int latency;
	};

	/*
	* Fs,fc,bw,upsampling
	Nyquist == Fs / 2
	fc < Nyquist
	bw < Nyquist
	fc + bw < Nyquist
	*/
	std::vector<float> makeWindowedSinc(float, float, float, bool);

	/*
	* Fs,fc,upsampling
	Nyquist == Fs / 2
	fc < Nyquist
	*/
	std::vector<float> makeWindowedSinc(float, float, bool);

	struct Convolver
	{
		Convolver(const ImpulseResponse&, const WHead&);

		void prepare();

		/*samples,numChannels,numSamples*/
		void processBlock(float**, int, int) noexcept;

	protected:
		AudioBuffer ring;
		const ImpulseResponse& ir;
		const WHead& wHead;
		int irSize;

	private:
		/*smpls,rng,numSamples*/
		void processBlock(float*, float*, int) noexcept;
		/*smpl,rng,w*/
		float processSample(float, float*, int) noexcept;
	};

	/*samplesUp,samplesIn,numChannels,numSamples1x*/
	void zeroStuff(float**, const float**, int, int) noexcept;

	/*samplesOut,samplesUp,numChannels,numSamples1x*/
	void decimate(float**, const float**, int, int) noexcept;

	class Oversampler
	{
		static constexpr float CutoffFreq = 18000.f;
	public:
		Oversampler();

		Oversampler(Oversampler&);

		/*sampleRate,blockSize*/
		void prepare(const double, const int);

		/*inputBuffer*/
		AudioBuffer& upsample(AudioBuffer&) noexcept;

		/*outputBuffer*/
		void downsample(AudioBuffer&) noexcept;

		const int getLatency() const noexcept;
		
		double getFsUp() const noexcept;
		
		int getBlockSizeUp() const noexcept;

		bool isEnabled() const noexcept;

		/* only call this if processor is suspended! */
		void setEnabled(bool) noexcept;
	protected:
		double Fs;
		int blockSize;

		AudioBuffer buffer;

		ImpulseResponse irUp, irDown;
		WHead wHead;
		Convolver filterUp, filterDown;

		double FsUp;
		int blockSizeUp;

		int numSamples1x, numSamples2x;

		std::atomic<bool> enabled;
		bool enbld;
	};

}