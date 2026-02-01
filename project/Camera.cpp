#include "Camera.h"
#include "Matrix4x4Math.h"
#include "WinApp.h"
#include "ImguiManger.h"


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

void Camera::DebugUpdate()
{

#ifdef USE_IMGUI
	ImGui::Begin("CameraSettings");

	// 現在のカメラ値を取得
	Transform cameraTransform;
	cameraTransform.rotate = GetRotate();
	cameraTransform.translate = GetTranslate();

	// 移動
	if (ImGui::DragFloat3("CameraTranslate", &cameraTransform.translate.x, 0.1f)) {
		SetTranslate(cameraTransform.translate);
	}

	// 回転
	bool updated = false;
	updated |= ImGui::SliderAngle("CameraRotX", &cameraTransform.rotate.x, -89.0f, 89.0f);
	updated |= ImGui::SliderAngle("CameraRotY", &cameraTransform.rotate.y, -180.0f, 180.0f);
	updated |= ImGui::SliderAngle("CameraRotZ", &cameraTransform.rotate.z, -180.0f, 180.0f);

	if (updated) {
		SetRotate(cameraTransform.rotate);
	}

	ImGui::End();


#endif 


}


void Camera::Update()
{
	worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

	viewMatrix = Inverse(worldMatrix);

	projectionMatrix = MakePerspectiveFovMatrix(fovY, aspectRatio, nearClip, farClip);

	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);

}

