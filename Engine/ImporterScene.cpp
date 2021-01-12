#include "ImporterScene.h"
#include "ImporterMesh.h"

#include "ModuleSceneManager.h"
#include "ModuleCamera.h"
#include "Component.h"
#include "GameObject.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

// Assimp Callback
struct aiLogStream stream;		// Assimp logs are registered in this variable
void AssimpLog(const char* msg, char* user) {
	if (msg)
		LOG("[info] Assimp Log: %s", msg);
}

void ImporterScene::Load(std::string const& _path)
{
	stream.callback = AssimpLog;
	aiAttachLogStream(&stream);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(_path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		LOG("[error] Error loading %s: %s", _path, importer.GetErrorString());
	}
	else
	{
		std::string directory = _path.substr(0, _path.find_last_of('/')).size() == _path.size() ? _path.substr(0, _path.find_last_of('\\')) : _path.substr(0, _path.find_last_of('/'));
		std::string filename = _path.substr(0, _path.find_last_of('/')).size() == _path.size() ? _path.substr(_path.find_last_of('\\') + 1, _path.size()) : _path.substr(_path.find_last_of('/') + 1, _path.size());

		GameObject* newModel = new GameObject(filename.c_str());
		newModel->SetParent(App->sceneMng->GetRoot());
		ProcessNode(scene->mRootNode, scene, newModel, directory);

		// Set default shading program for this GameObject and all its children
		newModel->SetProgram(App->sceneMng->GetProgram());
		for (std::vector<GameObject*>::const_iterator it = newModel->GetChildren().begin(); it != newModel->GetChildren().end(); ++it) {
			(*it)->SetProgram(App->sceneMng->GetProgram());
		}
	}
	importer.FreeScene();

	// TODO: What if new scene has no transform? (could it be possible?)
	App->camera->onFocus(App->sceneMng->GetRoot()->GetChildren()[App->sceneMng->GetRoot()->GetChildren().size() - 1]->GetModelMatrix().Col3(3), 10);
}

void ImporterScene::ProcessNode(aiNode* node, const aiScene* scene, GameObject* object, std::string _dir)
{
	object->AddComponent(TRANSFORM, nullptr);
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		std::string nodeName = "./Library/Meshes/";
		nodeName.append(node->mName.C_Str());

		bool imported = ImporterMesh::Import(mesh, object);
		// TODO: the if nesting shouldnt be included?
		if (imported)
		{
			// save the custom file format
			unsigned int fsize = ImporterMesh::Save(object->GetComponent<CMesh>(), nodeName.c_str());
			
			object->RemoveComponent(object->GetComponent<CMesh>()->GetUID()); // empty the cmesh (THIS IS PROVISIONAL UNTIL THE FILESYSTEM IS CORRECTLY IMPLEMENTED) (we will rather import or load, but not both)
			if (fsize > 0)
			{
				// load from custom file format
				ImporterMesh::Load(nodeName.c_str(), object, fsize);
			}
			
			
			//	TODO: log time
		}

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		object->AddComponent(MATERIAL, (void*)material, _dir);
		//	log time
		//	save the custom file format
		//	load from custom file format
		//	log time
	}
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		GameObject* newChild = new GameObject(node->mChildren[i]->mName.C_Str());
		ProcessNode(node->mChildren[i], scene, newChild, _dir);
		newChild->SetParent(object);
	}
}