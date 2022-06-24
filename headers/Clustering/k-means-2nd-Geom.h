#ifndef _kmean_2nd_Geom_H
#define _kmean_2nd_Geom_H

#include <math.h>
#include "../pre-defs.h"
#include <vector>
#include <algorithm>

namespace GeoUtil
{
#define geoType double
	void quadratic_equation_solver(distype a, distype b, distype c, distype &x1, distype &x2)
	{
		distype delta = b * b - 4 * a * c;
		if (delta == 0)
		{
			x1 = x2 = -b / (2.0 * a);
			return;
		}
		distype sqrt_delta = sqrt(delta);
		x1 = (-b + sqrt_delta) / (2.0 * a);
		x2 = (-b - sqrt_delta) / (2.0 * a);
	}
	inline bool isInvalid(distype x)
	{
		return !(x > 0 || x <= 0);
	}
	bool isEqual(distype x, distype y)
	{
		distype eps = 0.0001, eps_2 = 0.0001;

		if ((x == 0 && y < eps) || (y == 0 && x < eps))
			return true;
		if (x == 0 || y == 0)
			return false;

		distype r = x / y;
		if (r > 1.0 - eps_2 && r < 1 + eps_2)
			return true;

		return false;
	}
	bool isEqual_costum(distype x, distype y, distype eps = 0.001)
	{
		if ((x == 0 && y < eps) || (y == 0 && x < eps))
			return true;
		if (x == 0 || y == 0)
			return false;

		distype r = x / y;
		if (r > 1.0 - eps && r < 1 + eps)
			return true;

		return false;
	}
	distype cos_between_ab(distype a, distype b, distype c)
	{

		geoType a2 = a * a, b2 = b * b, c2 = c * c;
		geoType up = a2 + b2 - c2;

		geoType down = (2 * a * b);
		geoType ans = up / down;

		if (ans > 1) ans = 1;
		else if (ans < -1) ans = -1;

		return ans;
	}
	distype front_angle_magnitude(distype a, distype b, distype cos_angle_ab)
	{
		geoType a2 = a * a, b2 = b * b, c2;
		c2 = a2 + b2 - 2 * cos_angle_ab * a * b;
		return sqrt(c2);
	}
	distype areaTriangle(distype a, distype b, distype c)
	{
		geoType s = (a + b + c) / 2;
		return sqrt(s * (s - a) * (s - b) * (s - c));
	}
	distype volume(distype xy, distype xz, distype yz, distype ox, distype oy, distype oz)
	{
		geoType aa = xy * xy, bb = yz * yz, cc = xz * xz, ee = ox * ox, dd = oy * oy, ff = oz * oz;

		geoType sum_abcdef = aa + bb + cc + dd + ee + ff;
		geoType sum = aa * ff * (sum_abcdef - 2 * (aa + ff))
			+ bb * ee * (sum_abcdef - 2 * (bb + ee))
			+ cc * dd * (sum_abcdef - 2 * (cc + dd))
			- (aa + ff) * (bb + ee) * (cc + dd) / 2
			- (aa - ff) * (bb - ee) * (cc - dd) / 2;

		geoType v = sqrt(sum / 144);
		return v;
	}	
	distype dis_o_from_ab(distype oa, distype ob, distype ab)
	{
		return 2.0 * areaTriangle(oa, ob, ab) / ab;
	}

	void oz_xyzAreIn3d(distype xy, distype xz, distype yz, distype ox, distype oy, distype height_o_from_xyz, vector<distype>&ans)
	{
		geoType s = areaTriangle(xy, xz, yz), h = height_o_from_xyz;
		geoType v2_144 = s * h / 3;
		v2_144 = v2_144 * v2_144 * 144;

		geoType a = xy * xy, b = yz * yz, c = xz * xz, e = ox * ox, d = oy * oy;

		geoType cd = c * d, ab = a * b, be = b * e, de = d * e;

		geoType B = b * d + c * e - ab - a * c - a * d - cd - a * e - be + a * a;
		geoType A = a;

		geoType C = v2_144 +
			ab * c + a * de - a * cd - b * cd - ab * e - be * c - b * de - cd * e + c * cd + cd * d + b * be + be * e;

		geoType delta2 = B * B - 4 * A * C;
		if (delta2 <= 0)
		{
			ans.push_back(sqrt(-B / (2 * A)));
			return;
		}
		/*if (delta2 < 0)
			return;*/
		geoType delta = sqrt(delta2);


		geoType f1 = (-B - delta) / (2 * A);
		geoType f2 = (-B + delta) / (2 * A);
		ans.push_back(sqrt(f1));
		ans.push_back(sqrt(f2));
	}
	distype ow_xyzwoAre2d(distype xy, distype xz, distype xw, distype ox, distype yz, distype oy, distype oz, distype wz)
	{
		/*
		case 1: oz is between xz and yz (ozy < oxc && xzo < oxc)
		case 2: oz is between other sides of xz and yz (xzo + ozy + oxc = 2pi)
		case 3: cc is near of c not o:
		case 3.1 xzw + xzo < pi
		case 3.2 xzw + xzo > pi (else of 3.1)
		case 4: cc is near of o not c:
		case 4.1 xzw < xzo (p is between o and cc)
		case 4.2 xzw > xzo (else of 4.1: cc is between o and p)
		===============================================================================
		case 1: ozw= xzo + xzw
		case 2: ozw= xzo - xzw
		case 3
		case 3.1: ozw= xzo + xzw
		case 3.2: ozw= 2pi - 3.1
		case 4
		case 4.1: ozw= xzo - xzw
		case 4.2: ozw= xzw - xzo
		*/


		distype ozw;

		distype cos_oxc = cos_between_ab(xz, yz, xy);
		distype cos_ozy = cos_between_ab(yz, oz, oy);
		distype cos_xzo = cos_between_ab(xz, oz, ox);
		distype cos_xzw = cos_between_ab(xz, wz, xw);


		distype oxc = acos(cos_oxc);
		distype ozy = acos(cos_ozy);
		distype xzo = acos(cos_xzo);
		distype xzw = acos(cos_xzw);

		if (ozy <= oxc && xzo <= oxc)//case 1: oz is between xz and yz (xzo < oxc && xzo < oxc)
			ozw = xzo + xzw;
		else if (isEqual_costum(xzo + ozy + oxc, 2 * M_PI, 0.001))
			ozw = xzo - xzw;
		else if (xzo > ozy && xzo > oxc)
		{
			ozw = xzw + xzo;
			if (ozw> M_PI)
				ozw = 2 * M_PI - ozw;
		}
		else
		{
			if (xzw < xzo)
				ozw = xzo - xzw;
			else ozw = xzw - xzo;
		}
		return sqrt(oz * oz + wz * wz - 2 * oz * wz * cos(ozw));
	}

	distype dis_o_from_xyz(distype xy, distype xz, distype yz, distype ox,
		distype oy, distype oz)
	{
		distype S = areaTriangle(xy, xz, yz);
		if (isInvalid(S))
			return S;
		distype V = volume(xy, xz, yz, ox, oy, oz);
		distype h = 3 * V / S;
		return h;
	}
	distype ow_xyzwAreIn3d(distype xy, distype xz, distype xw, distype ox,
		distype yz, distype yw, distype oy, distype oz, distype wz)
	{
		if (oz == 0)
			return wz;
		if (oy == 0)
			return yw;
		if (ox == 0)
			return xw;
		distype S = areaTriangle(xy, xz, yz);
		if (isInvalid(S))
			return S;
		distype V = volume(xy, xz, yz, ox, oy, oz);
		distype h = 3 * V / S;
		if (isInvalid(V))
			h = 0;
		vector<distype> answers;
		oz_xyzAreIn3d(xy, xw, yw, ox, oy, h, answers);
		if (answers.size() == 0)
			return ow_xyzwoAre2d(xy, xz, xw, ox, yz, oy, oz, wz);
		if (!(answers[0] > 0 || answers[0] <= 0))
			return ow_xyzwoAre2d(xy, xz, xw, ox, yz, oy, oz, wz);
		if (answers.size() == 1)
			return answers[0];
		oz_xyzAreIn3d(yz, yw, wz, oy, oz, h, answers);
		oz_xyzAreIn3d(xz, xw, wz, ox, oz, h, answers);

		sort(answers.begin(), answers.end());
		size_t index = 0;
		distype mindif = answers[2] - answers[0];
		for (size_t i = 1; i < answers.size() - 2; i++)
		{
			distype diff = answers[i + 2] - answers[i];
			if (diff < mindif)
			{
				mindif = diff;
				index = i;
			}
		}
		return answers[index];
	}

	distype owMAX_xyzwAreIn3d(distype xy, distype xz, distype xw, distype ox,
		distype yz, distype yw, distype oy, distype oz, distype wz)
	{
		if (oz == 0)
			return wz;
		if (oy == 0)
			return yw;
		if (ox == 0)
			return xw;
		distype S = areaTriangle(xy, xz, yz);
		if (isInvalid(S))
			return S;
		distype V = volume(xy, xz, yz, ox, oy, oz);
		distype h = 3 * V / S;
		if (isInvalid(V))
			h = 0;
		vector<distype> answers;
		oz_xyzAreIn3d(xy, xw, yw, ox, oy, h, answers);
		if (answers.size() == 0)
			return ow_xyzwoAre2d(xy, xz, xw, ox, yz, oy, oz, wz);
		if (!(answers[0] > 0 || answers[0] <= 0))
			return ow_xyzwoAre2d(xy, xz, xw, ox, yz, oy, oz, wz);
		if (answers.size() == 1)
			return answers[0];
		oz_xyzAreIn3d(yz, yw, wz, oy, oz, h, answers);
		oz_xyzAreIn3d(xz, xw, wz, ox, oz, h, answers);

		sort(answers.begin(), answers.end());
		return answers[answers.size() - 1];
	}
	distype owMIN_xyzwAreIn3d(distype xy, distype xz, distype xw, distype ox,
		distype yz, distype yw, distype oy, distype oz, distype wz)
	{
		if (oz == 0)
			return wz;
		if (oy == 0)
			return yw;
		if (ox == 0)
			return xw;
		distype S = areaTriangle(xy, xz, yz);
		if (isInvalid(S))
			return S;
		distype V = volume(xy, xz, yz, ox, oy, oz);
		distype h = 3 * V / S;
		if (isInvalid(V))
			h = 0;
		vector<distype> answers;
		oz_xyzAreIn3d(xy, xw, yw, ox, oy, h, answers);
		if (answers.size() == 0)
			return ow_xyzwoAre2d(xy, xz, xw, ox, yz, oy, oz, wz);
		if (!(answers[0] > 0 || answers[0] <= 0))
			return ow_xyzwoAre2d(xy, xz, xw, ox, yz, oy, oz, wz);
		if (answers.size() == 1)
			return answers[0];
		oz_xyzAreIn3d(yz, yw, wz, oy, oz, h, answers);
		oz_xyzAreIn3d(xz, xw, wz, ox, oz, h, answers);

		sort(answers.begin(), answers.end());
		return answers[0];
	}
	distype ow_1stMAX_xyzwAreIn3d(distype xy, distype xz, distype xw, distype ox,
		distype yz, distype yw, distype oy, distype oz, distype wz)
	{
		if (oz == 0)
			return wz;
		if (oy == 0)
			return yw;
		if (ox == 0)
			return xw;
		distype S = areaTriangle(xy, xz, yz);
		if (isInvalid(S))
			return S;
		distype V = volume(xy, xz, yz, ox, oy, oz);
		distype h = 3 * V / S;
		if (isInvalid(V))
			h = 0;
		vector<distype> answers;
		oz_xyzAreIn3d(xy, xw, yw, ox, oy, h, answers);
		if (answers.size() == 0)
			return ow_xyzwoAre2d(xy, xz, xw, ox, yz, oy, oz, wz);
		if (!(answers[0] > 0 || answers[0] <= 0))
			return ow_xyzwoAre2d(xy, xz, xw, ox, yz, oy, oz, wz);
		if (answers.size() == 1)
			return answers[0];
		oz_xyzAreIn3d(yz, yw, wz, oy, oz, h, answers);
		if (answers[0] < answers[1])
			return answers[1];
		return answers[0];
	}
	distype ow_1stMIN_xyzwAreIn3d(distype xy, distype xz, distype xw, distype ox,
		distype yz, distype yw, distype oy, distype oz, distype wz)
	{
		if (oz == 0)
			return wz;
		if (oy == 0)
			return yw;
		if (ox == 0)
			return xw;
		distype S = areaTriangle(xy, xz, yz);
		if (isInvalid(S))
			return S;
		distype V = volume(xy, xz, yz, ox, oy, oz);
		distype h = 3 * V / S;
		if (isInvalid(V))
			h = 0;
		vector<distype> answers;
		oz_xyzAreIn3d(xy, xw, yw, ox, oy, h, answers);
		if (answers.size() == 0)
			return ow_xyzwoAre2d(xy, xz, xw, ox, yz, oy, oz, wz);
		if (!(answers[0] > 0 || answers[0] <= 0))
			return ow_xyzwoAre2d(xy, xz, xw, ox, yz, oy, oz, wz);
		if (answers.size() == 1)
			return answers[0];
		oz_xyzAreIn3d(yz, yw, wz, oy, oz, h, answers);
		if (answers[0] < answers[1])
			return answers[0];
		return answers[1];
	}
}

#endif