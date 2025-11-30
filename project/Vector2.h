#pragma once


struct Vector2
{
    float x;
    float y;


    Vector2 operator+(const Vector2& rhs)const
    {
        return { x + rhs.x ,y + rhs.y};
    }

    Vector2 operator-(const Vector2& rhs) const
    {
        return { x - rhs.x, y - rhs.y };
    }

    Vector2 operator*(const Vector2& rhs)const
    {
        return{ x * rhs.x,y * rhs.y };
    }

    Vector2 operator/(const Vector2& rhs) const
    {
        return { x / rhs.x,y / rhs.y };
    }

    Vector2 operator+=(const Vector2& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vector2 operator-=(const Vector2& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    Vector2 operator*=(const Vector2& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    Vector2 operator/=(const Vector2& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }


};

