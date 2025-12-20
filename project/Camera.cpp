#include "Camera.h"
#include "Matrix4x4Math.h"
#include "WinApp.h"


using namespace math;


Camera::Camera()
	: transform({ { 1.0f,1.0f,1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f,0.0f,0.0f } })
	, fovY(0.45f)
	, aspectRatio(float(WinApp::kClientWidth) / float(WinApp::kClientHeight))
	, nearClip(0.1f)
	, farClip(100.0f)
	, worldMatrix(MakeAffineMatrix(transform.scale, transform.rotate, transform.translate))
	, viewMatrix(Inverse(worldMatrix))
	, projectionMatrix(MakePerspectiveFovMatrix(fovY, aspectRatio, nearClip, farClip))
	, viewProjectionMatrix(Multiply(viewMatrix, projectionMatrix))

{}


void Camera::Update()
{
	worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

	viewMatrix = Inverse(worldMatrix);

	projectionMatrix = MakePerspectiveFovMatrix(fovY, aspectRatio, nearClip, farClip);

	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);

}

