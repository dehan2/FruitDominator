#pragma once

class Point3
{
private:
	double m_x = 0.0;
	double m_y = 0.0;
	double m_z = 0.0;

public:
	Point3() = default;
	Point3(double x, double y, double z) : m_x(x), m_y(y), m_z(z) {}
	Point3(const Point3& rhs) { copy(rhs); }
	~Point3() = default;

	Point3& operator=(const Point3& rhs);

	Point3 operator+(const Point3& rhs);
	Point3 operator-(const Point3& rhs);
	Point3 operator*(const double& rhs);
	Point3 operator/(const double& rhs);

	Point3& operator+=(const Point3& rhs);
	Point3& operator-=(const Point3& rhs);
	Point3& operator*=(const double& rhs);
	Point3& operator/=(const double& rhs);

	friend Point3 operator*(const double& lhs, const Point3& rhs);
	
	const double& x() const { return m_x; }
	const double& y() const { return m_y; }
	const double& z() const { return m_z; }

	void set_x(const double& x) { m_x = x; }
	void set_y(const double& y) { m_y = y; }
	void set_z(const double& z) { m_z = z; }

	void copy(const Point3& rhs);

	double distance_to(const Point3& rhs) const;
};

