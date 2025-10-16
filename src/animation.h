#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "bone.h"
#include <functional>
#include "animdata.h"
#include "model_animation.h"
#include <iostream> // Add missing include
#include <algorithm> // Add for std::find_if

struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation
{
public:
    Animation() = default;
    ~Animation() = default; // Use default destructor

    // Delete copy constructor and assignment to prevent shallow copies
    Animation(const Animation&) = delete;
    Animation& operator=(const Animation&) = delete;

    bool LoadAnimation(const std::string& animationPath, ModelCPP* model)
    {
        if (!model) {
            std::cerr << "[ERROR] Invalid model pointer" << std::endl;
            return false;
        }

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(animationPath,
            aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_LimitBoneWeights);

        if (!scene || !scene->mRootNode)
        {
            std::cerr << "[ERROR] Failed to load animation: " << animationPath << std::endl;
            std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
            return false;
        }

        if (scene->mNumAnimations == 0)
        {
            std::cerr << "[ERROR] No animations found in file: " << animationPath << std::endl;
            return false;
        }

        auto animation = scene->mAnimations[0];
        m_Duration = static_cast<float>(animation->mDuration);
        m_TicksPerSecond = static_cast<float>(animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f);

        ReadHierarchyData(m_RootNode, scene->mRootNode);
        return ReadMissingBones(animation, *model);
    }

    Bone* FindBone(const std::string& name)
    {
        auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
            [&](const Bone& Bone)
            {
                return Bone.GetBoneName() == name;
            }
        );
        if (iter == m_Bones.end()) return nullptr;
        else return &(*iter);
    }

    inline float GetTicksPerSecond() const { return m_TicksPerSecond; }
    inline float GetDuration() const { return m_Duration; }
    inline const AssimpNodeData& GetRootNode() const { return m_RootNode; }
    inline const std::map<std::string, BoneInfo>& GetBoneIDMap() const
    {
        return m_BoneInfoMap;
    }

private:
    bool ReadMissingBones(const aiAnimation* animation, ModelCPP& model)
    {
        if (!animation) return false;

        int size = animation->mNumChannels;

        auto& boneInfoMap = model.GetBoneInfoMap();
        int& boneCount = model.GetBoneCount();

        // Check if we have channels
        if (size <= 0) {
            std::cerr << "[WARNING] Animation has no channels" << std::endl;
            return false;
        }

        // reading channels(bones engaged in an animation and their keyframes)
        for (int i = 0; i < size; i++)
        {
            auto channel = animation->mChannels[i];
            if (!channel) continue;

            std::string boneName = channel->mNodeName.data;

            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                boneInfoMap[boneName].id = boneCount;
                boneCount++;
            }
            m_Bones.push_back(Bone(channel->mNodeName.data,
                boneInfoMap[channel->mNodeName.data].id, channel));
        }

        m_BoneInfoMap = boneInfoMap;
        return true;
    }

    void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
    {
        if (!src) return;

        dest.name = src->mName.data;
        dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
        dest.childrenCount = src->mNumChildren;

        for (int i = 0; i < src->mNumChildren; i++)
        {
            AssimpNodeData newData;
            ReadHierarchyData(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }

    float m_Duration = 0.0f;
    int m_TicksPerSecond = 0;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
};