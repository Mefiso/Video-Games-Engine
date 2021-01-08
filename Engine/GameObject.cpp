#include "Application.h"
#include "ModuleSceneManager.h"
#include "GameObject.h"
#include "CMesh.h"
#include "CTransform.h"
#include "CMaterial.h"
#include "CCamera.h"
#include "debugdraw.h"


int GameObject::objectCount = 0;
bool GameObject::drawOBB = false;

GameObject::GameObject()
{
	aabb.SetNegativeInfinity();
	obb.SetNegativeInfinity();
	++objectCount;
}

GameObject::GameObject(const std::string& _name) : name(_name)
{
	aabb.SetNegativeInfinity();
	obb.SetNegativeInfinity();
	++objectCount;
}

GameObject::~GameObject()
{
	CleanUp();
}

void GameObject::CleanUp()
{
	if (GetComponent<CMesh>())
		App->sceneMng->octree.Erase(this);
	// Clean components
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		RELEASE(*it);
	}
	components.clear();

	// Clean childs
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		RELEASE(*it);
	}
	children.clear();
}

void GameObject::Draw()
{
	//dd::aabb(aabb.minPoint, aabb.maxPoint, float3(0.9f));
	if (name.compare("Scene 1") != 0 && drawOBB)
	{
		dd::box(obbPoints, float3(0.9f));
	}
	// update components
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		(*it)->Draw();
	}
	//update children accordingly
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->Draw();
	}
}

void GameObject::AddComponent(ComponentType _type, void* arg, const std::string& path)
{
	Component* newComp;
	//TODO: for (unsigned int i = 0; i< components.size())
	//check if already exists CMaterial or CTransform
	switch (_type)
	{
	case TRANSFORM:
		newComp = new CTransform(this);
		transform = (CTransform*)newComp;
		break;
	case MESH:
		newComp = new CMesh(this, (aiMesh*)arg);
		break;
	case MATERIAL:
		newComp = new CMaterial(this, (aiMaterial*)arg, path);
		break;
	case CAMERA:
		newComp = new CCamera(this);
		break;
	default:
		newComp = new Component(INVALID, this);
	}

	components.push_back(newComp);
	if (_type == MESH)
	{
		UpdateBoundingBoxes();
		App->sceneMng->octree.Insert(this);
	}
}

void GameObject::RemoveComponent(int _cID)
{
	int toRemove = -1;

	for (unsigned int i = 0u; i < components.size(); ++i)
	{
		if (components[i]->ID == _cID)
		{
			toRemove = (int)i;
			break;
		}
	}
	if (toRemove >= 0)
	{
		if (components[toRemove]->GetType() == TRANSFORM)
			transform = nullptr;
		RELEASE(components[toRemove]);
		components.erase(components.begin() + toRemove);
	}
}

void GameObject::SetParent(GameObject* _newParent)
{
	// Check if _newParent if a child of this GameObject
	bool isChild = false;
	for (unsigned int i = 0; i < this->GetChildren().size(); ++i)
	{
		if (this->children[i]->GetUID() == _newParent->GetUID())
		{
			isChild = true;
			break;
		}
	}
	// Change parents (only if the parent was not one of its childs)
	if (!isChild)
	{
		_newParent->AddChild(this);
		if (parent)
		{
			parent->RemoveChild(this->uID);
		}
		parent = _newParent;
	}
}

void GameObject::AddChild(GameObject* _newChild)
{
	if (_newChild->transform && _newChild->parent != nullptr)
	{
		float4x4 worldTransform = _newChild->GetModelMatrix();
		float4x4 thisTransform = this->transform ? this->GetModelMatrix() : float4x4::identity;
		float3 thisScale = float3(thisTransform.Col3(0).Length(), thisTransform.Col3(1).Length(), thisTransform.Col3(2).Length());
		if (thisScale.Equals(float3::one))
			thisTransform.InverseOrthonormal();
		else if (thisScale.xxx().Equals(thisScale))
			thisTransform.InverseOrthogonalUniformScale();
		else
			thisTransform.InverseColOrthogonal();

		_newChild->SetTransform(thisTransform * worldTransform);
	}
	children.push_back(_newChild);
}

void GameObject::RemoveChild(int childID)
{
	int toRemove = -1;
	for (unsigned int i = 0u; i < children.size(); ++i)
	{
		if (children[i]->uID == childID)
		{
			toRemove = (int)i;
			break;
		}
	}
	if (toRemove >= 0)
		children.erase(children.begin() + toRemove);
}

float4x4 GameObject::GetModelMatrix() const
{
	if (parent)
		return parent->GetModelMatrix() * (transform ? this->transform->GetTransformationMatrix() : float4x4::identity);
	else
		return  transform ? this->transform->GetTransformationMatrix() : float4x4::identity;
}

const float3 GameObject::GetAccumulatedScale() const
{
	if (parent)
		return parent->GetAccumulatedScale().Mul(transform ? this->transform->GetScale() : float3::one);
	else
		return transform ? this->transform->GetScale() : float3::one;
}

void GameObject::SetTransform(float3& _scale, float3& _rotation, float3& _translation)
{
	transform->SetPos(_translation);
	transform->SetRotation(_rotation);
	transform->SetScale(_scale);
	transform->UpdateTransformMatrix();
}

void GameObject::SetTransform(float4x4& _newTransform)
{
	transform->SetPos((float3)(_newTransform.Col3(3)));

	transform->SetScale(float3(_newTransform.Col3(0).Length(), _newTransform.Col3(1).Length(), _newTransform.Col3(2).Length()));

	float3 rotation;
	rotation.x = -atan2(_newTransform.Col3(2)[1], _newTransform.Col3(2)[2]);
	rotation.y = -atan2(-_newTransform.Col3(2)[0], sqrt(_newTransform.Col3(2)[1] * _newTransform.Col3(2)[1] + _newTransform.Col3(2)[2] * _newTransform.Col3(2)[2]));
	rotation.z = -atan2(_newTransform.Col3(1)[0], _newTransform.Col3(0)[0]);
	transform->SetRotation(rotation);

	transform->UpdateTransformMatrix();
}

void GameObject::SetProgram(unsigned int program)
{
	// update components
	for (std::vector<Component*>::iterator it = components.begin(); it != components.end(); ++it)
	{
		if ((*it)->GetType() == MESH)
			((CMesh*)(*it))->SetProgram(program);
	}
	//update children accordingly
	for (std::vector<GameObject*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->SetProgram(program);
	}
}

void GameObject::UpdateBoundingBoxes()
{
	aabb.SetNegativeInfinity();
	CMesh* mesh = GetComponent<CMesh>();
	bool nonuniformScaling = false;
	if (mesh)
	{
		aabb.Enclose(mesh->AABBmin, mesh->AABBmax);
		if (GetAccumulatedScale().MaxElement() != GetAccumulatedScale().MinElement()) // Non uniform scaling
		{
			nonuniformScaling = true;
			aabb.Scale(aabb.CenterPoint(), GetAccumulatedScale());
		}
	}

	for (GameObject* child : children)
		child->UpdateBoundingBoxes();

	obb = aabb.Transform(GetModelMatrix());
	if (nonuniformScaling)
		obb.Scale(obb.CenterPoint(), 1.0 / GetAccumulatedScale().x);

	//Added, calculate obb vertices only when obb is updated
	ddVec3 points[8];
	obb.GetCornerPoints(points);
	ddVec3 points2[8] = { points[0], points[1], points[3], points[2], points[4], points[5], points[7], points[6] };
	memcpy(obbPoints, points2, sizeof(ddVec3) * 8);
}

void GameObject::UpdateOctreePosition()
{
	if (GetComponent<CMesh>())
		App->sceneMng->octree.UpdateGO(this);
	for (GameObject* child : children)
	{
		child->UpdateOctreePosition();
	}

}

float4 GameObject::ComputeCenterAndDistance() const
{
	if (GetComponent<CMesh>() != nullptr)
	{
		return float4(GetModelMatrix().Col3(3), aabb.Size().Length() * 2.f);
	}
	else
	{
		float3 minPoint = float3::inf, maxPoint = -float3::inf;
		for (GameObject* child : children)
		{
			minPoint = minPoint.Min(child->aabb.minPoint);
			maxPoint = maxPoint.Max(child->aabb.maxPoint);
		}
		return float4(GetModelMatrix().Col3(3), (maxPoint - minPoint).Length() * 2.f);
	}
}
