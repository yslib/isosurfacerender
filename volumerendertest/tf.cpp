
#include "tf.h"

namespace ysl
{
	void ColorInterpulator::AddColorKey(float intensity, ysl::Color color)
	{
		const auto spectrum = TranslateColor(color);
		AddColorKey(intensity, spectrum);
	}

	void ColorInterpulator::AddColorKey(float intensity, const RGBASpectrum& spectrum)
	{
		keys.emplace_back(intensity, spectrum, spectrum);
		m_valid = false;
	}

	void ColorInterpulator::Sort()
	{
		if (keys.empty())
			return;
		std::sort(keys.begin(), keys.end(), [](const MappingKey & k1, const MappingKey & k2) {return k1.Intensity() < k2.Intensity(); });
		m_valid = true;
	}

	void ColorInterpulator::Read(const std::string& fileName)
	{
		FILE* fp = fopen(fileName.c_str(), "r");
		if (!fp)
		{
			m_valid = false;
			return;
		}
		int keyNum;
		fscanf(fp, "%d %f %f\n", &keyNum, &leftThreshold, &rightThreshold);

		keys.reserve(keyNum);

		float intensity;
		int rl, gl, bl, al, rr, gr, br, ar;
		Float rgba1[4], rgba2[4];
		for (auto i = 0; i < keyNum; ++i)
		{
			fscanf(fp, "%f %d %d %d %d %d %d %d %d\n", &intensity,
				&rl, &gl, &bl, &al, &rr, &gr, &br, &ar);
			rgba1[0] = rl / 255.0f;
			rgba1[1] = gl / 255.0f;
			rgba1[2] = bl / 255.0f;
			rgba1[3] = al / 255.0f;
			rgba2[0] = rr / 255.0f;
			rgba2[1] = gr / 255.0f;
			rgba2[2] = br / 255.0f;
			rgba2[3] = ar / 255.0f;

			const auto s1 = ysl::RGBASpectrum{ rgba1 };
			const auto s2 = ysl::RGBASpectrum{ rgba2 };

			keys.emplace_back(intensity, s1, s2);
		}
		m_valid = true;
		fclose(fp);
	}

	void ColorInterpulator::FetchData(RGBASpectrum* transferFunction, int dimension)const
	{
		const auto frontEnd = static_cast<int>(std::floor(leftThreshold * dimension + 0.5));
		const auto backStart = static_cast<int>(std::floor(rightThreshold * dimension + 0.5));
		//all values before front_end and after back_start are set to zero
		//all other values remain the same
		for (int x = 0; x < frontEnd; ++x)
			transferFunction[x] = RGBASpectrum{ 0.f };

		auto keyIterator = keys.begin();
		RGBASpectrum color;
		for (auto x = frontEnd; x < backStart; ++x)
		{
			const auto value = static_cast<float>(x) / (dimension - 1);
			while ((keyIterator != keys.end()) && (value > (*keyIterator).Intensity()))
				++keyIterator;
			if (keyIterator == keys.begin())
			{
				color = keys[0].LeftColor();
			}
			else if (keyIterator == keys.end())
			{
				color = (*(keyIterator - 1)).RightColor();
			}
			else
			{
				// calculate the value weighted by the destination to the next left and right key
				const auto& leftKey = *(keyIterator - 1);
				const auto& rightKey = *keyIterator;
				const auto fraction = (value - leftKey.Intensity()) / (rightKey.Intensity() - leftKey.Intensity());
				const auto leftDest = leftKey.RightColor();
				const auto rightDest = rightKey.LeftColor();
				color = leftDest + (rightDest - leftDest) * fraction;
			}
			//if (factor != 1)
			//	a = 1.0 - pow(1.f - a, factor);
			transferFunction[x] = color;
		}
		for (int x = backStart; x < dimension; ++x)
			transferFunction[x] = RGBASpectrum{ 0.f };


	}

	TransferFunction::TransferFunction(const std::string& fileName) :ColorInterpulator(fileName)
	{
		compareFunc = [](const MappingKey& m1, const MappingKey & m2)->bool {return m1.Intensity() < m2.Intensity(); };
	}

	TransferFunction::~TransferFunction()
	{

	}

	void TransferFunction::AddMappingKey(const MappingKey& key)
	{
		keys.push_back(key);
		std::sort(keys.begin(), keys.end(), compareFunc);
		Notify();
	}

	void TransferFunction::UpdateMappingKey(const Point2f& at, const Point2f & to, const Vector2f & scale)
	{
		auto index = -1;
		for (auto i = 0; i < keys.size(); i++)
		{
			const auto k = keys[i];
			const auto p = TransToPos(k);
			const Point2f coords = { p.x*scale.x,p.y*scale.y };
			if ((coords - at).LengthSquared() <= 5)
			{
				index = i;
				std::cout << "Hit:" << i << std::endl;
				break;
			}
		}

		if (index == -1)
		{
			///TODO:: Adding a new mapping key if there is no key at the click pos
			return;
		}

		const auto newAlpha = to.y / scale.y;
		const auto newIntensity = to.x / scale.x;
		keys[index].SetRightAlpha(newAlpha);
		keys[index].SetLeftAlpha(newAlpha);
		keys[index].SetIntensity(newIntensity);
		const auto & key = keys[index];

		// Keep the keys which are left of the selected key also being left 
		for (auto i = 0; i < index; i++)
		{
			auto & k = keys[i];
			if (k.Intensity() > key.Intensity())
				k.SetIntensity(key.Intensity());
		}

		// Keep the keys which are right of the selected key also being right 
		for (auto i = index + 1; i < keys.size(); i++)
		{
			auto & k = keys[i];
			if (k.Intensity() < key.Intensity())
				k.SetIntensity(key.Intensity());
		}

		Notify();
	}

	void TransferFunction::UpdateMappingKey(const Point2f& at, const MappingKey& key, const Vector2f & scale)
	{

		Notify();
	}

	void TransferFunction::AddUpdateCallBack(const Callback& func)
	{
		callbacks.push_back(func);
	}

	Point2f TransferFunction::KeyPosition(int i, Float sx, Float sy) const
	{
		const auto p = TransToPos(keys[i]);
		return { p.x*sx,p.y*sy };
	}

	int TransferFunction::HitAt(const Point2f& at, const Vector2f& scale)
	{
		for (auto i = 0; i < keys.size(); i++)
		{
			const auto k = keys[i];
			const auto p = TransToPos(k);
			const Point2f coords = { p.x*scale.x,p.y*scale.y };
			if ((coords - at).LengthSquared() <= 5)
				return i;
		}
		return -1;
	}

	void TransferFunction::UpdateMappingKey(int index, const RGBASpectrum& color)
	{
		keys[index].SetLeftColor(color);
		keys[index].SetRightColor(color);
		Notify();
	}

	void TransferFunction::Notify()
	{
		for (const auto &c : callbacks)
			c(this);
	}

	Point2f TransferFunction::TransToPos(const MappingKey& key)
	{
		return { key.Intensity(),key.LeftColor()[3] };		//(x, y) = (intensity, alpha)
	}
}