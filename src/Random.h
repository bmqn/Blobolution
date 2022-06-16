#pragma once

class Random
{
private:
	using DistType = std::mt19937::result_type;
	
	static constexpr DistType kMaxDistTypeValue = std::numeric_limits<DistType>::max();

public:
	static void Create()
	{
		s_RandomGenerator.seed(std::random_device()());
	}

	static void Destroy() {}

	static bool Bool()
	{
		return Float<float>() < 0.5f;
	}

	template<
		typename I,
		std::enable_if_t<std::is_integral_v<I>, bool> = true
	>
	static I Int(I min, I max)
	{
		static constexpr I kMaxValue = std::numeric_limits<I>::max();

		I retVal;

		if (min >= max)
		{
			retVal = min;
		}
		else
		{
			retVal = min + (static_cast<I>(
				GetFromDist() % kMaxValue) % ((max + 1) - min)
			);
		}

		return retVal;
	}

	template<
		typename F,
		std::enable_if_t<std::is_floating_point_v<F>, bool> = true
	>
	static F Float(F min = 0, F max = 1)
	{
		F retVal;

		if (min >= max)
		{
			retVal = min;
		}
		else
		{
			retVal = min + ( static_cast<F>(GetFromDist())
			               / static_cast<F>(kMaxDistTypeValue)) * (max - min);
		}

		return retVal;
	}

private:
	static DistType GetFromDist() { return s_Distribution(s_RandomGenerator); }

private:
	static std::mt19937 s_RandomGenerator;
	static std::uniform_int_distribution<DistType> s_Distribution;
};