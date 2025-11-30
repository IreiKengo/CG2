#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"

namespace math
{
	//単位行列の作成
	Matrix4x4 MakeIdentity4x4();

	//平行移動行列
	Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

	//拡大縮小行列
	Matrix4x4 MakeScaleMatrix(const Vector3& scale);

	//x軸回転行列
	Matrix4x4 MakeRotateXMatrix(float radian);

	// y軸回転行列
	Matrix4x4 MakeRotateYMatrix(float radian);

	// z軸回転行列
	Matrix4x4 MakeRotateZMatrix(float radian);

	//行列の積
	Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

	//三次元アフィン変換行列
	Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

	//透視投影行列
	Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

	//平行投影行列
	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

	//逆行列
	Matrix4x4 Inverse(const Matrix4x4& m);

}