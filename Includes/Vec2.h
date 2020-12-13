#ifndef VEC2_H
#define VEC2_H

class Vec2
{
public:
		double x, y;

		Vec2() : x(0), y(0){ }
		Vec2(double a, double b) { x=a; y=b; }
		Vec2(int a, int b) { x=a; y=b; }
		~Vec2(){ };

		Vec2& operator-();

		bool operator==(Vec2 v);
		bool operator!=(Vec2 v);

		Vec2  operator+(Vec2 v);	// +translate
		Vec2  operator-(Vec2 v);	// -translate
		Vec2& operator+=(Vec2 v);	// inc translate
		Vec2& operator-=(Vec2 v);	// dec translate

		double operator*(Vec2 v);		// dot product
		Vec2 operator*(double s);		// scale
		Vec2 operator/(double s);		// scale
		void FrameMove(double radians);

		Vec2 Normalize(){ return *this * (1/Magnitude()); }
		double Magnitude() const;			// Polar magnitude
		double Argument() const;			// Polar argument
		double Distance(Vec2 v) const;		// Distance
};

Vec2 Polar(double r, double radians);
double PrincipleAngle(double radians);

#endif
