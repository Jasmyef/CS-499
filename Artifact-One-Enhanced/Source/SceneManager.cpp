///////////////////////////////////////////////////////////////////////////////
// scenemanager.cpp
// ============
// manage the preparing and rendering of 3D scenes - textures, materials, lighting
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager* pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationZ * rotationY * rotationX * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/


/***********************************************************
 *  LoadSceneTextures()
 *
 *  Loads all image textures used in the scene into memory
 *  and registers each one with a tag name for later use.
 ***********************************************************/
void SceneManager::LoadSceneTextures()
{
	// Load wood texture for the ground plane
	CreateGLTexture("textures/wood.jpg", "wood");

	// Load brick texture for the monitor box
	CreateGLTexture("textures/brick.jpg", "brick");

	// Send all loaded textures to the graphics card
	BindGLTextures();
}

//Define Object Materials
void SceneManager::DefineObjectMaterials()
{
	OBJECT_MATERIAL woodMaterial;
	woodMaterial.diffuseColor = glm::vec3(0.8f, 0.7f, 0.5f);
	woodMaterial.specularColor = glm::vec3(0.2f, 0.2f, 0.2f);
	woodMaterial.shininess = 16.0f;
	woodMaterial.tag = "wood";
	m_objectMaterials.push_back(woodMaterial);

	OBJECT_MATERIAL brickMaterial;
	brickMaterial.diffuseColor = glm::vec3(0.9f, 0.6f, 0.4f);
	brickMaterial.specularColor = glm::vec3(0.1f, 0.1f, 0.1f);
	brickMaterial.shininess = 8.0f;
	brickMaterial.tag = "brick";
	m_objectMaterials.push_back(brickMaterial);
}
//Scene Lights
void SceneManager::SetupSceneLights()
{
	// Enable lighting
	m_pShaderManager->setBoolValue("bUseLighting", true);

	// 1 light
	m_pShaderManager->setBoolValue("pointLights[0].bActive", true);
	m_pShaderManager->setVec3Value("pointLights[0].position", 0.0f, 8.0f, 0.0f);
	m_pShaderManager->setVec3Value("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);  
	m_pShaderManager->setVec3Value("pointLights[0].diffuse", 1.5f, 1.5f, 1.5f);    
	m_pShaderManager->setVec3Value("pointLights[0].specular", 2.0f, 2.0f, 2.0f);
	// 2 light
	m_pShaderManager->setBoolValue("pointLights[1].bActive", true);
	m_pShaderManager->setVec3Value("pointLights[1].position", -5.0f, 5.0f, 5.0f);
	m_pShaderManager->setVec3Value("pointLights[1].ambient", 0.1f, 0.1f, 0.1f);
	m_pShaderManager->setVec3Value("pointLights[1].diffuse", 0.4f, 0.4f, 0.5f);
	m_pShaderManager->setVec3Value("pointLights[1].specular", 0.3f, 0.3f, 0.3f);

	
	m_pShaderManager->setBoolValue("pointLights[2].bActive", false);
	m_pShaderManager->setBoolValue("pointLights[3].bActive", false);
	m_pShaderManager->setBoolValue("pointLights[4].bActive", false);

	
	m_pShaderManager->setBoolValue("directionalLight.bActive", false);
	m_pShaderManager->setBoolValue("spotLight.bActive", false);
}

/***********************************************************
 *  PrepareScene()
 *
 *  Loads all shapes and textures into memory to prepare
 *  the 3D scene for rendering.
 ***********************************************************/
void SceneManager::PrepareScene()
{
	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadTorusMesh();

	// Load all textures into memory
	LoadSceneTextures();
	DefineObjectMaterials();
	SetupSceneLights();

}

/***********************************************************
 *  RenderScene()
 *
 *  Renders all 3D shapes by applying transformations
 *  and textures to each object in the scene.
 ***********************************************************/
void SceneManager::RenderScene()
{
	// Enhancement (Software Design and Engineering):
	if (m_sceneObjects.empty())
	{
		LoadSceneObjects("scene_config.txt");
	}

	for (const SceneObject& object : m_sceneObjects)
	{
		DrawSceneObject(object);
	}
}

/***********************************************************
 *  LoadSceneObjects()
 *
 *  Reads the scene layout from a text file into m_sceneObjects.
 *  Each non-comment line describes one object using this format:
 *
 *  meshType,scaleX,scaleY,scaleZ,rotX,rotY,rotZ,posX,posY,posZ,mode,texture,material,uvU,uvV,r,g,b,a
 *
 *  mode is either TEXTURE (uses texture/material/uvU/uvV) or COLOR (uses r,g,b,a).
 *  Lines starting with # are comments and are skipped.
 ***********************************************************/
void SceneManager::LoadSceneObjects(std::string filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cout << "WARNING: could not open scene config file: " << filename << std::endl;
		return;
	}

	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
		{
			continue;
		}

		std::stringstream lineStream(line);
		std::string field;
		std::vector<std::string> fields;

		while (std::getline(lineStream, field, ','))
		{
			fields.push_back(field);
		}

		if (fields.size() < 19)
		{
			std::cout << "WARNING: skipping malformed scene line: " << line << std::endl;
			continue;
		}

		SceneObject object;
		object.meshType = fields[0];
		object.scale = glm::vec3(std::stof(fields[1]), std::stof(fields[2]), std::stof(fields[3]));
		object.rotationX = std::stof(fields[4]);
		object.rotationY = std::stof(fields[5]);
		object.rotationZ = std::stof(fields[6]);
		object.position = glm::vec3(std::stof(fields[7]), std::stof(fields[8]), std::stof(fields[9]));
		object.useTexture = (fields[10] == "TEXTURE");
		object.textureTag = fields[11];
		object.materialTag = fields[12];
		object.uvScaleU = std::stof(fields[13]);
		object.uvScaleV = std::stof(fields[14]);
		object.colorR = std::stof(fields[15]);
		object.colorG = std::stof(fields[16]);
		object.colorB = std::stof(fields[17]);
		object.colorA = std::stof(fields[18]);

		m_sceneObjects.push_back(object);
	}

	file.close();
}

/***********************************************************
 *  DrawSceneObject()
 *
 *  Applies one object's transform, texture or color, and draws
 *  its mesh. This is the one place that used to be copy-pasted
 *  once per object inside RenderScene().
 ***********************************************************/
void SceneManager::DrawSceneObject(const SceneObject& object)
{
	SetTransformations(
		object.scale,
		object.rotationX,
		object.rotationY,
		object.rotationZ,
		object.position);

	if (object.useTexture)
	{
		SetShaderTexture(object.textureTag);
		if (!object.materialTag.empty())
		{
			SetShaderMaterial(object.materialTag);
		}
		SetTextureUVScale(object.uvScaleU, object.uvScaleV);
	}
	else
	{
		SetShaderColor(object.colorR, object.colorG, object.colorB, object.colorA);
	}

	if (object.meshType == "plane")
	{
		m_basicMeshes->DrawPlaneMesh();
	}
	else if (object.meshType == "box")
	{
		m_basicMeshes->DrawBoxMesh();
	}
	else if (object.meshType == "cylinder")
	{
		m_basicMeshes->DrawCylinderMesh();
	}
	else if (object.meshType == "torus")
	{
		m_basicMeshes->DrawTorusMesh();
	}
	else
	{
		std::cout << "WARNING: unknown mesh type in scene config: " << object.meshType << std::endl;
	}
}
