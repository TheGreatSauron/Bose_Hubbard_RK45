#include "RK45_1D.h"

#include <iostream>

RK45_1D::RK45_1D(double step_size, double accuracy, std::valarray<comp> initial, double norm, double mu, double U) :
	RK45(step_size, accuracy, initial), mu(mu), U(U), J(1.0), N(norm)
{
	// Ensure the initial wave-function is normalized
	double sum = 0.0;

	for (int i = 0; i < y.size(); i++)
	{
		sum += std::norm(y[i]);
	}

	if (sum != 0.0)
	{
		for (int i = 0; i < y.size(); i++)
		{
			y[i] *= std::sqrt(N/sum);
		}
	}
}

std::valarray<comp> RK45_1D::func(std::valarray<comp> y1)
{
	int n = y1.size();
	std::valarray<comp> der = y1;

	// Calculate derivative for every term
	for (int i = 0; i < n; i++)
	{
		der[i] = (-mu * y1[i] - J * (y1[((i - 1) % n + n) % n] + y1[((i + 1) % n + n) % n]) + U * y1[i] * std::norm(y[i])) / i1;
	}

	return der;
}

void RK45_1D::groundState()
{
	double diff;
	int n = 0;

	do {
		// Ensure that the imaginary time propagation eventually ends,
		// limit is arbitrary
		if (n > 50000)
		{
			std::cout << "Warning: step overflow, no convergence.\n";
			break;
		}

		diff = 0.0;
		std::valarray<comp> y1 = y;

		n += full_step(comp(0.0, -1.0));

		// Flip the sign on mu and reset it if mu becomes too small
		if (std::abs(mu) <= getAcc())
		{
			mu = -mu/std::abs(mu);
		}

		// Rescale mu every step
		if (mu > 0)
		{
			mu = mu / (getNorm()/N);
		}
		else
		{ 
			mu = mu * (getNorm()/N);
		}

		//std::cout << func(y1)[0] << ' ' << getMu() << ' ' << (1 - getNorm()) << " | " << printNorms() << '\n';

		// Ensure normalization
		y *= std::sqrt(N/getNorm());

		// Calculate difference
		y1 = y - y1;

		for (int i = 0; i < y1.size(); i++)
		{
			diff += std::norm(y1[i]);
		}

		diff = std::sqrt(diff / y1.size());
	} while (diff > (getAcc() * 10000));

	setTime(0.0);
}

double RK45_1D::getMu() const
{
	return mu;
}

double RK45_1D::getU() const
{
	return U;
}
