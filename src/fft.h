#ifndef FFT_H
#define FFT_H

#include <ffft/FFTRealFixLen.h>
#include <vector>
#include <exception>

#include <QDebug>

namespace FFT {
	template<int Exp>
	struct Impl {
		static void transform(float * out, const float * in) {
			auto & fft = instance();
			fft.do_fft(out, in);
			fft.rescale(out);
		}

		static void inverse(float * out, const float * in) {
			auto & fft = instance();
			fft.do_ifft(in, out);
		}

		static ffft::FFTRealFixLen<Exp> & instance() {
			static ffft::FFTRealFixLen<Exp> _fft;
			return _fft;
		}
	};

	inline void select_impl(std::vector<float> & s, const std::vector<float> & v, int exp) {
		switch (exp) {
			case 0:  return Impl<0 >::transform(s.data(), v.data());
			case 1:  return Impl<1 >::transform(s.data(), v.data());
			case 2:  return Impl<2 >::transform(s.data(), v.data());
			case 3:  return Impl<3 >::transform(s.data(), v.data());
			case 4:  return Impl<4 >::transform(s.data(), v.data());
			case 5:  return Impl<5 >::transform(s.data(), v.data());
			case 6:  return Impl<6 >::transform(s.data(), v.data());
			case 7:  return Impl<7 >::transform(s.data(), v.data());
			case 8:  return Impl<8 >::transform(s.data(), v.data());
			case 9:  return Impl<9 >::transform(s.data(), v.data());
			case 10: return Impl<10>::transform(s.data(), v.data());
			case 11: return Impl<11>::transform(s.data(), v.data());
			case 12: return Impl<12>::transform(s.data(), v.data());
			case 13: return Impl<13>::transform(s.data(), v.data());
			case 14: return Impl<14>::transform(s.data(), v.data());
			case 15: return Impl<15>::transform(s.data(), v.data());
			case 16: return Impl<16>::transform(s.data(), v.data());
			case 17: return Impl<17>::transform(s.data(), v.data());
			case 18: return Impl<18>::transform(s.data(), v.data());
			case 19: return Impl<19>::transform(s.data(), v.data());
			case 20: return Impl<20>::transform(s.data(), v.data());
			case 21: return Impl<21>::transform(s.data(), v.data());
			case 22: return Impl<22>::transform(s.data(), v.data());
			case 23: return Impl<23>::transform(s.data(), v.data());
			case 24: return Impl<24>::transform(s.data(), v.data());
			case 25: return Impl<25>::transform(s.data(), v.data());
			case 26: return Impl<26>::transform(s.data(), v.data());
			case 27: return Impl<27>::transform(s.data(), v.data());
			case 28: return Impl<28>::transform(s.data(), v.data());
			case 29: return Impl<29>::transform(s.data(), v.data());
			case 30: return Impl<30>::transform(s.data(), v.data());
			default:
				throw std::exception();
		}
	}

	inline int transform(std::vector<float> & out, const std::vector<float> & v, qreal min, qreal max, qreal rate) {
		if(v.size() <= 1) {
			return 0;
		}

		int exp = 0, size = 1;

		while (size <= (v.size() >> 1)) {
			size = 1 << (++exp);
		}

		std::vector<float> s;
		s.resize(size);

		try {
			select_impl(s, v, exp);
		} catch(const std::exception &) {
			qDebug() << "Unsupported FFT exp:" << exp;
			return 0;
		}

		qreal step = rate / size;

		min = std::min(qreal(size / 2), min / step);
		max = std::min(qreal(size / 2), max / step);

		qDebug() << min << max;

		for(int i = int(min); i < int(max); ++i) {
			int x = size / 2 - std::abs(size / 2 - i);
			int y = size - std::abs(size / 2 - i);

			float f = x % (size / 2) == 0 ? std::abs(s[x]) : std::sqrt(s[x] * s[x] + s[y] * s[y]);
			out.push_back(f);
		}

		return exp;
	}
}

#endif // FFT_H
