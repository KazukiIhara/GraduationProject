#include "Rice.h"

#include "LGF/Parameter.h"

#include <stdexcept>

using namespace LGF;

namespace {

	constexpr const char* kRiceParameterFilePath = "../Assets/Parameters/Rice.json";

	struct RiceParameters {
		float radius;
		float mass;
		float restitution;
		float friction;
	};

	float ReadParameter(const ParameterDocument& document, const char* path) {
		const auto value = document.Get<float>(path);
		if (!value) {
			throw std::runtime_error(std::string("Invalid rice parameter: ") + path);
		}
		return *value;
	}

	RiceParameters LoadRiceParameters() {
		ParameterDocument document;
		const ParameterIoResult result = document.Load(kRiceParameterFilePath);
		if (!result) {
			throw std::runtime_error(result.message);
		}

		return RiceParameters{
			.radius = ReadParameter(document, "Radius"),
			.mass = ReadParameter(document, "Mass"),
			.restitution = ReadParameter(document, "Material/Restitution"),
			.friction = ReadParameter(document, "Material/Friction"),
		};
	}

	const RiceParameters& GetRiceParameters() {
		static const RiceParameters parameters = LoadRiceParameters();
		return parameters;
	}

}

Rice::Rice(Physics::PhysicsWorld& physicsWorld, const Vector3& position, ObjectHandle objectHandle, Physics::PhysicsParticleRenderGroupHandle renderGroup) :
	physicsWorld_(&physicsWorld) {
	const RiceParameters& parameters = GetRiceParameters();
	physicsHandle_ = physicsWorld_->AddParticle({
		.position = position,
		.radius = parameters.radius,
		.mass = parameters.mass,
		.material = {
			.restitution = parameters.restitution,
			.friction = parameters.friction,
		},
		.objectHandle = objectHandle,
		.renderGroup = renderGroup,
	});
}

Rice::~Rice() {
	if (physicsWorld_ != nullptr) {
		physicsWorld_->RemoveParticle(physicsHandle_);
	}
}
