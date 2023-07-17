#include "Point3.h"
#include <cmath>

Point3& Point3::operator=(const Point3& rhs)
{
    if (this != &rhs)
    	copy(rhs);

    return *this;
}



Point3 Point3::operator+(const Point3& rhs)
{
    Point3 result(*this);
    return result += rhs;
}



Point3 Point3::operator-(const Point3& rhs)
{
    Point3 result(*this);
    return result -= rhs;
}



Point3 Point3::operator*(const double& rhs)
{
    Point3 result(*this);
    return result *= rhs;
}



Point3 Point3::operator/(const double& rhs)
{
    Point3 result(*this);
    return result /= rhs;
}



Point3& Point3::operator+=(const Point3& rhs)
{
    m_x += rhs.m_x;
    m_y += rhs.m_y;
    m_z += rhs.m_z;
    return *this;
}



Point3& Point3::operator-=(const Point3& rhs)
{
    m_x -= rhs.m_x;
    m_y -= rhs.m_y;
    m_z -= rhs.m_z;
    return *this;
}



Point3& Point3::operator*=(const double& rhs)
{
    m_x *= rhs;
    m_y *= rhs;
    m_z *= rhs;
    return *this;
}



Point3& Point3::operator/=(const double& rhs)
{
    m_x /= rhs;
    m_y /= rhs;
    m_z /= rhs;
    return *this;
}



void Point3::copy(const Point3& rhs)
{
    m_x = rhs.m_x;
	m_y = rhs.m_y;
	m_z = rhs.m_z;
}



double Point3::distance_to(const Point3& rhs) const
{
    double distance = sqrt((m_x - rhs.m_x)*(m_x - rhs.m_x) + (m_y - rhs.m_y)*(m_y - rhs.m_y) + (m_z - rhs.m_z)*(m_z - rhs.m_z));
    return distance;
}



Point3 operator*(const double& lhs, const Point3& rhs)
{
    Point3 result(rhs);
    return result *= lhs;
}
